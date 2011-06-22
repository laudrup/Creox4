/***************************************************************************
					distortionprocessor.cpp  -  description
						 -------------------
		begin                : Thu Aug 31 2000
		copyright            : (C) 2000 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

/* to do: peakingEQ for output signal
 */

#ifndef _GNU_SOURCE
 #define _GNU_SOURCE
#endif
#include "control.h"
#include <cmath>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include "crdatastream.h"
#include "distortionprocessor.h"

const DistortionProcessor::FilterValues DistortionProcessor::s_zeroValues = { 0.0f, 0.0f, 0.0f, 0.0f };

DistortionProcessor::DistortionProcessor(DistortionParameters* parameters)
	: SoundProcessor(parameters), mo_parameters(parameters)
{
	registerProcessor("distortion", 1.0f);

	pthread_mutex_init(&m_lowPass_mutex, NULL);
	pthread_mutex_init(&m_accent_mutex, NULL);
	pthread_mutex_init(&m_inBass_mutex,NULL);
	pthread_mutex_init(&m_outBass_mutex,NULL);

	for (int count=0; count<NUM_OF_DISTORT_FUNCTIONS; count++)
	{
		m_distortFunction[count] = new float[MAX_DISTORT_VALUE+1];
	}

	//compute distort functions
	makeDistortFunction_0(m_distortFunction[0]); //Flat (no distortion)
	makeDistortFunction_1(m_distortFunction[1]); //ArcSinh
	makeDistortFunction_2(m_distortFunction[2]); //ArcTan
	makeDistortFunction_3(m_distortFunction[3]); //Tanh
}

void DistortionProcessor::makeDistortFunction_0(float* array)
{
	//Flat
	for (float x=0.0f; x<float(MAX_DISTORT_VALUE+1); x++)
	{
		*(array++) = x;
	}
}

void DistortionProcessor::makeDistortFunction_1(float* array)
{
	//ArcSinh
	static const double Q = 0.001;
	static const double lastValue = asinh(double(MAX_DISTORT_VALUE)*Q);
	for (double x=0.0; x<double(MAX_DISTORT_VALUE+1); x++)
	{
		*(array++) = static_cast<float>(10240.0*asinh(Q*x)/lastValue);
	}
}

void DistortionProcessor::makeDistortFunction_2(float* array)
{
	//ArcTan
	static const double Q = 0.001;
	static const double lastValue = std::atan(double(MAX_DISTORT_VALUE)*Q);
	for (double x=0.0; x<double(MAX_DISTORT_VALUE+1); x++)
	{
		*(array++) = static_cast<float>(10240.0*std::atan(Q*x)/lastValue);
	}
}

void DistortionProcessor::makeDistortFunction_3(float* array)
{
	//Tanh
	static const double Q = 0.001;
	static const double lastValue = std::tanh(double(MAX_DISTORT_VALUE)*Q);
	for (double x=0.0; x<double(MAX_DISTORT_VALUE+1); x++)
	{
		*(array++) = static_cast<float>(10240.0*std::tanh(Q*x)/lastValue);
	}
}

DistortionProcessor::~DistortionProcessor()
{
	//cleanup();
	for (int count=0; count<NUM_OF_DISTORT_FUNCTIONS; count++)
	{
		delete[] m_distortFunction[count];
	}

	pthread_mutex_destroy(&m_inBass_mutex);
	pthread_mutex_destroy(&m_outBass_mutex);
	pthread_mutex_destroy(&m_accent_mutex);
	pthread_mutex_destroy(&m_lowPass_mutex);
#ifdef _DEBUG
	std::cerr << "DistortionProcessor deleted...\n";
#endif
}

void DistortionProcessor::initialize()
{
	mp_inBass_frequency = mp_inBass_gain = mp_inBass_slope = -9999.0f;
	mp_outBass_frequency = mp_outBass_gain = mp_outBass_slope = -9999.0f;
	mp_accent_frequency = mp_accent_bandwidth = -9999.0f;
	mp_lowPass_cutoffFrequency = mp_lowPass_Q = -9999.0f;
}

void DistortionProcessor::computeParameters()
{
	mr_inputGain = exp10f(mo_parameters->inputGain/20.0f);
	mr_wetMix = mo_parameters->wetMix/100.0f;
	mr_dryMix = mo_parameters->dryMix/100.0f;

	//distort function
	assert(mo_parameters->accent_distortFunction > -1 &&
		   mo_parameters->accent_distortFunction < NUM_OF_DISTORT_FUNCTIONS);
	mr_accent_currentDistortFunction = m_distortFunction[mo_parameters->accent_distortFunction];
	assert(mo_parameters->main_distortFunction > -1 &&
		   mo_parameters->main_distortFunction < NUM_OF_DISTORT_FUNCTIONS);
	mr_main_currentDistortFunction = m_distortFunction[mo_parameters->main_distortFunction];

	//mainDrive
	mr_mainDrive_in = exp10f(mo_parameters->main_drive/20.0f);
	mr_mainDrive_out = exp10f((mo_parameters->main_gain - std::sqrt(mo_parameters->main_drive)*2.0f)/20.0f);

	//accented frequency (BPF)
	mr_accent_drive_in = exp10f(mo_parameters->accent_drive/20.0f);
	mr_accent_drive_out = exp10f((mo_parameters->accent_gain -
								  sqrtf(mo_parameters->accent_drive)*2.0f)/20.0f);
	if (mp_accent_frequency != mo_parameters->accent_frequency ||
		mp_accent_bandwidth != mo_parameters->accent_bandwidth)
	{
		mp_accent_frequency = mo_parameters->accent_frequency;
		mp_accent_bandwidth = mo_parameters->accent_bandwidth;
		computeBPFCoefficients(&mr_accentIIRCoefficients,
							   mp_accent_frequency,
							   mp_accent_bandwidth,
							   &m_accent_mutex);
		mr_accent_changed = true;
	}
	else
	{
		mr_accent_changed = false;
	}

	//input bass cut/boost
	if (mp_inBass_frequency != mo_parameters->inBass_frequency ||
		mp_inBass_gain != mo_parameters->inBass_gain ||
		mp_inBass_slope != mo_parameters->inBass_slope)
	{

		mp_inBass_frequency = mo_parameters->inBass_frequency;
		mp_inBass_gain = mo_parameters->inBass_gain;
		mp_inBass_slope = mo_parameters->inBass_slope;
		computeLowShelfCoefficients(&mr_inBassIIRCoefficients,
									mp_inBass_frequency,
									mp_inBass_gain,
									mp_inBass_slope,
									&m_inBass_mutex);
		mr_inBass_changed = true;
	}
	else
	{
		mr_inBass_changed = false;
	}

	//output bass boost/cut
	if (mp_outBass_frequency != mo_parameters->outBass_frequency ||
		mp_outBass_gain != mo_parameters->outBass_gain ||
		mp_outBass_slope != mo_parameters->outBass_slope)
	{

		mp_outBass_frequency = mo_parameters->outBass_frequency;
		mp_outBass_gain = mo_parameters->outBass_gain;
		mp_outBass_slope = mo_parameters->outBass_slope;
		computeLowShelfCoefficients(&mr_outBassIIRCoefficients,
									mp_outBass_frequency,
									mp_outBass_gain,
									mp_outBass_slope,
									&m_outBass_mutex);
		mr_outBass_changed = true;
	}
	else
	{
		mr_outBass_changed = false;
	}

	//low pass
	if (mp_lowPass_cutoffFrequency != mo_parameters->lowPass_cutoffFrequency ||
		mp_lowPass_Q != mo_parameters->lowPass_Q)
	{
		mp_lowPass_cutoffFrequency = mo_parameters->lowPass_cutoffFrequency;
		mp_lowPass_Q = mo_parameters->lowPass_Q;
		computeLowPassCoefficients(&mr_lowPassIIRCoefficients,
								   mp_lowPass_cutoffFrequency,
								   mp_lowPass_Q,
								   &m_lowPass_mutex);
		mr_lowPass_changed = true;
	}
	else
	{
		mr_lowPass_changed = false;
	}
}

void DistortionProcessor::setParameters_run()
{
	m_inputGain = mr_inputGain;
	m_wetMix = mr_wetMix;
	m_dryMix = mr_dryMix;

	//distort function
	m_accent_currentDistortFunction = const_cast<float*>(mr_accent_currentDistortFunction);
	m_main_currentDistortFunction = const_cast<float*>(mr_main_currentDistortFunction);

	//mainDrive
	m_mainDrive_in = mr_mainDrive_in;
	m_mainDrive_out = mr_mainDrive_out;

	//accent frequency (BPF)
	m_accent_drive_in = mr_accent_drive_in;
	m_accent_drive_out = mr_accent_drive_out;
	if (mr_accent_changed)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_accent_mutex) != EBUSY)
		{
			m_accentIIRCoefficients.b0 = mr_accentIIRCoefficients.b0;
			m_accentIIRCoefficients.b1 = mr_accentIIRCoefficients.b1;
			m_accentIIRCoefficients.b2 = mr_accentIIRCoefficients.b2;
			m_accentIIRCoefficients.a1 = mr_accentIIRCoefficients.a1;
			m_accentIIRCoefficients.a2 = mr_accentIIRCoefficients.a2;
			// mutex unlock
			pthread_mutex_unlock(&m_accent_mutex);
		}
		m_accentFilterValues = m_mainFilterValues = s_zeroValues;
	}

	//input bass boost/cut (lowShelf)
	if (mr_inBass_changed)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_inBass_mutex) != EBUSY)
		{
			m_inBassIIRCoefficients.b0 = mr_inBassIIRCoefficients.b0;
			m_inBassIIRCoefficients.b1 = mr_inBassIIRCoefficients.b1;
			m_inBassIIRCoefficients.b2 = mr_inBassIIRCoefficients.b2;
			m_inBassIIRCoefficients.a1 = mr_inBassIIRCoefficients.a1;
			m_inBassIIRCoefficients.a2 = mr_inBassIIRCoefficients.a2;
			// mutex unlock
			pthread_mutex_unlock(&m_inBass_mutex);
		}
		m_inBassFilterValues = s_zeroValues;
	}

	//output bass boost/cut (lowShelf)
	if (mr_outBass_changed)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_outBass_mutex) != EBUSY)
		{
			m_outBassIIRCoefficients.b0 = mr_outBassIIRCoefficients.b0;
			m_outBassIIRCoefficients.b1 = mr_outBassIIRCoefficients.b1;
			m_outBassIIRCoefficients.b2 = mr_outBassIIRCoefficients.b2;
			m_outBassIIRCoefficients.a1 = mr_outBassIIRCoefficients.a1;
			m_outBassIIRCoefficients.a2 = mr_outBassIIRCoefficients.a2;
			// mutex unlock
			pthread_mutex_unlock(&m_outBass_mutex);
		}
		m_outBassFilterValues = s_zeroValues;
	}

	//low pass
	if (mr_lowPass_changed)
	{
		// mutex lock
		if (pthread_mutex_trylock(&m_lowPass_mutex) != EBUSY)
		{
			m_lowPassIIRCoefficients.b0 = mr_lowPassIIRCoefficients.b0;
			m_lowPassIIRCoefficients.b1 = mr_lowPassIIRCoefficients.b1;
			 //m_lowPassIIRCoefficients.b2 = mr_lowPassIIRCoefficients.b2;
			m_lowPassIIRCoefficients.a1 = mr_lowPassIIRCoefficients.a1;
			m_lowPassIIRCoefficients.a2 = mr_lowPassIIRCoefficients.a2;
			// mutex unlock
			pthread_mutex_unlock(&m_lowPass_mutex);
		}
		m_lowPassFilterValues = s_zeroValues;
	}
}

void DistortionProcessor::signalFlow_run(const int length)
{
	const float* inputBuffer = m_inputBuffer;
	float* outputBuffer = m_outputBuffer;

	for (int count=0; count<length; count++)
	{

		float inputSample =  *(inputBuffer++) * m_inputGain;

		//input bass cut/boost (lowShelf)
		const float inBassOutput = (m_inBassIIRCoefficients.b0 * inputSample) +
								   (m_inBassIIRCoefficients.b1 * m_inBassFilterValues.x1) +
								   (m_inBassIIRCoefficients.b2 * m_inBassFilterValues.x2) -
								   (m_inBassIIRCoefficients.a1 * m_inBassFilterValues.y1) -
								   (m_inBassIIRCoefficients.a2 * m_inBassFilterValues.y2);
		m_inBassFilterValues.x2 = m_inBassFilterValues.x1;
		m_inBassFilterValues.x1 = inputSample;
		m_inBassFilterValues.y2 = m_inBassFilterValues.y1;
		m_inBassFilterValues.y1 = inBassOutput;

		//distortion
		float distortOutput;
		//accented frequency (BPF)
		{
			const float accentOutput = (m_accentIIRCoefficients.b0 * inBassOutput) +
									   (m_accentIIRCoefficients.b2 * m_accentFilterValues.x2) -
									   (m_accentIIRCoefficients.a1 * m_accentFilterValues.y1) -
									   (m_accentIIRCoefficients.a2 * m_accentFilterValues.y2);
			m_accentFilterValues.x2 = m_accentFilterValues.x1;
			m_accentFilterValues.x1 = inBassOutput;
			m_accentFilterValues.y2 = m_accentFilterValues.y1;
			m_accentFilterValues.y1 = accentOutput;

			int inputSignal = static_cast<int>(accentOutput*m_accent_drive_in);
			if (inputSignal > MAX_DISTORT_VALUE)
			{
				inputSignal = MAX_DISTORT_VALUE;
			}
			else if (inputSignal < (-MAX_DISTORT_VALUE))
			{
				inputSignal = -MAX_DISTORT_VALUE;
			}
			//!! interpolation !!
			distortOutput = ((inputSignal>0) ? *(m_accent_currentDistortFunction + inputSignal)
											 : -(*(m_accent_currentDistortFunction - inputSignal))) *
																				  m_accent_drive_out;
		}
		// the rest of the spectrum (notch)
		{
			const float mainOutput = (m_accentIIRCoefficients.b1 * inBassOutput) +
									 (m_accentIIRCoefficients.a1 * m_mainFilterValues.x1) +
									 (m_accentIIRCoefficients.b1 * m_mainFilterValues.x2) -
									 (m_accentIIRCoefficients.a1 * m_mainFilterValues.y1) -
									 (m_accentIIRCoefficients.a2 * m_mainFilterValues.y2);
			m_mainFilterValues.x2 = m_mainFilterValues.x1;
			m_mainFilterValues.x1 = inBassOutput;
			m_mainFilterValues.y2 = m_mainFilterValues.y1;
			m_mainFilterValues.y1 = mainOutput;

			int inputSignal = static_cast<int>(mainOutput*m_mainDrive_in);
			if (inputSignal > MAX_DISTORT_VALUE)
			{
				inputSignal = MAX_DISTORT_VALUE;
			}
			else if (inputSignal < (-MAX_DISTORT_VALUE))
			{
				inputSignal = -MAX_DISTORT_VALUE;
			}
			distortOutput += ((inputSignal>0) ? *(m_main_currentDistortFunction + inputSignal)
											  : -(*(m_main_currentDistortFunction - inputSignal))) *
																					m_mainDrive_out;
			//if(mainOutput > inBassOutput){
			//  std::cerr << "Clip " << mainOutput << " | " << inBassOutput << "\n";
			//}
		}

		//low pass
		const float lowPassOutput = (m_lowPassIIRCoefficients.b0 * distortOutput) +
									(m_lowPassIIRCoefficients.b1 * m_lowPassFilterValues.x1) +
									(m_lowPassIIRCoefficients.b0 * m_lowPassFilterValues.x2) -
									(m_lowPassIIRCoefficients.a1 * m_lowPassFilterValues.y1) -
									(m_lowPassIIRCoefficients.a2 * m_lowPassFilterValues.y2);
		m_lowPassFilterValues.x2 = m_lowPassFilterValues.x1;
		m_lowPassFilterValues.x1 = distortOutput;
		m_lowPassFilterValues.y2 = m_lowPassFilterValues.y1;
		m_lowPassFilterValues.y1 = lowPassOutput;

		//output bass boost/cut (lowShelf)
		const float outBassOutput = (m_outBassIIRCoefficients.b0 * lowPassOutput) +
									(m_outBassIIRCoefficients.b1 * m_outBassFilterValues.x1) +
									(m_outBassIIRCoefficients.b2 * m_outBassFilterValues.x2) -
									(m_outBassIIRCoefficients.a1 * m_outBassFilterValues.y1) -
									(m_outBassIIRCoefficients.a2 * m_outBassFilterValues.y2);
		m_outBassFilterValues.x2 = m_outBassFilterValues.x1;
		m_outBassFilterValues.x1 = lowPassOutput;
		m_outBassFilterValues.y2 = m_outBassFilterValues.y1;
		m_outBassFilterValues.y1 = outBassOutput;

		*(outputBuffer++) = (outBassOutput * m_wetMix) + (inputSample * m_dryMix);
	}
}

void DistortionProcessor::cleanup()
{

}

void DistortionProcessor::prepareEnabled()
{
	m_inBassFilterValues = m_outBassFilterValues = m_accentFilterValues =
						   m_mainFilterValues = m_lowPassFilterValues = s_zeroValues;
}

void DistortionProcessor::computeBPFCoefficients(volatile IIRCoefficients* const coeffs,
												 const float centerFrequency,
												 const float bandwidth,
												 pthread_mutex_t* mutex) const
{
	const float omega = 2.0f*float(M_PI)*centerFrequency/m_samplerate_f;
	float sn,cs;
	sincosf(omega, &sn, &cs);
	const float alpha = sn*sinhf(float(M_LN2/2.0)*bandwidth*(omega/sn));
	const float a0 = 1.0f/(1.0f + alpha);   //a0 = 1/a0

	//BPF
	const float b0_bpf = alpha * a0;
	 //const float b1_bpf = 0.0f;
	const float b2_bpf = -b0_bpf;
	const float a1_bpf = (-2.0f * cs) * a0;
	const float a2_bpf = (1.0f - alpha) * a0;

	//notch
	 //const float b0_notch = a0;
	 //const float b1_notch = a1_bpf;
	 //const float b2_notch = b0_notch;
	 //const float a1_notch = a1_bpf;
	 //const float a2_notch = a2_bpf;


	// mutex lock
	pthread_mutex_lock(mutex);
	coeffs->b0 = b0_bpf;
	coeffs->b1 = a0;  //<-- this is b0_notch! (b1=0 in BPF, so we can save a space)
	coeffs->b2 = b2_bpf;
	coeffs->a1 = a1_bpf;
	coeffs->a2 = a2_bpf;
	// mutex unlock
	pthread_mutex_unlock(mutex);
}

void DistortionProcessor::computeLowShelfCoefficients(volatile IIRCoefficients* const coeffs,
													  const float frequency,
													  const float gain,
													  const float slope,
													  pthread_mutex_t* mutex) const
{
	const float A = exp10f(gain/40.0f);
	const float omega = 2.0f*float(M_PI)*frequency/m_samplerate_f;
	float sn,cs;
	sincosf(omega, &sn, &cs);
	register const float beta = sn * sqrtf( (A*A + 1.0f)/slope - powf(A-1.0f, 2.0f) );

	register const float AplusOne  = A + 1.0f;
	register const float AminusOne = A - 1.0f;
	register const float ApCs = AplusOne * cs;
	register const float AmCs = AminusOne * cs;

	const float b0 =      A*( AplusOne  - AmCs + beta );
	const float b1 = 2.0f*A*( AminusOne - ApCs        );
	const float b2 =      A*( AplusOne  - AmCs - beta );
	const float a0 =   1.0f/( AplusOne  + AmCs + beta );
	const float a1 =  -2.0f*( AminusOne + ApCs        );
	const float a2 =          AplusOne  + AmCs - beta  ;

	const IIRCoefficients tmpCoeff = { b0*a0,
									   b1*a0,
									   b2*a0,
									   a1*a0,
									   a2*a0 };
	// mutex lock
	(void)pthread_mutex_lock(mutex);
	coeffs->b0 = tmpCoeff.b0;
	coeffs->b1 = tmpCoeff.b1;
	coeffs->b2 = tmpCoeff.b2;
	coeffs->a1 = tmpCoeff.a1;
	coeffs->a2 = tmpCoeff.a2;
	// mutex unlock
	(void)pthread_mutex_unlock(mutex);
}

void DistortionProcessor::computeLowPassCoefficients(volatile IIRCoefficients* const coeffs,
													 const float cutoffFrequency,
													 const float Q,
													 pthread_mutex_t* mutex) const
{
	const float omega = 2.0f*float(M_PI)*cutoffFrequency/m_samplerate_f;
	float sn,cs;
	sincosf(omega, &sn, &cs);
	const float alpha = sn/(2.0f * Q);
	const float a0 = 1.0f/(1.0f + alpha);   //a0 = 1/a0

	const float OneMinusCs = 1.0f - cs;
	//LPF
	const float b0_lpf = (OneMinusCs/2.0f) * a0;
	const float b1_lpf = OneMinusCs * a0;
	 //const float b2_lpf = b0_lpf;
	const float a1_lpf = (-2.0f * cs) * a0;
	const float a2_lpf = (1.0f - alpha) * a0;

	// mutex lock
	pthread_mutex_lock(mutex);
	coeffs->b0 = b0_lpf;
	coeffs->b1 = b1_lpf;
	 //coeffs->b2 = b2_lpf;
	coeffs->a1 = a1_lpf;
	coeffs->a2 = a2_lpf;
	// mutex unlock
	pthread_mutex_unlock(mutex);
}

void DistortionProcessor::operator>>(CrInputDataStream& inputStream) const
{
	inputStream << *mo_parameters;
}

void DistortionProcessor::operator<<(CrOutputDataStream& outputStream)
{
	outputStream >> *mo_parameters;
}
