/***************************************************************************
					crsavenewpresetdialog.cpp  -  description
							 -------------------
		begin                : Sun Jul 1 16:39:29 2001
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


#include <cassert>
#include <iostream>
#include <typeinfo>
#include <q3frame.h>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3GridLayout>
#include <Q3VBoxLayout>
#include <klocale.h>
#include <kmessagebox.h>
#include "effectkeeper.h"
#include "crpresetview.h"
#include "crpresetviewitem.h"
#include "crchainview.h"
#include "creffectgui.h"
#include "soundprocessor.h"
#include "crsavenewpresetdialog.h"

CrSaveNewPresetDialog::CrSaveNewPresetDialog(EffectKeeper* const effectKeeper, CrPresetView* const presetView, QWidget* parent, const char* name, Qt::WFlags fl )
	: QDialog( parent, name, true, fl ), m_effectKeeper(effectKeeper), m_presetView(presetView)
{
	if(!name) setName( "CrSaveNewPresetDialog" );
	resize( 248, 238 );
	setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)3, sizePolicy().hasHeightForWidth() ) );
	setCaption( i18n( "Save New Preset" ) );
	CrSaveNewPresetDialogLayout = new Q3VBoxLayout( this );
	CrSaveNewPresetDialogLayout->setSpacing( 3 );
	CrSaveNewPresetDialogLayout->setMargin( 3 );

	m_effectFrame = new Q3Frame( this, "m_effectFrame" );
	m_effectFrame->setFrameShape( Q3Frame::StyledPanel );
	m_effectFrame->setFrameShadow( Q3Frame::Raised );
	m_effectFrame->setLineWidth( 1 );
	m_effectFrame->setMargin( 0 );
	m_effectFrame->setMidLineWidth( 0 );

	m_effectFrameLayout = new Q3VBoxLayout( m_effectFrame );
	m_effectFrameLayout->setSpacing( 3 );
	m_effectFrameLayout->setMargin( 6 );

	m_effectTextLabel = new QLabel( m_effectFrame, "m_effectTextLabel" );
	m_effectTextLabel->setText( i18n( "<b>Select effects to save:</b>" ) );
	m_effectFrameLayout->addWidget( m_effectTextLabel );

	m_line1 = new Q3Frame( m_effectFrame, "m_line1" );
	m_line1->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
	m_effectFrameLayout->addWidget( m_line1 );

	// ------------------ effects ---------------------------
	m_effectLayout = new Q3GridLayout(this);
	m_effectLayout->setSpacing( 5 );
	m_effectLayout->setMargin( 0 );

	// create  effect QCheckBoxes
	int activeEffectsCount = 0;
	int xPos=0, yPos=0;
	for(QListIterator<CrEffectGui*> effectIterator(m_effectKeeper->effectList()) ; effectIterator.hasNext();){
          CrEffectGui* effect = effectIterator.next();
		CrCheckBox_private* const effectCheckBox = new CrCheckBox_private(effect->getProcessor()->getId(), m_effectFrame);
		effectCheckBox->setText(effect->effectName());
		if(effect->getProcessor()->mode() == SoundProcessor::enabled){
			effectCheckBox->setChecked(true);
			activeEffectsCount++;
		}
		else{
			effectCheckBox->setEnabled(false);
		}
		m_effectLayout->addWidget(effectCheckBox, yPos++, xPos);
		if(yPos==3){
			xPos++;
			yPos=0;
		}
		m_effectCheckBoxList.append(effectCheckBox);
	}

	m_effectFrameLayout->addLayout( m_effectLayout );
	// ------------------------------------------------------

	m_line2 = new Q3Frame( m_effectFrame, "m_line2" );
	m_line2->setFrameStyle( Q3Frame::HLine | Q3Frame::Sunken );
	m_effectFrameLayout->addWidget( m_line2 );

	m_saveChainCheckBox = new QCheckBox( m_effectFrame, "m_saveChainCheckBox" );
	m_saveChainCheckBox->setText( i18n( "Save chain (Effects order)" ) );
	if(activeEffectsCount > 1){
		m_saveChainCheckBox->setChecked(true);
	}

	m_effectFrameLayout->addWidget( m_saveChainCheckBox );
	CrSaveNewPresetDialogLayout->addWidget( m_effectFrame );

	m_presetFrame = new Q3Frame( this, "m_presetFrame" );
	m_presetFrame->setFrameShape( Q3Frame::StyledPanel );
	m_presetFrame->setFrameShadow( Q3Frame::Raised );
	m_presetFrameLayout = new Q3VBoxLayout( m_presetFrame );
	m_presetFrameLayout->setSpacing( 3 );
	m_presetFrameLayout->setMargin( 6 );

	m_presetNameTextLabel = new QLabel( m_presetFrame, "m_presetNameTextLabel" );
	m_presetNameTextLabel->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)0, m_presetNameTextLabel->sizePolicy().hasHeightForWidth() ) );
	m_presetNameTextLabel->setText( i18n( "<b>New Preset Name:</b>" ) );
	m_presetFrameLayout->addWidget( m_presetNameTextLabel );

	m_presetNameLineEdit = new QLineEdit( m_presetFrame, "m_presetNameLineEdit" );
	m_presetNameLineEdit->setFocusPolicy( Qt::StrongFocus );
	m_presetNameLineEdit->setText( i18n( "New Preset" ) );
	m_presetNameLineEdit->setEdited( false );
	m_presetNameLineEdit->selectAll();
	m_presetNameLineEdit->setFocus();
	m_presetFrameLayout->addWidget( m_presetNameLineEdit );

	m_rootPresetCheckBox = new QCheckBox( m_presetFrame, "m_rootPresetCheckBox" );
	m_rootPresetCheckBox->setText( i18n( "Root Preset" ) );
	if(!m_presetView->selectedItem()){
		m_rootPresetCheckBox->setChecked(true);
		m_rootPresetCheckBox->setEnabled(false);
	}

	m_presetFrameLayout->addWidget( m_rootPresetCheckBox );

	CrSaveNewPresetDialogLayout->addWidget( m_presetFrame );
	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	CrSaveNewPresetDialogLayout->addItem( spacer );

	m_buttonLayout = new Q3HBoxLayout;
	m_buttonLayout->setSpacing( 6 );
	m_buttonLayout->setMargin( 0 );
	QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	m_buttonLayout->addItem( spacer_2 );

	m_okButton = new QPushButton( this, "m_okButton" );
	m_okButton->setText( i18n( "Save" ) );
	m_okButton->setDefault( true );
	m_buttonLayout->addWidget( m_okButton );

	m_cancelButton = new QPushButton( this, "m_cancelButton" );
	m_cancelButton->setText( i18n( "Cancel" ) );
	m_buttonLayout->addWidget( m_cancelButton );
	QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	m_buttonLayout->addItem( spacer_3 );
	CrSaveNewPresetDialogLayout->addLayout( m_buttonLayout );

	// signals and slots connections
	connect( m_okButton, SIGNAL( released() ), this, SLOT( accept() ) );
	connect( m_cancelButton, SIGNAL( released() ), this, SLOT( reject() ) );
	connect( m_presetNameLineEdit, SIGNAL( returnPressed() ), this, SLOT( accept() ) );

	// tab order
	//setTabOrder( m_checkBox5, m_saveChainCheckBox );
	setTabOrder( m_saveChainCheckBox, m_presetNameLineEdit );
	setTabOrder( m_presetNameLineEdit, m_rootPresetCheckBox );
	setTabOrder( m_rootPresetCheckBox, m_okButton );
	setTabOrder( m_okButton, m_cancelButton );
}

void CrSaveNewPresetDialog::accept()
{
	const QString folderName(m_presetNameLineEdit->text().simplified());
	Q3ListViewItem* newPresetParent;

	// check whether a preset name is unique
	if(folderName.isEmpty()){
		KMessageBox::sorry(this, i18n("You have to specify a preset name!"));
		return;
	}
	Q3ListViewItem* selectedItem = m_presetView->selectedItem();
	if(!selectedItem || m_rootPresetCheckBox->isChecked()){
		if(!checkUniqueName(m_presetView->firstChild(), folderName)) return;
		newPresetParent=0;
	}
	else{
		if(typeid(*(m_presetView->selectedItem())) == typeid(CrPresetViewDir)){
			if(!checkUniqueName(m_presetView->selectedItem()->firstChild(), folderName)) return;
			newPresetParent = m_presetView->selectedItem();
		}
		else{
			assert(typeid(*(m_presetView->selectedItem())) == typeid(CrPresetViewItem));
			if(m_presetView->selectedItem()->parent()){ //item(preset) has a parent(folder)
				if(!checkUniqueName(m_presetView->selectedItem()->parent(), folderName)) return;
				newPresetParent = m_presetView->selectedItem()->parent();
			}
			else{ // no parent --> top level item(preset)
				if(!checkUniqueName(m_presetView->firstChild(), folderName)) return;
				newPresetParent=0;
			}
		}
	}

	m_presetName = folderName;
	m_saveChain = m_saveChainCheckBox->isChecked();

	for(Q3PtrListIterator<CrCheckBox_private> checkBoxIterator(m_effectCheckBoxList) ; checkBoxIterator.current(); ++checkBoxIterator ){
		if(checkBoxIterator.current()->isChecked()){
			m_processorIdList.push_back(checkBoxIterator.current()->m_processorId);
		}
	}

	if(m_processorIdList.size() > 0){
		if(!newPresetParent){
			(void) new CrPresetViewItem(*this, m_effectKeeper, m_presetView);
		}
		else{
			(void) new CrPresetViewItem(*this, m_effectKeeper, newPresetParent);
			newPresetParent->setOpen(true);
		}
	}

	done(Accepted);
}


/*
 *  Destroys the object and frees any allocated resources
 */
CrSaveNewPresetDialog::~CrSaveNewPresetDialog()
{
	// no need to delete child widgets, Qt does it all for us
}
