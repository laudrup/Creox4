/***************************************************************************
						effectkeeper.h  -  description
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

#ifndef EFFECTKEEPER_H
#define EFFECTKEEPER_H

#include <QObject>
#include <QList>
#include "creffectgui.h"

class ThreadEffector;
class CrChainView;

/**
	*@author Jozef Kosoru
	*/
class EffectKeeper : public QObject  {
	 Q_OBJECT
public:
	EffectKeeper(QObject *parent=0, const char *name=0);
	~EffectKeeper();
	/** Add an effect to the chain. */
	int registerEffect(CrEffectGui* effect);
	/** Store pointer to the relevant chainView */
	void registerChainView(CrChainView* chainView) { m_chainView = chainView; }
	/** Create JACK device, threadEffector and add all registered effects
			to the effector chain. */
	void activate();
	/** Stop DSP processing. */
	void stop();
	/** Start DSP processing. */
	void start();
	/** get effect list */
	QList<CrEffectGui*>& effectList() { return m_effectList; }
	/** get threadEffector */
	ThreadEffector* threadEffector() const { return m_threadEffector; }
	/** get corresponding chainView */
	CrChainView* chainView() const { return m_chainView; }

	void shutdown();

private: // Private attributes
	QList<CrEffectGui*> m_effectList;
	ThreadEffector* m_threadEffector;
	CrChainView* m_chainView;
};

#endif
