/***************************************************************************
						creox.h  -  description
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

#ifndef CREOX_H
#define CREOX_H

#include <QEvent>
#include <QWidget>
#include <QCustomEvent>

#include <KXmlGuiWindow>

class QString;
class QTimerEvent;
class KAction;
class EffectKeeper;
class CrChainView;
class CrPresetView;
class CrPresetPopupMenu;
class CrThreadEventDispatcher;

/**
 * This class implements custom event for dispatching messages from
 * working threads to the main GUI thread.
 * @author Jozef Kosoru
 */
class CrMessageEvent : public QCustomEvent {
 public:
  enum Type { ErrorMessageEvent = User + 31333 };

 CrMessageEvent(const QString strMessage, const int type = ErrorMessageEvent)
   : QCustomEvent(type), m_strMessage(strMessage) { }

  QString messageText() { return m_strMessage; }

 private:
  QString m_strMessage;
};

/**
 *@author Jozef Kosoru
 */
class Creox : public KXmlGuiWindow {
  Q_OBJECT

 public:
  Creox(QWidget *parent=0);
  ~Creox();
  CrThreadEventDispatcher* getEventDispatcher() const { return m_ptrEventDispatcher; }

  public slots:
  /** start / stop the effector engine */
  void slotStartStopEffector();
  /** save a new preset */
  void slotSaveNewPreset();
  /** create a new preset folder */
  void slotNewPresetFolder();
  void removeSplashScreen();

 private:
  /** Creates the effects widgets. */
  void initEffectsGui();
  /** An ugly fix for min effects width */
  void fixEffectsWidth();

  friend class CrPresetPopupMenu;

  KAction* m_playAction;
  KAction* m_savePresetAction;
  KAction* m_newPresetFolderAction;

  EffectKeeper* m_effectKeeper;
  CrChainView* m_chainView;
  CrPresetView* m_presetView;

  CrThreadEventDispatcher* m_ptrEventDispatcher;

 private slots:
  void showPrefDialog();

protected:
  /** process custom events, especially CrMessageEvent */
  virtual void customEvent(QEvent* event);
};

#endif
