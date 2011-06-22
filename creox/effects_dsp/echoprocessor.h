/***************************************************************************
						echoprocessor.h  -  description
							 -------------------
		begin                : Wed Aug 16 2000
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

#ifndef ECHOPROCESSOR_H
#define ECHOPROCESSOR_H

#include <pthread.h>
#include "soundprocessor.h"

/**
	*@author Jozef Kosoru
	*/
struct EchoParameters : public SoundParameters{
	float inputGain;      // <-15, +15> [dB]
	float wetMix;         // <0,100> [%]
	float dryMix;         // <0,100> [%]
	float finalFeedback;    // <0,100> [%]
	float finalDelay;       // <0, 1000> [ms]
	struct ParallelEcho{
		float delay;          // <0, 1000> [ms]
		float decay;          // <0,100> [%]
		float feedback;       // <0,100> [%]
	}parallelEcho[10];
	int parallelEchoCount;  // <0,10>
};

/**
	*@author Jozef Kosoru
	*/
class EchoProcessor : public SoundProcessor  {
public:
	EchoProcessor(EchoParameters* parameters);
	~EchoProcessor();
	virtual int getParametersSize() const { return sizeof(EchoParameters); }
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
	volatile float mr_wet_f;
	float m_wet_f;
	volatile float mr_dry_f;
	float m_dry_f;
	volatile float mr_finalFeedback_f;
	float m_finalFeedback_f;
	volatile float mr_inputGain_f;
	float m_inputGain_f;
	volatile int mr_finalDelay;
	int m_finalDelay;

	struct ParallelEchoChain{
		float* buffer;
		int offset;
	};
	struct ParallelEchoParam{
		int delay;
		float decay_f;
		float feedback_f;
	};

	volatile ParallelEchoChain mr_parallelEchoChain[10];
	volatile ParallelEchoChain mw_parallelEchoChain[10];
	ParallelEchoChain m_parallelEchoChain[10];

	volatile ParallelEchoParam mr_parallelEchoParam[10];
	ParallelEchoParam m_parallelEchoParam[10];

	volatile int mr_numOfParallelEchoes;
	int m_numOfParallelEchoes;

	int m_delayBufferSize;
	int m_finalEchoBufferOffset;
	float* m_finalEchoBuffer;

	EchoParameters* mo_parameters;

	pthread_mutex_t m_mutex;

	static const float s_maxBufferSize_d; // = 1000.0f ms

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
