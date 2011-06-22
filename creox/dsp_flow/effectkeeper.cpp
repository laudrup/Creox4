/***************************************************************************
					effectkeeper.cpp  -  description
						 -------------------
		begin                : Sat Apr 14 2001
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

#include <Q3ValueList>
#include <QString>

#include <KGlobal>
#include <KConfig>
#include <KConfigGroup>

#include "effectkeeper.h"
#include "threadeffector.h"
#include "soundprocessor.h"

#include <QDebug>

EffectKeeper::EffectKeeper(QObject *parent, const char *name ) :
 QObject(parent,name), m_threadEffector(0), m_chainView(0)
{
  //m_effectList.setAutoDelete(false);
}

EffectKeeper::~EffectKeeper()
{
  //assert(!m_threadEffector);
}

void EffectKeeper::shutdown()
{
  //save the effectChain order
  KConfigGroup conf = KGlobal::config()->group("EffectKeeper");
  QList<int> effectIdList;
  const int chainSize = m_threadEffector->getProcessorChainSize();
  for(int count=0; count<chainSize; count++){
    conf.writeEntry(QString::fromLatin1((*m_threadEffector)[count]->getName()),
                    (*m_threadEffector)[count]->mode());
    effectIdList.append( (*m_threadEffector)[count]->getId() );
  }
  conf.writeEntry("effectChainOrder", effectIdList);

  delete m_threadEffector; //must be deleted first!
  m_threadEffector = 0;
}

/** Add an effect to the chain. */
int EffectKeeper::registerEffect(CrEffectGui* effect)
{
  m_effectList.append(effect);
  return static_cast<int>(m_effectList.count());
}

void EffectKeeper::activate()
{
  m_threadEffector = new ThreadEffector(m_effectList.count());

  //add effects to the threadEffector chain
  QList<int> effectIdList;
  KConfigGroup conf = KGlobal::config()->group("EffectKeeper");
  effectIdList = conf.readEntry("effectChainOrder", QList<int>());

  if(m_effectList.count() == effectIdList.count()){
    QList<int>::Iterator effectListIterator;
    int count;
    for(effectListIterator = effectIdList.begin(), count = 0; effectListIterator != effectIdList.end(); ++effectListIterator, count++){
      const int effectId = *effectListIterator;

      /*
      for(QListIterator<CrEffectGui*> effectIterator(m_effectList); effectIterator.value(); ++effectIterator){
        if(effectIterator.value()->getProcessor()->getId() == effectId){
          (*m_threadEffector)[count] = effectIterator.value()->getProcessor();
          (*m_threadEffector)[count]->setMode(conf.readEntry(QString::fromLatin1((*m_threadEffector)[count]->getName()), SoundProcessor::disabled));
          break;
        }
      }
      */
      QListIterator<CrEffectGui*> effectIterator(m_effectList);
      while (effectIterator.hasNext()) {
        CrEffectGui* effect = effectIterator.next();
        if (effect->getProcessor()->getId() == effectId) {
          (*m_threadEffector)[count] = effect->getProcessor();
          (*m_threadEffector)[count]->setMode(conf.readEntry(QString::fromLatin1((*m_threadEffector)[count]->getName()), int(SoundProcessor::disabled)));
          break;
        }
      }
    }
  } else {
    QListIterator<CrEffectGui*> effectIterator(m_effectList);
    for(int count=0; effectIterator.hasNext(); count++){
      (*m_threadEffector)[count] = effectIterator.next()->getProcessor();
    }
  }

  m_threadEffector->updateProcessorChain();
}

/** Start DSP processing. */
void EffectKeeper::start()
{
  qDebug() << "Start DSP processing";
  m_threadEffector->run();
}

/** Stop DSP processing. */
void EffectKeeper::stop()
{
  m_threadEffector->stop();
}
