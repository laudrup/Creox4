/***************************************************************************
							crpresetview.h  -  description
						 -------------------
		begin                : Fri Jun 8 2001
		copyright            : (C) 2001 by Jozef Kosoru
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

#ifndef CRPRESETVIEW_H
#define CRPRESETVIEW_H

#include <new>
#include <qwidget.h>
#include <q3listview.h>
#include <q3valuelist.h>
#include <QStringList>

class EffectKeeper;
class QString;
class QDataStream;
class QPoint;

/**
	*@author Jozef Kosoru
	*/
class CrPresetViewGlobal {
public:
	static bool checkUniqueName(const Q3ListViewItem* item, const QString& folderName);
};

class CrPresetViewDir : public Q3ListViewItem {
public:
	CrPresetViewDir(const QString& dirName, Q3ListView *parent) : Q3ListViewItem(parent, dirName) { init(); }
	CrPresetViewDir(const QString& dirName, Q3ListViewItem *parent) : Q3ListViewItem(parent, dirName) { init(); }
	~CrPresetViewDir() { }
private:
	void init();
};

/**
	*@author Jozef Kosoru
	*/
class CrPresetView : public Q3ListView  {
	 Q_OBJECT
public:
	CrPresetView(EffectKeeper* effectKeeper, QWidget *parent=0, const char *name=0);
	~CrPresetView();
	void loadPresets();
	void savePresets();
public slots:
	void slotApplyPreset(Q3ListViewItem* preset);
private slots:
	void slotMouseButtonPressed(int button, Q3ListViewItem* item, const QPoint&, int);
	void slotShowPresetViewPopupMenu(Q3ListViewItem* item, const QPoint&, int);
private:
	void saveDir(QDataStream& dataStream, Q3ListViewItem* const parent) const;
	void loadDir(QDataStream& dataStream, Q3ListViewItem* const parent);

	EffectKeeper* m_effectKeeper;

	QList<int>* m_pDirIsOpenMap;
	QStringList* m_pDirNamesMap;

	QList<int>::const_iterator m_dirIsOpenIterator;
        QStringList::const_iterator m_dirNamesIterator;

};

#endif
