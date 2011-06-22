/***************************************************************************
						tremoloprocessor.h  -  description
							 -------------------
		begin                : Sat Sep 23 2000
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

#ifndef TREMOLOPROCESSOR_H
#define TREMOLOPROCESSOR_H

#include <pthread.h>
#include "soundprocessor.h"

/**
	*@author Jozef Kosoru
	*/
struct TremoloParameters: public SoundParameters{
	float inputGain;
	float wetMix;
	float dryMix;
	float frequency;  // [Hz]
	float depth;      // [dB]
	int waveform;     // sine/triangle
};

/**
	*@author Jozef Kosoru
	*/
class TremoloProcessor : public SoundProcessor  {
public:
	enum Waveform{ sine=0, triangle=1 };

	TremoloProcessor(TremoloParameters* parameters);
	~TremoloProcessor();
	virtual int getParametersSize() const { return sizeof(TremoloParameters); }
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
	TremoloParameters* mo_parameters;
	pthread_mutex_t m_mutex;

	volatile bool mr_recalculate;
	volatile int mr_waveform;
	int m_waveform;

	int mp_waveform;
	int mp_samplesPerCycle;

	volatile float mr_inputGain_f;
	float m_inputGain_f;
	volatile float mr_wet_f;
	float m_wet_f;
	volatile float mr_dry_f;
	float m_dry_f;

	//sine
	volatile double mr_y_d[3];
	double m_y_d[3];
	double m_y_d_first[3];
	volatile double mr_p_d;
	double m_p_d;

	//triangle
	double m_triangleCount_d;
	volatile double mr_linearPhaseStep_d;
	double m_linearPhaseStep_d;

	volatile float mr_modulationDepth;
	float m_modulationMulti;
	float m_modulationShift;

	int m_halfPeriodCount;
	bool m_firstHalf;
	volatile int mr_samplesPerHalfCycle;
	int m_samplesPerHalfCycle;

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
