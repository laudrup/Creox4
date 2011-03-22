/***************************************************************************
					creffectguitray.h  -  description
						 -------------------
		begin                : Fri Apr 13 2001
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

#ifndef CREFFECTGUITRAY_H
#define CREFFECTGUITRAY_H

#include <qwidget.h>
#include <qframe.h>

class CrEffectGui;
class QHBoxLayout;

/**
	*@author Jozef Kosoru
	*/
class CrEffectGuiTray : public QFrame  {
	 Q_OBJECT
public:
	CrEffectGuiTray(QWidget *parent=0, const char *name=0);
	~CrEffectGuiTray();
	/** insert CrEffectGui widget into space */
	void insertEffectGuiWidget(CrEffectGui* effectGui);
private: // Private attributes
	/** main layout */
	QHBoxLayout* m_layout;
};

#endif
