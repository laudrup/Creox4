/***************************************************************************
						creox.cpp  -  description
							 -------------------
		begin                : Tue Dec 26 2000
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

#include <QString>
#include <QCustomEvent>
#include <QDockWidget>
#include <QTabWidget>
#include <QTimer>

#include <KApplication>
#include <KAction>
#include <KActionMenu>
#include <KActionCollection>
#include <KStandardAction>
#include <KLocale>
#include <KLocale>
#include <KGlobal>
#include <KMessageBox>
#include <KToolBar>
#include <KMenuBar>
#include <KIconLoader>
#include <KConfigDialog>

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
#include "crsplashscreen.h"
#include "settings.h"
#include "audioprefs.h"
#include "creox.h"

#include <QDebug>

namespace {
  const int g_iSplashScreenTimeOut = 1000; //ms
}

Creox::Creox(QWidget *parent)
  : KXmlGuiWindow(parent)
{
  setAutoSaveSettings();

  // create actions
  KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
  KStandardAction::preferences(this, SLOT(showPrefDialog()), actionCollection());

  m_playAction = new KAction(KIcon("media-playback-start"), i18n("&Play"), this);

  m_playAction->setShortcut(Qt::Key_Space);
  actionCollection()->addAction("playAction", m_playAction);
  connect(m_playAction, SIGNAL(triggered(bool)),
          SLOT(slotStartStopEffector()));

  const QString saveNewPresetString(i18n("&Save New Preset..."));
  m_savePresetAction = new KAction(KIcon("document-save"), saveNewPresetString, this);
  m_savePresetAction->setShortcut(Qt::CTRL + Qt::Key_S);
  actionCollection()->addAction("saveNewPresetAction", m_savePresetAction);
  connect(m_savePresetAction, SIGNAL(activated()), SLOT(slotSaveNewPreset()));

  m_newPresetFolderAction = new KAction(KIcon("document-new"), i18n("&New Preset Folder..."), this);
  m_newPresetFolderAction->setShortcut(Qt::CTRL + Qt::Key_N);
  actionCollection()->addAction("newPresetFolderAction", m_newPresetFolderAction);
  connect(m_newPresetFolderAction, SIGNAL(activated()), SLOT(slotNewPresetFolder()));

  KActionMenu* const presetActionMenu = new KActionMenu(KIcon("document-save"), saveNewPresetString, this);
  actionCollection()->addAction("presetActionMenu", presetActionMenu);
  connect(presetActionMenu, SIGNAL(activated()), SLOT(slotSaveNewPreset()));

  setupGUI();

  m_effectKeeper = new EffectKeeper(this, "m_effectKeeper");

  initEffectsGui();

  m_effectKeeper->activate();
  m_chainView->activate();
  m_presetView->loadPresets();

  // event dispatcher catches exceptions from the working dsp thread
  m_ptrEventDispatcher = new CrThreadEventDispatcher(kapp);

  // start timer to remove splashScreen
  QTimer::singleShot(g_iSplashScreenTimeOut, this, SLOT(removeSplashScreen()));
}

Creox::~Creox()
{
  if (m_effectKeeper->threadEffector()->getStatus() == ThreadEffector::status_Run) {
    m_effectKeeper->stop();
  }

  m_presetView->savePresets();
  m_effectKeeper->shutdown();
  Settings::self()->writeConfig();
}

void Creox::removeSplashScreen()
{
  CrSplashScreen::removeSplashScreen();
}

/** Creates the effects widgets. */
void Creox::initEffectsGui()
{
  //KIconLoader* iconLoader = KIconLoader::global();

  /* docking */
  //QDockWidget* mainDock = new QDockWidget(this); //0L;

  //addDockWidget(Qt::LeftDockWidgetArea, mainDock);

  //setCentralWidget(mainDock);
  //setMainDockWidget(mainDock);

  //KDockWidget* mainDock = 0L;
  //setView(mainDock);
  //setMainDockWidget(mainDock);

  /* Distortion */
  /*
  KDockWidget* dockDistortion = createDockWidget("Distortion",
                                                 iconLoader->loadIcon("distortion",
                                                                      KIcon::Small),
                                                 0L,
                                                 i18n("Distortion"));
  */

  QDockWidget* dockDistortion = new QDockWidget(i18n("Distortion"), this);
  dockDistortion->setObjectName("distortion");
  /*
                                                 iconLoader->loadIcon("distortion",
                                                                      KIcon::Small),
                                                 0L,
                                                 i18n("Distortion"));
  */

  CrEffectGuiTray* trayDistortion = new CrEffectGuiTray(dockDistortion);
  CrDistortion* distortion = new CrDistortion(trayDistortion);
  trayDistortion->insertEffectGuiWidget(distortion);
  dockDistortion->setWidget(trayDistortion);
  dockDistortion->setWindowTitle(distortion->effectName());

  //dockDistortion->setWindowIcon(KIcon("media-playback-start"));//, iconLoader));

  // dockDistortion->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
  //                             QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
  addDockWidget(Qt::LeftDockWidgetArea, dockDistortion);
  setTabPosition (Qt::LeftDockWidgetArea, QTabWidget::North);
  //tabifyDockWidget(dockDistortion)

  //dockDistortion->setTabPageLabel(const QString& label);
  //dockDistortion->manualDock(mainDock, KDockWidget:: DockLeft, 50);
  //setView(dockDistortion);
  //dockDistortion->setDockSite(KDockWidget::DockCenter);
  //mainDock = dockDistortion;
  //setMainDockWidget(mainDock);

  /* Phaser */
  /*
  KDockWidget* dockPhaser = createDockWidget("Phaser",
                                             iconLoader->loadIcon("phaser",
                                                                  KIcon::Small),
                                             0L,
                                             i18n("Phaser"));
  */
  QDockWidget* dockPhaser = new QDockWidget(i18n("Phaser"), this);
  dockPhaser->setObjectName("phaser");

  CrEffectGuiTray* trayPhaser = new CrEffectGuiTray(dockPhaser);
  CrPhaser* phaser = new CrPhaser(trayPhaser);
  trayPhaser->insertEffectGuiWidget(phaser);
  dockPhaser->setWidget(trayPhaser);
  //dockPhaser->setCaption(phaser->effectName());
  //dockPhaser->manualDock(mainDock, KDockWidget::DockCenter, 100);
  // dockPhaser->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable |
  //                             QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar);
  //addDockWidget(Qt::LeftDockWidgetArea, dockPhaser);
  tabifyDockWidget(dockDistortion, dockPhaser);

  //new dockHeader
  //KDockWidgetHeader* dockHeaderPhaser = new KDockWidgetHeader(dockPhaser);
  //dockHeaderPhaser->setDragEnabled(false);
  //dockHeaderPhaser->setTopLevel(false);
  //dockPhaser->setHeader(dockHeaderPhaser);
  //CrEffectGuiTray* trayPhaser = new CrEffectGuiTray(dockPhaser);
  //CrPhaser* phaser = new CrPhaser(trayPhaser);
  //trayPhaser->insertEffectGuiWidget(phaser);
  //dockPhaser->setWidget(trayPhaser);
  //dockPhaser->setCaption(phaser->effectName());
  //dockPhaser->manualDock(mainDock, KDockWidget::DockCenter, 100);

  /* Flanger */
  /*
  KDockWidget* dockFlanger = createDockWidget("Flanger",
                                              iconLoader->loadIcon("flanger",
                                                                   KIcon::Small),
                                              0L,
                                              i18n("Flanger"));
  */

  QDockWidget* dockFlanger = new QDockWidget(i18n("Flanger"), this);
  dockFlanger->setObjectName("flanger");

  CrEffectGuiTray* trayFlanger = new CrEffectGuiTray(dockFlanger);
  CrFlanger* flanger = new CrFlanger(trayFlanger);
  trayFlanger->insertEffectGuiWidget(flanger);
  dockFlanger->setWidget(trayFlanger);
  //dockFlanger->setCaption(flanger->effectName());
  //dockFlanger->manualDock(mainDock, KDockWidget:: DockCenter, 100 );
  tabifyDockWidget(dockDistortion, dockFlanger);

  /* Tremolo */

  QDockWidget* dockTremolo = new QDockWidget(i18n("Tremolo"), this);
  dockTremolo->setObjectName("tremolo");

  /*
  KDockWidget* dockTremolo = createDockWidget("Tremolo",
                                              iconLoader->loadIcon("tremolo",
                                                                   KIcon::Small),
                                              0L,
                                              i18n("Tremolo"));
  */

  CrEffectGuiTray* trayTremolo = new CrEffectGuiTray(dockTremolo);
  CrTremolo* tremolo = new CrTremolo(trayTremolo);
  trayTremolo->insertEffectGuiWidget(tremolo);
  dockTremolo->setWidget(trayTremolo);
  //dockTremolo->setCaption(tremolo->effectName());
  //dockTremolo->manualDock(mainDock, KDockWidget:: DockCenter, 100 );
  tabifyDockWidget(dockDistortion, dockTremolo);

  /* Echo */
  QDockWidget* dockEcho = new QDockWidget(i18n("Echo"), this);
  dockEcho->setObjectName("echo");

  /*
                                           iconLoader->loadIcon("echo",
                                                                KIcon::Small),
                                           0L,
                                           i18n("Echo"));
  */

  CrEffectGuiTray* trayEcho = new CrEffectGuiTray(dockEcho);
  CrEcho* echo = new CrEcho(trayEcho);
  trayEcho->insertEffectGuiWidget(echo);
  dockEcho->setWidget(trayEcho);
  //dockEcho->setCaption(echo->effectName());
  //mainDock = dockEcho->manualDock(mainDock, KDockWidget:: DockCenter, 100 );
  tabifyDockWidget(dockDistortion, dockEcho);


  /* ChainView */

  QDockWidget* dockChainView = new QDockWidget(i18n("ChainView"), this);
  dockChainView->setObjectName("chainview");
  /*
  KDockWidget* dockChainView = createDockWidget("ChainView",
                                                iconLoader->loadIcon("preset_chain",
                                                                     KIcon::Small),
                                                0L,
                                                i18n("ChainView"));
  */

  m_chainView = new CrChainView(m_effectKeeper, dockChainView);
  //chainView->setMinimumWidth(70);
  dockChainView->setWidget(m_chainView);
  //dockChainView->manualDock(mainDock, KDockWidget:: DockRight, 60);
  addDockWidget(Qt::RightDockWidgetArea, dockChainView);

  /* PresetView */
  QDockWidget* dockPresetView = new QDockWidget(i18n("PresetView"), this);
  dockPresetView->setObjectName("presetview");
  /*
                                                 iconLoader->loadIcon("preset_chain",
                                                                      KIcon::Small),
                                                 0L,
                                                 i18n("PresetView"));
  */
  m_presetView = new CrPresetView(m_effectKeeper, dockPresetView);
  dockPresetView->setWidget(m_presetView);
  //dockPresetView->manualDock(dockChainView, KDockWidget:: DockBottom, 40);
  addDockWidget(Qt::RightDockWidgetArea, dockPresetView);

  //register effects
  m_effectKeeper->registerEffect(distortion);
  m_effectKeeper->registerEffect(phaser);
  m_effectKeeper->registerEffect(flanger);
  m_effectKeeper->registerEffect(tremolo);
  m_effectKeeper->registerEffect(echo);

  //register chainView
  m_effectKeeper->registerChainView(m_chainView);
}

/** start / stop the effector engine */
void Creox::slotStartStopEffector()
{
  //if(m_playAction->isChecked()){
    m_effectKeeper->start();
    /*  } else {
    m_effectKeeper->stop();
    } */
}

/** save a new preset */
void Creox::slotSaveNewPreset()
{
  CrSaveNewPresetDialog newPresetDialog(m_effectKeeper, m_presetView, this);
  newPresetDialog.exec();
}

/** create a new preset folder */
void Creox::slotNewPresetFolder()
{
  CrNewPresetFolderDialogImpl newFolderDialog(m_presetView, this);
  newFolderDialog.exec();
}

void Creox::customEvent(QEvent* event)
{
    if(static_cast<int>(event->type()) == CrMessageEvent::ErrorMessageEvent) {
      KMessageBox::error(0, static_cast<CrMessageEvent*>(event)->messageText());
      m_effectKeeper->stop();
      m_playAction->setChecked(false);
    }
}

void Creox::showPrefDialog()
{
  if(KConfigDialog::showDialog("settings"))
    return;
  KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
  dialog->setFaceType(KPageDialog::List);
  AudioPrefs* audioPrefs = new AudioPrefs(0);
  dialog->addPage(audioPrefs, i18n("Jack audio configuration"), "sound_card_properties");
  // XXX: Connect this signal to restart sound service
  //connect(dialog, SIGNAL(settingsChanged(const QString&)), audioPrefs, SLOT(loadSettings()));
  dialog->show();
}
