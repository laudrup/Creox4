/***************************************************************************
						phaserprocessor.h  -  description
								 -------------------
		begin                : Tue Aug 22 2000
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

#ifndef PHASERPROCESSOR_H
#define PHASERPROCESSOR_H

#include <pthread.h>
#include "soundprocessor.h"

/**
	*@author Jozef Kosoru
	*/
struct PhaserParameters : public SoundParameters{
	float inputGain;      // <-15, +15> [dB]
	float dryMix;         // <0, 100> [%]
	float sweepGain;       // <-30, +30> [dB]
	float centerFrequency; // <20,20k> [Hz]
	float depth;           // <1,15> [/Q]
	float resonance;       // <0,15> [Q]
	float sweepFrequency;  // <0.1, 30> [Hz]
};

/**
	*@author Jozef Kosoru
	*/
class PhaserProcessor : public SoundProcessor  {
public:
	PhaserProcessor(PhaserParameters* parameters);
	~PhaserProcessor();
	virtual int getParametersSize() const { return sizeof(PhaserParameters); }
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
	PhaserParameters* mo_parameters;

	float m_halfSamplerate;

	float mp_samplesPerHalfCycle;
	float mp_bottomFrequency;
	float mp_topFrequency;
	float mp_resonance;

	bool m_useLookupTable;
	int m_phaseCounter;  //used if no lookupTab
	int m_halfPhaseSize; //used if no lookupTab
	float m_stepFrequency_first; //used if no lookupTab

	bool m_sweepDirection;
	float* m_coefBuffPrelim;
	float* m_coefBuffSweep;
	float* m_coefBuffEnd;

	volatile float mr_stepFrequency;
	float m_stepFrequency;
	volatile float mr_resonance;
	float m_resonance;
	volatile float mr_expStep;
	float m_expStep;
	volatile bool mr_recalculate;
	bool m_recalculate;

	volatile int mr_coefBufferSize;
	volatile float* mr_coefBuffer;
	float* m_coefBuffer;

	volatile float mr_inputGain;
	float m_inputGain;
	volatile float mr_sweepGain;
	float m_sweepGain;
	volatile float mr_dryMix;
	float m_dryMix;

	pthread_mutex_t m_mutex;

	float m_x1, m_x2, m_y1, m_y2;

private:
	static inline int roundToInt(float f)
	{
		int sign=1;
		if(f<0.0f){
			f=-f;
			sign=-1;
		}
		const int fi = static_cast<int>(f);
		if((f-static_cast<float>(fi))>=0.5f){
			return (fi+1)*sign;
		}
		else{
			return fi*sign;
		}
	}
};

#endif
