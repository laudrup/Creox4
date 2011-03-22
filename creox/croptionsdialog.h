/***************************************************************************
					croptionsdialog.h  -  description
						 -------------------
		begin                : Sun Aug 26 2001
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

#ifndef CROPTIONSDIALOG_H
#define CROPTIONSDIALOG_H

#include <qwidget.h>
#include <kdialogbase.h>

class QComboBox;
class QCheckBox;

/**	Creox Options dialog.
 *	@author	Jozef Kosoru
 */
class CrOptionsDialog : public KDialogBase
{
	 Q_OBJECT

public:
	CrOptionsDialog(QWidget *parent=0, const char *name=0);
	~CrOptionsDialog();

public slots:
	virtual void slotOk();

private:
	void loadSettings();

private:
	// tab1 - JACK options
	QComboBox* m_pLeftInputPort;
	QComboBox* m_pRightInputPort;
	QCheckBox* m_disconnectedInputCheckBox;

	QComboBox* m_pLeftOutputPort;
	QComboBox* m_pRightOutputPort;
	QCheckBox* m_disconnectedOutputCheckBox;

public:
	static const char* const DEFAULT_LEFT_INPUT_PORT;
	static const char* const DEFAULT_RIGHT_INPUT_PORT;
	static const char* const DEFAULT_LEFT_OUTPUT_PORT;
	static const char* const DEFAULT_RIGHT_OUTPUT_PORT;
};

#endif
