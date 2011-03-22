/***************************************************************************
						creox.cpp  -  description
							 -------------------
		begin                : Tue Dec 26 2000
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

#include "control.h"
#include <iostream>
#include <qstring.h>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <QCustomEvent>
#include <QTimerEvent>
#include <klocale.h>
#include <kglobal.h>
#include <kmessagebox.h>
//#include <kconfig.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kmenubar.h>
#include <kapplication.h>
#include <kaction.h>
#include <kiconloader.h>
#include "crdistortion.h"
#include "crphaser.h"
#include "crflanger.h"
#include "crtremolo.h"
#include "crecho.h"
#include "creffectguitray.h"
#include "creffectgui.h"
#include "effectkeeper.h"
#include "crchainview.h"
#include "crpresetview.h"
#include "crnewpresetfolderdialogimpl.h"
#include "crsavenewpresetdialog.h"
#include "crthreadeventdispatcher.h"
#include "crexception.h"
#include "croptionsdialog.h"
#include "crsplashscreen.h"
#include "creox.h"

namespace{
const int g_iSplashScreenTimeOut = 1000; //ms
}

Creox::Creox(QWidget *parent, const char *name)
  : KMainWindow(parent)
{
  // XXX: set name

  // XXX!
  setAutoSaveSettings();

  // create actions
  /*
	KStandardAction::quit(kapp, SLOT(closeAllWindows()), actionCollection());
	m_playAction = new KToggleAction(i18n("&Play"), QString::fromLatin1("run"),
									 Qt::Key_Space, this,
									 SLOT(slotStartStopEffector()),
									 actionCollection(), "playAction");
	m_optionsAction = new KAction(i18n("&Options..."),
								  QString::fromLatin1("configure"),
								  Qt::CTRL+Qt::Key_O, this, SLOT(slotOptions()),
								  actionCollection(), "optionsAction");

	const QString saveNewPresetString(i18n("&Save New Preset..."));
	m_savePresetAction = new KAction(saveNewPresetString,
									 QString::fromLatin1("filesave"),
									 Qt::CTRL+Qt::Key_S, this, SLOT(slotSaveNewPreset()),
									 actionCollection(), "saveNewPresetAction");
	KActionMenu* const presetActionMenu =
						new KActionMenu(saveNewPresetString,
										QString::fromLatin1("filesave"),
										actionCollection(), "presetActionMenu");

	m_newPresetFolderAction = new KAction(i18n("&New Preset Folder..."),
										  QString::fromLatin1("folder_new"),
										  Qt::CTRL+Qt::Key_N, this,
										  SLOT(slotNewPresetFolder()),
										  actionCollection(),
										  "newPresetFolderAction");
	presetActionMenu->insert(m_newPresetFolderAction);
	connect(presetActionMenu, SIGNAL(activated()), this,
					SLOT(slotSaveNewPreset()));
	createGUI();

	menuBar()->insertItem(i18n("&View"), dockHideShowMenu(), -1, 1);

	m_effectKeeper = new EffectKeeper(this, "m_effectKeeper");

	initEffectsGui();

	m_effectKeeper->activate();
	m_chainView->activate();

	try{
			m_presetView->loadPresets();
	}catch(Cr::CrException_presetDataFileError& error){
		KMessageBox::error(0, error.what());
#ifdef _DEBUG
		std::cerr << error.what().latin1() << "\n";
#endif
	}

	//!! preliminary fix !!
	fixEffectsWidth();

	// event dispatcher catches exceptions from the working dsp thread
	m_ptrEventDispatcher = new CrThreadEventDispatcher(kapp);

	// start timer to remove splashScreen
	startTimer(g_iSplashScreenTimeOut);

	readDockConfig();
  */
}

Creox::~Creox()
{
  //try {
#if 0

		if (m_effectKeeper->threadEffector()->getStatus() ==
													 ThreadEffector::status_Run)
		{
			m_effectKeeper->stop();
		}

		m_presetView->savePresets();
		m_effectKeeper->shutdown();
#endif
                /*	}
	catch(Cr::CrException_presetDataFileError& error){
		KMessageBox::error(0, error.what());
#ifdef _DEBUG
		std::cerr << error.what().latin1() << "\n";
#endif
} */

	// there are so many bugs in the kdelibs dock implementation,
	// that I can't set this option on
                // XXX!
	//writeDockConfig();
#ifdef _DEBUG
	std::cerr << "Creox deleted..." << "\n";
#endif
}

void Creox::timerEvent(QTimerEvent*)
{
  // XXX!
  //killTimers();
	CrSplashScreen::removeSplashScreen();
}

/** Creates the effects widgets. */
void Creox::initEffectsGui()
{
  KIconLoader* iconLoader = KIconLoader::global();
#if 0

	/* docking */
	KDockWidget* mainDock = 0L;
	//setView(mainDock);
	//setMainDockWidget(mainDock);

	/* Distortion */
	KDockWidget* dockDistortion = createDockWidget("Distortion",
												   iconLoader->loadIcon("distortion",
												   						KIcon::Small),
												   0L,
												   i18n("Distortion"));
	CrEffectGuiTray* trayDistortion = new CrEffectGuiTray(dockDistortion);
	CrDistortion* distortion = new CrDistortion(trayDistortion);
	trayDistortion->insertEffectGuiWidget(distortion);
	dockDistortion->setWidget(trayDistortion);
	dockDistortion->setCaption(distortion->effectName());
	//dockDistortion->setTabPageLabel(const QString& label);
	//dockDistortion->manualDock(mainDock, KDockWidget:: DockLeft, 50);
	setView(dockDistortion);
	//dockDistortion->setDockSite(KDockWidget::DockCenter);
	mainDock = dockDistortion;
	setMainDockWidget(mainDock);

	/* Phaser */
	KDockWidget* dockPhaser = createDockWidget("Phaser",
											   iconLoader->loadIcon("phaser",
											   						KIcon::Small),
											   0L,
											   i18n("Phaser"));
#if 0
	//new dockHeader
	KDockWidgetHeader* dockHeaderPhaser = new KDockWidgetHeader(dockPhaser);
	dockHeaderPhaser->setDragEnabled(false);
	dockHeaderPhaser->setTopLevel(false);
	dockPhaser->setHeader(dockHeaderPhaser);
#endif
	CrEffectGuiTray* trayPhaser = new CrEffectGuiTray(dockPhaser);
	CrPhaser* phaser = new CrPhaser(trayPhaser);
	trayPhaser->insertEffectGuiWidget(phaser);
	dockPhaser->setWidget(trayPhaser);
	dockPhaser->setCaption(phaser->effectName());
	dockPhaser->manualDock(mainDock, KDockWidget::DockCenter, 100);

	/* Flanger */
	KDockWidget* dockFlanger = createDockWidget("Flanger",
												iconLoader->loadIcon("flanger",
																	 KIcon::Small),
												0L,
												i18n("Flanger"));
	CrEffectGuiTray* trayFlanger = new CrEffectGuiTray(dockFlanger);
	CrFlanger* flanger = new CrFlanger(trayFlanger);
	trayFlanger->insertEffectGuiWidget(flanger);
	dockFlanger->setWidget(trayFlanger);
	dockFlanger->setCaption(flanger->effectName());
	dockFlanger->manualDock(mainDock, KDockWidget:: DockCenter, 100 );

	/* Tremolo */
	KDockWidget* dockTremolo = createDockWidget("Tremolo",
												iconLoader->loadIcon("tremolo",
																	 KIcon::Small),
												0L,
												i18n("Tremolo"));
	CrEffectGuiTray* trayTremolo = new CrEffectGuiTray(dockTremolo);
	CrTremolo* tremolo = new CrTremolo(trayTremolo);
	trayTremolo->insertEffectGuiWidget(tremolo);
	dockTremolo->setWidget(trayTremolo);
	dockTremolo->setCaption(tremolo->effectName());
	dockTremolo->manualDock(mainDock, KDockWidget:: DockCenter, 100 );

	/* Echo */
	KDockWidget* dockEcho = createDockWidget("Echo",
											 iconLoader->loadIcon("echo",
											 					  KIcon::Small),
											 0L,
											 i18n("Echo"));
	CrEffectGuiTray* trayEcho = new CrEffectGuiTray(dockEcho);
	CrEcho* echo = new CrEcho(trayEcho);
	trayEcho->insertEffectGuiWidget(echo);
	dockEcho->setWidget(trayEcho);
	dockEcho->setCaption(echo->effectName());
	mainDock = dockEcho->manualDock(mainDock, KDockWidget:: DockCenter, 100 );

	/* ChainView */
	KDockWidget* dockChainView = createDockWidget("ChainView",
												  iconLoader->loadIcon("preset_chain",
												  					   KIcon::Small),
												  0L,
												  i18n("ChainView"));
	m_chainView = new CrChainView(m_effectKeeper, dockChainView);
	//chainView->setMinimumWidth(70);
	dockChainView->setWidget(m_chainView);
	dockChainView->manualDock(mainDock, KDockWidget:: DockRight, 60);

	/* PresetView */
	KDockWidget* dockPresetView = createDockWidget("PresetView",
												   iconLoader->loadIcon("preset_chain",
												   						KIcon::Small),
												   0L,
												   i18n("PresetView"));
	m_presetView = new CrPresetView(m_effectKeeper, dockPresetView);
	dockPresetView->setWidget(m_presetView);
	dockPresetView->manualDock(dockChainView, KDockWidget:: DockBottom, 40);

	//register effects
	m_effectKeeper->registerEffect(distortion);
	m_effectKeeper->registerEffect(phaser);
	m_effectKeeper->registerEffect(flanger);
	m_effectKeeper->registerEffect(tremolo);
	m_effectKeeper->registerEffect(echo);

	//register chainView
	m_effectKeeper->registerChainView(m_chainView);
#endif
}

/** start / stop the effector engine */
void Creox::slotStartStopEffector()
{
  /*
	if(m_playAction->isChecked()){
		m_optionsAction->setEnabled(false);
		m_effectKeeper->start();
	}
	else{
		m_effectKeeper->stop();
		m_optionsAction->setEnabled(true);
	}
  */
}

/** save a new preset */
void Creox::slotSaveNewPreset()
{
#ifdef _DEBUG
	std::cerr << "Creox::slotSaveNewPreset\n";
#endif
	CrSaveNewPresetDialog newPresetDialog(m_effectKeeper, m_presetView, this);
	newPresetDialog.exec();
}

/** create a new preset folder */
void Creox::slotNewPresetFolder()
{
#ifdef _DEBUG
	std::cerr << "Creox::slotNewPresetFolder\n";
#endif
	CrNewPresetFolderDialogImpl newFolderDialog(m_presetView, this);
	newFolderDialog.exec();
}

/** An ugly fix for min effects width */
void Creox::fixEffectsWidth()
{
  /*
	int minWidth = 0;
	for(Q3PtrListIterator<CrEffectGui> effectIterator(m_effectKeeper->effectList());
	effectIterator.current(); ++effectIterator){
		const int curWidth = effectIterator.current()->sizeHint().width();
		if(curWidth > minWidth){
			minWidth = curWidth;
		}
	}
	for(Q3PtrListIterator<CrEffectGui> effectIterator(m_effectKeeper->effectList());
	effectIterator.current(); ++effectIterator){
		effectIterator.current()->setMinimumWidth(minWidth);
	}
  */
}

void Creox::customEvent(QCustomEvent* event)
{
  /*
	if(static_cast<int>(event->type()) == CrMessageEvent::ErrorMessageEvent) {
		KMessageBox::error(0, static_cast<CrMessageEvent*>(event)->messageText());
		m_effectKeeper->stop();
		m_playAction->setChecked(false);
		m_optionsAction->setEnabled(true);
	}
  */
}

/** Options action. */
void Creox::slotOptions()
{
	CrOptionsDialog optionsDialog(this, "optionsDialog");
	optionsDialog.exec();
}

