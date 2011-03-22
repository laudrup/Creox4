/***************************************************************************
					crfloatspinbox.cpp  -  description
							 -------------------
		begin                : Sat Oct 7 2000
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
#include <cassert>
#include <iostream>
#include <qvalidator.h>
#include <qstring.h>
#include "crvalidator.h"
#include "crfloatspinbox.h"

CrFloatSpinBox::CrFloatSpinBox(CrValidator* validator, QWidget* parent, const char* name)
  : QSpinBox(CrValidator::minValue(), CrValidator::maxValue(), 1, parent, name), m_validator(validator)
{
  //setValidator(validator);
	setLineStep(validator->getStep());
}

CrFloatSpinBox::~CrFloatSpinBox()
{

}

QString CrFloatSpinBox::mapValueToText(const int value)
{
	return QString().setNum(m_validator->getFloatValue(value), 'f', 2);
}

int CrFloatSpinBox::mapTextToValue(bool* ok)
{
	return m_validator->getIntValue(text().toFloat(ok));
}

