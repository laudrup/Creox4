/***************************************************************************
					creffectgui.h  -  description
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

#ifndef CREFFECTGUI_H
#define CREFFECTGUI_H

#include <QWidget>

class SoundProcessor;
class SoundParameters;
class QString;

#define CR_FRAME_WIDTH (5)

/**
 *@author Jozef Kosoru
 */
class CrEffectGui : public QWidget  {
  Q_OBJECT

 public:
  CrEffectGui(QWidget *parent=0, const char *name=0);
  virtual ~CrEffectGui();

  QString effectName() const { return m_effectName; }

  virtual SoundProcessor* getProcessor() const = 0;
  virtual SoundParameters* getParameters() const = 0;
  virtual void synchronize() = 0;

 protected:
  void setEffectName(const QString& name) { m_effectName = name; }

  virtual void restoreParameters() = 0;
  virtual void saveParameters() = 0;

 private:
  QString m_effectName;
};

#endif
