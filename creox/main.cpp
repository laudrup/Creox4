/***************************************************************************
						main.cpp  -  description
						 -------------------
		begin                : Ne apr  1 11:28:13 CEST 2001
		copyright            : (C) 2001 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#include "control.h"
#include <stdexcept>
#include <QMessageBox>
#include <QApplication>
#include <QString>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <KAboutData>
#include <klocale.h>
#include "creox.h"
#include "privilegesswitcher.h"
#include "crexception.h"
#include "crsplashscreen.h"

namespace Cr {
  void fatalMessage(int argc, char *argv[], const QString& strErrorText);
}

static const char *description = "<b>creox</b> - The Realtime Sound Effector";

/*
  static KCmdLineOptions options[] =
  {
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
  };
*/

int main(int argc, char *argv[])
{
  //try{
  PrivilegesSwitcher privilegesSwitcher;
  privilegesSwitcher.releasePrivileges();

  KAboutData aboutData("creox", 0, ki18n("creox"),
                       QByteArray("0.3"), //VERSION, // XXX!
                       ki18n(description),
                       KAboutData::License_GPL,
                       ki18n("(c) 2003, Jozef Kosoru"));

  aboutData.addAuthor(ki18n("Jozef Kosoru"), ki18n("Original author"), "jozef.kosoru@pobox.sk");
  aboutData.addCredit(ki18n("Claus Bjerre Pedersen"), ki18n("Tester, bug-reporter"), "claus.bjerre@altavista.net");
  //aboutData.setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"),
  //						I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));
  KCmdLineArgs::init( argc, argv, &aboutData );
  //KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;

  //CrSplashScreen* const splashScreen = new CrSplashScreen(QString::fromLatin1("crsplash.png"));
  //splashScreen->show();

  Creox *creox = new Creox();
  a.setMainWidget(creox);
  creox->show();

  return a.exec();

  // As far I know, each KDE distribution is compiled with
  // no exception support, so catching exceptions in this manner
  // doesn't work for now.
  /*
    }catch(Cr::CrException& exceptError){
    Cr::fatalMessage(argc, argv, exceptError.what());
    return -1;
    }catch(std::exception& stdExceptError){
    Cr::fatalMessage(argc, argv, QString::fromLatin1(stdExceptError.what()));
    return -1;
    }
  */
}

void Cr::fatalMessage(int argc, char *argv[], const QString& strErrorText)
{
  QApplication fatalApp(argc, argv);
  QMessageBox::about(0, i18n("creox - Fatal Error!"), strErrorText);
}
