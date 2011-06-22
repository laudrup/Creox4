/***************************************************************************
					crsavenewpresetdialog.h  -  description
							 -------------------
		begin                : Sun Jul 1 16:38:43 2001
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

#ifndef CRSAVENEWPRESETDIALOG_H
#define CRSAVENEWPRESETDIALOG_H

#include <QVariant>
#include <q3ptrlist.h>
#include <QDialog>
#include <QCheckBox>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3Frame>
#include <Q3GridLayout>
#include <Q3HBoxLayout>
#include <QLabel>
#include "effectpreset.h"
#include "crpresetview.h"

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class Q3Frame;
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
	CrSaveNewPresetDialog(EffectKeeper* const effectKeeper, CrPresetView* const presetView, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
	~CrSaveNewPresetDialog();
protected slots:
	virtual void accept();
public:
	class CrCheckBox_private;
protected:
	EffectKeeper* m_effectKeeper;
	CrPresetView* m_presetView;

	Q3Frame* m_effectFrame;
	QLabel* m_effectTextLabel;
	Q3Frame* m_line1;
	Q3PtrList<CrCheckBox_private> m_effectCheckBoxList;
	Q3Frame* m_line2;
	QCheckBox* m_saveChainCheckBox;
	Q3Frame* m_presetFrame;
	QLabel* m_presetNameTextLabel;
	QLineEdit* m_presetNameLineEdit;
	QCheckBox* m_rootPresetCheckBox;
	QPushButton* m_okButton;
	QPushButton* m_cancelButton;

	Q3VBoxLayout* CrSaveNewPresetDialogLayout;
	Q3VBoxLayout* m_effectFrameLayout;
	Q3GridLayout* m_effectLayout;
	Q3VBoxLayout* m_presetFrameLayout;
	Q3HBoxLayout* m_buttonLayout;
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
