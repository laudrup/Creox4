/***************************************************************************
					flangerprocessor.h  -  description
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

#ifndef FLANGERPROCESSOR_H
#define FLANGERPROCESSOR_H

#include <pthread.h>
#include "soundprocessor.h"

/**
	*@author Jozef Kosoru
	*/
struct FlangerParameters : public SoundParameters{
	float inputGain;      // <-15, +15> [dB]
	float crossfade;         // <-100,100> -100=dry 100=wet
	float feedback;         // <0, 100> [%]
	int waveform;           // sine/triangle
	float delay;            // <0, 40> [ms]
	float modulationDepth;  // <0, 40> [ms]
	float frequency;        // <0, 200> [Hz]
	bool inverted;          // false/true
};

/**
	*@author Jozef Kosoru
	*/
class FlangerProcessor : public SoundProcessor  {
public:
	enum Waveform { sine=0, triangle=1 };

	FlangerProcessor(FlangerParameters* parameters);
	~FlangerProcessor();
	virtual int getParametersSize() const { return sizeof(FlangerParameters); }
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
	volatile bool mr_recalculate;
	bool m_recalculate;

	volatile int mr_waveform;
	int m_waveform;

	bool m_useLookupTable;

	//sine
	volatile double mr_y_dd[3];
	double m_y_dd[3];
	double m_y_dd_first[3];  //used if no lookupTab
	volatile double mr_p_dd;
	double m_p_dd;
	volatile double mr_modulationHalfDepth_dd;
	double m_modulationHalfDepth_dd;

	//triangle
	double m_triangleCount_dd;
	volatile double mr_linearPhaseStep_dd;
	double m_linearPhaseStep_dd;

	int m_halfPeriodCount; //used if no lookupTab
	bool m_firstHalf;  //used if no lookupTab
	int m_samplesPerHalfCycle; //used if no lookupTab

	float* m_lookupTabPrelim;
	float* m_lookupTabHalfPrelim;
	float* m_lookupTabHalfPrelimStatic;
	float* m_lookupTabEndPrelim;


	volatile float mr_wet_f;
	float m_wet_f;
	volatile float mr_dry_f;
	float m_dry_f;
	volatile float mr_feedback_f;
	float m_feedback_f;
	volatile float mr_inputGain_f;
	float m_inputGain_f;
	volatile float* mr_lookupTab;
	float* m_lookupTab;
	volatile int mr_samplesPerCycle;
	int m_samplesPerCycle;
	volatile int mr_delay;
	int m_delay;

	int mp_samplesPerCycle;
	double mp_modulationDepth_dd;
	int mp_waveform;

	int m_delayBufferOffset;
	int m_phaseCount;
	float* m_delayBuffer;
	int m_delayBufferSize;

	pthread_mutex_t m_mutex;

	FlangerParameters* mo_parameters;

	static const float s_maxBufferSize_d; // = 80.0 ms

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
