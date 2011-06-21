/***************************************************************************
						audioprefs.h  -  description
						 -------------------
		begin                : Tue Jun 21 2011
		copyright            : (C) 2011 by Kasper Laudrup
		email                : laudrup@stacktrace.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "audioprefs.h"
#include "jack/jack.h"
#include "settings.h"
#include <QStringList>

AudioPrefs::AudioPrefs(QWidget* parent)
  : QWidget(parent)
{
  setupUi(this);

  jack_client_t* pJackClient;
  if ((pJackClient = jack_client_open("creox_options", JackNullOption, 0)) != 0) {
    const char** const ppInputPortList = jack_get_ports(pJackClient, 0, 0, JackPortIsInput);
      if (ppInputPortList) {
        int idx = 0;
        for (const char** ppPortCount = ppInputPortList; *ppPortCount; ++ppPortCount, ++idx) {
          QString chanName = QString::fromLatin1(*ppPortCount);
          kcfg_leftInputChannel->addItem(chanName);
          kcfg_rightInputChannel->addItem(chanName);
          if (chanName == Settings::leftInputChannel())
            kcfg_leftInputChannel->setCurrentIndex(idx);
          if (chanName == Settings::rightInputChannel())
            kcfg_rightInputChannel->setCurrentIndex(idx);
        }
        free(ppInputPortList);
      }

      const char** const ppOutputPortList = jack_get_ports(pJackClient, 0, 0, JackPortIsOutput);
      if (ppOutputPortList) {
        int idx = 0;
        for (const char** ppPortCount = ppOutputPortList; *ppPortCount; ++ppPortCount, ++idx) {
          QString chanName = QString::fromLatin1(*ppPortCount);
          kcfg_leftOutputChannel->addItem(chanName);
          kcfg_rightOutputChannel->addItem(chanName);
          if (chanName == Settings::leftOutputChannel())
            kcfg_leftOutputChannel->setCurrentIndex(idx);
          if (chanName == Settings::rightOutputChannel())
            kcfg_rightOutputChannel->setCurrentIndex(idx);
        }
        free(ppOutputPortList);
      }
      jack_client_close(pJackClient);
  }
}

AudioPrefs::~AudioPrefs()
{
}
