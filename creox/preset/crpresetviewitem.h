/***************************************************************************
					crpresetviewitem.h  -  description
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

#ifndef CRPRESETVIEWITEM_H
#define CRPRESETVIEWITEM_H

#include <qwidget.h>
#include <q3listview.h>
#include "effectpreset.h"

class EffectKeeper;
class QDataStream;

/**
	*@author Jozef Kosoru
	*/
class CrPresetViewItem : public Q3ListViewItem, public EffectPreset  {
public:
	CrPresetViewItem(const EffectPresetSaveRequest& saveRequest, const EffectKeeper* effectKeeper, Q3ListView *parent);
	CrPresetViewItem(QDataStream& dataStream, Q3ListView *parent);
	CrPresetViewItem(const EffectPresetSaveRequest& saveRequest, const EffectKeeper* effectKeeper, Q3ListViewItem *parent);
	CrPresetViewItem(QDataStream& dataStream, Q3ListViewItem *parent);
	~CrPresetViewItem();
private:
	CrPresetViewItem(const CrPresetViewItem&);
	CrPresetViewItem& operator=(const CrPresetViewItem&);

	void init();
};

#endif
