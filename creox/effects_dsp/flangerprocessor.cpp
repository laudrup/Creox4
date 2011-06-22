/***************************************************************************
					flangerprocessor.cpp  -  description
						 -------------------
		begin                : Sat Sep 9 2000
		copyright            : (C) 2000 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
                                     : laudrup@stacktrace.dk
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

#include <cstring>
#include <cmath>
#include <cerrno>
#include <iostream>
#include "crdatastream.h"
#include "flangerprocessor.h"

const float FlangerProcessor::s_maxBufferSize_d = 80.0f; //ms

FlangerProcessor::FlangerProcessor(FlangerParameters* parameters)
	: SoundProcessor(parameters)
{
	registerProcessor("flanger", 1.0f);

	mo_parameters = parameters;
	pthread_mutex_init(&m_mutex,NULL);
	m_delayBuffer = 0;
	m_lookupTab = 0;
	mr_lookupTab = 0;

}

FlangerProcessor::~FlangerProcessor()
{
	cleanup();
	pthread_mutex_destroy(&m_mutex);
#ifdef _DEBUG
	std::cerr << "FlangerProcessor deleted...\n";
#endif
}

void FlangerProcessor::initialize()
{
	mp_samplesPerCycle = -1;
	mp_modulationDepth_dd = -1;
	mp_waveform = -1;
	m_phaseCount = 0;
	m_delayBufferOffset = 0;
	m_delayBufferSize = roundToInt((s_maxBufferSize_d/1000.0f)*m_samplerate_d);
	m_delayBuffer = new float[m_delayBufferSize];

}

void FlangerProcessor::cleanup()
{
	delete[] m_delayBuffer;
	m_delayBuffer = 0;

	if (mr_lookupTab == m_lookupTab)
	{
		delete[] mr_lookupTab;
	}
	else
	{
		delete[] m_lookupTab;
		delete[] mr_lookupTab;
	}
	mr_lookupTab = m_lookupTab = 0;
}

void FlangerProcessor::setParameters_run()
{
	m_wet_f = mr_wet_f;
	m_dry_f = mr_dry_f;
	m_feedback_f = mr_feedback_f;
	m_inputGain_f = mr_inputGain_f;
	m_delay = mr_delay;

	if (mr_recalculate)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_mutex) != EBUSY)
		{
			m_recalculate = true;
			m_samplesPerCycle = mr_samplesPerCycle;
			m_waveform = mr_waveform;
			if (m_waveform == sine)
			{
				m_y_dd[0] = m_y_dd_first[0] = mr_y_dd[0];
				m_y_dd[1] = m_y_dd_first[1] = mr_y_dd[1];
				m_p_dd = mr_p_dd;
				m_modulationHalfDepth_dd = mr_modulationHalfDepth_dd;
			}
			else
			{ // triangle
				m_triangleCount_dd = 0;
				m_linearPhaseStep_dd = mr_linearPhaseStep_dd;
			}
			delete[] m_lookupTab;
			if (mr_lookupTab)
			{  //use lookupTable
				m_lookupTab = m_lookupTabPrelim = const_cast<float*>(mr_lookupTab);
				m_lookupTabHalfPrelim = m_lookupTabHalfPrelimStatic
									  = m_lookupTab + (m_samplesPerCycle>>1);
				m_lookupTabEndPrelim = m_lookupTab + m_samplesPerCycle - 1;
				mr_lookupTab = 0;
				m_phaseCount = 0;
				m_useLookupTable = true;
			}
			else
			{
				m_lookupTab = 0;
				m_useLookupTable = false;
				m_halfPeriodCount = 0;
				m_firstHalf = true;
				m_samplesPerHalfCycle =  m_samplesPerCycle>>1;
			}

			// mutex unlock
			pthread_mutex_unlock(&m_mutex);
		}
	}
}

void FlangerProcessor::computeParameters()
{
	mr_inputGain_f = exp10f(mo_parameters->inputGain/20.0f);
	const float tmpVal = 0.5f * mo_parameters->crossfade;
	mr_dry_f = (50.0f - tmpVal)/100.0f;
	mr_wet_f = (50.0f + tmpVal)/100.0f;
	mr_feedback_f = mo_parameters->feedback/100.0f;
	if (mo_parameters->inverted == true)
	{
		mr_wet_f *= -1.0f;
		mr_feedback_f *= -1.0f;
	}
	mr_delay = roundToInt((mo_parameters->delay/1000.0f) * m_samplerate_f);
	if (!mr_delay)
	{
		mr_delay++;
	}
	const int samplesPerCycle = ((roundToInt(m_samplerate_f/mo_parameters->frequency)>>1)<<1);
	const double modulationDepth_dd = static_cast<double>
										((mo_parameters->modulationDepth/1000.0f) * m_samplerate_f);

	mr_recalculate = false;
	if (samplesPerCycle != mp_samplesPerCycle ||
		modulationDepth_dd != mp_modulationDepth_dd ||
		mo_parameters->waveform != mp_waveform)
	{
		// make lookup table for short periods only  (88200 samples = 0.5Hz = 334.5Kb in 44100bit/sec)
		float* const lookupTab = (samplesPerCycle<88201) ? (new float[samplesPerCycle]) : 0;
#ifdef _DEBUG_FP
		if (!lookupTab) std::cerr << "no lookupTab!" << "\n";
#endif
		const double modulationHalfDepth_dd = modulationDepth_dd/2.0;
		const int samplesPerHalfCycle = samplesPerCycle>>1;

		// mutex lock
		pthread_mutex_lock(&m_mutex);

		if (mo_parameters->waveform == sine )
		{ //sine
			const double w = (2.0*M_PI)/static_cast<double>(samplesPerCycle);
			mr_y_dd[0] = std::sin(-2.0*w);
			mr_y_dd[1] = std::sin(-w);
			mr_p_dd = 2.0*std::cos(w);
			mr_modulationHalfDepth_dd = modulationHalfDepth_dd;
		}
		else
		{ //triangle
			mr_linearPhaseStep_dd = modulationDepth_dd/static_cast<double>(samplesPerHalfCycle);
		}
		mr_samplesPerCycle = mp_samplesPerCycle = samplesPerCycle;
		mr_waveform = mp_waveform = mo_parameters->waveform;
		delete[] mr_lookupTab;
		mr_lookupTab = lookupTab;

		// mutex unlock
		pthread_mutex_unlock(&m_mutex);

		mr_recalculate = true;
		mp_modulationDepth_dd = modulationDepth_dd;
	}
}

void FlangerProcessor::signalFlow_run(const int length)
{
	const float* inputBuffer = m_inputBuffer;
	float* outputBuffer = m_outputBuffer;

	if (m_useLookupTable)
	{
		for (int count=0; count<length; count++)
		{

			if (m_recalculate)
			{
				if (m_waveform == sine )
				{ //sine
					m_y_dd[2] = m_p_dd*m_y_dd[1] - m_y_dd[0];
					m_y_dd[0] = m_y_dd[1];
					m_y_dd[1] = m_y_dd[2];
					*(m_lookupTabPrelim++) = static_cast<float>
													(m_modulationHalfDepth_dd*(m_y_dd[2]+1.0));
					*(m_lookupTabHalfPrelim++) = static_cast<float>
													(m_modulationHalfDepth_dd*(-m_y_dd[2]+1.0));
				}
				else
				{ //triangle
					register const float value = static_cast<float>
													((m_triangleCount_dd++) * m_linearPhaseStep_dd);
					*(m_lookupTabPrelim++) = value;
					*(m_lookupTabEndPrelim--) = value;
				}

				if (m_lookupTabPrelim == m_lookupTabHalfPrelimStatic)
				{
					m_recalculate = false;
				}
			}

			register const float inputSample = *(inputBuffer++) * m_inputGain_f;
			register const float delayPhase_f = *(m_lookupTab + m_phaseCount);
			register const int delayOffset = static_cast<int>(delayPhase_f) + m_delayBufferOffset + m_delay;
			register const float flangeOut1 = *(m_delayBuffer + (delayOffset % m_delayBufferSize));
			register const float flangeOut2 = *(m_delayBuffer + ((delayOffset+1) % m_delayBufferSize));
			register const float linearIntrpMlt_f = delayPhase_f
												  - static_cast<float>
														(static_cast<int>(delayPhase_f));
																	//do you know better solution?
			register const float flangeOut = ((1.0f-linearIntrpMlt_f) * flangeOut1)
										   + (linearIntrpMlt_f * flangeOut2);
			*(m_delayBuffer+m_delayBufferOffset) = inputSample + (flangeOut * m_feedback_f);
			*(outputBuffer++) = (inputSample * m_dry_f) + (flangeOut * m_wet_f);
			if ((--m_delayBufferOffset) == -1) m_delayBufferOffset = m_delayBufferSize-1;
			if ((++m_phaseCount) == m_samplesPerCycle) m_phaseCount = 0;
		}
	}
	else
	{ //do not use lookupTab
		for (int count=0; count<length; count++)
		{
			float delayPhase_f;
			if (m_waveform == sine )
			{ //sine
				m_y_dd[2] = m_p_dd*m_y_dd[1] - m_y_dd[0];
				m_y_dd[0] = m_y_dd[1];
				m_y_dd[1] = m_y_dd[2];
				if (m_firstHalf)
				{
					delayPhase_f = static_cast<float>(m_modulationHalfDepth_dd*(m_y_dd[2]+1.0));
				}
				else
				{
					delayPhase_f = static_cast<float>(m_modulationHalfDepth_dd*(-m_y_dd[2]+1.0));
				}
			}
			else
			{ //triangle
				if (m_firstHalf)
				{
					delayPhase_f = static_cast<float>((m_triangleCount_dd++) * m_linearPhaseStep_dd);
				}
				else
				{
					delayPhase_f = static_cast<float>((m_triangleCount_dd--) * m_linearPhaseStep_dd);
				}
			}
			if ((++m_halfPeriodCount) == m_samplesPerHalfCycle)
			{
				m_halfPeriodCount = 0;
				m_firstHalf = !m_firstHalf;
				if (m_waveform == sine )
				{ //sine
					m_y_dd[0] = m_y_dd_first[0];
					m_y_dd[1] = m_y_dd_first[1];
				}
			}

			register const float inputSample = *(inputBuffer++) * m_inputGain_f;
			register const int delayOffset = static_cast<int>(delayPhase_f) + m_delayBufferOffset + m_delay;
			register const float flangeOut1 = *(m_delayBuffer + (delayOffset % m_delayBufferSize));
			register const float flangeOut2 = *(m_delayBuffer + ((delayOffset+1) % m_delayBufferSize));
			register const float linearIntrpMlt_f = delayPhase_f
												  - static_cast<float>
															(static_cast<int>(delayPhase_f));
																			//do you know better solution?
			register const float flangeOut = ((1.0f-linearIntrpMlt_f) * flangeOut1)
										   + (linearIntrpMlt_f * flangeOut2);
			*(m_delayBuffer+m_delayBufferOffset) = inputSample + (flangeOut * m_feedback_f);
			*(outputBuffer++) = (inputSample * m_dry_f) + (flangeOut * m_wet_f);
			if ((--m_delayBufferOffset) == -1) m_delayBufferOffset = m_delayBufferSize-1;
		}
	}
}

void FlangerProcessor::prepareEnabled()
{
	if (m_delayBuffer)
	{
		std::memset(m_delayBuffer, 0, sizeof(float)*m_delayBufferSize);
	}
}

void FlangerProcessor::operator>>(CrInputDataStream& inputStream) const
{
	inputStream << *mo_parameters;
}

void FlangerProcessor::operator<<(CrOutputDataStream& outputStream)
{
	outputStream >> *mo_parameters;
}
