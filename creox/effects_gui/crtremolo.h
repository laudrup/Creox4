/***************************************************************************
						crtremolo.h  -  description
							 -------------------
		begin                : Thu Jan 11 2001
		copyright            : (C) 2001 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CRTREMOLO_H
#define CRTREMOLO_H

#include "creffectgui.h"
#include "tremoloprocessor.h"

class CrSliderArray;
class QRadioButton;

/**
	*@author Jozef Kosoru
	*/
class CrTremolo : public CrEffectGui  {
		Q_OBJECT
	public:
		CrTremolo(QWidget *parent=0, const char *name=0);
		~CrTremolo();
		virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }
	private:
		TremoloProcessor* m_tremolo;
		TremoloParameters* m_tpar;

		CrSliderArray* m_mixArray;
		CrSliderArray* m_tremoloArray;
		QRadioButton* m_lfoSine;
		QRadioButton* m_lfoTriangle;
	private slots:
		void slotChangeLfoType(int type);
	protected:
		virtual void restoreParameters();
		virtual void saveParameters();
	public:
		virtual TremoloProcessor* getProcessor() const { return m_tremolo; }
		virtual TremoloParameters* getParameters() const { return m_tpar; }
		virtual void synchronize();
};

#endif
