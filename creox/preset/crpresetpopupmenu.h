/***************************************************************************
						crpresetpopupmenu.h  -  description
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

#ifndef CRPRESETPOPUPMENU_H
#define CRPRESETPOPUPMENU_H

#include <qwidget.h>
#include <kmenu.h>

class Q3ListViewItem;

/**
	*@author Jozef Kosoru
	*/
class CrPresetPopupMenu : public KMenu  {
	 Q_OBJECT
public:
	CrPresetPopupMenu(Q3ListViewItem* const selectedItem, QWidget* const parent=0, const char* const name=0);
	~CrPresetPopupMenu();
private:
	Q3ListViewItem* m_selectedItem;
private slots:
	void slotLoadPreset();
	void slotDeleteItem();
};

#endif
