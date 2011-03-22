/***************************************************************************
						crphaser.cpp  -  description
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

#include "control.h"
#include <cassert>
#include <iostream>
#include <qlayout.h>
#include <q3vgroupbox.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include "crphaser.h"
#include "crvalidator.h"
#include "crsliderarray.h"

#include <KConfigGroup>

CrPhaser::CrPhaser(QWidget *parent, const char *name )
		: CrEffectGui(parent,name)
{
	setEffectName(i18n("Phaser"));
	m_ppar = new PhaserParameters;
	restoreParameters();

	m_phaser = new PhaserProcessor(m_ppar);

	CrValidator* inputGainValid = new CrValidator(-15.0f, 15.0f, 0.5f, this);
	CrValidator* mixValid = new CrValidator(0.0f, 100.0f, 0.5f, this);
	CrValidator* sweepGainValid = new CrValidator(-30.0f, 30.0f, 0.5f, this);
	CrValidator* centerFreqValid = new CrValidator(20.0f, 20000.0f, 0.2f, this);
	CrValidator* depthValid = new CrValidator(1.0f, 15.f, 0.4f, this);
	CrValidator* resonanceValid = new CrValidator(0.01f, 15.f, 0.4f, this);
	CrValidator* sweepFreqValid = new CrValidator(0.1f, 30.f, 0.25f, this);

	Q3VBoxLayout* mainLayout = new Q3VBoxLayout(this, CR_FRAME_WIDTH);
	//mix
	Q3VGroupBox* mixBox = new  Q3VGroupBox(i18n("Mix"), this);
	m_mixArray = new CrSliderArray(3, 5, m_phaser, mixBox);
	m_mixArray->addSlider(i18n("Input Gain"), i18n("dB"), inputGainValid, &m_ppar->inputGain);
	m_mixArray->addSlider(i18n("Dry Mix"), "%", mixValid, &m_ppar->dryMix);
	m_mixArray->addSlider(i18n("Sweep Gain"), i18n("dB"), sweepGainValid, &m_ppar->sweepGain);

	//sweep
	Q3VBox* sweepBox = new  Q3VBox(this);
	sweepBox->setMargin(CR_CELL_SPACING*2);
	m_sweepArray = new CrSliderArray(4, 7, m_phaser, sweepBox);
	m_sweepArray->addSlider(i18n("Center Freq"), i18n("Hz"), centerFreqValid, &m_ppar->centerFrequency);
	m_sweepArray->addSlider(i18n("Depth"), i18n("/Q"), depthValid, &m_ppar->depth);
	m_sweepArray->addSlider(i18n("Resonance"), i18n("Q"), resonanceValid, &m_ppar->resonance);
	m_sweepArray->addSlider(i18n("Sweep Freq"), i18n("Hz"), sweepFreqValid, &m_ppar->sweepFrequency);

	mainLayout->addWidget(mixBox);
	mainLayout->addWidget(sweepBox);
	mainLayout->addStretch(10);

	setFixedHeight(sizeHint().height());
	setMinimumWidth(sizeHint().width());
}

void CrPhaser::synchronize()
{
	m_mixArray->synchronizeSlider();
	m_sweepArray->synchronizeSlider();
}

CrPhaser::~CrPhaser()
{
	saveParameters();
	delete m_phaser;
	delete m_ppar;
#ifdef _DEBUG
	std::cerr << "CrPhaser deleted..." << "\n";
#endif
}

void CrPhaser::restoreParameters()
{
	KConfigGroup conf = KGlobal::config()->group("Phaser");
	m_ppar->inputGain = conf.readEntry("inputGain", -5.0);
	m_ppar->sweepGain = conf.readEntry("sweepGain", 10.0);
	m_ppar->dryMix = conf.readEntry("dryMix", 80.0);
	m_ppar->centerFrequency = conf.readEntry("centerFrequency", 900.0);
	m_ppar->depth = conf.readEntry("depth", 3.0);
	m_ppar->resonance = conf.readEntry("resonance", 1.7);
	m_ppar->sweepFrequency = conf.readEntry("sweepFrequency", 0.75);
}

void CrPhaser::saveParameters()
{
	KConfigGroup conf = KGlobal::config()->group("Phaser");
	conf.writeEntry("inputGain", m_ppar->inputGain);
	conf.writeEntry("sweepGain", m_ppar->sweepGain);
	conf.writeEntry("dryMix", m_ppar->dryMix);
	conf.writeEntry("centerFrequency", m_ppar->centerFrequency);
	conf.writeEntry("depth", m_ppar->depth);
	conf.writeEntry("resonance", m_ppar->resonance);
	conf.writeEntry("sweepFrequency", m_ppar->sweepFrequency);
}
