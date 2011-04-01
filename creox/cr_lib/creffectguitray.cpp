/***************************************************************************
					creffectguitray.cpp  -  description
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

#include "control.h"
#include <cassert>
#include <iostream>
#include <QLayout>
//Added by qt3to4:
#include <Q3Frame>
#include <Q3HBoxLayout>
#include "creffectguitray.h"
#include "creffectgui.h"

CrEffectGuiTray::CrEffectGuiTray(QWidget *parent, const char *name ) :
	Q3Frame(parent,name)
{
	//setBackgroundColor(Qt::black);
	setBackgroundMode(Qt::PaletteBackground);
	setFrameStyle(Q3Frame::Box | Q3Frame::Raised);
	setLineWidth(1);
	setMidLineWidth(1);
	setMargin(0);

	m_layout = new Q3HBoxLayout(this, 4, 0);
}

CrEffectGuiTray::~CrEffectGuiTray()
{
#ifdef _DEBUG
	std::cerr << "CrEffectGuiTray deleted...\n";
#endif
}

/** insert CrEffectGui widget into space */
void CrEffectGuiTray::insertEffectGuiWidget(CrEffectGui* effectGui)
{
	assert(effectGui->parent() == this);
	m_layout->addWidget(effectGui, 0, Qt::AlignVCenter);
}
