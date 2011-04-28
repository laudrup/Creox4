/***************************************************************************
					threadeffector.h  -  description
						 -------------------
		begin                : Thu Sep 7 2000
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

#ifndef THREADEFFECTOR_H
#define THREADEFFECTOR_H

#include <pthread.h>
#include <string>
#include <jack/jack.h>

class SoundProcessor;

/**
 *	@author Jozef Kosoru
 */
class ThreadEffector {
 private:
  enum { ERROR_MESSAGE_BUFFER_SIZE = 100 };

 public:
  ThreadEffector(const int processorsNumber=0);
  ~ThreadEffector();

  enum EffectorStatus{status_Run, status_Stop};

  void run();
  void stop();

  void updateGain(const float inputGain, const float outputGain);
  void updateInputGain(const float inputGain);
  void updateOutputGain(const float outputGain);

  //!! next two functions return linear value instead of log !!
  float getInputGain() const { return mr_inputGain;}
  float getOutputGain() const { return mr_outputGain; }

  void updateProcessorChain();
  SoundProcessor** getProcessorChain() const { return m_ppOutsideProcessorChain; }
  SoundProcessor*& operator[](const int procNum) const { return m_ppOutsideProcessorChain[procNum]; }
  // look! ^^^ this is a reference to a pointer
  void setProcessorChainSize(const int size) { m_outsideProcessorChainSize=size; }

  int getProcessorChainSize() const { return mr_processorChainSize; }
  int getMaxProcessors() const { return s_maxProcessors; }

  EffectorStatus getStatus() const { return m_status; }

 private:
  ThreadEffector(ThreadEffector&);  //disable copy
  ThreadEffector& operator=(ThreadEffector&);

  int process(jack_nframes_t nframes);
  int bufsize(jack_nframes_t nframes);
  int srate(jack_nframes_t nframes);

  static int processCallback(jack_nframes_t nframes, void* p_effector);
  static int bufsizeCallback(jack_nframes_t nframes, void* p_effector);
  static int srateCallback(jack_nframes_t nframes, void* p_effector);
  static void errorCallback(const char* p_msg);

 private:
  volatile float mr_inputGain;
  float m_inputGain;
  volatile float mr_outputGain;
  float m_outputGain;

  volatile bool mw_newParameters;
  volatile bool mr_newParameters;

  volatile EffectorStatus m_status;

  int m_outsideProcessorChainSize;
  volatile int mr_processorChainSize;
  int m_processorChainSize;

  SoundProcessor** m_ppOutsideProcessorChain;
  volatile SoundProcessor** m_ppMiddleProcessorChain;
  SoundProcessor** m_ppInsideProcessorChain;
  SoundProcessor** m_ppChainEnd;

  pthread_mutex_t m_mutex;

  std::string m_sLeftInputPortName;
  std::string m_sRightInputPortName;
  std::string m_sLeftOutputPortName;
  std::string m_sRightOutputPortName;

  float* m_pAudioBuffer_f;	// if creox will support stereo,
  //		audio buffer won't be needed
  std::size_t m_iAudioBufferSize;
  int m_iSampleRate;

  jack_client_t* m_pJackClient;
  jack_port_t* m_pInputPort1;
  jack_port_t* m_pInputPort2;
  jack_port_t* m_pOutputPort1;
  jack_port_t* m_pOutputPort2;

  static const int s_maxProcessors = 10;
  static std::string s_sErrorMessageString;
};

#endif
