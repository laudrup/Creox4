/***************************************************************************
					crsliderarray.cpp  -  description
						 -------------------
		begin                : Fri Oct 6 2000
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

#include <QLayout>
#include <QString>
#include <QSlider>
#include <QLabel>
#include <QFontMetrics>
#include <Q3GridLayout>

#include "crfloatspinbox.h"
#include "crvalidator.h"
#include "soundprocessor.h"
#include "crsliderarray.h"

/* CrSliderArray */

CrSliderArray::CrSliderArray(const int entries, const int spinBoxWidth, SoundProcessor* processor, QWidget *parent, const char *name)
  : QWidget(parent, name), m_entries(entries), m_entryCount(0), m_spinBoxWidth(spinBoxWidth), m_processor(processor), m_leftColWidth(0),
    m_firstNameLabel(0), m_rightColWidth(0), m_firstUnitLabel(0)
{
  m_layout = new Q3GridLayout(this, m_entries, 4, 0, CR_CELL_SPACING, "CrSliderArray_layout");
  m_sliderArray = new CrSliderEntry*[m_entries];
  for(int count=0; count<m_entries; count++){
    m_sliderArray[count] = 0;
  }
  if(m_spinBoxWidth<1){
    m_spinBoxWidth=1;
  }
}

CrSliderArray::~CrSliderArray()
{
  delete[] m_sliderArray;
}

int CrSliderArray::addSlider(const QString& name, const QString& unit, CrValidator* validator, float* variable, bool tracking, bool emitSignal)
{
  //assert(m_entryCount<m_entries);
  m_sliderArray[m_entryCount] = new CrSliderEntry(variable, validator, tracking, emitSignal, this, "CrSliderArray_sliderArray");

  QLabel* const nameLabel = new QLabel(name, this, "CrSliderArray_nameLabel");
  nameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  const int hintWidth = nameLabel->sizeHint().width();
  if(m_leftColWidth < hintWidth){
    m_leftColWidth = hintWidth;
  }
  if(!m_entryCount){
    m_firstNameLabel = nameLabel;
  }
  QLabel* const unitLabel = new QLabel(unit, this, "CrSliderArray_unitLabel");
  unitLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
  const int hintWidth2 = unitLabel->sizeHint().width();
  if(m_rightColWidth < hintWidth2){
    m_rightColWidth = hintWidth2;
  }
  if(!m_entryCount){
    m_firstUnitLabel = unitLabel;
  }

  m_layout->addWidget(nameLabel, m_entryCount, 0);
  m_layout->addWidget(m_sliderArray[m_entryCount]->m_slider, m_entryCount, 1);
  m_layout->addWidget(m_sliderArray[m_entryCount]->m_spinBox, m_entryCount, 2);
  m_layout->addWidget(unitLabel, m_entryCount, 3);
  m_layout->setColStretch(0, 0);
  m_layout->setColStretch(1, 100);
  m_layout->setColStretch(2, 0);
  m_layout->setColStretch(3, 0);
  m_layout->setRowStretch(m_entryCount , 0);

  return m_entryCount++;
}

int CrSliderArray::setMinLeftColWidth(const int width)
{
  if(width != -1 && m_leftColWidth < width){
    m_leftColWidth = width;
    if(m_firstNameLabel){
      m_firstNameLabel->setFixedWidth(width);
    }
  }
  return m_leftColWidth;
}

int CrSliderArray::setMinRightColWidth(const int width)
{
  if(width != -1 && m_rightColWidth < width){
    m_rightColWidth = width;
    if(m_firstUnitLabel){
      m_firstUnitLabel->setFixedWidth(width);
    }
  }
  return m_rightColWidth;
}

void CrSliderArray::synchronizeSlider(const int sliderNum)
{
  //assert(sliderNum>-2 && sliderNum<m_entryCount);
  if(sliderNum==-1){
    for(int count=0; count<m_entryCount; count++){
      //assert(m_sliderArray[count]);
      m_sliderArray[count]->synchronize();
    }
  }
  else{
    //assert(m_sliderArray[sliderNum]);
    m_sliderArray[sliderNum]->synchronize();
  }
}

float* CrSliderArray::changeSliderVariable(const int sliderNum, float* variable)
{
  //assert(sliderNum>-1 && sliderNum<m_entryCount);
  float* oldVariable = m_sliderArray[sliderNum]->m_variable;
  m_sliderArray[sliderNum]->m_variable = variable;
  m_sliderArray[sliderNum]->synchronize();
  return oldVariable;
}

int CrSliderArray::spinBoxPixelWidth() const
{
  //get width according spinBoxWidth
  QString widthSample;
  for(int count=0; count<(m_spinBoxWidth+4); count++){
    widthSample+='0';
  };
  const QFontMetrics fm(font());
  return fm.width(widthSample);
}

/* CrSliderEntry */

CrSliderEntry::CrSliderEntry(float* variable, CrValidator* validator, bool tracking, bool emitSignal, CrSliderArray* parent, const char *name)
  : QObject(parent, name), m_variable(variable), m_validator(validator), m_tracking(tracking), m_emitSignal(emitSignal), m_parent(parent),
    prevValueFromSlider(-999999)
{
  const int intValue = m_validator->getIntValue(*m_variable);
  m_slider = new QSlider(CrValidator::minValue(), CrValidator::maxValue(),
                         1, intValue, Qt::Horizontal, parent,
                         "CrSliderEntry_slider");
  m_slider->setTracking(m_tracking);
  m_slider->setTickmarks(QSlider::NoTicks);

  m_spinBox = new CrFloatSpinBox(m_validator, parent, "CrSliderEntry_spinBox");
  m_spinBox->setValue(intValue);
  //set width according spinBoxWidth
  QString widthSample;
  for(int count=0; count<(parent->spinBoxWidth()+5); count++){
    widthSample+='0';
  };
  const QFontMetrics fm(m_spinBox->font());
  const int w = fm.width(widthSample);
  m_spinBox->setFixedWidth(w);

  connect(m_slider, SIGNAL(valueChanged(int)), this, SLOT(slotChangeValueFromSlider(int)));
  connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeValueFromSpinBox(int)));
}

CrSliderEntry::~CrSliderEntry()
{
}

void CrSliderEntry::slotChangeValueFromSlider(int value)
{
  //!! fixes the bug in Qt-2.3.x !!
  if(value == prevValueFromSlider) return;
  prevValueFromSlider = value;

  *m_variable = m_validator->getFloatValue(value);

  m_parent->m_processor->updateParameters();

  m_spinBox->blockSignals(true);
  m_spinBox->setValue(value);
  m_spinBox->blockSignals(false);

  if(m_emitSignal){
    emit valueChanged(*m_variable);
  }
}

void CrSliderEntry::slotChangeValueFromSpinBox(int value)
{
  *m_variable = m_validator->getFloatValue(value);
  m_parent->m_processor->updateParameters();

  m_slider->blockSignals(true);
  m_slider->setValue(value);
  m_slider->blockSignals(false);

  if(m_emitSignal){
    emit valueChanged(*m_variable);
  }
}

void CrSliderEntry::synchronize()
{
  const int value = m_validator->getIntValue(*m_variable);

  m_slider->blockSignals(true);
  m_spinBox->blockSignals(true);

  m_slider->setValue(value);
  m_spinBox->setValue(value);

  m_slider->blockSignals(false);
  m_spinBox->blockSignals(false);
}
