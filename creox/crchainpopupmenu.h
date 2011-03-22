/***************************************************************************
					crchainpopupmenu.h  -  description
							 -------------------
		begin                : Sat May 19 2001
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

#ifndef CRCHAINPOPUPMENU_H
#define CRCHAINPOPUPMENU_H

#include <qwidget.h>
#include <kpopupmenu.h>

class CrChainButton;

/**
	*@author Jozef Kosoru
	*/
class CrChainPopupMenu : public KPopupMenu  {
	 Q_OBJECT
public:
	CrChainPopupMenu(CrChainButton *parent=0, const char *name=0);
	~CrChainPopupMenu();
private slots:
	void slotMoveUp();
	void slotMoveDown();
private:
	CrChainButton* m_parent;
};

#endif
