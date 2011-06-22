/***************************************************************************
						main.cpp  -  description
						 -------------------
		begin                : Ne apr  1 11:28:13 CEST 2001
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

#include <KApplication>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KLocale>

#include "creox.h"
#include "privilegesswitcher.h"
#include "crsplashscreen.h"

static const char *description = "<b>creox</b> - The Realtime Sound Effector";

int main(int argc, char *argv[])
{
  PrivilegesSwitcher privilegesSwitcher;
  privilegesSwitcher.releasePrivileges();

  KAboutData aboutData("creox", 0, ki18n("creox"),
                       QByteArray("0.3.0"),
                       ki18n(description),
                       KAboutData::License_GPL,
                       ki18n("(c) 2003, Jozef Kosoru, (c) 2011, Kasper Laudrup"));

  aboutData.addAuthor(ki18n("Jozef Kosoru"), ki18n("Original author"), "jozef.kosoru@pobox.sk");
  aboutData.addAuthor(ki18n("Kasper Laudrup"), ki18n("KDE 4 port"), "laudrup@stacktrace.dk");
  aboutData.addCredit(ki18n("Claus Bjerre Pedersen"), ki18n("Tester, bug-reporter"),
                      "claus.bjerre@altavista.net");
  KCmdLineArgs::init(argc, argv, &aboutData);

  KApplication a;

  CrSplashScreen* const splashScreen = new CrSplashScreen(QString::fromLatin1("crsplash.png"));
  splashScreen->show();

  Creox *creox = new Creox();
  a.setMainWidget(creox);
  creox->show();

  return a.exec();
}
