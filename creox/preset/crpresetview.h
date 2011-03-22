/***************************************************************************
							crpresetview.h  -  description
						 -------------------
		begin                : Fri Jun 8 2001
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

#ifndef CRPRESETVIEW_H
#define CRPRESETVIEW_H

#include <new>
#include <qwidget.h>
#include <qlistview.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include "crexception.h"

class EffectKeeper;
class QString;
class QDataStream;
class QPoint;

/**
	*@author Jozef Kosoru
	*/
namespace Cr {
class CrException_presetDataFileError : public CrException_runtimeError {
public:
	CrException_presetDataFileError(const QString& what_arg): CrException_runtimeError(what_arg) { }
};
}

/**
	*@author Jozef Kosoru
	*/
class CrPresetViewGlobal {
public:
	static bool checkUniqueName(const QListViewItem* item, const QString& folderName);
};

class CrPresetViewDir : public QListViewItem {
public:
	CrPresetViewDir(const QString& dirName, QListView *parent) : QListViewItem(parent, dirName) { init(); }
	CrPresetViewDir(const QString& dirName, QListViewItem *parent) : QListViewItem(parent, dirName) { init(); }
	~CrPresetViewDir() { }
private:
	void init();
};

/**
	*@author Jozef Kosoru
	*/
class CrPresetView : public QListView  {
	 Q_OBJECT
public:
	CrPresetView(EffectKeeper* effectKeeper, QWidget *parent=0, const char *name=0);
	~CrPresetView();
	void loadPresets() throw(Cr::CrException_presetDataFileError,std::bad_alloc);
	void savePresets() throw(Cr::CrException_presetDataFileError,std::bad_alloc);
public slots:
	void slotApplyPreset(QListViewItem* preset);
private slots:
	void slotMouseButtonPressed(int button, QListViewItem* item, const QPoint&, int);
	void slotShowPresetViewPopupMenu(QListViewItem* item, const QPoint&, int);
private:
	void saveDir(QDataStream& dataStream, QListViewItem* const parent) const;
	void loadDir(QDataStream& dataStream, QListViewItem* const parent);

	EffectKeeper* m_effectKeeper;

	QValueList<int>* m_pDirIsOpenMap;
	QStringList* m_pDirNamesMap;

	QValueListConstIterator<int> m_dirIsOpenIterator;
	QValueListConstIterator<QString> m_dirNamesIterator;

};

#endif
