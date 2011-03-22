/***************************************************************************
					crpresetviewitem.cpp  -  description
						 -------------------
		begin                : Mon Jun 18 2001
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
#include <qstring.h>
#include <kglobal.h>
#include <kiconloader.h>
#include "crpresetviewitem.h"

CrPresetViewItem::CrPresetViewItem(const EffectPresetSaveRequest& saveRequest, const EffectKeeper* effectKeeper, QListView *parent)
	: QListViewItem(parent), EffectPreset(saveRequest, effectKeeper)
{
	init();
}

CrPresetViewItem::CrPresetViewItem(QDataStream& dataStream, QListView *parent)
	: QListViewItem(parent), EffectPreset(dataStream)
{
	init();
}

CrPresetViewItem::CrPresetViewItem(const EffectPresetSaveRequest& saveRequest, const EffectKeeper* effectKeeper, QListViewItem *parent)
	: QListViewItem(parent), EffectPreset(saveRequest, effectKeeper)
{
	init();
}

CrPresetViewItem::CrPresetViewItem(QDataStream& dataStream, QListViewItem *parent)
	: QListViewItem(parent), EffectPreset(dataStream)
{
	init();
}

CrPresetViewItem::~CrPresetViewItem()
{
#ifdef _DEBUG_CRPRESETVIEWITEM
	std::cerr << "CrPresetViewItem: " << m_presetName.latin1() << " deleted...\n";
#endif
}

void CrPresetViewItem::init()
{
	KIconLoader* iconLoader = KGlobal::iconLoader();
	setPixmap(0, iconLoader->loadIcon(QString::fromLatin1((m_saveChain)? "preset_chain" : "preset_nochain"), KIcon::Small));
	setText(0, m_presetName);
}
