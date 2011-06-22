/***************************************************************************
					crchainbutton.cpp  -  description
						 -------------------
		begin                : Tue Apr 17 2001
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

#include <QString>
#include <QEvent>
#include <QPoint>
#include <QCursor>
#include <QMouseEvent>

#include <KIcon>

#include "creffectgui.h"
#include "crchainview.h"
#include "soundprocessor.h"
#include "crchainpopupmenu.h"
#include "crchainbutton.h"

#include <QDebug>

CrChainButton::CrChainButton(CrEffectGui* effect, QWidget *parent, const char *name ) :
	CrButton(parent,name), m_effect(effect)
{
  m_parentChainView = dynamic_cast<CrChainView*>(parent);
  setText(m_effect->effectName());
  setIcon(QString::fromLatin1(m_effect->getProcessor()->getName()));
  synchronize();
  connect(this, SIGNAL(toggled(bool)), SLOT(toggleEffect(bool)));
}

CrChainButton::~CrChainButton()
{
}

void CrChainButton::toggleEffect(bool on)
{
  if(on) {
    m_effect->getProcessor()->setMode(SoundProcessor::enabled);
  } else {
    m_effect->getProcessor()->setMode(SoundProcessor::disabled);
  }
}

void CrChainButton::synchronize()
{
  blockSignals(true);
  setOn( (m_effect->getProcessor()->mode() == SoundProcessor::enabled) ? true : false );
  blockSignals(false);
}

void CrChainButton::mouseMoveEvent(QMouseEvent* mouseEvent)
{
  const int yPos =  mouseEvent->y();
  if(yPos<0 && mouseEvent->state()==Qt::MidButton) {
    m_parentChainView->moveUp(this);
  } else if(yPos>height() && mouseEvent->state()==Qt::MidButton) {
    m_parentChainView->moveDown(this);
  }
}

void CrChainButton::mousePressEvent(QMouseEvent* mouseEvent)
{
  qDebug() << "mousePressEvent";
  if(mouseEvent->button() == Qt::LeftButton){
    CrButton::mousePressEvent(mouseEvent);
  } else if(mouseEvent->button() == Qt::RightButton && !(mouseEvent->state() & (Qt::MidButton | Qt::LeftButton) )){
    CrChainPopupMenu popupMenu(this);
    popupMenu.exec(QCursor::pos() - QPoint(3,3));
  } else if(mouseEvent->button() == Qt::MidButton){
    setCursor(Qt::SizeVerCursor);
    setIntermediateState(true);
  }
}

void CrChainButton::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
  if(mouseEvent->button() == Qt::MidButton){
    unsetCursor();
    setIntermediateState(false);
  } else {
    CrButton::mouseReleaseEvent(mouseEvent);
  }
}

