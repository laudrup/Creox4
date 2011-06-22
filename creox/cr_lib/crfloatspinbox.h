/***************************************************************************
						crfloatspinbox.h  -  description
							 -------------------
		begin                : Sat Oct 7 2000
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

#ifndef CRFLOATSPINBOX_H
#define CRFLOATSPINBOX_H

#include <QWidget>
#include <QSpinBox>

class CrValidator;
class QStrign;

/**
 *@author Jozef Kosoru
 */
class CrFloatSpinBox : public QSpinBox  {
  Q_OBJECT
 public:
  CrFloatSpinBox(CrValidator* validator, QWidget* parent=0, const char* name=0);
  ~CrFloatSpinBox();

 protected:
  virtual QString mapValueToText(const int value);
  virtual int mapTextToValue(bool* ok);

 private:
  CrValidator* m_validator;
};

#endif
