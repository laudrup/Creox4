/***************************************************************************
						crdistortion.h  -  description
								 -------------------
		begin                : Tue Dec 26 2000
		copyright            : (C) 2000 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CRDISTORTION_H
#define CRDISTORTION_H

#include "creffectgui.h"
#include "distortionprocessor.h"

class QComboBox;
class CrSliderArray;

/**
	*@author Jozef Kosoru
	*/
class CrDistortion : public CrEffectGui  {
		Q_OBJECT
	public:
		CrDistortion(QWidget *parent=0, const char *name=0);
		~CrDistortion();
		virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }
	protected:
		void restoreParameters();
		void saveParameters();
	private:
		DistortionProcessor* m_distortion;
		DistortionParameters* m_dpar;

		CrSliderArray* m_mixArray;
		CrSliderArray* m_inputBassArray;
		CrSliderArray* m_outputBassArray;
		CrSliderArray* m_accentArray;
		CrSliderArray* m_mainDriveArray;
		CrSliderArray* m_lowPassArray;
		QComboBox* m_accentFreqCombo;
		QComboBox* m_mainDriveCombo;
	public:
		DistortionProcessor* getProcessor() const { return m_distortion; }
		DistortionParameters* getParameters() const { return m_dpar; }
		virtual void synchronize();
	private slots:
		void slotChangeAccentFreqMode(int mode);
		void slotChangeMainDriveMode(int mode);
};

#endif
