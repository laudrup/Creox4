/***************************************************************************
						crpresetpopupmenu.cpp  -  description
						 -------------------
		begin                : Sun Jul 1 2001
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
#include <typeinfo>
#include <QString>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <kstdguiitem.h>
#include "crpresetviewitem.h"
#include "crpresetview.h"
#include "creox.h"
#include "crpresetpopupmenu.h"

CrPresetPopupMenu::CrPresetPopupMenu(Q3ListViewItem* const selectedItem, QWidget* const parent, const char* const)
	: KMenu(parent), m_selectedItem(selectedItem)
{
  //XXX!! Set name
  //Creox* const creox = dynamic_cast<Creox*>(kapp->mainWidget());
  //KIconLoader* const iconLoader = KIconLoader::global();

        // XXX!
	//insertTitle(*(selectedItem->pixmap(0)), selectedItem->text(0));
	if(typeid(*selectedItem) == typeid(CrPresetViewItem)){
		insertItem(*(selectedItem->pixmap(0)), i18n("&Load Preset"), this, SLOT(slotLoadPreset()));
		insertSeparator();
	}
	//insertItem(iconLoader->loadIcon(creox->m_savePresetAction->icon(), KIcon::Small), creox->m_savePresetAction->text(), creox, SLOT(slotSaveNewPreset()));
	//insertItem(iconLoader->loadIcon(creox->m_newPresetFolderAction->icon(), KIcon::Small), creox->m_newPresetFolderAction->text(), creox, SLOT(slotNewPresetFolder()));
	//insertSeparator();
	//insertItem(iconLoader->loadIcon(QString::fromLatin1("list-remove"), KIcon::Small), i18n("&Delete"), this, SLOT(slotDeleteItem()));
}

void CrPresetPopupMenu::slotLoadPreset()
{
	dynamic_cast<CrPresetView*>(parent())->slotApplyPreset(m_selectedItem);
}

void CrPresetPopupMenu::slotDeleteItem()
{

  //XXX!
  /*
	if(typeid(*m_selectedItem) == typeid(CrPresetViewItem) || m_selectedItem->childCount()>0){
		const int answer =
			KMessageBox::warningContinueCancel(dynamic_cast<QWidget*>(parent()),
									  i18n("<p>Do you really want to delete <b>%1</b>?<p>").
																arg(m_selectedItem->text(0)),
									  i18n("Delete Item"),
									  i18n("Delete"));
		if(answer == KMessageBox::Cancel){
			return;
		}
	}
	delete m_selectedItem;
	m_selectedItem = 0;
  */
}

CrPresetPopupMenu::~CrPresetPopupMenu()
{
#ifdef _DEBUG
	std::cerr << "CrPresetPopupMenu deleted...\n";
#endif
}
