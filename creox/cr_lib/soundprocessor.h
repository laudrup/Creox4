/***************************************************************************
					soundprocessor.h  -  description
						 -------------------
		begin                : Fri Jul 28 2000
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

#ifndef SOUNDPROCESSOR_H
#define SOUNDPROCESSOR_H

class ThreadEffector;
class CrOutputDataStream;
class CrInputDataStream;

/**
 *@author Jozef Kosoru
 */
struct SoundParameters {
  //float inputGain;
  //float wetMix;
  //float dryMix;
};

/**
 *@author Jozef Kosoru
 */
class SoundProcessor {
 public:
  enum RunMode{ enabled, disabled };

  SoundProcessor(SoundParameters* parameters);
  virtual ~SoundProcessor();

  const char* getName() const {return m_processorName; }
  float getVersion() const {return m_processorVersion; }
  int getId() const {return m_id; };

  virtual int getParametersSize() const = 0;
  virtual void operator>>(CrInputDataStream& inputStream) const = 0;
  virtual void operator<<(CrOutputDataStream& outputStream) = 0;

  void activate();
  void deactivate();
  void process_run(const int length);
  void updateParameters();

  bool active() const { return m_active; }
  int mode() const { return mr_mode; }

  void setMode(const int mode);
  void setInputBuffer(float* input) { m_inputBuffer = input; }
  void setOutputBuffer(float* output) { m_outputBuffer = output; }

 protected:
  virtual void initialize() = 0;
  virtual void signalFlow_run(const int length) = 0;
  virtual void cleanup() = 0;
  virtual void setParameters_run() = 0;
  virtual void computeParameters() = 0;
  virtual void prepareEnabled() = 0;
  void registerProcessor(const char* processorName, const float processorVersion);

  float* m_inputBuffer;
  float* m_outputBuffer;
  bool m_stereoInput;
  bool m_stereoOutput;
  int m_samplerate;
  float m_samplerate_f;
  double m_samplerate_d;
  float m_processorVersion;
  const char* m_processorName;
  bool m_registered;
  int m_id;

 private:
  friend class ThreadEffector;
  bool m_active;
  volatile int mr_mode;
  volatile bool mw_newParameters;
  volatile bool mr_newParameters;
  SoundParameters* m_parameters;
};

#endif
