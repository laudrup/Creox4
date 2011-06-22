/***************************************************************************
					crchainpopupmenu.cpp  -  description
							 -------------------
		begin                : Sat May 19 2001
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

#include <QString>

#include <KLocale>
#include <KGlobal>
#include <KIconLoader>

#include "soundprocessor.h"
#include "creffectgui.h"
#include "crchainview.h"
#include "crchainbutton.h"
#include "crchainpopupmenu.h"

CrChainPopupMenu::CrChainPopupMenu(CrChainButton* parent, const char* name )
  : KMenu(name, parent), m_parent(parent)
{
  KIconLoader* const iconLoader = KIconLoader::global();
  // XXX!
  //insertTitle(iconLoader->loadIcon(QString::fromLatin1(m_parent->getEffect()->getProcessor()->getName()), KIcon::Small), m_parent->getEffect()->effectName());

  insertItem(iconLoader->loadIcon("go-up", KIconLoader::Small), i18n("Move Up"), this, SLOT(slotMoveUp()), 0, 0);
  insertItem(iconLoader->loadIcon("go-down", KIconLoader::Small), i18n("Move Down"), this, SLOT(slotMoveDown()), 0, 1);

  if(m_parent->getParentChainView()->isFirstButton(m_parent)){
    setItemEnabled(0, false);
  }

  if(m_parent->getParentChainView()->isLastButton(m_parent)){
    setItemEnabled(1, false);
  }
}

CrChainPopupMenu::~CrChainPopupMenu()
{
}

void CrChainPopupMenu::slotMoveUp()
{
  m_parent->getParentChainView()->moveUp(m_parent);
}

void CrChainPopupMenu::slotMoveDown()
{
  m_parent->getParentChainView()->moveDown(m_parent);
}

