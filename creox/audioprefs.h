/***************************************************************************
						audioprefs.h  -  description
						 -------------------
		begin                : Tue Jun 21 2011
		copyright            : (C) 2011 by Kasper Laudrup
                                     : (C) 2011 by Kasper Laudrup
		email                : laudrup@stacktrace.dk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef AUDIOPREFS_H
#define AUDIOPREFS_H

#include "settings.h"
#include "ui_audioprefs.h"

class AudioPrefs : public QWidget, public Ui_AudioPrefs  {
  Q_OBJECT
 public:
  AudioPrefs(QWidget* parent);
  virtual ~AudioPrefs();
};

#endif
