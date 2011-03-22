/***************************************************************************
						crecho.h  -  description
						 -------------------
		begin                : Thu Jan 11 2001
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

#ifndef CRECHO_H
#define CRECHO_H

#include <qstringlist.h>
#include "creffectgui.h"
#include "echoprocessor.h"

class CrSliderArray;
class QSpinBox;
class QListBox;
class QVGroupBox;
class CrEchoTapsView;

/**
	*@author Jozef Kosoru
	*/
class CrEcho : public CrEffectGui  {
		Q_OBJECT
	public:
		CrEcho(QWidget *parent=0, const char *name=0);
		~CrEcho();
		virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }
	private:
		EchoProcessor* m_echo;
		EchoParameters* m_epar;

		CrSliderArray* m_mixArray;
		CrSliderArray* m_finalEchoArray;
		CrSliderArray* m_voiceArray;
		QVGroupBox* m_voiceBox;
		QSpinBox* m_voiceNumSpinBox;
		QListBox* m_voicesListBox;
		QStringList m_voicesStringList;
		int m_visibleVoice;
		CrEchoTapsView* m_echoTapsView;
		void updateVoicesListBox();
	private slots:
		void slotChangeVoicesNum(int voicesNum);
		void slotChangeSelectedVoice(int voice);
	protected:
		virtual void restoreParameters();
		virtual void saveParameters();
	public:
		virtual EchoProcessor* getProcessor() const { return m_echo; }
		virtual EchoParameters* getParameters() const { return m_epar; }
		virtual void synchronize();
};

#endif
