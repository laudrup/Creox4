/***************************************************************************
						phaserprocessor.cpp  -  description
							 -------------------
		begin                : Tue Aug 22 2000
		copyright            : (C) 2000 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/*
 * to do: linear sweep
 *        notch filter
 */

#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif
#include "control.h"
#include <cstring>
#include <cmath>
#include <iostream>
#include <cerrno>
#include "crdatastream.h"
#include "phaserprocessor.h"

PhaserProcessor::PhaserProcessor(PhaserParameters* parameters)
 :SoundProcessor(parameters)
{
	registerProcessor("phaser", 1.0f);

	mo_parameters = parameters;
	(void)pthread_mutex_init(&m_mutex, NULL);
	m_coefBuffer = 0;
	mr_coefBuffer = 0;

}

PhaserProcessor::~PhaserProcessor()
{
	cleanup();
	pthread_mutex_destroy(&m_mutex);
#ifdef _DEBUG
	std::cerr << "PhaserProcessor deleted...\n";
#endif
}

void PhaserProcessor::initialize()
{
	mp_samplesPerHalfCycle = mp_bottomFrequency = mp_topFrequency = mp_resonance = -1.0f;
	m_halfSamplerate = (m_samplerate_f / 2.0f) * 0.85f;
}

void PhaserProcessor::computeParameters()
{
	mr_inputGain = exp10f(mo_parameters->inputGain / 20.0f);
	mr_sweepGain = exp10f(mo_parameters->sweepGain / 20.0f);
	mr_dryMix = mo_parameters->dryMix / 100.0f;

	float bottomFrequency = mo_parameters->centerFrequency / mo_parameters->depth;
	if (bottomFrequency < 16.0f)
	{
		bottomFrequency = 16.0f;
	}
	float topFrequency = mo_parameters->centerFrequency * mo_parameters->depth;
	if (topFrequency > m_halfSamplerate)
	{
		topFrequency = m_halfSamplerate;
	}

	const float resonance = mo_parameters->resonance * 2.0f;
	const int samplesPerHalfCycle = (roundToInt(m_samplerate_d / mo_parameters->sweepFrequency) >> 1);
	mr_recalculate = false;

	if (samplesPerHalfCycle != mp_samplesPerHalfCycle ||
		bottomFrequency != mp_bottomFrequency ||
		topFrequency != mp_topFrequency ||
		resonance != mp_resonance)
	{
		const float expStep = powf(topFrequency / bottomFrequency,
								   1.0f / static_cast<float>(samplesPerHalfCycle - 1));
		// make lookup table for short periods only  (2 x 44100(halfPeriod)
		//							samples = 0.5Hz = 334.5Kb in 44100bit/sec)
		float* const coefBuffer = ((samplesPerHalfCycle << 1) < 88201)
								? (new float[samplesPerHalfCycle << 1]) : 0;
		//we need two lookupTables (alpha & cs)
		pthread_mutex_lock(&m_mutex);
		mr_expStep = expStep;
		mr_stepFrequency = mp_bottomFrequency = bottomFrequency;
		mr_resonance = mp_resonance = resonance;
		delete[] mr_coefBuffer;
		mr_coefBuffer = coefBuffer;
		mr_coefBufferSize = samplesPerHalfCycle << 1;
		pthread_mutex_unlock(&m_mutex);

		mr_recalculate = true;

		mp_samplesPerHalfCycle = samplesPerHalfCycle;
		mp_topFrequency = topFrequency;
	}
}

void PhaserProcessor::setParameters_run()
{
	m_inputGain = mr_inputGain;
	m_sweepGain = mr_sweepGain;
	m_dryMix = mr_dryMix;
	if (mr_recalculate)
	{
		if (pthread_mutex_trylock(&m_mutex) != -EBUSY)
		{
			m_recalculate = true;
			m_expStep = mr_expStep;
			m_stepFrequency = mr_stepFrequency;
			m_resonance = mr_resonance;
			delete[] m_coefBuffer;
			if (mr_coefBuffer)
			{	//use lookupTable
				m_coefBuffer = m_coefBuffPrelim = m_coefBuffSweep = const_cast<float*>(mr_coefBuffer);
				mr_coefBuffer = 0;
				m_coefBuffEnd = m_coefBuffer + mr_coefBufferSize;
				m_useLookupTable = true;
			}
			else
			{
				m_coefBuffer = 0;
				m_phaseCounter = 0;
				m_halfPhaseSize = mr_coefBufferSize >> 1;
				m_stepFrequency_first = m_stepFrequency;
				m_useLookupTable = false;
			}

			pthread_mutex_unlock(&m_mutex);
		}
		m_x1 = m_x2 = m_y1 = m_y2 = 0.0f;
		m_sweepDirection = true;
	}
}

void PhaserProcessor::signalFlow_run(const int length)
{
	const float* inputBuffer = m_inputBuffer;
	float* outputBuffer = m_outputBuffer;

	if (m_useLookupTable)
	{
		for (int count = 0; count < length; count++)
		{
			// precalculate cs & alpha
			if (m_recalculate)
			{
				const float omega = 2.0f * float(M_PI) * m_stepFrequency / m_samplerate_d;
				float sn, cs;
				sincosf(omega, &sn, &cs);
				*(m_coefBuffPrelim++) = cs; // cs
				*(m_coefBuffPrelim++) = sn / m_resonance; // alpha (m_resonance = 2.0*resonance)
				m_stepFrequency *= m_expStep;
				if (m_coefBuffPrelim == m_coefBuffEnd)
				{
					m_recalculate=false;
				}
			}
			// coefficients
			register float cs, alpha;
			if (m_sweepDirection)
			{
				cs = *(m_coefBuffSweep++);
				alpha = *(m_coefBuffSweep++);
				if (m_coefBuffSweep == m_coefBuffEnd)
				{
					m_sweepDirection = !m_sweepDirection;
				}
			}
			else
			{
				alpha = *(--m_coefBuffSweep);
				cs = *(--m_coefBuffSweep);
				if (m_coefBuffSweep == m_coefBuffer)
				{
					m_sweepDirection = !m_sweepDirection;
				}
			}

			//b0 = alpha;
			//b1 = 0.0f;
			//b2 = -b0;
			//a0 = 1.0f + alpha;
			//a1 = (-2.0f*cs);
			//a2 = (1.0f-alpha);

			// perform filter
			const float input = *(inputBuffer++) * m_inputGain;
			const float output = ((alpha * input) + ((-alpha) * m_x2)
							   - (((-2.0f) * cs) * m_y1) - ((1.0f - alpha) * m_y2))
							   / (1.0f + alpha);
			m_x2 = m_x1;
			m_x1 = input;
			m_y2 = m_y1;
			m_y1 = output;

			*(outputBuffer++) = (output * m_sweepGain) + (input * m_dryMix);
		}
	}
	else
	{ //don't use lookupTable
		for (int count=0; count<length; count++)
		{
			const float omega = 2.0f * float(M_PI) * m_stepFrequency / m_samplerate_d;
			if(m_sweepDirection)
			{
				m_stepFrequency *= m_expStep;
			}
			else
			{
				m_stepFrequency /= m_expStep;
			}
			if ((++m_phaseCounter) == m_halfPhaseSize)
			{
				m_phaseCounter = 0;
				if (!m_sweepDirection)
				{
					m_stepFrequency = m_stepFrequency_first;
					//this protects against cumulating of a floating point error
				}
				m_sweepDirection = !m_sweepDirection;
			}

			float sn, cs;
			sincosf(omega, &sn, &cs);
			register const float alpha = sn / m_resonance; // alpha (m_resonance = 2.0 * resonance)

			// perform filter
			const float input = *(inputBuffer++) * m_inputGain;
			const float output = ((alpha * input) + ((-alpha) * m_x2)
							   - (((-2.0f) * cs) * m_y1) - ((1.0f - alpha) * m_y2))
							   / (1.0f + alpha);
			m_x2 = m_x1;
			m_x1 = input;
			m_y2 = m_y1;
			m_y1 = output;

			*(outputBuffer++) = (output*m_sweepGain) + (input * m_dryMix);
		}
	}
}

void PhaserProcessor::cleanup()
{
	if (mr_coefBuffer == m_coefBuffer)
	{
		delete[] mr_coefBuffer;
	}
	else
	{
		delete[] m_coefBuffer;
		delete[] mr_coefBuffer;
	}
	mr_coefBuffer = m_coefBuffer = 0;
}

void PhaserProcessor::prepareEnabled()
{
	m_x1 = m_x2 = m_y1 = m_y2 = 0.0f;
}

void PhaserProcessor::operator>>(CrInputDataStream& inputStream) const
{
	inputStream << *mo_parameters;
}

void PhaserProcessor::operator<<(CrOutputDataStream& outputStream)
{
	outputStream >> *mo_parameters;
}
