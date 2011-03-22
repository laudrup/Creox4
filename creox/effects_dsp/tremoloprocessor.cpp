/***************************************************************************
						tremoloprocessor.cpp  -  description
								 -------------------
		begin                : Sat Sep 23 2000
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

#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif
#include "control.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include "crdatastream.h"
#include "tremoloprocessor.h"

TremoloProcessor::TremoloProcessor(TremoloParameters* parameters)
 : SoundProcessor(parameters), mo_parameters(parameters)
{
	registerProcessor("tremolo", 1.0f);

	pthread_mutex_init(&m_mutex,NULL);
}

TremoloProcessor::~TremoloProcessor()
{
	//cleanup();
	pthread_mutex_destroy(&m_mutex);
#ifdef _DEBUG
	std::cerr << "TremoloProcessor deleted...\n";
#endif
}

void TremoloProcessor::initialize()
{

}

void TremoloProcessor::computeParameters()
{
	mr_inputGain_f = exp10f(mo_parameters->inputGain/20.0f);
	mr_dry_f = mo_parameters->dryMix/100.0f;
	mr_wet_f = mo_parameters->wetMix/100.0f;
	mr_modulationDepth = exp10f(-mo_parameters->depth/20.0f);
	const int samplesPerCycle = ((roundToInt(m_samplerate_f/mo_parameters->frequency)>>1)<<1);

	if (samplesPerCycle != mp_samplesPerCycle || mo_parameters->waveform != mp_waveform)
	{
		mp_samplesPerCycle = samplesPerCycle;
		// mutex lock
		pthread_mutex_lock(&m_mutex);

		mr_waveform = mp_waveform = mo_parameters->waveform;
		mr_samplesPerHalfCycle = samplesPerCycle>>1;

		if (mp_waveform == sine )
		{ //sine
			const double w = (2.0*M_PI)/static_cast<double>(samplesPerCycle);
			mr_y_d[0] = std::sin(-2.0*w);
			mr_y_d[1] = std::sin(-w);
			mr_p_d = 2.0*std::cos(w);
		}
		else
		{ //triangle
			mr_linearPhaseStep_d = 1.0/static_cast<double>(samplesPerCycle>>1);
		}

		mr_recalculate = true;
		// mutex unlock
		pthread_mutex_unlock(&m_mutex);

	}
	else
	{
		mr_recalculate = false;
	}
}

void TremoloProcessor::setParameters_run()
{
	m_wet_f = mr_wet_f;
	m_dry_f = mr_dry_f;
	m_inputGain_f = mr_inputGain_f;

	if (mr_recalculate)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_mutex) != EBUSY)
		{
			m_waveform = mr_waveform;

			if (m_waveform == sine)
			{
				m_y_d[0] = m_y_d_first[0] = mr_y_d[0];
				m_y_d[1] = m_y_d_first[1] = mr_y_d[1];
				m_p_d = mr_p_d;
			}
			else
			{ // triangle
				m_triangleCount_d = 0;
				m_linearPhaseStep_d = mr_linearPhaseStep_d;
			}

			m_halfPeriodCount = 0;
			m_firstHalf = true;
			m_samplesPerHalfCycle = mr_samplesPerHalfCycle;

			// mutex unlock
			pthread_mutex_unlock(&m_mutex);
		}
	}

	const float tmpDepth = mr_modulationDepth;
	if (m_waveform == sine)
	{
		m_modulationMulti = (1.0 - tmpDepth)/2.0;
		m_modulationShift = m_modulationMulti + tmpDepth;
	}
	else
	{
		m_modulationMulti = 1.0 - tmpDepth;
		m_modulationShift = tmpDepth;
	}

}

void TremoloProcessor::signalFlow_run(const int length)
{
	const float* inputBuffer = m_inputBuffer;
	float* outputBuffer = m_outputBuffer;

	for (int count=0; count<length; count++)
	{
		float amplitudeModulation;
		if (m_waveform == sine )
		{ //sine
			m_y_d[2] = m_p_d*m_y_d[1] - m_y_d[0];
			m_y_d[0] = m_y_d[1];
			m_y_d[1] = m_y_d[2];
			if (m_firstHalf)
			{
				amplitudeModulation = static_cast<float>(m_y_d[2]+1.0);
			}
			else
			{
				amplitudeModulation = static_cast<float>(-m_y_d[2]+1.0);
			}
		}
		else
		{ //triangle
			if (m_firstHalf)
			{
				amplitudeModulation = static_cast<float>((m_triangleCount_d++) * m_linearPhaseStep_d);
			}
			else
			{
				amplitudeModulation = static_cast<float>((m_triangleCount_d--) * m_linearPhaseStep_d);
			}
		}
		if ((++m_halfPeriodCount) == m_samplesPerHalfCycle)
		{
			m_halfPeriodCount = 0;
			m_firstHalf = !m_firstHalf;
			if (m_waveform == sine )
			{ //sine
				m_y_d[0] = m_y_d_first[0];
				m_y_d[1] = m_y_d_first[1];
			}
		}

		register const float inputSample = *(inputBuffer++) * m_inputGain_f;
#ifdef FP_FAST_FMAF
		*(outputBuffer++) = (inputSample * m_dry_f)
						  + (inputSample * m_wet_f * std::fmaf(amplitudeModulation,
															   m_modulationMulti, m_modulationShift);
#else
		*(outputBuffer++) = (inputSample * m_dry_f)
						  + (inputSample * m_wet_f * ((amplitudeModulation * m_modulationMulti)
						  + m_modulationShift));
#endif
	}
}

void TremoloProcessor::cleanup()
{
}

void TremoloProcessor::prepareEnabled()
{

}

void TremoloProcessor::operator>>(CrInputDataStream& inputStream) const
{
	inputStream << *mo_parameters;
}

void TremoloProcessor::operator<<(CrOutputDataStream& outputStream)
{
	outputStream >> *mo_parameters;
}
