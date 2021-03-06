/***************************************************************************
			crthreadeventdispatcher.cpp  -  description
				 -------------------
		begin                : Sun Aug 26 2001
		copyright            : (C) 2001 by Jozef Kosoru
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

#include <unistd.h>

#include <QString>
#include <QSocketNotifier>
#include <QApplication>
#include <QEvent>

#include <KLocale>

#include "crthreadeventdispatcher.h"

CrThreadEventDispatcher::CrThreadEventDispatcher(QApplication* app, const char* name)
	: QObject(app, name), m_ptrApp(app)
{
	if(::pipe(m_fds) == -1){
          //throw(Cr::CrException_runtimeError(i18n("Unable to create pipes!")));
	}
	m_notifier = new QSocketNotifier( m_fds[0], QSocketNotifier::Read);
	connect(m_notifier, SIGNAL(activated(int)), SLOT(eventHandler()));

	pthread_mutex_init(&m_eventLock, NULL);
}

CrThreadEventDispatcher::~CrThreadEventDispatcher()
{
  delete m_notifier;
  (void)::close(m_fds[0]);
  (void)::close(m_fds[1]);
  (void)pthread_mutex_destroy(&m_eventLock);
}

void CrThreadEventDispatcher::postEvent(QObject* receiver, QEvent* event)
{
  static const char* buf = "";

  pthread_mutex_lock(&m_eventLock);
  m_ptrReceiver = receiver;
  m_ptrEvent = event;
  pthread_mutex_unlock(&m_eventLock);

  if(::write(m_fds[1], buf, 1) == -1){
    //std::perror("Writing to pipe");
    pthread_exit(0);
  }
}

void CrThreadEventDispatcher::eventHandler()
{
  static char buf;
  if(::read(m_fds[0], &buf, 1) == -1){
    //throw(Cr::CrException_runtimeError(i18n("Unable to read from pipe!")));
  }

  pthread_mutex_lock(&m_eventLock);
  QApplication::postEvent(const_cast<QObject*>(m_ptrReceiver), const_cast<QEvent*>(m_ptrEvent));
  pthread_mutex_unlock(&m_eventLock);
}

