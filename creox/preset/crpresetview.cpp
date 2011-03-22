/***************************************************************************
						crpresetview.cpp  -  description
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

#include "control.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <typeinfo>
#include <qheader.h>
#include <qstring.h>
#include <qfile.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qdatastream.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include "effectkeeper.h"
#include "crpresetviewitem.h"
#include "crpresetpopupmenu.h"
#include "crpresetview.h"

#define CR_PRESET_DATA_FILE_NAME          "preset.data"
#define CR_PRESET_DATA_FILE_VERSION       (1.0)
#define CR_PRESET_DATA_FILE_BEGIN_STRING  "CreoxPresetDataFile!"
#define CR_PRESET_DATA_FILE_BEGIN         (0x35a225f3)
#define CR_PRESET_DATA_FILE_PRESET_BEGIN  (0x11a2119c)
#define CR_PRESET_DATA_FILE_DIR_BEGIN     (0xffc9f001)
#define CR_PRESET_DATA_FILE_DIR_END       (0x0000ffff)

void CrPresetViewDir::init()
{
	KIconLoader* iconLoader = KGlobal::iconLoader();
	setPixmap(0, iconLoader->loadIcon(QString::fromLatin1("folder"), KIcon::Small));
	//setExpandable(true);
}

bool CrPresetViewGlobal::checkUniqueName(const QListViewItem* item, const QString& folderName)
{
	if(item){
		while(item){
			if(item->text(0) == folderName){
				KMessageBox::sorry(0, i18n("<p>A preset item named <b>%1</b> already exists!</p>").arg(folderName));
				return false;
			}
			item = item->nextSibling();
		}
	}
	return true;
}

CrPresetView::CrPresetView(EffectKeeper* effectKeeper, QWidget *parent, const char *name )
	: QListView(parent, name, WResizeNoErase|WRepaintNoErase), m_effectKeeper(effectKeeper)
{
	setFrameStyle(QFrame::Box | QFrame::Raised);
	setLineWidth(1);
	setMidLineWidth(1);
	setMargin(0);
	setFocusPolicy(ClickFocus);
	setRootIsDecorated(true);
	setSorting(-1);
	header()->hide();
	addColumn("");

	connect(this, SIGNAL(rightButtonPressed(QListViewItem*, const QPoint&, int)),
								SLOT(slotShowPresetViewPopupMenu(QListViewItem*, const QPoint&, int)));

	connect(this, SIGNAL(mouseButtonPressed(int,QListViewItem*,const QPoint&,int)), SLOT(slotMouseButtonPressed(int,QListViewItem*,const QPoint&,int)));
	connect(this, SIGNAL(returnPressed(QListViewItem*)), SLOT(slotApplyPreset(QListViewItem*)));
}

void CrPresetView::slotShowPresetViewPopupMenu(QListViewItem* item, const QPoint&, int)
{
	if(item){
		CrPresetPopupMenu popupMenu(item, this);
		popupMenu.exec(QCursor::pos() - QPoint(3,3));
	}
}

CrPresetView::~CrPresetView()
{
#ifdef _DEBUG
	std::cerr << "CrPresetView deleted...\n";
#endif
}

void CrPresetView::slotMouseButtonPressed(int button, QListViewItem* item, const QPoint&, int)
{
	if(button == LeftButton){
		slotApplyPreset(item);
	}
}

void CrPresetView::slotApplyPreset(QListViewItem* preset)
{
	if(preset && typeid(*preset) == typeid(CrPresetViewItem)){
		CrPresetViewItem* const effectPreset = dynamic_cast<CrPresetViewItem*>(preset);
		effectPreset->loadEffect(m_effectKeeper);
#ifdef _DEBUG
		std::cerr << "CrPresetView::slotApplyPreset - " << effectPreset->text(0).latin1() << "\n";
#endif
	}
}

void CrPresetView::loadPresets() throw(Cr::CrException_presetDataFileError,std::bad_alloc)
{
	clear();
	const KStandardDirs* const stdDirs = KGlobal::dirs();
	const QString dataFileName = stdDirs->findResource("appdata", QString::fromLatin1(CR_PRESET_DATA_FILE_NAME));
	if(!dataFileName.isNull()){
#ifdef _DEBUG
		std::cerr << "CrPresetView::loadPresets - loading: " << dataFileName.latin1() << "\n";
#endif
		QFile presetDataFile(dataFileName);
		if(presetDataFile.open(IO_ReadOnly)){
#ifdef _DEBUG
			std::cerr << "Data file opened!\n";
#endif
			QDataStream presetDataStream(&presetDataFile);

			char* beginString;
			Q_INT32 magicNumber;
			float version;
			presetDataStream >> beginString >> magicNumber >> version;

			if(std::strcmp(beginString, CR_PRESET_DATA_FILE_BEGIN_STRING) || magicNumber!=CR_PRESET_DATA_FILE_BEGIN){
				delete[] beginString;
				presetDataFile.close();
				throw(Cr::CrException_presetDataFileError(i18n("Bad preset data file magic type!\nCreox is unable to load presets.")));
			}
			delete[] beginString;
			if(version!=CR_PRESET_DATA_FILE_VERSION){
				presetDataFile.close();
				throw(Cr::CrException_presetDataFileError(i18n("Wrong preset data file version!\nCreox is unable to load presets.")));
			}

			KConfig* const conf = KGlobal::config();
			conf->setGroup("Preset_View");
			QValueList<int> dirIsOpenMap = conf->readIntListEntry(QString::fromLatin1("dirIsOpenMap"));
			QStringList dirNamesMap = conf->readListEntry(QString::fromLatin1("dirNamesMap"), '|');
			if(dirIsOpenMap.isEmpty() || dirNamesMap.isEmpty()){
				m_pDirIsOpenMap = 0;
				m_pDirNamesMap = 0;
			}
			else{
				m_pDirIsOpenMap = &dirIsOpenMap;
				m_pDirNamesMap = &dirNamesMap;
				m_dirIsOpenIterator = dirIsOpenMap.begin();
				m_dirNamesIterator = dirNamesMap.begin();
			}

			while(!presetDataStream.atEnd()){
				Q_INT32 itemType;
				presetDataStream >> itemType;
				switch(static_cast<int>(itemType)){
					case CR_PRESET_DATA_FILE_PRESET_BEGIN:
						try{
							(void) new CrPresetViewItem(presetDataStream, this);
						}catch(Cr::CrException_presetDataError& error){
#ifdef _DEBUG
							std::cerr << "Error loading preset: " << error.what().latin1() << "\n";
#endif
						}
						break;
					case CR_PRESET_DATA_FILE_DIR_BEGIN:
						{ //!! <-- bug in the compiler? (gcc 2.95.3) !!
							QString dirName;
							presetDataStream >> dirName;
							CrPresetViewDir* const rootDir = new CrPresetViewDir(dirName, this);

							if(m_pDirNamesMap && m_dirNamesIterator != m_pDirNamesMap->end()
							&& m_dirIsOpenIterator != m_pDirIsOpenMap->end()
							&& *m_dirNamesIterator == dirName){
								rootDir->setOpen(static_cast<bool>(*m_dirIsOpenIterator));
								++m_dirIsOpenIterator;
								++m_dirNamesIterator;
							}
							else{
								rootDir->setOpen(true);
							}

							loadDir(presetDataStream, rootDir);
						}
						break;
					default:
						throw(Cr::CrException_presetDataFileError(i18n("Preset data file inconsistency!\nCreox is unable to continue loading presets.")));
				}
			}

			presetDataFile.close();
			m_pDirIsOpenMap = 0;
			m_pDirNamesMap = 0;
		}
	}
}

void CrPresetView::savePresets() throw(Cr::CrException_presetDataFileError,std::bad_alloc)
{
	setSorting(0, false);
	sort();
	const KStandardDirs* const stdDirs = KGlobal::dirs();
	const QString dataFileName(stdDirs->saveLocation("appdata") + QString::fromLatin1(CR_PRESET_DATA_FILE_NAME));
	QFile presetDataFile(dataFileName);
	if(!presetDataFile.open(IO_WriteOnly)){
		throw(Cr::CrException_presetDataFileError(i18n("Cannot create preset data file!\n") + dataFileName));
	}
#ifdef _DEBUG
		std::cerr << "CrPresetView::savePresets - saving: " << dataFileName.latin1() << "\n";
#endif
	QDataStream presetDataStream(&presetDataFile);
	presetDataStream << CR_PRESET_DATA_FILE_BEGIN_STRING << static_cast<Q_INT32>(CR_PRESET_DATA_FILE_BEGIN) << static_cast<float>(CR_PRESET_DATA_FILE_VERSION);

	m_pDirIsOpenMap = new QValueList<int>;
	m_pDirNamesMap = new QStringList;

	QListViewItem* presetItem = firstChild();
	while(presetItem){
		if(typeid(*presetItem) == typeid(CrPresetViewItem)){
			presetDataStream << CR_PRESET_DATA_FILE_PRESET_BEGIN;
			dynamic_cast<EffectPreset*>(presetItem)->saveEffect(presetDataStream);
		}
		else{
			assert(typeid(*presetItem) == typeid(CrPresetViewDir));
			presetDataStream << CR_PRESET_DATA_FILE_DIR_BEGIN;
			presetDataStream << presetItem->text(0);

			m_pDirIsOpenMap->append( static_cast<int>(presetItem->isOpen()) );
			m_pDirNamesMap->append( presetItem->text(0) );

			saveDir(presetDataStream, presetItem);
		}
		presetItem = presetItem->nextSibling();
	}
	presetDataFile.close();

	KConfig* conf = KGlobal::config();
	conf->setGroup("Preset_View");
	conf->writeEntry(QString::fromLatin1("dirIsOpenMap"), *m_pDirIsOpenMap);
	conf->writeEntry(QString::fromLatin1("dirNamesMap"), *m_pDirNamesMap, '|');

	delete m_pDirIsOpenMap;
	m_pDirIsOpenMap = 0;
	delete m_pDirNamesMap;
	m_pDirNamesMap = 0;
}

void CrPresetView::saveDir(QDataStream& dataStream, QListViewItem* const parent) const
{
	QListViewItem* presetItem = parent->firstChild();
	while(presetItem){
		if(typeid(*presetItem) == typeid(CrPresetViewItem)){
			dataStream << CR_PRESET_DATA_FILE_PRESET_BEGIN;
			dynamic_cast<EffectPreset*>(presetItem)->saveEffect(dataStream);
		}
		else{
			assert(typeid(*presetItem) == typeid(CrPresetViewDir));
			dataStream << CR_PRESET_DATA_FILE_DIR_BEGIN;
			dataStream << presetItem->text(0);

			m_pDirIsOpenMap->append( static_cast<int>(presetItem->isOpen()) );
			m_pDirNamesMap->append( presetItem->text(0) );

			saveDir(dataStream, presetItem);
		}
		presetItem = presetItem->nextSibling();
	}
	dataStream << CR_PRESET_DATA_FILE_DIR_END;
}

void CrPresetView::loadDir(QDataStream& dataStream, QListViewItem* const parent)
{
	while(true){
		Q_INT32 itemType;
		dataStream >> itemType;
		switch(static_cast<int>(itemType)){
			case CR_PRESET_DATA_FILE_PRESET_BEGIN:
				try{
					(void) new CrPresetViewItem(dataStream, parent);
				}catch(Cr::CrException_presetDataError& error){
#ifdef _DEBUG
					std::cerr << "Error loading preset: " << error.what().latin1() << "\n";
#endif
				}
				break;
			case CR_PRESET_DATA_FILE_DIR_BEGIN:
				{ //!! <-- bug in the compiler? (gcc 2.95.3) !!
					QString dirName;
					dataStream >> dirName;
					CrPresetViewDir* const pPresetDir = new CrPresetViewDir(dirName, parent);

					if(m_pDirNamesMap && m_dirNamesIterator != m_pDirNamesMap->end()
					&& m_dirIsOpenIterator != m_pDirIsOpenMap->end()
					&& *m_dirNamesIterator == dirName){
						pPresetDir->setOpen(static_cast<bool>(*m_dirIsOpenIterator));
						++m_dirIsOpenIterator;
						++m_dirNamesIterator;
					}

					loadDir(dataStream, pPresetDir);
				}
				break;
			case CR_PRESET_DATA_FILE_DIR_END:
				return;
			default:
				throw(Cr::CrException_presetDataFileError(i18n("Preset data file inconsistency!\nCreox is unable to continue loading presets.")));
		}
	}
}
