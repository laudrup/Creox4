/***************************************************************************
					crnewpresetfolderdialogimpl.cpp  -  description
						 -------------------
		begin                : Sat Jun 30 2001
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

#include "control.h"
#include <cassert>
#include <typeinfo>
#include <iostream>
#include <QLineEdit>
#include <QCheckBox>
#include <QString>
#include <kmessagebox.h>
#include <klocale.h>
#include "crpresetview.h"
#include "crpresetviewitem.h"
#include "crnewpresetfolderdialogimpl.h"

CrNewPresetFolderDialogImpl::CrNewPresetFolderDialogImpl(CrPresetView* const presetView, QWidget *parent, const char *name )
	: CrNewPresetFolderDialog(parent,name,true), m_presetView(presetView)
{
	m_folderNameLineEdit->selectAll();
	m_folderNameLineEdit->setFocus();
	if(!m_presetView->selectedItem()){
		m_rootFolderCheckBox->setChecked(true);
		m_rootFolderCheckBox->setEnabled(false);
	}
}

CrNewPresetFolderDialogImpl::~CrNewPresetFolderDialogImpl()
{
#ifdef _DEBUG
	std::cerr << "CrNewPresetFolderDialogImpl deleted...\n";
#endif
}

void CrNewPresetFolderDialogImpl::accept()
{
	const QString folderName(m_folderNameLineEdit->text().simplified());
	if(folderName.isEmpty()){
		KMessageBox::sorry(this, i18n("You have to specify a folder name!"));
		return;
	}
	Q3ListViewItem* selectedItem = m_presetView->selectedItem();
	if(!selectedItem || m_rootFolderCheckBox->isChecked()){
		if(!checkUniqueName(m_presetView->firstChild(), folderName)) return;
		(void) new CrPresetViewDir(folderName, m_presetView);
	}
	else{
		if(typeid(*(m_presetView->selectedItem())) == typeid(CrPresetViewDir)){
			if(!checkUniqueName(m_presetView->selectedItem()->firstChild(), folderName)) return;
			(void) new CrPresetViewDir(folderName, m_presetView->selectedItem());
			m_presetView->selectedItem()->setOpen(true);
		}
		else{
			assert(typeid(*(m_presetView->selectedItem())) == typeid(CrPresetViewItem));
			if(m_presetView->selectedItem()->parent()){ //item has a parent
				if(!checkUniqueName(m_presetView->selectedItem()->parent(), folderName)) return;
				(void) new CrPresetViewDir(folderName, m_presetView->selectedItem()->parent());
				m_presetView->selectedItem()->parent()->setOpen(true);
			}
			else{ // no parent --> top level item
				if(!checkUniqueName(m_presetView->firstChild(), folderName)) return;
				(void) new CrPresetViewDir(folderName, m_presetView);
			}
		}
	}
	done(Accepted);
}
