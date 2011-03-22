/***************************************************************************
						crtremolo.cpp  -  description
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
#include <qvgroupbox.h>
#include <qvbox.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include "crvalidator.h"
#include "crsliderarray.h"
#include "crtremolo.h"

CrTremolo::CrTremolo(QWidget *parent, const char *name )
		: CrEffectGui(parent,name)
{
	setEffectName(i18n("Tremolo"));
	m_tpar = new TremoloParameters;
	restoreParameters();

	m_tremolo = new TremoloProcessor(m_tpar);

	CrValidator* inputGainValid = new CrValidator(-15.0f, 15.0f, 0.5f, this);
	CrValidator* mixValid = new CrValidator(0.0f, 100.0f, 0.5f, this);
	CrValidator* freqValid = new CrValidator(0.01f, 40.0f, 0.3f, this);
	CrValidator* depthValid = new CrValidator(0.0f, 35.0f, 0.5f, this);

	QVBoxLayout* mainLayout = new QVBoxLayout(this, CR_FRAME_WIDTH);
	//mix
	QVGroupBox* mixBox = new  QVGroupBox(i18n("Mix"), this);
	m_mixArray = new CrSliderArray(3, 5, m_tremolo, mixBox);
	m_mixArray->addSlider(i18n("Input Gain"), i18n("dB"), inputGainValid, &m_tpar->inputGain);
	m_mixArray->addSlider(i18n("Dry Mix"), "%", mixValid, &m_tpar->dryMix);
	m_mixArray->addSlider(i18n("Wet Mix"), "%", mixValid, &m_tpar->wetMix);

	QVBox* tremoloBox = new  QVBox(this);
	tremoloBox->setMargin(CR_CELL_SPACING*2);
	m_tremoloArray = new CrSliderArray(2, 4, m_tremolo, tremoloBox);
	m_tremoloArray->addSlider(i18n("Frequency"), i18n("Hz"), freqValid, &m_tpar->frequency);
	m_tremoloArray->addSlider(i18n("Depth"), i18n("dB"), depthValid, &m_tpar->depth);

	QWidget* lfoBox = new QWidget(this);
	QHBoxLayout* lfoLay = new QHBoxLayout(lfoBox);
	QVButtonGroup *lfoGroup = new QVButtonGroup(i18n("LFO Type"), lfoBox);
	m_lfoSine = new QRadioButton(i18n("Sine"), lfoGroup);
	m_lfoTriangle = new QRadioButton(i18n("Triangle"), lfoGroup);
	if(m_tpar->waveform == TremoloProcessor::triangle){
		m_lfoTriangle->setChecked(true);
	}
	else{
		m_lfoSine->setChecked(true);
	}
	connect(lfoGroup, SIGNAL(clicked(int)), this, SLOT(slotChangeLfoType(int)));
	lfoLay->addWidget(lfoGroup,6);
	lfoLay->addStretch(10);

	mainLayout->addWidget(mixBox);
	mainLayout->addWidget(tremoloBox);
	mainLayout->addWidget(lfoBox);
	mainLayout->addStretch(10);

	setFixedHeight(sizeHint().height());
	setMinimumWidth(sizeHint().width());
}

void CrTremolo::synchronize()
{
	m_mixArray->synchronizeSlider();
	m_tremoloArray->synchronizeSlider();

	m_lfoTriangle->blockSignals(true);
	m_lfoSine->blockSignals(true);
	if(m_tpar->waveform == TremoloProcessor::triangle){
		m_lfoTriangle->setChecked(true);
		m_lfoSine->setChecked(false);
	}
	else{
		m_lfoTriangle->setChecked(false);
		m_lfoSine->setChecked(true);
	}
	m_lfoTriangle->blockSignals(false);
	m_lfoSine->blockSignals(false);
}

CrTremolo::~CrTremolo()
{
	saveParameters();
	delete m_tremolo;
	delete m_tpar;
#ifdef _DEBUG
	std::cerr << "CrTremolo deleted...\n";
#endif
}

void CrTremolo::restoreParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Tremolo");
	m_tpar->inputGain = conf->readDoubleNumEntry("inputGain", -5.0);
	m_tpar->wetMix = conf->readDoubleNumEntry("wetMix", 100.0);
	m_tpar->dryMix = conf->readDoubleNumEntry("dryMix", 0.0);
	m_tpar->frequency = conf->readDoubleNumEntry("frequency", 9.0);
	m_tpar->depth = conf->readDoubleNumEntry("depth", 25.0);
	m_tpar->waveform = conf->readNumEntry("waveform", TremoloProcessor::sine);
}

void CrTremolo::saveParameters()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup("Tremolo");
	conf->writeEntry("inputGain", m_tpar->inputGain);
	conf->writeEntry("wetMix", m_tpar->wetMix);
	conf->writeEntry("dryMix", m_tpar->dryMix);
	conf->writeEntry("frequency", m_tpar->frequency);
	conf->writeEntry("depth", m_tpar->depth);
	conf->writeEntry("waveform", m_tpar->waveform);
}

void CrTremolo::slotChangeLfoType(int type)
{
	assert(type==0 || type==1);
	m_tpar->waveform = type;
	m_tremolo->updateParameters();
}
