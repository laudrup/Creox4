/***************************************************************************
					croptionsdialog.cpp  -  description
						 -------------------
		begin                : Sun Aug 26 2001
		copyright            : (C) 2001 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation                          *
 *                                                                         *
 ***************************************************************************/

#include "control.h"
#include <cassert>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <jack/jack.h>
#include <qstring.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qstring.h>
#include <q3whatsthis.h>
#include <qcheckbox.h>
#include <q3vgroupbox.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <kconfig.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include "croptionsdialog.h"

const char* const CrOptionsDialog::DEFAULT_LEFT_INPUT_PORT = "alsa_pcm:playback_1";
const char* const CrOptionsDialog::DEFAULT_RIGHT_INPUT_PORT = "alsa_pcm:playback_2";
const char* const CrOptionsDialog::DEFAULT_LEFT_OUTPUT_PORT = "alsa_pcm:capture_1";
const char* const CrOptionsDialog::DEFAULT_RIGHT_OUTPUT_PORT = "alsa_pcm:capture_2";

CrOptionsDialog::CrOptionsDialog(QWidget *parent, const char *name )
	: KDialogBase(IconList, i18n("Options"), Ok | Cancel, Ok, parent, name, true, true)
{
	/* page 1 */
	Q3Frame* const page1 = addPage(i18n("Jack"),
								  i18n("Jack audio configuration:"),
								  BarIcon("sound_card_properties", KIcon::SizeLarge ));
	Q3GridLayout* const page1Layout = new Q3GridLayout(page1, 6, 4, 0,
													 spacingHint());

	// input channel
    Q3VGroupBox* const inputChannelGroupBox = new Q3VGroupBox(i18n("Input Channel Connection"), page1);
	page1Layout->addMultiCellWidget(inputChannelGroupBox, 0, 0, 0, 3);

    QWidget* const inputGroupBoxHelper = new QWidget(inputChannelGroupBox);
    Q3GridLayout* const inputGroupBoxHelperLayout = new Q3GridLayout(inputGroupBoxHelper,
    														       2, 2, 0, spacingHint());
	QLabel* const leftInputChannelLabel = new QLabel(i18n("Left:"), inputGroupBoxHelper);
	leftInputChannelLabel->setAlignment(AlignVCenter | AlignRight);
	leftInputChannelLabel->setMinimumWidth(leftInputChannelLabel->sizeHint().width() * 2);
	inputGroupBoxHelperLayout->addWidget(leftInputChannelLabel, 0, 0);
	m_pLeftInputPort = new QComboBox(false, inputGroupBoxHelper);
	inputGroupBoxHelperLayout->addWidget(m_pLeftInputPort, 0, 1);

	QLabel* const rightInputChannelLabel = new QLabel(i18n("Right:"), inputGroupBoxHelper);
	rightInputChannelLabel->setAlignment(AlignVCenter | AlignRight);
	rightInputChannelLabel->setMinimumWidth(rightInputChannelLabel->sizeHint().width() * 2);
	inputGroupBoxHelperLayout->addWidget(rightInputChannelLabel, 1, 0);
	m_pRightInputPort = new QComboBox(false, inputGroupBoxHelper);
	inputGroupBoxHelperLayout->addWidget(m_pRightInputPort, 1, 1);

	m_disconnectedInputCheckBox = new QCheckBox(i18n("Leave disconnected"), inputChannelGroupBox);
	connect(m_disconnectedInputCheckBox, SIGNAL(toggled(bool)),
			leftInputChannelLabel, SLOT(setDisabled(bool)));
	connect(m_disconnectedInputCheckBox, SIGNAL(toggled(bool)),
			m_pLeftInputPort, SLOT(setDisabled(bool)));
	connect(m_disconnectedInputCheckBox, SIGNAL(toggled(bool)),
			rightInputChannelLabel, SLOT(setDisabled(bool)));
	connect(m_disconnectedInputCheckBox, SIGNAL(toggled(bool)),
			m_pRightInputPort, SLOT(setDisabled(bool)));
	
	inputGroupBoxHelperLayout->setColStretch(0, 2);
	inputGroupBoxHelperLayout->setColStretch(1, 5);

	// output channel
    Q3VGroupBox* const outputChannelGroupBox = new Q3VGroupBox(i18n("Output Channel Connection"), page1);
	page1Layout->addMultiCellWidget(outputChannelGroupBox, 1, 1, 0, 3);

    QWidget* const outputGroupBoxHelper = new QWidget(outputChannelGroupBox);
    Q3GridLayout* const outputGroupBoxHelperLayout = new Q3GridLayout(outputGroupBoxHelper,
    														       2, 2, 0, spacingHint());
	QLabel* const leftOutputChannelLabel = new QLabel(i18n("Left:"), outputGroupBoxHelper);
	leftOutputChannelLabel->setAlignment(AlignVCenter | AlignRight);
	leftOutputChannelLabel->setMinimumWidth(leftOutputChannelLabel->sizeHint().width() * 2);
	outputGroupBoxHelperLayout->addWidget(leftOutputChannelLabel, 0, 0);
	m_pLeftOutputPort = new QComboBox(false, outputGroupBoxHelper);
	outputGroupBoxHelperLayout->addWidget(m_pLeftOutputPort, 0, 1);

	QLabel* const rightOutputChannelLabel = new QLabel(i18n("Right:"), outputGroupBoxHelper);
	rightOutputChannelLabel->setAlignment(AlignVCenter | AlignRight);
	rightOutputChannelLabel->setMinimumWidth(rightOutputChannelLabel->sizeHint().width() * 2);
	outputGroupBoxHelperLayout->addWidget(rightOutputChannelLabel, 1, 0);
	m_pRightOutputPort = new QComboBox(false, outputGroupBoxHelper);
	outputGroupBoxHelperLayout->addWidget(m_pRightOutputPort, 1, 1);

	m_disconnectedOutputCheckBox = new QCheckBox(i18n("Leave disconnected"), outputChannelGroupBox);
	connect(m_disconnectedOutputCheckBox, SIGNAL(toggled(bool)),
			leftOutputChannelLabel, SLOT(setDisabled(bool)));
	connect(m_disconnectedOutputCheckBox, SIGNAL(toggled(bool)),
			m_pLeftOutputPort, SLOT(setDisabled(bool)));
	connect(m_disconnectedOutputCheckBox, SIGNAL(toggled(bool)),
			rightOutputChannelLabel, SLOT(setDisabled(bool)));
	connect(m_disconnectedOutputCheckBox, SIGNAL(toggled(bool)),
			m_pRightOutputPort, SLOT(setDisabled(bool)));

	outputGroupBoxHelperLayout->setColStretch(0, 2);
	outputGroupBoxHelperLayout->setColStretch(1, 5);

	page1Layout->addRowSpacing(2, spacingHint());

	// sampling rate
	QLabel* const samplerateLabel = new QLabel(i18n("Sampling Rate:"), page1);
	samplerateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	page1Layout->addWidget(samplerateLabel, 3, 1);
	QLabel* const samplerateNumLabel = new QLabel(QString::fromLatin1("48000"),
												  page1);
	samplerateNumLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	samplerateNumLabel->setFrameStyle(Q3Frame::Panel | Q3Frame::Sunken);
	page1Layout->addWidget(samplerateNumLabel, 3, 2);
	QLabel* const hzLabel = new QLabel(i18n("Hz"), page1);
	hzLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	page1Layout->addWidget(hzLabel, 3, 3);

	// resolution
	QLabel* const resolutionLabel = new QLabel(i18n("Resolution:"), page1);
	resolutionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	page1Layout->addWidget(resolutionLabel, 4, 1);
	QLabel* const resolutionNumLabel = new QLabel(QString::fromLatin1("32"),
												  page1);
	resolutionNumLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	resolutionNumLabel->setFrameStyle(Q3Frame::Panel | Q3Frame::Sunken);
	page1Layout->addWidget(resolutionNumLabel, 4, 2);
	QLabel* const bitLabel = new QLabel(i18n("bit"), page1);
	bitLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	page1Layout->addWidget(bitLabel, 4, 3);

	page1Layout->setColStretch(0, 2);
	page1Layout->setColStretch(1, 2);
	page1Layout->setColStretch(2, 5);
	page1Layout->setColStretch(3, 2);

	page1Layout->setRowStretch(5, 10);

	loadSettings();
}

CrOptionsDialog::~CrOptionsDialog()
{

}

void CrOptionsDialog::loadSettings()
{
	// set error function to default one
	jack_set_error_function(jack_error_callback);

	std::vector<QString> inputPortsList,
						 outputPortsList;

	// if the jack server can be connected, retrieve a ports list
	jack_client_t* pJackClient;
	if ((pJackClient = jack_client_new("creox_options")) != 0)
	{
		// get input ports list
		const char** const ppInputPortList = jack_get_ports(pJackClient,
															0, 0,
															JackPortIsInput);
		if (ppInputPortList)
		{
			for (const char** ppPortCount = ppInputPortList;
				 *ppPortCount; ++ppPortCount)
			{
				inputPortsList.push_back(QString::fromLatin1(*ppPortCount));
			}

			std::free(ppInputPortList);
		}

		// get output ports list
		const char** const ppOutputPortList = jack_get_ports(pJackClient,
															 0, 0,
															 JackPortIsOutput);
		if (ppOutputPortList)
		{
			for (const char** ppPortCount = ppOutputPortList;
				 *ppPortCount; ++ppPortCount)
			{
				outputPortsList.push_back(QString::fromLatin1(*ppPortCount));
			}

			std::free(ppOutputPortList);
		}

		// close the session
		jack_client_close(pJackClient);
	}

	// get the pointer to the config object
	KConfig* const conf = KGlobal::config();
	conf->setGroup(QString::fromLatin1("Jack_Options"));

    m_disconnectedInputCheckBox->setChecked(conf->readBoolEntry("DisconnectedInput", false));
    m_disconnectedOutputCheckBox->setChecked(conf->readBoolEntry("DisconnectedOutput", false));
    
	/* input channels */

	// read the left input port name from the config
	const QString sConfigLeftInputPort = conf->readEntry("LeftOutputPort");
	int iConfigLeftInputPortNumber = 0;

	// if there is a value which is not in the list already; add it
	if (!sConfigLeftInputPort.isNull())
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(inputPortsList.begin(),
								inputPortsList.end(),
								sConfigLeftInputPort)) == inputPortsList.end())
		{
			inputPortsList.push_back(sConfigLeftInputPort);
			iConfigLeftInputPortNumber = inputPortsList.size() - 1;
		}
		else
		{
			// remember the config input port name entry position
			iConfigLeftInputPortNumber = std::distance(inputPortsList.begin(),
													   itPort);
		}
	}
	else
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(inputPortsList.begin(),
								inputPortsList.end(),
								QString::fromLatin1(DEFAULT_LEFT_INPUT_PORT))) != inputPortsList.end())
		{
			iConfigLeftInputPortNumber = std::distance(inputPortsList.begin(),
													   itPort);
		}
	}

	// read the right input port name from the config
	const QString sConfigRightInputPort = conf->readEntry("RightOutputPort");
	int iConfigRightInputPortNumber = 0;

	// if there is a value which is not in the list already; add it
	if (!sConfigRightInputPort.isNull())
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(inputPortsList.begin(),
								inputPortsList.end(),
								sConfigRightInputPort)) == inputPortsList.end())
		{
			inputPortsList.push_back(sConfigRightInputPort);
			iConfigRightInputPortNumber = inputPortsList.size() - 1;
		}
		else
		{
			// remember the config input port name entry position
			iConfigRightInputPortNumber = std::distance(inputPortsList.begin(),
														itPort);
		}
	}
	else
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(inputPortsList.begin(),
								inputPortsList.end(),
								QString::fromLatin1(DEFAULT_RIGHT_INPUT_PORT))) != inputPortsList.end())
		{
			iConfigRightInputPortNumber = std::distance(inputPortsList.begin(),
														itPort);
		}
	}

	if (inputPortsList.empty())
	{
		assert(iConfigLeftInputPortNumber == 0 &&
			   iConfigRightInputPortNumber == 0);
		m_pLeftOutputPort->insertItem(QString::fromLatin1(DEFAULT_LEFT_INPUT_PORT));
		m_pRightOutputPort->insertItem(QString::fromLatin1(DEFAULT_RIGHT_INPUT_PORT));
	}
	else
	{
		// insert input port list into the left/right input channel combo
		for (std::vector<QString>::const_iterator iter = inputPortsList.begin(),
												 itEnd = inputPortsList.end();
			 iter != itEnd; ++iter)
		{
			m_pLeftOutputPort->insertItem(*iter);
			m_pRightOutputPort->insertItem(*iter);
		}
	}

	// set positions in the combo boxes
	m_pLeftOutputPort->setCurrentItem(iConfigLeftInputPortNumber);
	m_pRightOutputPort->setCurrentItem(iConfigRightInputPortNumber);

	/* output channels */

	// read the left output port name from the config
	const QString sConfigLeftOutputPort = conf->readEntry("LeftInputPort");
	int iConfigLeftOutputPortNumber = 0;

	// if there is a value which is not in the list already; add it
	if (!sConfigLeftOutputPort.isNull())
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(outputPortsList.begin(),
								outputPortsList.end(),
								sConfigLeftOutputPort)) == outputPortsList.end())
		{
			outputPortsList.push_back(sConfigLeftOutputPort);
			iConfigLeftOutputPortNumber = outputPortsList.size() - 1;
		}
		else
		{
			// remember the config output port name entry position
			iConfigLeftOutputPortNumber = std::distance(outputPortsList.begin(),
														itPort);
		}
	}
	else
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(outputPortsList.begin(),
								outputPortsList.end(),
								QString::fromLatin1(DEFAULT_LEFT_OUTPUT_PORT))) != outputPortsList.end())
		{
			iConfigLeftOutputPortNumber = std::distance(outputPortsList.begin(),
													   itPort);
		}
	}

	// read the right output port name from the config
	const QString sConfigRightOutputPort = conf->readEntry("RightInputPort");
	int iConfigRightOutputPortNumber = 0;

	// if there is a value which is not in the list already; add it
	if (!sConfigRightOutputPort.isNull())
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(outputPortsList.begin(),
								outputPortsList.end(),
								sConfigRightOutputPort)) == outputPortsList.end())
		{
			outputPortsList.push_back(sConfigRightOutputPort);
			iConfigRightOutputPortNumber = outputPortsList.size() - 1;
		}
		else
		{
			// remember the config output port name entry position
			iConfigRightOutputPortNumber = std::distance(outputPortsList.begin(),
														 itPort);
		}
	}
	else
	{
		std::vector<QString>::iterator itPort;
		if ((itPort = std::find(outputPortsList.begin(),
								outputPortsList.end(),
								QString::fromLatin1(DEFAULT_RIGHT_OUTPUT_PORT))) != outputPortsList.end())
		{
			iConfigRightOutputPortNumber = std::distance(outputPortsList.begin(),
														 itPort);
		}
	}

	if (outputPortsList.empty())
	{
		assert(iConfigLeftOutputPortNumber == 0 &&
			   iConfigRightOutputPortNumber == 0);
		m_pLeftInputPort->insertItem(QString::fromLatin1(DEFAULT_LEFT_OUTPUT_PORT));
		m_pRightInputPort->insertItem(QString::fromLatin1(DEFAULT_RIGHT_OUTPUT_PORT));
	}
	else
	{
		// insert output port list into the left/right output channel combo
		for (std::vector<QString>::const_iterator iter = outputPortsList.begin(),
												 itEnd = outputPortsList.end();
			 iter != itEnd; ++iter)
		{
			m_pLeftInputPort->insertItem(*iter);
			m_pRightInputPort->insertItem(*iter);
		}
	}

	// set positions in the combo boxes
	m_pLeftInputPort->setCurrentItem(iConfigLeftOutputPortNumber);
	m_pRightInputPort->setCurrentItem(iConfigRightOutputPortNumber);
}


void CrOptionsDialog::slotOk()
{
	KConfig* conf = KGlobal::config();
	conf->setGroup(QString::fromLatin1("Jack_Options"));

	conf->writeEntry("LeftInputPort", m_pLeftInputPort->currentText());
	conf->writeEntry("RightInputPort", m_pRightInputPort->currentText());
	conf->writeEntry("LeftOutputPort", m_pLeftOutputPort->currentText());
	conf->writeEntry("RightOutputPort", m_pRightOutputPort->currentText());
    conf->writeEntry("DisconnectedInput", m_disconnectedInputCheckBox->isChecked());
    conf->writeEntry("DisconnectedOutput", m_disconnectedOutputCheckBox->isChecked());

	accept();
}
