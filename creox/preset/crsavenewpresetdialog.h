/***************************************************************************
					crsavenewpresetdialog.h  -  description
							 -------------------
		begin                : Sun Jul 1 16:38:43 2001
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

#ifndef CRSAVENEWPRESETDIALOG_H
#define CRSAVENEWPRESETDIALOG_H

#include <qvariant.h>
#include <qptrlist.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include "effectpreset.h"
#include "crpresetview.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;
class EffectKeeper;

/**
	*@author Jozef Kosoru
	*/
class CrSaveNewPresetDialog : public QDialog, public EffectPresetSaveRequest, public CrPresetViewGlobal
{
	Q_OBJECT
public:
	CrSaveNewPresetDialog(EffectKeeper* const effectKeeper, CrPresetView* const presetView, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
	~CrSaveNewPresetDialog();
protected slots:
	virtual void accept();
public:
	class CrCheckBox_private;
protected:
	EffectKeeper* m_effectKeeper;
	CrPresetView* m_presetView;

	QFrame* m_effectFrame;
	QLabel* m_effectTextLabel;
	QFrame* m_line1;
	QPtrList<CrCheckBox_private> m_effectCheckBoxList;
	QFrame* m_line2;
	QCheckBox* m_saveChainCheckBox;
	QFrame* m_presetFrame;
	QLabel* m_presetNameTextLabel;
	QLineEdit* m_presetNameLineEdit;
	QCheckBox* m_rootPresetCheckBox;
	QPushButton* m_okButton;
	QPushButton* m_cancelButton;

	QVBoxLayout* CrSaveNewPresetDialogLayout;
	QVBoxLayout* m_effectFrameLayout;
	QGridLayout* m_effectLayout;
	QVBoxLayout* m_presetFrameLayout;
	QHBoxLayout* m_buttonLayout;
};

/**
	*@author Jozef Kosoru
	*/
class CrSaveNewPresetDialog::CrCheckBox_private : public QCheckBox
{
	Q_OBJECT
public:
	CrCheckBox_private(const int processorId, QWidget* const parent, const char* const name=0)
		: QCheckBox(parent, name), m_processorId(processorId) { }
	~CrCheckBox_private() { }
	int m_processorId;
};

#endif // CRSAVENEWPRESETDIALOG_H
