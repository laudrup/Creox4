/***************************************************************************
					distortionprocessor.h  -  description
						 -------------------
		begin                : Thu Aug 31 2000
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

#ifndef DISTORTIONPROCESSOR_H
#define DISTORTIONPROCESSOR_H

#include <pthread.h>
#include "soundprocessor.h"

#define MAX_DISTORT_VALUE (65536)
#define NUM_OF_DISTORT_FUNCTIONS (4)

/**
	*@author Jozef Kosoru
	*/
struct DistortionParameters : public SoundParameters{
	float inputGain;      // <-15, +15> [dB]
	float dryMix;         // <0, 100> [%]
	float wetMix;         // <0, 100> [%]

	//input bass cut/boost (lowShelf)
	float inBass_frequency;  // <20, 1000> [Hz]
	float inBass_gain;       // <-35, +35> [dB]
	float inBass_slope;      // <0, 1> [Q]

	//output bass boost/cut (lowShelf)
	float outBass_frequency;  // <20, 1000> [Hz]
	float outBass_gain;       // <-35, +35> [dB]
	float outBass_slope;      // <0, 1> [Q]

	//accented frequency (BPF)
	float accent_frequency;   // <20, 20k> [Hz]
	float accent_bandwidth;   // <0, 15> [octaves]
	float accent_drive;       // <0, 100> [dB]
	float accent_gain;        // [dB]

	//mainDrive
	float main_drive;         // <0, 100> [dB]
	float main_gain;          // [dB]

	//distort function
	int accent_distortFunction;      // <0, 3>
	int main_distortFunction;        // <0, 3>

	//low Pass
	float lowPass_cutoffFrequency;   // <100, 20000> [Hz]
	float lowPass_Q;                 // <0, 15> [Q]
};

/**
	*@author Jozef Kosoru
	*/
class DistortionProcessor : public SoundProcessor  {
public:
	DistortionProcessor(DistortionParameters* parameters);
	~DistortionProcessor();
	virtual int getParametersSize() const { return sizeof(DistortionParameters); }
	virtual void operator>>(CrInputDataStream& inputStream) const;
	virtual void operator<<(CrOutputDataStream& outputStream);
protected:
	void initialize();
	void computeParameters();
	void setParameters_run();
	void signalFlow_run(const int length);
	void cleanup();
	void prepareEnabled();
private:
	struct IIRCoefficients{
		float b0;
		float b1;
		float b2;
		float a1;
		float a2;
	};

	struct FilterValues{
		float x1;
		float x2;
		float y1;
		float y2;
	};

	DistortionParameters* mo_parameters;
	float* m_distortFunction[NUM_OF_DISTORT_FUNCTIONS];

	volatile float* mr_accent_currentDistortFunction;
	float* m_accent_currentDistortFunction;
	volatile float* mr_main_currentDistortFunction;
	float* m_main_currentDistortFunction;

	volatile float mr_inputGain;
	float m_inputGain;
	volatile float mr_wetMix;
	float m_wetMix;
	volatile float mr_dryMix;
	float m_dryMix;

	//low Pass
	pthread_mutex_t m_lowPass_mutex;
	volatile bool mr_lowPass_changed;
	float mp_lowPass_cutoffFrequency;
	float mp_lowPass_Q;
	volatile IIRCoefficients mr_lowPassIIRCoefficients;
	IIRCoefficients m_lowPassIIRCoefficients;
	FilterValues m_lowPassFilterValues;

	//mainDrive
	volatile float mr_mainDrive_in;
	float m_mainDrive_in;
	volatile float mr_mainDrive_out;
	float m_mainDrive_out;

	//accented frequency (BPF)
	volatile float mr_accent_drive_in;
	float m_accent_drive_in;
	volatile float mr_accent_drive_out;
	float m_accent_drive_out;
	pthread_mutex_t m_accent_mutex;
	volatile bool mr_accent_changed;
	float mp_accent_frequency;
	float mp_accent_bandwidth;
	volatile IIRCoefficients mr_accentIIRCoefficients;
	IIRCoefficients m_accentIIRCoefficients;
	FilterValues m_accentFilterValues;
	FilterValues m_mainFilterValues;

	//input bass boost/cut (lowShelf)
	pthread_mutex_t m_inBass_mutex;
	volatile bool mr_inBass_changed;
	float mp_inBass_frequency;
	float mp_inBass_gain;
	float mp_inBass_slope;
	volatile IIRCoefficients mr_inBassIIRCoefficients;
	IIRCoefficients m_inBassIIRCoefficients;
	FilterValues m_inBassFilterValues;

	//output bass boost/cut (lowShelf)
	pthread_mutex_t m_outBass_mutex;
	volatile bool mr_outBass_changed;
	float mp_outBass_frequency;
	float mp_outBass_gain;
	float mp_outBass_slope;
	volatile IIRCoefficients mr_outBassIIRCoefficients;
	IIRCoefficients m_outBassIIRCoefficients;
	FilterValues m_outBassFilterValues;

private:
	inline void computeBPFCoefficients(volatile IIRCoefficients* const coeffs, const float centerFrequency, const float bandwidth, pthread_mutex_t* mutex) const;
	inline void computeLowShelfCoefficients(volatile IIRCoefficients* const coeffs, const float frequency, const float gain, const float slope, pthread_mutex_t* mutex) const;
	inline void computeLowPassCoefficients(volatile IIRCoefficients* const coeffs, const float cutoffFrequency, const float Q, pthread_mutex_t* mutex) const;

	static void makeDistortFunction_0(float* array);
	static void makeDistortFunction_1(float* array);
	static void makeDistortFunction_2(float* array);
	static void makeDistortFunction_3(float* array);

	static inline int roundToInt(double f)
	{
		int sign=1;
		if(f<0.0){
			f=-f;
			sign=-1;
		}
		const int fi = static_cast<int>(f);
		if((f-static_cast<double>(fi))>=0.5){
			return (fi+1)*sign;
		}
		else{
			return fi*sign;
		}
	}

	static const FilterValues s_zeroValues;
};

#endif
