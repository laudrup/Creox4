/***************************************************************************
						crbutton.cpp  -  description
						 -------------------
		begin                : Sun Apr 15 2001
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

#include <QBrush>
#include <QSize>
#include <QPainter>
#include <QStyle>
#include <QApplication>
#include <QPalette>
#include <QFontMetrics>
#include <QString>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QStyleOptionButton>

#include <KIconLoader>
#include <KGlobal>
#include <KIcon>

#include "crbutton.h"

#include <QDebug>

#define ICON_VOFFSET 5
#define ICON_HOFFSET 10
#define TEXT_HOFFSET 5

CrButton::CrButton(QWidget *parent, const char *name ) :
  QAbstractButton(parent ,name, Qt::WResizeNoErase|Qt::WNoAutoErase), m_heightSizeHint(25), m_iconWidth(0),
  m_intermediateState(false)
{
  // XXX!
  //setToggleType(QButton::Toggle);
  setCheckable(true);
  setBackgroundMode(Qt::NoBackground);
  setFocusPolicy(Qt::NoFocus);
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

void CrButton::paintEvent(QPaintEvent*)
{
  QPainter painter(this);
  drawButton(&painter);
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
  }
  else if(!isOn() && isDown()){
    //#! Qt-3.2.x bug?
    //setBackgroundMode(PaletteMid);
    buttonColorGroup.setColor(QColorGroup::Button, origColorGroup.mid());
    buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.foreground());
    buttonColorGroup.setColor(QColorGroup::Midlight, origColorGroup.dark());
    buttonColorGroup.setColor(QColorGroup::Dark, origColorGroup.midlight());
    //!! The mistery of the black line !!
  }
  else{
    //#! Qt-3.2.x bug?
    //setBackgroundMode(PaletteMid);
    buttonColorGroup.setColor(QColorGroup::Light, origColorGroup.dark());
    buttonColorGroup.setColor(QColorGroup::Button, origColorGroup.mid());
    //!!d
    //painter->fillRect(rect(), Qt::red);
    //return;
  }

  QStyleOptionButton option;
  option.init(this);
  option.state = isOn() ? QStyle::State_Sunken : QStyle::State_Raised;
  //if (isOn())
  //  option.features |= QStyleOptionButton::DefaultButton;
  option.text = text();
  KIconLoader* const iconLoader = KIconLoader::global();
  option.icon = iconLoader->loadIcon(m_iconName, KIconLoader::NoGroup, KIconLoader::SizeSmall,
                                     KIconLoader::DefaultState);

  //QPainter painter(this);
  style()->drawControl(QStyle::CE_PushButton, &option, painter, this);

  //const QRect newDrawRect = rect();
  //QApplication::style()->drawToolButton(painter, newDrawRect.x(), newDrawRect.y(), newDrawRect.width(), newDrawRect.height(), buttonColorGroup);

  //QApplication::style()->drawPrimitive(QStyle::PE_PanelButtonBevel, painter);//newDrawRect);//, buttonColorGroup);

  //drawButtonLabel(painter);
}

void CrButton::drawButtonLabel(QPainter* painter)
{
  //drawIcon
  if(!m_iconName.isNull()){
    KIconLoader* const iconLoader = KIconLoader::global();
    const int state = (isOn()) ? KIconLoader::DefaultState : KIconLoader::DisabledState;
    const QPixmap icon = iconLoader->loadIcon(m_iconName, KIconLoader::NoGroup,
                                              KIconLoader::SizeSmall, state);
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
    KIconLoader* const iconLoader = KIconLoader::global();
    const QPixmap icon = iconLoader->loadIcon(m_iconName, KIconLoader::NoGroup, KIconLoader::SizeSmall,
                                              KIconLoader::DefaultState);
    m_heightSizeHint = icon.height()+(ICON_VOFFSET*2);
    m_iconWidth = icon.width();
  }
}

void CrButton::setIntermediateState(const bool on)
{
  m_intermediateState = on;
  repaint(false);
}
