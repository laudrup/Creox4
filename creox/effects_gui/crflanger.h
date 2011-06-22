/***************************************************************************
						crflanger.h  -  description
							 -------------------
		begin                : Mon Jan 1 2001
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

#ifndef CRFLANGER_H
#define CRFLANGER_H

#include "creffectgui.h"
#include "flangerprocessor.h"

class QRadioButton;
class QCheckBox;
class CrSliderArray;

/**
	*@author Jozef Kosoru
	*/
class CrFlanger : public CrEffectGui  {
		Q_OBJECT
	public:
		CrFlanger(QWidget *parent=0, const char *name=0);
		~CrFlanger();
		virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }
	private:
		FlangerProcessor* m_flanger;
		FlangerParameters* m_fpar;

		CrSliderArray* m_mixArray;
		CrSliderArray* m_flangerArray;
		QRadioButton* m_lfoSine;
		QRadioButton* m_lfoTriangle;
		QCheckBox* m_checkInvertBox;
	private slots:
		void slotChangeLfoType(int type);
		void slotChangeInverted(bool inverted);
	protected:
		virtual void restoreParameters();
		virtual void saveParameters();
	public:
		virtual FlangerProcessor* getProcessor() const { return m_flanger; }
		virtual FlangerParameters* getParameters() const { return m_fpar; }
		virtual void synchronize();
};

#endif
