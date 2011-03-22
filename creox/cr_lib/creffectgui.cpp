/***************************************************************************
					creffectgui.cpp  -  description
						 -------------------
		begin                : Sun Dec 31 2000
		copyright            : (C) 2000 by Jozef Kosoru
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
#include <iostream>
#include <klocale.h>
#include "creffectgui.h"

CrEffectGui::CrEffectGui(QWidget *parent, const char *name )
		: QWidget(parent,name), m_effectName(i18n("unknown effect"))
{

}

CrEffectGui::~CrEffectGui()
{
#ifdef _DEBUG
	std::cerr << "CrEffectGui deleted..." << "\n";
#endif
}
