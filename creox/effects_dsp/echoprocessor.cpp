/***************************************************************************
					echoprocessor.cpp  -  description
						 -------------------
		begin                : Wed Aug 16 2000
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
#include <cerrno>
#include <cstring>
#include <iostream>
#include "crdatastream.h"
#include "echoprocessor.h"

const float EchoProcessor::s_maxBufferSize_d = 1000.0f; //ms

EchoProcessor::EchoProcessor(EchoParameters* parameters)
	: SoundProcessor(parameters)
{
	registerProcessor("echo", 1.0f);

	mo_parameters = parameters;
	pthread_mutex_init(&m_mutex, NULL);

	for (int count=0; count<10; count++)
	{
		mr_parallelEchoChain[count].buffer = 0;
		mw_parallelEchoChain[count].buffer = 0;
		m_parallelEchoChain[count].buffer = 0;
	}
	m_finalEchoBuffer = 0;
}

EchoProcessor::~EchoProcessor()
{
	cleanup();
	pthread_mutex_destroy(&m_mutex);
#ifdef _DEBUG
	std::cerr << "EchoProcessor deleted...\n";
#endif
}

void EchoProcessor::initialize()
{
	m_delayBufferSize = roundToInt((s_maxBufferSize_d/1000.0f)*m_samplerate_d);
	m_finalEchoBufferOffset = 0;
	m_numOfParallelEchoes = 0;
	mr_numOfParallelEchoes = 0;
	for (int count=0; count<10; count++)
	{
		m_parallelEchoChain[count].offset = 0;

		mr_parallelEchoParam[count].delay = -1;
		m_parallelEchoParam[count].delay = -1;
		mr_parallelEchoParam[count].decay_f = -1;
		m_parallelEchoParam[count].decay_f = -1;
		mr_parallelEchoParam[count].feedback_f = -1;
		m_parallelEchoParam[count].feedback_f = -1;
	}
	m_finalEchoBuffer = new float[m_delayBufferSize];
}

void EchoProcessor::prepareEnabled()
{
	for (int count = 0; count < mr_numOfParallelEchoes; count++)
	{
		if(mr_parallelEchoChain[count].buffer){
			(void)std::memset(mr_parallelEchoChain[count].buffer, 0, sizeof(float)*m_delayBufferSize);
		}
	}
	if(m_finalEchoBuffer){
		(void)std::memset(m_finalEchoBuffer, 0, sizeof(float)*m_delayBufferSize);
	}
}

void EchoProcessor::computeParameters()
{
	mr_inputGain_f = exp10f(mo_parameters->inputGain/20.0f);
	mr_dry_f = mo_parameters->dryMix/100.0f;
	mr_wet_f = mo_parameters->wetMix/100.0f;
	mr_finalFeedback_f = mo_parameters->finalFeedback/100.0f;

	mr_finalDelay = roundToInt((mo_parameters->finalDelay/1000.0f)*m_samplerate_d);
	if(!mr_finalDelay){
		mr_finalDelay++;
	}

	// mutex lock
	pthread_mutex_lock(&m_mutex);
	const int prevEchoesCount = mr_numOfParallelEchoes;
	mr_numOfParallelEchoes = mo_parameters->parallelEchoCount;
	assert(mr_numOfParallelEchoes<11 && mr_numOfParallelEchoes>-1);

	const int echoCount = mr_numOfParallelEchoes;
	if (prevEchoesCount < echoCount)
	{
		for (int count=prevEchoesCount; count<echoCount; count++)
		{
			if (!mr_parallelEchoChain[count].buffer)
			{
				mr_parallelEchoChain[count].buffer = new float[m_delayBufferSize];
			}
			(void)std::memset(mr_parallelEchoChain[count].buffer, 0, sizeof(float)*m_delayBufferSize);
		}
	}
	else if (prevEchoesCount > echoCount)
	{
		for (int count=echoCount; count<prevEchoesCount; count++)
		{
			if (mr_parallelEchoChain[count].buffer != mw_parallelEchoChain[count].buffer)
			{
				delete[] mr_parallelEchoChain[count].buffer;
			}
			mr_parallelEchoChain[count].buffer = 0;
		}
	}
	for (int count=0; count<echoCount; count++)
	{
		mr_parallelEchoParam[count].delay = roundToInt((mo_parameters->parallelEcho[count].delay/1000.0f)
										  * m_samplerate_d);
		mr_parallelEchoParam[count].decay_f = mo_parameters->parallelEcho[count].decay/100.0f;
		mr_parallelEchoParam[count].feedback_f = mo_parameters->parallelEcho[count].feedback/100.0f;
	}
	pthread_mutex_unlock(&m_mutex);
	// mutex unlock
}

void EchoProcessor::setParameters_run()
{
	m_wet_f = mr_wet_f;
	m_dry_f = mr_dry_f;
	m_inputGain_f = mr_inputGain_f;
	m_finalFeedback_f = mr_finalFeedback_f;
	m_finalDelay = mr_finalDelay;

	int checkCount = 0;
	// mutex lock
	if (pthread_mutex_trylock(&m_mutex) != EBUSY)
	{
		for (int count=0; count<10; count++)
		{
			if (m_parallelEchoChain[count].buffer != mr_parallelEchoChain[count].buffer)
			{
				delete[] m_parallelEchoChain[count].buffer;
				m_parallelEchoChain[count].buffer = mw_parallelEchoChain[count].buffer
												  = mr_parallelEchoChain[count].buffer;
			}
		}
		checkCount = (mr_numOfParallelEchoes < m_numOfParallelEchoes)
				   ? mr_numOfParallelEchoes
				   : m_numOfParallelEchoes;
		m_numOfParallelEchoes = mr_numOfParallelEchoes;
		pthread_mutex_unlock(&m_mutex);
		// mutex unlock
	}

	for (int count=0; count<checkCount; count++)
	{
		if (std::memcmp(&m_parallelEchoParam[count],
						reinterpret_cast<void*>(const_cast<ParallelEchoParam*>
													(&mr_parallelEchoParam[count])),
						sizeof(ParallelEchoParam))){
			std::memset(m_parallelEchoChain[count].buffer, 0, sizeof(float)*m_delayBufferSize);
		}
	}
	std::memcpy(&m_parallelEchoParam,
				reinterpret_cast<void*>(const_cast<ParallelEchoParam(*)[10]>(&mr_parallelEchoParam)),
				sizeof(ParallelEchoParam)*10);
}

void EchoProcessor::cleanup()
{
	delete[] m_finalEchoBuffer;
	m_finalEchoBuffer = 0;

	for (int count=0; count<10; count++)
	{
		if (mr_parallelEchoChain[count].buffer==mw_parallelEchoChain[count].buffer)
		{
			delete[] mr_parallelEchoChain[count].buffer;
		}
		else
		{
			delete[] mr_parallelEchoChain[count].buffer;
			delete[] mw_parallelEchoChain[count].buffer;
		}
		mr_parallelEchoChain[count].buffer = m_parallelEchoChain[count].buffer
										   = mw_parallelEchoChain[count].buffer = 0;
	}
}

void EchoProcessor::signalFlow_run(const int length)
{
	const float* inputBuffer = m_inputBuffer;
	float* outputBuffer = m_outputBuffer;

	for (int count=0; count<length; count++)
	{
		register const float inputSample = *(inputBuffer++) * m_inputGain_f;

		register float parallelEchoes = 0.0f;
		for (int count=0; count<m_numOfParallelEchoes; count++)
		{
			register const float parEcho = *(m_parallelEchoChain[count].buffer
										 + ((m_parallelEchoParam[count].delay
										 + m_parallelEchoChain[count].offset) % m_delayBufferSize));
			parallelEchoes += parEcho * m_parallelEchoParam[count].decay_f;
#ifdef FP_FAST_FMAF
			*(m_parallelEchoChain[count].buffer + m_parallelEchoChain[count].offset) =
								std::fmaf(parEcho, m_parallelEchoParam[count].feedback_f, inputSample);
#else
			*(m_parallelEchoChain[count].buffer + m_parallelEchoChain[count].offset) =
									inputSample + (parEcho * m_parallelEchoParam[count].feedback_f);
#endif
			if ((--m_parallelEchoChain[count].offset) == -1)
			{
				m_parallelEchoChain[count].offset = m_delayBufferSize - 1;
			}
		}

		register const float finalEcho = parallelEchoes + *(m_finalEchoBuffer
									   + ((m_finalDelay + m_finalEchoBufferOffset) % m_delayBufferSize));
		*(m_finalEchoBuffer + m_finalEchoBufferOffset) = (inputSample + finalEcho) * m_finalFeedback_f;
		*(outputBuffer++) = (finalEcho * m_wet_f) + (inputSample * m_dry_f);

		if ((--m_finalEchoBufferOffset) == -1)
		{
			m_finalEchoBufferOffset = m_delayBufferSize -1;
		}

	}
}

void EchoProcessor::operator>>(CrInputDataStream& inputStream) const
{
	inputStream << *mo_parameters;
}

void EchoProcessor::operator<<(CrOutputDataStream& outputStream)
{
	outputStream >> *mo_parameters;
}
