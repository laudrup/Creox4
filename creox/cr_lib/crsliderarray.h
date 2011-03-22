/***************************************************************************
						crsliderarray.h  -  description
							 -------------------
		begin                : Fri Oct 6 2000
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

#ifndef CRSLIDERARRAY_H
#define CRSLIDERARRAY_H

#include <qwidget.h>

class SoundProcessor;
class QGridLayout;
class QString;
class QSlider;
class QLabel;
class CrValidator;
class CrFloatSpinBox;
class CrSliderEntry;

#define CR_CELL_SPACING (5)

/**
	*@author Jozef Kosoru
	*/
class CrSliderArray : public QWidget  {
		Q_OBJECT
	public:
		CrSliderArray(const int entries, const int spinBoxWidth, SoundProcessor* processor=0, QWidget *parent=0, const char *name=0);
		~CrSliderArray();
		int entries() const { return m_entries; }
		int addSlider(const QString& name, const QString& unit, CrValidator* validator, float* variable=0, bool tracking=true, bool emitSignal=false);
		void synchronizeSlider(const int sliderNum=-1); // -1 = all sliders
		int spinBoxWidth() const { return m_spinBoxWidth; }
		int spinBoxPixelWidth() const;
		SoundProcessor* getProcessor() const { return m_processor; }
		int leftColWidth() const { return m_leftColWidth; }
		int setMinLeftColWidth(const int width=-1);
		int setMinRightColWidth(const int width=-1);
		float* changeSliderVariable(const int sliderNum, float* variable);
		CrSliderEntry* getSliderEntry(const int sliderNum) const { return m_sliderArray[sliderNum]; }
	private:
		friend class CrSliderEntry;
		int m_entries;
		int m_entryCount;
		int m_spinBoxWidth;
		SoundProcessor* m_processor;
		QGridLayout* m_layout;
		CrSliderEntry** m_sliderArray;
		int m_leftColWidth;
		QLabel* m_firstNameLabel;
		int m_rightColWidth;
		QLabel* m_firstUnitLabel;
};

/**
	*@author Jozef Kosoru
	*/
class CrSliderEntry : public QObject {
		Q_OBJECT
	public:
		CrSliderEntry(float* variable, CrValidator* validator, bool tracking, bool emitSignal, CrSliderArray* parent, const char *name=0);
		~CrSliderEntry();
		void synchronize();
	signals:
		void valueChanged(float);
	private:
		friend int CrSliderArray::addSlider(const QString& name, const QString& unit, CrValidator* validator, float* variable, bool tracking, bool emitSignal);
		friend float* CrSliderArray::changeSliderVariable(const int sliderNum, float* variable);
		float* m_variable;
		CrValidator* m_validator;
		bool m_tracking;
		bool m_emitSignal;
		QSlider* m_slider;
		CrFloatSpinBox* m_spinBox;
		CrSliderArray* m_parent;
		//!! fixes the bug in Qt-2.3.x !!
		int prevValueFromSlider;
	private slots:
		void slotChangeValueFromSpinBox(int value);
		void slotChangeValueFromSlider(int value);
};

#endif
