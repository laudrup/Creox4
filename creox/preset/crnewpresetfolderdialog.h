/***************************************************************************
					crnewpresetfolderdialog.h  -  description
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

#ifndef CRNEWPRESETFOLDERDIALOG_H
#define CRNEWPRESETFOLDERDIALOG_H

#include <QVariant>
#include <QDialog>
//Added by qt3to4:
#include <Q3GridLayout>
#include <Q3Frame>
#include <QLabel>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

class Q3VBoxLayout;
class Q3HBoxLayout;
class Q3GridLayout;
class QCheckBox;
class Q3Frame;
class QLabel;
class QLineEdit;
class QPushButton;

class CrNewPresetFolderDialog : public QDialog
{
	Q_OBJECT

public:
	CrNewPresetFolderDialog(QWidget* parent = 0, const char* name = 0,
							bool modal = FALSE, Qt::WFlags fl = 0 );
	~CrNewPresetFolderDialog();

	Q3Frame* m_mainFrame;
	QLabel* m_textLabel;
	QLineEdit* m_folderNameLineEdit;
	QCheckBox* m_rootFolderCheckBox;
	QPushButton* m_okButton;
	QPushButton* m_cancelButton;

protected:
	Q3VBoxLayout* CrNewPresetFolderDialogLayout;
	Q3VBoxLayout* m_mainFrameLayout;
	Q3HBoxLayout* m_buttonLayout;
};

#endif // CRNEWPRESETFOLDERDIALOG_H
