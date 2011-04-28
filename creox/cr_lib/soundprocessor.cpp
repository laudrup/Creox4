/***************************************************************************
			soundprocessor.cpp  -  description
					 -------------------
		begin                : Fri Jul 28 2000
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

#include <cstring>

#include "soundprocessor.h"

SoundProcessor::SoundProcessor(SoundParameters* parameters) :
  m_registered(false)
{
  m_samplerate = 0;
  m_samplerate_f = 0.0;
  m_samplerate_d = 0.0;
  m_stereoInput = false;
  m_inputBuffer = 0;
  m_outputBuffer = 0;
  m_stereoOutput = false;
  m_active = false;
  m_parameters = parameters;
  mr_mode = enabled;
  mw_newParameters = mr_newParameters = false;
}

SoundProcessor::~SoundProcessor()
{
}

void SoundProcessor::activate()
{
  if (m_active)
    {
      cleanup();
    }

  //m_samplerate = m_pcmDevice->getSamplerateReal();
  //m_samplerate_f = static_cast<float>(m_samplerate);
  //m_samplerate_d = static_cast<double>(m_samplerate);
  //m_stereoInput = (m_pcmDevice->getVoices() == PcmDevice::stereo) ? true : false;
  //assert(m_inputBuffer);
  //assert(m_outputBuffer);

  mr_newParameters = mw_newParameters = false;
  initialize();
  computeParameters();
  setParameters_run();
  prepareEnabled();
  m_active = true;
}

void SoundProcessor::deactivate()
{
  if (m_active)
    {
      cleanup();
      m_active = false;
    }
}

void SoundProcessor::process_run(const int length)
{
  if (mw_newParameters != mr_newParameters)
    {
      mw_newParameters = !mw_newParameters;
      setParameters_run();
    }

  if (mr_mode == enabled)
    {
      signalFlow_run(length);
    }
  else if (m_inputBuffer != m_outputBuffer)
    {
      (void)std::memcpy(m_outputBuffer, m_inputBuffer, sizeof(float)*length);
    }
}

void SoundProcessor::updateParameters()
{
  if (m_active)
    {
      computeParameters();
      if (mr_newParameters == mw_newParameters)
        {
          mr_newParameters = !mr_newParameters;
        }
    }
}

void SoundProcessor::setMode(const int mode)
{
  if (mode == enabled && mr_mode != enabled)
    {
      if (m_active)
    	{
          prepareEnabled();
    	}
    }
  mr_mode = mode;
}

void SoundProcessor::registerProcessor(const char* processorName, const float processorVersion)
{
  //assert(!m_registered);
  m_processorName = processorName;
  m_processorVersion = processorVersion;
  //compute id from the name and version (a simple hash)
  const int slength = std::strlen(processorName);
  //assert(slength != 0);

  int id = 0;
  for (int count=0; count<slength; count++)
    {
      id += static_cast<int>(processorName[count])*(count+1);
    }
  //assert(id > 0);
  id *= static_cast<int>(processorVersion)*1000;
  m_id = id;
  m_registered = true;

}
