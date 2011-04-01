/***************************************************************************
						crchainview.cpp  -  description
							 -------------------
		begin                : Sat Apr 14 2001
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
#include <QEvent>
#include <QSize>
//Added by qt3to4:
#include <QResizeEvent>
#include <Q3Frame>
#include "soundprocessor.h"
#include "effectkeeper.h"
#include "crchainview.h"
#include "crchainbutton.h"

#include <QDebug>

#include <QVBoxLayout>

CrChainView::CrChainView(EffectKeeper* effectKeeper, QWidget *parent, const char *name ) :
	Q3Frame(parent,name), m_effectKeeper(effectKeeper), m_threadEffector(0),
	m_chainButtonArray(0), m_sizeHint(100,25)

{
	setBackgroundMode(Qt::PaletteMid);
	setFrameStyle(Q3Frame::Box | Q3Frame::Raised);
	setLineWidth(1);
	setMidLineWidth(1);
	setMargin(0);
	m_chainButtonList.setAutoDelete(false);
}

CrChainView::~CrChainView()
{
	delete[] m_chainButtonArray;
#ifdef _DEBUG
	std::cerr << "CrChainView deleted...\n";
#endif
}

/** must be called after EffectKeeper activation! */
void CrChainView::activate()
{
	assert(m_threadEffector == 0);
	m_threadEffector = m_effectKeeper->threadEffector();
	assert(m_threadEffector->getProcessorChainSize() == static_cast<int>(m_effectKeeper->effectList().count()));

	m_chainButtonArray = new CrChainButton*[m_threadEffector->getProcessorChainSize()];


	//create chainButtons
	int maxWidth = 0, maxHeight = 2*frameWidth();
	for(QListIterator<CrEffectGui*> effectIterator(m_effectKeeper->effectList()) ; effectIterator.hasNext();){
          qDebug() << "Add button";
		CrChainButton* chainButton = new CrChainButton(effectIterator.next(), this);
		m_chainButtonList.append(chainButton);
		//compute sizeHint
		const QSize buttonSize = chainButton->sizeHint();
		if(buttonSize.width() > maxWidth){
			maxWidth = buttonSize.width();
		}
		maxHeight += buttonSize.height();
	}
	maxWidth += 2*frameWidth();
	m_sizeHint.setWidth(maxWidth);
	m_sizeHint.setHeight(maxHeight);
	syncChainButtons();
}

void CrChainView::resizeEvent(QResizeEvent*)
{
	reorderChainButtons();
}

void CrChainView::synchronize()
{
  syncChainButtons();
  reorderChainButtons();
}

/** Set an order and the size of the chain buttons. */
void CrChainView::reorderChainButtons()
{
	const int chainSize = m_threadEffector->getProcessorChainSize();
	const int tmpFrameWidth = frameWidth();

	for(int count=0, yPos=tmpFrameWidth; count<chainSize; count++){
		const int heightHint = m_chainButtonArray[count]->sizeHint().height();
		m_chainButtonArray[count]->setGeometry(tmpFrameWidth,
											   yPos,
											   size().width()-(2*tmpFrameWidth),
											   heightHint);
		yPos += heightHint;
	}
}

/** Synchronize m_chainButtonArray according the m_threadEffector processor order. */
void CrChainView::syncChainButtons()
{
  const int chainSize = m_threadEffector->getProcessorChainSize();
  qDebug() << "syncedChainButtons: " << chainSize;
  for(int count=0; count<chainSize; count++){
    const int effectId = m_threadEffector->getProcessorChain()[count]->getId();

    for(Q3PtrListIterator<CrChainButton> chainButtonIterator(m_chainButtonList) ; chainButtonIterator.current(); ++chainButtonIterator ){
      if(chainButtonIterator.current()->getEffect()->getProcessor()->getId() == effectId){
        qDebug() << "Setting chainButton: " << effectId;
        (m_chainButtonArray[count] = chainButtonIterator.current())->synchronize();
        break;
      }
    }
  }
}

void CrChainView::moveUp(const CrChainButton* button)
{
	const int chainSize = m_threadEffector->getProcessorChainSize();
	for(int count=1; count<chainSize; count++){
		if(button == m_chainButtonArray[count]){
			SoundProcessor* tmpProcessor = (*m_threadEffector)[count-1];
			(*m_threadEffector)[count-1] = (*m_threadEffector)[count];
			(*m_threadEffector)[count] = tmpProcessor;
			m_threadEffector->updateProcessorChain();
			CrChainButton* tmpPointer = m_chainButtonArray[count-1];
			m_chainButtonArray[count-1] = m_chainButtonArray[count];
			m_chainButtonArray[count] = tmpPointer;
			reorderChainButtons();
			break;
		}
	}
}

void CrChainView::moveDown(const CrChainButton* button)
{
	const int chainSize = m_threadEffector->getProcessorChainSize();
	for(int count=0; count<(chainSize-1); count++){
		if(button == m_chainButtonArray[count]){
			SoundProcessor* tmpProcessor = (*m_threadEffector)[count+1];
			(*m_threadEffector)[count+1] = (*m_threadEffector)[count];
			(*m_threadEffector)[count] = tmpProcessor;
			m_threadEffector->updateProcessorChain();
			CrChainButton* tmpPointer = m_chainButtonArray[count+1];
			m_chainButtonArray[count+1] = m_chainButtonArray[count];
			m_chainButtonArray[count] = tmpPointer;
			reorderChainButtons();
			break;
		}
	}
}

