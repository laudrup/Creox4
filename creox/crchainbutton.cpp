/***************************************************************************
					crchainbutton.cpp  -  description
						 -------------------
		begin                : Tue Apr 17 2001
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
#include <iostream>
#include <qstring.h>
#include <qevent.h>
#include <qpoint.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QMouseEvent>
#include "creffectgui.h"
#include "crchainview.h"
#include "soundprocessor.h"
#include "crchainpopupmenu.h"
#include "crchainbutton.h"

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
#ifdef _DEBUG
	std::cerr << "CrChainButton deleted...\n";
#endif
}

void CrChainButton::toggleEffect(bool on)
{
	if(on){
		m_effect->getProcessor()->setMode(SoundProcessor::enabled);
	}
	else{
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
	//std::cerr << "PosX:" << mouseEvent->x() << " PosY:" << mouseEvent->y() << "\n";
	const int yPos =  mouseEvent->y();
	if(yPos<0 && mouseEvent->state()==Qt::MidButton){
		m_parentChainView->moveUp(this);
	}
	else if(yPos>height() && mouseEvent->state()==Qt::MidButton){
		m_parentChainView->moveDown(this);
	}
}

void CrChainButton::mousePressEvent(QMouseEvent* mouseEvent)
{
	if(mouseEvent->button() == Qt::LeftButton){
		CrButton::mousePressEvent(mouseEvent);
	}
	else if(mouseEvent->button() == Qt::RightButton && !(mouseEvent->state() & (Qt::MidButton | Qt::LeftButton) )){
		CrChainPopupMenu popupMenu(this);
		popupMenu.exec(QCursor::pos() - QPoint(3,3));
	}
	else if(mouseEvent->button() == Qt::MidButton){
		setCursor(Qt::sizeVerCursor);
		setIntermediateState(true);
	}
}

void CrChainButton::mouseReleaseEvent(QMouseEvent* mouseEvent)
{
	if(mouseEvent->button() == Qt::MidButton){
		unsetCursor();
		setIntermediateState(false);
	}
	else{
		CrButton::mouseReleaseEvent(mouseEvent);
	}
}

