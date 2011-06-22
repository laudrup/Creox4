/***************************************************************************
					creffectgui.cpp  -  description
						 -------------------
		begin                : Sun Dec 31 2000
		copyright            : (C) 2000 by Jozef Kosoru
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

#include <KLocale>
#include "creffectgui.h"

CrEffectGui::CrEffectGui(QWidget *parent, const char *name )
  : QWidget(parent,name), m_effectName(i18n("unknown effect"))
{

}

CrEffectGui::~CrEffectGui()
{

}
