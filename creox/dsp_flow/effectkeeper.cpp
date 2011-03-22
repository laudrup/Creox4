/***************************************************************************
					effectkeeper.cpp  -  description
						 -------------------
		begin                : Sat Apr 14 2001
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
#include <cassert>
#include <iostream>
#include <qvaluelist.h>
#include <qstring.h>
#include <kglobal.h>
#include <kconfig.h>
#include "effectkeeper.h"
#include "threadeffector.h"
#include "soundprocessor.h"

EffectKeeper::EffectKeeper(QObject *parent, const char *name ) :
 QObject(parent,name), m_threadEffector(0), m_chainView(0)
{
	m_effectList.setAutoDelete(false);
}

EffectKeeper::~EffectKeeper()
{
	assert(!m_threadEffector);

#ifdef _DEBUG
	std::cerr << "EffectKeeper deleted...\n";
#endif
}

void EffectKeeper::shutdown()
{
	//save the effectChain order
	KConfig* conf = KGlobal::config();
	conf->setGroup("EffectKeeper");
	QValueList<int> effectIdList;
	const int chainSize = m_threadEffector->getProcessorChainSize();
	for(int count=0; count<chainSize; count++){
		conf->writeEntry(QString::fromLatin1((*m_threadEffector)[count]->getName()),
						 (*m_threadEffector)[count]->mode());
		effectIdList.append( (*m_threadEffector)[count]->getId() );
	}
	conf->writeEntry("effectChainOrder", effectIdList);

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
	QValueList<int> effectIdList;
	KConfig* conf = KGlobal::config();
	conf->setGroup("EffectKeeper");
	effectIdList = conf->readIntListEntry("effectChainOrder");

	if(m_effectList.count() == effectIdList.count()){
		QValueList<int>::Iterator effectListIterator;
		int count;
		for(effectListIterator = effectIdList.begin(), count = 0; effectListIterator != effectIdList.end(); ++effectListIterator, count++){
			const int effectId = *effectListIterator;

			for(QPtrListIterator<CrEffectGui> effectIterator(m_effectList); effectIterator.current(); ++effectIterator){
				if(effectIterator.current()->getProcessor()->getId() == effectId){
					(*m_threadEffector)[count] = effectIterator.current()->getProcessor();
					(*m_threadEffector)[count]->setMode(conf->readNumEntry(QString::fromLatin1((*m_threadEffector)[count]->getName()), SoundProcessor::disabled));
					break;
				}
			}
		}
	}
	else{
		QPtrListIterator<CrEffectGui> effectIterator(m_effectList);
		for(int count=0; effectIterator.current(); ++effectIterator, count++){
			(*m_threadEffector)[count] = effectIterator.current()->getProcessor();
		}
	}

	m_threadEffector->updateProcessorChain();
}

/** Start DSP processing. */
void EffectKeeper::start()
{
	m_threadEffector->run();
}

/** Stop DSP processing. */
void EffectKeeper::stop()
{
	m_threadEffector->stop();
}
