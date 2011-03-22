/***************************************************************************
					crnewpresetfolderdialogimpl.h  -  description
							 -------------------
		begin                : Sat Jun 30 2001
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

#ifndef CRNEWPRESETFOLDERDIALOGIMPL_H
#define CRNEWPRESETFOLDERDIALOGIMPL_H

#include <qwidget.h>
#include "crnewpresetfolderdialog.h"
#include "crpresetview.h"

class QString;

/**
	*@author Jozef Kosoru
	*/
class CrNewPresetFolderDialogImpl : public CrNewPresetFolderDialog, public CrPresetViewGlobal {
	 Q_OBJECT
public:
	CrNewPresetFolderDialogImpl(CrPresetView* const presetView, QWidget *parent=0, const char *name=0);
	~CrNewPresetFolderDialogImpl();
protected slots:
	virtual void accept();
private:
	CrPresetView* m_presetView;
};

#endif
