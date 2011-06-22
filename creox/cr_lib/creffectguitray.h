/***************************************************************************
					creffectguitray.h  -  description
						 -------------------
		begin                : Fri Apr 13 2001
		copyright            : (C) 2001 by Jozef Kosoru
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

#ifndef CREFFECTGUITRAY_H
#define CREFFECTGUITRAY_H

#include <QWidget>
#include <Q3Frame>
#include <Q3HBoxLayout>

class CrEffectGui;
class Q3HBoxLayout;

/**
 *@author Jozef Kosoru
 */
class CrEffectGuiTray : public Q3Frame  {
  Q_OBJECT

 public:
  CrEffectGuiTray(QWidget *parent=0, const char *name=0);
  ~CrEffectGuiTray();
  /** insert CrEffectGui widget into space */
  void insertEffectGuiWidget(CrEffectGui* effectGui);

 private: // Private attributes
  /** main layout */
  Q3HBoxLayout* m_layout;
};

#endif
