/***************************************************************************
				crthreadeventdispatcher.h  -  description
						 -------------------
		begin                : Sun Aug 26 2001
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

#ifndef CRTHREADEVENTDISPATCHER_H
#define CRTHREADEVENTDISPATCHER_H

#include <pthread.h>
#include <QObject>
#include <QEvent>

class QApplication;
class QSocketNotifier;

/**Dispatches events between a working thread and the main GIU thread.
	*This class implements roughly the same functionallity as
	*QThread::postEvent(...) function, which is available in the
	*multi-threaded version of the library only.
	*@author Jozef Kosoru
	*/
class CrThreadEventDispatcher : public QObject  {
	Q_OBJECT
public:
  CrThreadEventDispatcher(QApplication* app, const char* name = 0);
  ~CrThreadEventDispatcher();
  void postEvent(QObject* receiver, QEvent* event);
protected slots:
  void eventHandler();
private:
  QApplication* m_ptrApp;
  QSocketNotifier* m_notifier;
  int m_fds[2];
  pthread_mutex_t m_eventLock;
  volatile QObject* m_ptrReceiver;
  volatile QEvent* m_ptrEvent;
};

#endif
