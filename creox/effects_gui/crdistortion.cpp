/***************************************************************************
					crdistortion.cpp  -  description
						 -------------------
		begin                : Tue Dec 26 2000
		copyright            : (C) 2000 by Jozef Kosoru
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

#include "control.h"
#include <cassert>
#include <iostream>
#include <QLayout>
#include <q3vgroupbox.h>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include "crvalidator.h"
#include "crsliderarray.h"
#include "crdistortion.h"

#include <KConfigGroup>

CrDistortion::CrDistortion(QWidget *parent, const char *name )
	: CrEffectGui(parent,name)
{
	setEffectName(i18n("Distortion"));
	m_dpar = new DistortionParameters;
	restoreParameters();

	m_distortion = new DistortionProcessor(m_dpar);

	CrValidator* inputGainValid = new CrValidator(-15.0f, 15.0f, 0.5f, this);
	CrValidator* mixValid = new CrValidator(0.0f, 100.0f, 0.5f, this);
	CrValidator* bassFreqValid = new CrValidator(20.0f, 1000.0f, 0.3f, this);
	CrValidator* bassGainValid = new CrValidator(-35.0f, 35.0f, 0.5f, this);
	CrValidator* bassSlopeValid = new CrValidator(0.01f, 1.0f, 0.5f, this);
	CrValidator* accFreqValid = new CrValidator(20.0f, 20000.0f, 0.2f, this);
	CrValidator* accBandValid = new CrValidator(0.01f, 15.0f, 0.4f, this);
	CrValidator* driveValid = new CrValidator(0.0f, 100.0f, 0.4f, this);

	Q3GridLayout* gridLayout = new Q3GridLayout(this, 5, 2, CR_FRAME_WIDTH);

	//mix
	Q3VGroupBox* mixBox = new  Q3VGroupBox(i18n("Mix"), this);
	m_mixArray = new CrSliderArray(3, 5, m_distortion, mixBox);
	m_mixArray->addSlider(i18n("Input Gain"), i18n("dB"), inputGainValid, &m_dpar->inputGain);
	m_mixArray->addSlider(i18n("Wet Mix"), "%", mixValid, &m_dpar->wetMix);
	m_mixArray->addSlider(i18n("Dry Mix"), "%", mixValid, &m_dpar->dryMix);

	//accented frequency (BPF)
	Q3VGroupBox* accentBox = new  Q3VGroupBox(i18n("Accent Frequency"), this);
	m_accentArray = new CrSliderArray(4, 7, m_distortion, accentBox);
	m_accentArray->addSlider(i18n("Frequency"), i18n("Hz"), accFreqValid, &m_dpar->accent_frequency);
	m_accentArray->addSlider(i18n("Bandwidth"), i18n("oct"), accBandValid, &m_dpar->accent_bandwidth);
	m_accentArray->addSlider(i18n("Drive"), i18n("dB"), driveValid, &m_dpar->accent_drive);
	m_accentArray->addSlider(i18n("Gain"), i18n("dB"), bassGainValid, &m_dpar->accent_gain);
	QWidget* modeWidget = new QWidget(accentBox);
	Q3HBoxLayout* modeLayout = new Q3HBoxLayout(modeWidget, 0, CR_CELL_SPACING);
	QLabel* modeLabel = new QLabel(i18n("Mode"), modeWidget);
	modeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	modeLabel->setFixedWidth(m_accentArray->setMinLeftColWidth(modeLabel->sizeHint().width()));
	m_accentFreqCombo = new QComboBox(false, modeWidget);
	QStringList modeStringList;
	modeStringList << i18n("Clean") << i18n("Soft") << i18n("Hard") << i18n("Extra Hard");
	m_accentFreqCombo->insertStringList(modeStringList);
	m_accentFreqCombo->setCurrentItem(m_dpar->accent_distortFunction);
	connect(m_accentFreqCombo, SIGNAL(activated(int)), this, SLOT(slotChangeAccentFreqMode(int)));
	modeLayout->addWidget(modeLabel, 0);
	modeLayout->addWidget(m_accentFreqCombo, 0);
	modeLayout->addStretch(5);

	//mainDrive
	Q3VGroupBox* mainDriveBox = new  Q3VGroupBox(i18n("Main Drive"), this);
	m_mainDriveArray = new CrSliderArray(2, 5, m_distortion, mainDriveBox);
	m_mainDriveArray->addSlider(i18n("Drive"), i18n("dB"), driveValid, &m_dpar->main_drive);
	m_mainDriveArray->addSlider(i18n("Gain"), i18n("dB"), bassGainValid, &m_dpar->main_gain);
	QWidget* modeWidget2 = new QWidget(mainDriveBox);
	Q3HBoxLayout* modeLayout2 = new Q3HBoxLayout(modeWidget2, 0, CR_CELL_SPACING);
	QLabel* modeLabel2 = new QLabel(i18n("Mode"), modeWidget2);
	modeLabel2->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	modeLabel2->setFixedWidth(m_mainDriveArray->setMinLeftColWidth(modeLabel2->sizeHint().width()));
	m_mainDriveCombo = new QComboBox(false, modeWidget2);
	m_mainDriveCombo->insertStringList(modeStringList);
	m_mainDriveCombo->setCurrentItem(m_dpar->main_distortFunction);
	connect(m_mainDriveCombo, SIGNAL(activated(int)), this, SLOT(slotChangeMainDriveMode(int)));
	modeLayout2->addWidget(modeLabel2, 0);
	modeLayout2->addWidget(m_mainDriveCombo, 0);
	modeLayout2->addStretch(5);

	//low Pass
	Q3VGroupBox* lowPassBox = new  Q3VGroupBox(i18n("Low Pass Filter"), this);
	m_lowPassArray = new CrSliderArray(2, 7, m_distortion, lowPassBox);
	m_lowPassArray->addSlider(i18n("Cutoff Freq"), i18n("Hz"), accFreqValid, &m_dpar->lowPass_cutoffFrequency);
	m_lowPassArray->addSlider(i18n("Slope"), i18n("Q"), accBandValid, &m_dpar->lowPass_Q);

	//input bass cut/boost (lowShelf)
	Q3VGroupBox* inputBassBox = new  Q3VGroupBox(i18n("Input Bass"), this);
	m_inputBassArray = new CrSliderArray(3, 6, m_distortion, inputBassBox);
	m_inputBassArray->addSlider(i18n("Frequency"), i18n("Hz"), bassFreqValid, &m_dpar->inBass_frequency);
	m_inputBassArray->addSlider(i18n("Gain"), i18n("dB"), bassGainValid, &m_dpar->inBass_gain);
	m_inputBassArray->addSlider(i18n("Slope"), i18n("Q"), bassSlopeValid, &m_dpar->inBass_slope);

	//output bass boost/cut (lowShelf)
	Q3VGroupBox* outputBassBox = new  Q3VGroupBox(i18n("Output Bass"), this);
	m_outputBassArray = new CrSliderArray(3, 6, m_distortion, outputBassBox);
	m_outputBassArray->addSlider(i18n("Frequency"), i18n("Hz"), bassFreqValid, &m_dpar->outBass_frequency);
	m_outputBassArray->addSlider(i18n("Gain"), i18n("dB"), bassGainValid, &m_dpar->outBass_gain);
	m_outputBassArray->addSlider(i18n("Slope"), i18n("Q"), bassSlopeValid, &m_dpar->outBass_slope);

	gridLayout->addWidget(mixBox, 0, 0);
	gridLayout->addMultiCellWidget(accentBox, 0, 1, 1, 1);
	gridLayout->addMultiCellWidget(mainDriveBox, 2, 3, 0, 0);
	gridLayout->addWidget(lowPassBox, 2 ,1);
	gridLayout->addWidget(inputBassBox, 4 ,0);
	gridLayout->addWidget(outputBassBox, 4 ,1);

	setFixedHeight(sizeHint().height());
	setMinimumWidth(sizeHint().width());
}

void CrDistortion::synchronize()
{
	m_mixArray->synchronizeSlider();
	m_inputBassArray->synchronizeSlider();
	m_outputBassArray->synchronizeSlider();
	m_accentArray->synchronizeSlider();
	m_mainDriveArray->synchronizeSlider();
	m_lowPassArray->synchronizeSlider();

	m_mainDriveCombo->blockSignals(true);
	m_accentFreqCombo->blockSignals(true);
	m_mainDriveCombo->setCurrentItem(m_dpar->main_distortFunction);
	m_accentFreqCombo->setCurrentItem(m_dpar->accent_distortFunction);
	m_mainDriveCombo->blockSignals(false);
	m_accentFreqCombo->blockSignals(false);
}

CrDistortion::~CrDistortion()
{
	saveParameters();
	delete m_distortion;
	delete m_dpar;
#ifdef _DEBUG
	std::cerr << "CrDistortion deleted..." << "\n";
#endif
}

void CrDistortion::restoreParameters()
{
	KConfigGroup conf = KGlobal::config()->group("Distortion");
	m_dpar->inputGain = conf.readEntry("inputGain", 0.0f);
	m_dpar->wetMix = conf.readEntry("wetMix", 100.0f);
	m_dpar->dryMix = conf.readEntry("dryMix", 0.0f);
	m_dpar->inBass_frequency = conf.readEntry("inBass_frequency", 200.0f);
	m_dpar->inBass_gain = conf.readEntry("inBass_gain", -10.0f);
	m_dpar->inBass_slope = conf.readEntry("inBass_slope", 1.0f);
	m_dpar->outBass_frequency = conf.readEntry("outBass_frequency", 240.0f);
	m_dpar->outBass_gain = conf.readEntry("outBass_gain", 15.0f);
	m_dpar->outBass_slope = conf.readEntry("outBass_slope", 1.0f);
	m_dpar->accent_frequency = conf.readEntry("accent_frequency", 799.0f);
	m_dpar->accent_bandwidth = conf.readEntry("accent_bandwidth", 0.3f);
	m_dpar->accent_drive = conf.readEntry("accent_drive", 38.0f);
	m_dpar->accent_gain = conf.readEntry("accent_gain", 0.0f);
	m_dpar->main_drive = conf.readEntry("main_drive", 0.0f);
	m_dpar->main_gain = conf.readEntry("main_gain", -9.0f);
	m_dpar->accent_distortFunction = conf.readEntry("accent_distortFunction", 1);
	m_dpar->main_distortFunction = conf.readEntry("main_distortFunction", 3);
	m_dpar->lowPass_cutoffFrequency = conf.readEntry("lowPass_cutoffFrequency", 13000.0f);
	m_dpar->lowPass_Q = conf.readEntry("lowPass_Q", 2.0f);
}

void CrDistortion::saveParameters()
{
	KConfigGroup conf = KGlobal::config()->group("Distortion");
	conf.writeEntry("inputGain", m_dpar->inputGain);
	conf.writeEntry("wetMix", m_dpar->wetMix);
	conf.writeEntry("dryMix", m_dpar->dryMix);
	conf.writeEntry("inBass_frequency", m_dpar->inBass_frequency);
	conf.writeEntry("inBass_gain", m_dpar->inBass_gain);
	conf.writeEntry("inBass_slope", m_dpar->inBass_slope);
	conf.writeEntry("outBass_frequency", m_dpar->outBass_frequency);
	conf.writeEntry("outBass_gain", m_dpar->outBass_gain);
	conf.writeEntry("outBass_slope", m_dpar->outBass_slope);
	conf.writeEntry("accent_frequency", m_dpar->accent_frequency);
	conf.writeEntry("accent_bandwidth", m_dpar->accent_bandwidth);
	conf.writeEntry("accent_drive", m_dpar->accent_drive);
	conf.writeEntry("accent_gain", m_dpar->accent_gain);
	conf.writeEntry("main_drive", m_dpar->main_drive);
	conf.writeEntry("main_gain", m_dpar->main_gain);
	conf.writeEntry("accent_distortFunction", m_dpar->accent_distortFunction);
	conf.writeEntry("main_distortFunction", m_dpar->main_distortFunction);
	conf.writeEntry("lowPass_cutoffFrequency", m_dpar->lowPass_cutoffFrequency);
	conf.writeEntry("lowPass_Q", m_dpar->lowPass_Q);
}

void CrDistortion::slotChangeAccentFreqMode(int mode)
{
	assert(mode>-1 && mode<4);
	m_dpar->accent_distortFunction = mode;
	m_distortion->updateParameters();
}

void CrDistortion::slotChangeMainDriveMode(int mode)
{
	assert(mode>-1 && mode<4);
	m_dpar->main_distortFunction = mode;
	m_distortion->updateParameters();
}

