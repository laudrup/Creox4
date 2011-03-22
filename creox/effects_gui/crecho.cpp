/***************************************************************************
						crecho.cpp  -  description
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

#include "control.h"
#include <cassert>
#include <iostream>
#include <qlayout.h>
#include <q3vgroupbox.h>
#include <qspinbox.h>
#include <qlabel.h>
//#include <qvbox.h>
#include <qstringlist.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include "crvalidator.h"
#include "crsliderarray.h"
#include "crecho.h"
#include "crechotapsview.h"

CrEcho::CrEcho(QWidget *parent, const char *name )
		: CrEffectGui(parent,name)
{
	setEffectName(i18n("Echo"));
	m_epar = new EchoParameters;
	restoreParameters();

	m_echo = new EchoProcessor(m_epar);

	CrValidator* inputGainValid = new CrValidator(-15.0f, 15.0f, 0.5f, this);
	CrValidator* mixValid = new CrValidator(0.0f, 100.0f, 0.5f, this);
	CrValidator* delayValid = new CrValidator(0.0f, 1000.0f, 0.5f, this);

	for(int count=1; count<11; count++){
		m_voicesStringList << i18n("Voice %1").arg(count);
	}

	Q3VBoxLayout* mainLayout = new Q3VBoxLayout(this, CR_FRAME_WIDTH);
	//mix
	Q3HBoxLayout* topLayout = new Q3HBoxLayout(CR_CELL_SPACING);
	Q3VGroupBox* mixBox = new Q3VGroupBox(i18n("Mix"), this);
	m_mixArray = new CrSliderArray(3, 5, m_echo, mixBox);
	(void)m_mixArray->addSlider(i18n("Input Gain"), i18n("dB"), inputGainValid, &m_epar->inputGain);
	(void)m_mixArray->addSlider(i18n("Dry Mix"), "%", mixValid, &m_epar->dryMix);
	(void)m_mixArray->addSlider(i18n("Wet Mix"), "%", mixValid, &m_epar->wetMix);

	Q3VBoxLayout* finalLayout = new Q3VBoxLayout;
	finalLayout->setMargin(mixBox->frameWidth());
	m_finalEchoArray = new CrSliderArray(2, 6, m_echo, this);
	(void)m_finalEchoArray->addSlider(i18n("Final Delay"), i18n("ms"), delayValid, &m_epar->finalDelay, true, true);
	(void)m_finalEchoArray->addSlider(i18n("Final Feedback"), "%", mixValid, &m_epar->finalFeedback, true, true);
	Q3HBoxLayout* voiceNumLayout = new Q3HBoxLayout(CR_CELL_SPACING);
	QLabel* voiceNumLabel = new QLabel(i18n("Number of Voices"), this);
	m_voiceNumSpinBox = new QSpinBox(0, 10, 1, this);
	m_voiceNumSpinBox->setValue(m_epar->parallelEchoCount);
	m_voiceNumSpinBox->setFixedWidth(m_finalEchoArray->spinBoxPixelWidth());
	voiceNumLayout->addWidget(voiceNumLabel, 10, Qt::AlignRight);
	voiceNumLayout->addWidget(m_voiceNumSpinBox);
	voiceNumLayout->addSpacing(m_finalEchoArray->setMinRightColWidth()+CR_CELL_SPACING);

	finalLayout->addStretch(10);
	finalLayout->addWidget(m_finalEchoArray);
	finalLayout->addLayout(voiceNumLayout);
	finalLayout->addStretch(10);
	topLayout->addWidget(mixBox);
	topLayout->addLayout(finalLayout);

	//echo taps view
	m_echoTapsView  = new CrEchoTapsView(m_epar, m_visibleVoice, this);

	//voices list box
	Q3HBoxLayout* middleLayout = new Q3HBoxLayout(CR_CELL_SPACING);
	m_voicesListBox = new Q3ListBox(this);
	//voice box
	assert(m_visibleVoice <= m_epar->parallelEchoCount);
	m_voiceBox = new Q3VGroupBox(i18n("Voice %1").arg(m_visibleVoice+1), this);
	m_voiceArray = new CrSliderArray(3, 6, m_echo, m_voiceBox);
	(void)m_voiceArray->addSlider(i18n("Delay"), i18n("ms"), delayValid, &m_epar->parallelEcho[m_visibleVoice].delay, true, true);
	(void)m_voiceArray->addSlider(i18n("Decay"), "%", mixValid, &m_epar->parallelEcho[m_visibleVoice].decay, true, true);
	(void)m_voiceArray->addSlider(i18n("Feedback"), "%", mixValid, &m_epar->parallelEcho[m_visibleVoice].feedback);
	connect(m_voiceArray->getSliderEntry(0), SIGNAL(valueChanged(float)), m_echoTapsView, SLOT(slotUpdateSelectedTap()));
	connect(m_voiceArray->getSliderEntry(1), SIGNAL(valueChanged(float)), m_echoTapsView, SLOT(slotUpdateSelectedTap()));
	connect(m_finalEchoArray->getSliderEntry(0), SIGNAL(valueChanged(float)), m_echoTapsView, SLOT(slotUpdateFinalTap()));
	connect(m_finalEchoArray->getSliderEntry(1), SIGNAL(valueChanged(float)), m_echoTapsView, SLOT(slotUpdateFinalTap()));

	updateVoicesListBox();
	m_voicesListBox->setFixedWidth(m_voicesListBox->sizeHint().width());
	m_voicesListBox->setFixedHeight(m_voiceBox->sizeHint().height());
	m_voicesListBox->setCurrentItem(m_visibleVoice);
	connect(m_voicesListBox, SIGNAL(highlighted(int)), this, SLOT(slotChangeSelectedVoice(int)));
	connect(m_voicesListBox, SIGNAL(highlighted(int)), m_echoTapsView, SLOT(slotChangeSelectedTap(int)));

	middleLayout->addWidget(m_voicesListBox);
	middleLayout->addWidget(m_voiceBox);

	mainLayout->addLayout(topLayout);
	mainLayout->addWidget(m_echoTapsView);
	mainLayout->addLayout(middleLayout);
	mainLayout->addStretch(10);

	setFixedHeight(sizeHint().height());
	setMinimumWidth(sizeHint().width());

	connect(m_voiceNumSpinBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeVoicesNum(int)));
}

void CrEcho::synchronize()
{
	m_mixArray->synchronizeSlider();
	m_finalEchoArray->synchronizeSlider();

	m_voiceNumSpinBox->blockSignals(true);
	m_voiceNumSpinBox->setValue(m_epar->parallelEchoCount);
	m_voiceNumSpinBox->blockSignals(false);

	m_echoTapsView->setUpdatesEnabled(false);
	updateVoicesListBox();
	m_echoTapsView->setUpdatesEnabled(true);
	m_echoTapsView->repaint();

	slotChangeSelectedVoice(m_visibleVoice);
}

void CrEcho::updateVoicesListBox()
{
	const int listCount = m_voicesListBox->count();
	const int parallelEchoCount = m_epar->parallelEchoCount;
	if(parallelEchoCount > listCount){
		for(int count=listCount; count<parallelEchoCount; count++){
			m_voicesListBox->insertItem(m_voicesStringList[count]);
		}
	}
	else if(parallelEchoCount < listCount){
		m_voicesListBox->blockSignals(true);
		for(int count=listCount-1; count>parallelEchoCount; count--){
			m_voicesListBox->removeItem(count);
		}
		m_voicesListBox->blockSignals(false);
		m_voicesListBox->removeItem(parallelEchoCount);
	}
	if(parallelEchoCount==0){
		m_voiceBox->setDisabled(true);
	}
	else{
		assert(m_visibleVoice>-1 && m_visibleVoice<10);
		m_voicesListBox->setSelected(m_visibleVoice, true);
		m_voiceBox->setDisabled(false);
	}
}

void CrEcho::slotChangeSelectedVoice(int voice)
{
	assert(voice>-2 && voice<10);
	m_visibleVoice = (voice==-1) ? 0 : voice;
	if(voice!=-1){
		m_voicesListBox->setSelected(m_visibleVoice, true);
		m_voiceBox->setTitle(i18n("Voice %1").arg(m_visibleVoice+1));
		(void)m_voiceArray->changeSliderVariable(0, &m_epar->parallelEcho[m_visibleVoice].delay);
		(void)m_voiceArray->changeSliderVariable(1, &m_epar->parallelEcho[m_visibleVoice].decay);
		(void)m_voiceArray->changeSliderVariable(2, &m_epar->parallelEcho[m_visibleVoice].feedback);
	}
#ifdef _DEBUG_CRECHO
	std::cerr << "Voice: " << voice << "\n";
#endif
}

void CrEcho::slotChangeVoicesNum(int voicesNum)
{
	assert(voicesNum>-1 && voicesNum<11);
	m_epar->parallelEchoCount = voicesNum;
	m_echo->updateParameters();
	m_echoTapsView->repaint();
	updateVoicesListBox();
}

CrEcho::~CrEcho()
{
	saveParameters();
	delete m_echo;
	delete m_epar;
#ifdef _DEBUG
	std::cerr << "CrEcho deleted..." << "\n";
#endif

}

void CrEcho::restoreParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Echo");
	m_epar->inputGain = conf->readDoubleNumEntry("inputGain", 0.0);
	m_epar->wetMix = conf->readDoubleNumEntry("wetMix", 30.0);
	m_epar->dryMix = conf->readDoubleNumEntry("dryMix", 70.0);
	m_epar->finalFeedback = conf->readDoubleNumEntry("finalFeedback", 30.0);
	m_epar->finalDelay = conf->readDoubleNumEntry("finalDelay", 350.0);
	m_epar->parallelEchoCount = conf->readNumEntry("parallelEchoCount", 0);
	for(int count=0; count<10; count++){
		m_epar->parallelEcho[count].delay = conf->readDoubleNumEntry(QString("parallelEcho%1_delay").arg(count), 100.0*static_cast<double>(count)+100.0);
		m_epar->parallelEcho[count].decay = conf->readDoubleNumEntry(QString("parallelEcho%1_decay").arg(count), 50.0/(static_cast<double>(count)+1.0));
		m_epar->parallelEcho[count].feedback = conf->readDoubleNumEntry(QString("parallelEcho%1_feedback").arg(count), 0.0);
	}
	m_visibleVoice = conf->readNumEntry("visibleVoice", 0);
	if(m_visibleVoice > m_epar->parallelEchoCount){
		m_visibleVoice = m_epar->parallelEchoCount;
	}
}

void CrEcho::saveParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Echo");
	conf->writeEntry("inputGain", m_epar->inputGain);
	conf->writeEntry("wetMix", m_epar->wetMix);
	conf->writeEntry("dryMix", m_epar->dryMix);
	conf->writeEntry("finalFeedback", m_epar->finalFeedback);
	conf->writeEntry("finalDelay", m_epar->finalDelay);
	conf->writeEntry("parallelEchoCount", m_epar->parallelEchoCount);
	for(int count=0; count<10; count++){
		conf->writeEntry(QString("parallelEcho%1_delay").arg(count), m_epar->parallelEcho[count].delay);
		conf->writeEntry(QString("parallelEcho%1_decay").arg(count), m_epar->parallelEcho[count].decay);
		conf->writeEntry(QString("parallelEcho%1_feedback").arg(count), m_epar->parallelEcho[count].feedback);
	}
	conf->writeEntry("visibleVoice", m_visibleVoice);
}
