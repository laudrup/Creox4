/***************************************************************************
					crsplashscreen.cpp  -  description
							 -------------------
		begin                : Sat Dec 29 2001
		copyright            : (C) 2001 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation                          *
 *                                                                         *
 ***************************************************************************/

#include "control.h"
#include <iostream>
#include <qapplication.h>
#include <qstring.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QLabel>
#include <kconfig.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include "crsplashscreen.h"

CrSplashScreen* CrSplashScreen::s_this = 0;

CrSplashScreen::CrSplashScreen(const QString& strPixmapName, const char *name )
		: QLabel(0, name, Qt::WStyle_Customize | Qt::WStyle_NoBorder | Qt::WStyle_Tool
							| Qt::WStyle_StaysOnTop | Qt::WX11BypassWM | Qt::WNoAutoErase | Qt::WResizeNoErase)
{
	setBackgroundMode(Qt::NoBackground);

	KConfig* const conf = KGlobal::config();
	conf->setGroup(QString::fromLatin1("Splash_Screen"));
	m_bLoadOnStartup = conf->readBoolEntry("loadOnStartup", true);

	if(m_bLoadOnStartup){
		const KStandardDirs* const stdDirs = KGlobal::dirs();
		const QString strFileName(stdDirs->findResource("appdata", QString::fromLatin1("pics/") + strPixmapName));

		QPixmap pixmap(strFileName, "PNG");
		pixmap.setOptimization(QPixmap::NoOptim);

		setPixmap(pixmap);
		adjustSize();

		const QWidget* const desktop = QApplication::desktop();
		move((desktop->width() - width())/2, (desktop->height() - height())/2);
	}

	if(!s_this){
		s_this = this;
	}
}

CrSplashScreen::~CrSplashScreen()
{
#ifdef _DEBUG
	std::cerr << "CrSplashScreen deleted...\n";
#endif
}

void CrSplashScreen::show()
{
	if(m_bLoadOnStartup){
		QLabel::show();
		repaint(false);
		QApplication::flush();
	}
}

void CrSplashScreen::setLoadOnStartup(const bool bLoad)
{
	KConfig* const conf = KGlobal::config();
	conf->setGroup(QString::fromLatin1("Splash_Screen"));
	conf->writeEntry("loadOnStartup", bLoad);
}
