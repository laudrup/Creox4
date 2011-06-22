/***************************************************************************
				crsplashscreen.h  -  description
					 -------------------
		begin                : Sat Dec 29 2001
		copyright            : (C) 2001 by Jozef Kosoru
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

#ifndef CRSPLASHSCREEN_H
#define CRSPLASHSCREEN_H

#include <qwidget.h>
#include <QLabel>

/**
 *@author Jozef Kosoru
 */
class CrSplashScreen : public QLabel  {
  Q_OBJECT
public:
  CrSplashScreen(const QString& strPixmapName, const char *name=0);
  ~CrSplashScreen();

  static void setLoadOnStartup(const bool bLoad);
  static CrSplashScreen* getInstance() { return s_this; }
  static void removeSplashScreen() { if(s_this){ s_this->hide(); delete s_this; } }

public slots:
  virtual void show();

private:
  bool m_bLoadOnStartup;
  static CrSplashScreen* s_this;
};

#endif
