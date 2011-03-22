/***************************************************************************
					crflanger.cpp  -  description
						 -------------------
		begin                : Mon Jan 1 2001
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
#include <qvgroupbox.h>
#include <qvbox.h>
//#include <qhbox.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include "crvalidator.h"
#include "crsliderarray.h"
#include "crflanger.h"

CrFlanger::CrFlanger(QWidget *parent, const char *name )
	: CrEffectGui(parent,name)
{
	setEffectName(i18n("Flanger"));
	m_fpar = new FlangerParameters;
	restoreParameters();

	m_flanger = new FlangerProcessor(m_fpar);

	CrValidator* inputGainValid = new CrValidator(-15.0f, 15.0f, 0.5f, this);
	CrValidator* mixValid = new CrValidator(0.0f, 100.0f, 0.5f, this);
	CrValidator* crossfadeValid = new CrValidator(-100.0f, 100.0f, 0.5f, this);
	CrValidator* delayValid = new CrValidator(0.0f, 40.0f, 0.35f, this);
	CrValidator* freqValid = new CrValidator(0.1f, 20.0f, 0.3f, this);

	QVBoxLayout* mainLayout = new QVBoxLayout(this, CR_FRAME_WIDTH);

	//mix
	QVGroupBox* mixBox = new  QVGroupBox(i18n("Mix"), this);
	m_mixArray = new CrSliderArray(2, 6, m_flanger, mixBox);
	m_mixArray->addSlider(i18n("Input Gain"), i18n("dB"), inputGainValid, &m_fpar->inputGain);
	m_mixArray->addSlider(i18n("Dry"), i18n("Wet"), crossfadeValid, &m_fpar->crossfade);

	QVBox* flangerBox = new  QVBox(this);
	flangerBox->setMargin(CR_CELL_SPACING*2);
	m_flangerArray = new CrSliderArray(4, 5, m_flanger, flangerBox);
	m_flangerArray->addSlider(i18n("Feedback"), "%", mixValid, &m_fpar->feedback);
	m_flangerArray->addSlider(i18n("Delay"), i18n("ms"), delayValid, &m_fpar->delay);
	m_flangerArray->addSlider(i18n("Depth"), i18n("ms"), delayValid, &m_fpar->modulationDepth);
	m_flangerArray->addSlider(i18n("Frequency"), i18n("Hz"), freqValid, &m_fpar->frequency);

	QWidget* lfoBox = new QWidget(this);
	QHBoxLayout* lfoLay = new QHBoxLayout(lfoBox, 0, CR_CELL_SPACING*2);
	QVButtonGroup *lfoGroup = new QVButtonGroup(i18n("LFO Type"), lfoBox);
	m_lfoSine = new QRadioButton(i18n("Sine"), lfoGroup);
	m_lfoTriangle = new QRadioButton(i18n("Triangle"), lfoGroup);
	if(m_fpar->waveform == FlangerProcessor::triangle){
		m_lfoTriangle->setChecked(true);
	}
	else{
		m_lfoSine->setChecked(true);
	}
	connect(lfoGroup, SIGNAL(clicked(int)), this, SLOT(slotChangeLfoType(int)));
	QVBoxLayout* invertLay = new QVBoxLayout;
	m_checkInvertBox = new QCheckBox(i18n("Inverted"), lfoBox);
	if(m_fpar->inverted){
		m_checkInvertBox->setChecked(true);
	}
	connect(m_checkInvertBox, SIGNAL(toggled(bool)), this, SLOT(slotChangeInverted(bool)));
	invertLay->addSpacing(CR_CELL_SPACING*3);
	invertLay->addWidget(m_checkInvertBox, 0, Qt::AlignLeft | AlignTop);
	invertLay->addStretch(10);
	lfoLay->addWidget(lfoGroup,8);
	lfoLay->addLayout(invertLay);
	lfoLay->addStretch(10);

	mainLayout->addWidget(mixBox);
	mainLayout->addWidget(flangerBox);
	mainLayout->addWidget(lfoBox);
	mainLayout->addStretch(10);

	setFixedHeight(sizeHint().height());
	setMinimumWidth(sizeHint().width());
}

void CrFlanger::synchronize()
{
	m_mixArray->synchronizeSlider();
	m_flangerArray->synchronizeSlider();

	m_lfoTriangle->blockSignals(true);
	m_lfoSine->blockSignals(true);
	if(m_fpar->waveform == FlangerProcessor::triangle){
		m_lfoTriangle->setChecked(true);
		m_lfoSine->setChecked(false);
	}
	else{
		m_lfoTriangle->setChecked(false);
		m_lfoSine->setChecked(true);
	}
	m_lfoTriangle->blockSignals(false);
	m_lfoSine->blockSignals(false);

	m_checkInvertBox->blockSignals(true);
	m_checkInvertBox->setChecked((m_fpar->inverted)? true : false);
	m_checkInvertBox->blockSignals(false);
}

CrFlanger::~CrFlanger()
{
	saveParameters();
	delete m_flanger;
	delete m_fpar;
#ifdef _DEBUG
	std::cerr << "CrFlanger deleted..." << "\n";
#endif
}

void CrFlanger::restoreParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Flanger");
	m_fpar->inputGain = conf->readDoubleNumEntry("inputGain", 0.0);
	m_fpar->crossfade = conf->readDoubleNumEntry("crossfade", 0.0);
	m_fpar->feedback = conf->readDoubleNumEntry("feedback", 60.0);
	m_fpar->frequency = conf->readDoubleNumEntry("frequency", 0.5);
	m_fpar->modulationDepth = conf->readDoubleNumEntry("modulationDepth", 1.0);
	m_fpar->delay = conf->readDoubleNumEntry("delay", 1.53);
	m_fpar->waveform = conf->readNumEntry("waveform", FlangerProcessor::sine);
	m_fpar->inverted = conf->readBoolEntry("inverted", false);
}

void CrFlanger::saveParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Flanger");
	conf->writeEntry("inputGain", m_fpar->inputGain);
	conf->writeEntry("crossfade", m_fpar->crossfade);
	conf->writeEntry("feedback", m_fpar->feedback);
	conf->writeEntry("frequency", m_fpar->frequency);
	conf->writeEntry("modulationDepth", m_fpar->modulationDepth);
	conf->writeEntry("delay", m_fpar->delay);
	conf->writeEntry("waveform", m_fpar->waveform);
	conf->writeEntry("inverted", m_fpar->inverted);
}

void CrFlanger::slotChangeLfoType(int type)
{
	assert(type==0 || type==1);
	m_fpar->waveform = type;
	m_flanger->updateParameters();
}

void CrFlanger::slotChangeInverted(bool inverted)
{
	m_fpar->inverted = inverted;
	m_flanger->updateParameters();
}
