/***************************************************************************
						crphaser.h  -  description
							 -------------------
		begin                : Sun Dec 31 2000
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

#ifndef CRPHASER_H
#define CRPHASER_H

#include "creffectgui.h"
#include "phaserprocessor.h"

class CrSliderArray;

/**
	*@author Jozef Kosoru
	*/
class CrPhaser : public CrEffectGui  {
		Q_OBJECT
	public:
		CrPhaser(QWidget *parent=0, const char *name=0);
		~CrPhaser();
		virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }
	private:
		PhaserProcessor* m_phaser;
		PhaserParameters* m_ppar;

		CrSliderArray* m_mixArray;
		CrSliderArray* m_sweepArray;
	protected:
		virtual void restoreParameters();
		virtual void saveParameters();
	public:
		virtual PhaserProcessor* getProcessor() const { return m_phaser; }
		virtual PhaserParameters* getParameters() const { return m_ppar; }
		virtual void synchronize();
};

#endif
