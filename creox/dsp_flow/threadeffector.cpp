/***************************************************************************
					threadeffector.cpp  -  description
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

#include <cassert>
#include <cmath>
#include <cstring>

#include <klocale.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kconfig.h>

#include "soundprocessor.h"
#include "threadeffector.h"
#include "crthreadeventdispatcher.h"
#include "creox.h"
#include "settings.h"

#include <cerrno>

#include <QDebug>

const int ThreadEffector::s_maxProcessors;
std::string ThreadEffector::s_sErrorMessageString;

ThreadEffector::ThreadEffector(const int processorsNumber)
  : m_outsideProcessorChainSize(processorsNumber), m_pAudioBuffer_f(0),
    m_pJackClient(0)
{
  (void)pthread_mutex_init(&m_mutex,NULL);
  m_ppOutsideProcessorChain = new SoundProcessor*[s_maxProcessors];
  m_ppMiddleProcessorChain = new volatile SoundProcessor*[s_maxProcessors];
  m_ppInsideProcessorChain = new SoundProcessor*[s_maxProcessors];
  for (int count=0; count<s_maxProcessors; count++) {
    m_ppOutsideProcessorChain[count] = m_ppInsideProcessorChain[count] = 0;
  }
  m_processorChainSize = mr_processorChainSize = 0;
  mw_newParameters = mr_newParameters = false;

  m_status = status_Stop;
  mr_inputGain = mr_outputGain = 1.0f;
  m_inputGain = m_outputGain = 1.0f;
}

ThreadEffector::~ThreadEffector()
{
  if (m_status == status_Run) {
    //      try{
    stop();
    //      }catch(Cr::CrException_runtimeError &error){
    //      std::cerr << error.what().latin1() << "\n";
    //      }
  }
  (void)pthread_mutex_destroy(&m_mutex);
  delete[] m_ppOutsideProcessorChain;
  delete[] m_ppMiddleProcessorChain;
  delete[] m_ppInsideProcessorChain;
}

void ThreadEffector::updateProcessorChain()
{
  mr_processorChainSize = m_outsideProcessorChainSize;
  std::memcpy(m_ppMiddleProcessorChain, m_ppOutsideProcessorChain,
              sizeof(SoundProcessor*) * m_outsideProcessorChainSize);
  if(mr_newParameters == mw_newParameters){
    mr_newParameters = !mr_newParameters;
  }
}

int ThreadEffector::process(jack_nframes_t nframes)
{
  //	try {
  if (mw_newParameters != mr_newParameters) {
    mw_newParameters = !mw_newParameters;
    //mutex lock
    if (pthread_mutex_trylock(&m_mutex) != EBUSY) {
      m_processorChainSize = mr_processorChainSize;
      std::memcpy(m_ppInsideProcessorChain, m_ppMiddleProcessorChain,
                  sizeof(SoundProcessor*) * m_processorChainSize);
      m_ppChainEnd = m_ppInsideProcessorChain + m_processorChainSize;
      //mutex unlock
      pthread_mutex_unlock(&m_mutex);
    }
  }
  m_inputGain = mr_inputGain;
  m_outputGain = mr_outputGain;

  // get pointers to the jack port buffers
  jack_default_audio_sample_t* const pOutputBuffer1
    = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(m_pOutputPort1, nframes));
  jack_default_audio_sample_t* const pOutputBuffer2
    = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(m_pOutputPort2, nframes));
  const jack_default_audio_sample_t* const pInputBuffer1
    = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(m_pInputPort1, nframes));
  const jack_default_audio_sample_t* const pInputBuffer2
    = static_cast<jack_default_audio_sample_t*>(jack_port_get_buffer(m_pInputPort2, nframes));

  // join the input stereo signal to the mono buffer (really ugly)
  //#!TODO: stereo
  const jack_default_audio_sample_t m_monoInputGain = m_inputGain * 0.5f;
  for (std::size_t iCount=0; iCount != nframes; ++iCount)
    {
      m_pAudioBuffer_f[iCount] = (pInputBuffer1[iCount] +
                                  pInputBuffer2[iCount]) * m_monoInputGain;
    }

  // run all sound processors on the given signal
  SoundProcessor** ppProcessorChain = m_ppInsideProcessorChain;
  while (ppProcessorChain != m_ppChainEnd)
    {
      (*(ppProcessorChain++))->process_run(nframes);
    }

  // write the transformed signel to the output ports
  //#!TODO: stereo
  for (std::size_t iCount=0; iCount != nframes; ++iCount)
    {
      pOutputBuffer1[iCount] = pOutputBuffer2[iCount] =
        m_pAudioBuffer_f[iCount] * m_outputGain;
    }

  /*
    }
    catch(Cr::CrException_runtimeError &error)
    {
    CrMessageEvent* errorEvent = new CrMessageEvent(error.what());
    static_cast<Creox*>(kapp->mainWidget())->
    getEventDispatcher()->
    postEvent(kapp->mainWidget(),
    errorEvent);
    }
  */
  return 0;
}

int ThreadEffector::bufsize(jack_nframes_t nframes)
{
  // create the new audio buffer
  m_iAudioBufferSize = nframes;
  delete[] m_pAudioBuffer_f;
  m_pAudioBuffer_f = new float[m_iAudioBufferSize];

  for(int count=0; count<m_processorChainSize; ++count)
    {
      m_ppInsideProcessorChain[count]->m_inputBuffer =
        m_ppInsideProcessorChain[count]->
        m_outputBuffer = m_pAudioBuffer_f;
    }
  return 0;
}

int ThreadEffector::srate(jack_nframes_t nframes)
{
  // TODO: Not sure if this is needed
  /*
  qDebug() << "srate";
  CrMessageEvent* errorEvent =
    new CrMessageEvent(i18n("The JACK sample rate has changed to %1 Hz."
                            " Restart the sound processing.").arg(nframes));
  static_cast<Creox*>(kapp->mainWidget())->
    getEventDispatcher()->
    postEvent(kapp->mainWidget(),
              errorEvent);
  */
  return 0;
}

void ThreadEffector::run()
{
  updateProcessorChain();
  mr_newParameters = false;

  // set error function
  jack_set_error_function(errorCallback);

  // connect to JACK server
  if ((m_pJackClient = jack_client_open("creox", JackNullOption, 0)) == 0) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error: %1").arg(QString::fromLatin1(s_sErrorMessageString.c_str())));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }

  // register callbacks
  jack_set_process_callback(m_pJackClient, processCallback, this);
  jack_set_buffer_size_callback(m_pJackClient, bufsizeCallback, this);
  jack_set_sample_rate_callback(m_pJackClient, srateCallback, this);

  // Get input port names
  const char** const ppInputPortList = jack_get_ports(m_pJackClient, 0, 0, JackPortIsInput);
  if (!ppInputPortList || !ppInputPortList[0]) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("No jack input ports available"));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }
  QStringList inputPortList;
  for (const char** ppPortCount = ppInputPortList; *ppPortCount; ++ppPortCount) {
    inputPortList << QString::fromLatin1(*ppPortCount);
  }
  jack_free(ppInputPortList);

  if (inputPortList.size() <= Settings::leftInputChannel()
      || inputPortList.size() <= Settings::rightInputChannel()) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Selected jack input ports no longer available"));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }
  QString leftInputPortName = inputPortList[Settings::leftInputChannel()];
  QString rightInputPortName = inputPortList[Settings::rightInputChannel()];

  // Get output port names
  const char** const ppOutputPortList = jack_get_ports(m_pJackClient, 0, 0, JackPortIsOutput);
  if (!ppOutputPortList || !ppOutputPortList[0]) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("No jack output ports available"));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }
  QStringList outputPortList;
  for (const char** ppPortCount = ppOutputPortList; *ppPortCount; ++ppPortCount) {
    outputPortList << QString::fromLatin1(*ppPortCount);
  }
  jack_free(ppOutputPortList);

  if (outputPortList.size() <= Settings::leftOutputChannel()
      || outputPortList.size() <= Settings::rightOutputChannel()) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Selected jack output ports no longer available"));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }
  QString leftOutputPortName = outputPortList[Settings::leftOutputChannel()];
  QString rightOutputPortName = outputPortList[Settings::rightOutputChannel()];

  // register ports
  m_pInputPort1 = jack_port_register(m_pJackClient, "input_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  assert(m_pInputPort1);
  m_pInputPort2 = jack_port_register(m_pJackClient, "input_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
  assert(m_pInputPort2);
  m_pOutputPort1 = jack_port_register(m_pJackClient, "output_1", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  assert(m_pOutputPort1);
  m_pOutputPort2 = jack_port_register(m_pJackClient, "output_2", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
  assert(m_pOutputPort2);

  m_processorChainSize = mr_processorChainSize;
  std::memcpy(m_ppInsideProcessorChain, m_ppMiddleProcessorChain,
              sizeof(SoundProcessor*) * m_processorChainSize);
  mw_newParameters = mr_newParameters;

  // get sample rate
  m_iSampleRate = jack_get_sample_rate(m_pJackClient);

  // create the audio buffer
  m_iAudioBufferSize = jack_get_buffer_size(m_pJackClient);
  delete[] m_pAudioBuffer_f;
  m_pAudioBuffer_f = new float[m_iAudioBufferSize];

  // activate sound processors
  const float samplerate_f = static_cast<float>(m_iSampleRate);
  const double samplerate_d = static_cast<double>(m_iSampleRate);
  for(int count=0; count<m_processorChainSize; ++count)
    {
      assert(m_ppInsideProcessorChain[count]);
      m_ppInsideProcessorChain[count]->m_samplerate = m_iSampleRate;
      m_ppInsideProcessorChain[count]->m_samplerate_f = samplerate_f;
      m_ppInsideProcessorChain[count]->m_samplerate_d = samplerate_d;
      m_ppInsideProcessorChain[count]->m_stereoInput = false;
      m_ppInsideProcessorChain[count]->m_inputBuffer =
        m_ppInsideProcessorChain[count]->
        m_outputBuffer = m_pAudioBuffer_f;
      m_ppInsideProcessorChain[count]->activate();
    }

  // activate jack processing thread
  if (jack_activate(m_pJackClient)) {
    CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error: %1")
                                                    .arg(QString::fromLatin1(s_sErrorMessageString.c_str())));
    static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
    return;
  }

  m_ppChainEnd = m_ppInsideProcessorChain + m_processorChainSize;

  /* connect ports */
  // input ports

  if (!Settings::leaveInputDisconnected()) {
    if (jack_connect(m_pJackClient, leftOutputPortName.toLatin1(), jack_port_name(m_pInputPort1))) {
      CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error! Cannot connect output port: %1\n").
                                                      arg(leftInputPortName));
      static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
      return;
    }
    if (jack_connect(m_pJackClient, rightOutputPortName.toLatin1(), jack_port_name(m_pInputPort2))) {
      CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error! Cannot connect output port: %1\n").
                                                      arg(rightInputPortName));
      static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
      return;
    }
  }

  // output ports
  if (!Settings::leaveOutputDisconnected()) {
    if (jack_connect(m_pJackClient, jack_port_name(m_pOutputPort1), leftInputPortName.toLatin1())) {
      CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error! Cannot connect input port: %1").
                                                      arg(leftOutputPortName));
      static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
      return;
    }
    if (jack_connect(m_pJackClient, jack_port_name(m_pOutputPort2), rightInputPortName.toLatin1())) {
      CrMessageEvent* errorEvent = new CrMessageEvent(i18n("Error! Cannot connect input port: %1").
                                                      arg(rightOutputPortName));
      static_cast<Creox*>(kapp->mainWidget())->getEventDispatcher()->postEvent(kapp->mainWidget(), errorEvent);
      return;
    }
  }

  m_status = status_Run;
}

void ThreadEffector::stop()
{
  if (m_pJackClient)
    {
      jack_deactivate(m_pJackClient);
      jack_client_close(m_pJackClient);
      m_pJackClient = 0;
    }

  for(int count=0; count<m_processorChainSize; count++){
    assert(m_ppInsideProcessorChain[count]);
    m_ppInsideProcessorChain[count]->deactivate();
  }

  delete[] m_pAudioBuffer_f;
  m_pAudioBuffer_f = 0;
  m_status = status_Stop;
}

void ThreadEffector::updateGain(const float inputGain, const float outputGain)
{
  mr_inputGain = exp10f(inputGain/20.0f);
  mr_outputGain = exp10f(outputGain/20.0f);
}

void ThreadEffector::updateInputGain(const float inputGain)
{
  mr_inputGain = exp10f(inputGain/20.0f);
}

void ThreadEffector::updateOutputGain(const float outputGain)
{
  mr_outputGain = exp10f(outputGain/20.0f);
}

int ThreadEffector::processCallback(jack_nframes_t nframes, void* p_effector)
{
  return static_cast<ThreadEffector*>(p_effector)->process(nframes);
}

int ThreadEffector::bufsizeCallback(jack_nframes_t nframes, void* p_effector)
{
  return static_cast<ThreadEffector*>(p_effector)->bufsize(nframes);
}

int ThreadEffector::srateCallback(jack_nframes_t nframes, void* p_effector)
{
  return static_cast<ThreadEffector*>(p_effector)->srate(nframes);
}

void ThreadEffector::errorCallback(const char* p_msg)
{
  //std::cerr << "JACK error: " << p_msg << std::endl;

  s_sErrorMessageString = p_msg;
}
