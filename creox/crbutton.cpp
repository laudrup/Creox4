/***************************************************************************
						crbutton.cpp  -  description
						 -------------------
		begin                : Sun Apr 15 2001
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
#include <qbrush.h>
#include <iostream>
#include <qsize.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qfontmetrics.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <kiconloader.h>
#include <kglobal.h>
#include "crbutton.h"

#define ICON_VOFFSET 5
#define ICON_HOFFSET 10
#define TEXT_HOFFSET 5

CrButton::CrButton(QWidget *parent, const char *name ) :
	QButton(parent ,name, WResizeNoErase|WRepaintNoErase), m_heightSizeHint(25), m_iconWidth(0),
	m_intermediateState(false)
{
	setToggleType(QButton::Toggle);
	setBackgroundMode(NoBackground);
	setFocusPolicy(NoFocus);
}

CrButton::~CrButton()
{

}

QSize CrButton::sizeHint() const
{
	int widthSizeHint = 100;
	if(!text().isNull()){
		const QFontMetrics fm(font());
		widthSizeHint = fm.width(text()) + (ICON_HOFFSET*2) + TEXT_HOFFSET + m_iconWidth;
	}
	return QSize(widthSizeHint, m_heightSizeHint);
}

void CrButton::drawButton(QPainter* painter)
{
	//!! it is not possible to distinguish between the on/off state of buttons
	//!! in such themes like Marble,System++...
	const QColorGroup origColorGroup(colorGroup());
	QColorGroup buttonColorGroup(colorGroup());

	if(m_intermediateState == true){
		//#! Qt-3.2.x bug?
		//setBackgroundMode(PaletteMid);
		buttonColorGroup.setColor(QColorGroup::Button, origColorGroup.mid());
		buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.foreground());
		buttonColorGroup.setColor(QColorGroup::Midlight, origColorGroup.dark());
		buttonColorGroup.setColor(QColorGroup::Dark, origColorGroup.midlight());
		//!! The mistery of the black line !!
	}
	else if(isOn() && !isDown()){
		//#! Qt-3.2.x bug?
		//setBackgroundMode(PaletteButton);
		buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.dark());
		buttonColorGroup.setColor(QColorGroup::Dark, origColorGroup.light());
		buttonColorGroup.setColor(QColorGroup::Midlight, origColorGroup.mid());
#ifdef _DEBUG_CB
		std::cerr << "On\n";
#endif
	}
	else if(isOn() && isDown()){
		//#! Qt-3.2.x bug?
		//setBackgroundMode(PaletteButton);
		buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.foreground());
		buttonColorGroup.setColor(QColorGroup::Midlight, origColorGroup.dark());
		buttonColorGroup.setColor(QColorGroup::Dark, origColorGroup.midlight());
		//!! The mistery of the black line !!
/*
		buttonColorGroup.setColor(QColorGroup::Background, Qt::green);
		buttonColorGroup.setColor(QColorGroup::Mid, Qt::red);
		buttonColorGroup.setColor(QColorGroup::Text, Qt::red);
		buttonColorGroup.setColor(QColorGroup::ButtonText, Qt::red);
		buttonColorGroup.setColor(QColorGroup::Shadow, Qt::red);
		buttonColorGroup.setColor(QColorGroup::Highlight, Qt::red);
		buttonColorGroup.setColor(QColorGroup::Foreground, Qt::red);
		buttonColorGroup.setColor(QColorGroup::Base, Qt::blue);
*/
#ifdef _DEBUG_CB
		std::cerr << "On/Down\n";
#endif
	}
	else if(!isOn() && isDown()){
		//#! Qt-3.2.x bug?
		//setBackgroundMode(PaletteMid);
		buttonColorGroup.setColor(QColorGroup::Button, origColorGroup.mid());
		buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.foreground());
		buttonColorGroup.setColor(QColorGroup::Midlight, origColorGroup.dark());
		buttonColorGroup.setColor(QColorGroup::Dark, origColorGroup.midlight());
		//!! The mistery of the black line !!
#ifdef _DEBUG_CB
		std::cerr << "Off/Down\n";
#endif
	}
	else{
		//#! Qt-3.2.x bug?
		//setBackgroundMode(PaletteMid);
		buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.dark());
		buttonColorGroup.setColor(QColorGroup::Button, origColorGroup.mid());
		//!!d
		//painter->fillRect(rect(), Qt::red);
#ifdef _DEBUG_CB
		std::cerr << "Off\n";
#endif
		//return;
	}

	const QRect newDrawRect = rect();
	//QApplication::style().drawToolButton(painter, newDrawRect.x(), newDrawRect.y(), newDrawRect.width(), newDrawRect.height(), buttonColorGroup);
	QApplication::style().drawPrimitive(QStyle::PE_ButtonBevel, painter, newDrawRect, buttonColorGroup);

	drawButtonLabel(painter);
}

void CrButton::drawButtonLabel(QPainter* painter)
{
	//drawIcon
	if(!m_iconName.isNull()){
		KIconLoader* const iconLoader = KGlobal::iconLoader();
		const int state = (isOn()) ? KIcon::DefaultState : KIcon::DisabledState;
		const QPixmap icon = iconLoader->loadIcon(m_iconName, KIcon::Small, 0, state);
		const QPoint point(ICON_HOFFSET, (height() - icon.height())/2);
		painter->drawPixmap(point, icon);
	}
	if(!text().isNull()){
		const int hOffset = ICON_HOFFSET + m_iconWidth + TEXT_HOFFSET ;
		const QRect textRect(hOffset, 0, width() - hOffset - ICON_HOFFSET, height());
		painter->setPen(colorGroup().buttonText());
		painter->drawText(textRect, Qt::AlignLeft|Qt::AlignVCenter, text());
	}
}

void CrButton::setIcon(const QString& iconName)
{
	m_iconName = iconName;
	if(!m_iconName.isNull()){
		KIconLoader* const iconLoader = KGlobal::iconLoader();
		const QPixmap icon = iconLoader->loadIcon(m_iconName, KIcon::Small, 0, KIcon::DefaultState);
		m_heightSizeHint = icon.height()+(ICON_VOFFSET*2);
		m_iconWidth = icon.width();
	}
}

void CrButton::setIntermediateState(const bool on)
{
	m_intermediateState = on;
	repaint(false);
}
