/***************************************************************************
						crechotapsview.cpp  -  description
							 -------------------
		begin                : Sat Jan 13 2001
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
#include <qpainter.h>
#include <qpalette.h>
#include <qpen.h>
#include <qcolor.h>
#include <qwmatrix.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qapplication.h>
#include "crechotapsview.h"
#define WIDGET_HEIGHT (180)

CrEchoTapsView::CrEchoTapsView(const EchoParameters* epar, const int selectedEchoTap,
							   QWidget *parent, const char *name )
 : QFrame(parent,name), m_epar(epar), m_selectedEchoTap(selectedEchoTap)
{
	(void)std::memcpy(&m_parallelEcho,
					  reinterpret_cast<void*>(
					  		const_cast<EchoParameters::ParallelEcho(*)[10]>(&m_epar->parallelEcho)),
					  sizeof(EchoParameters::ParallelEcho) * 10);

	m_finalFeedback = m_epar->finalFeedback;
	m_finalDelay = m_epar->finalDelay;
	setBackgroundColor(Qt::black);
	//setBackgroundMode(QWidget::FixedColor);
	setFrameStyle(QFrame::Box | QFrame::Raised);
	setLineWidth(1);
	setMidLineWidth(1);
	setMargin(0);
}

CrEchoTapsView::~CrEchoTapsView()
{
#ifdef _DEBUG
	std::cerr << "CrEchoTapsView deleted...\n";
#endif
}

void CrEchoTapsView::drawContents(QPainter* painter)
{
	//draw grid
	painter->save();
	transformPainter(painter);
	drawGrid(painter);
	redrawAllTaps(painter);
	painter->restore();
}

void CrEchoTapsView::transformPainter(QPainter* painter)
{
	painter->setWindow(-20, -10, 1040, 120);
	const QWMatrix matrix(1, 0, 0, -1, 0, 100);
	painter->setWorldMatrix(matrix);
}

void CrEchoTapsView::drawGrid(QPainter* painter)
{
	painter->setPen(QPen(QColor(100, 100, 100), 0, Qt::SolidLine));
	for (int count = 10; count <= 100; count += 10)
	{
		painter->drawLine(0, count, 1000, count);
	}
	for (int count = 0; count <= 1000; count += 100)
	{
		painter->drawLine(count, 0, count, 100);
	}
	painter->setPen(Qt::gray);
	painter->drawLine(0, 0, 1000, 0);
	//draw circle
	const QPoint circlePos = painter->xForm(QPoint(0, 0));
	painter->save();
	painter->resetXForm();
	painter->setBrush(Qt::yellow);
	painter->drawRect(circlePos.x() - 3, circlePos.y() - 3, 6, 6);
	painter->restore();
}

void CrEchoTapsView::drawEchoTap(const float delay, const float decay, const TapType type,
								 QPainter* painter) const
{
	QPen pen;
	switch(type)
	{
	case tap_normal:
		pen.setColor(QColor(210,210,210));
		break;
	case tap_selected:
		pen.setColor(Qt::yellow);
		break;
	case tap_deleted:
		pen.setColor(Qt::black);
		break;
	case tap_final:
		pen.setColor(Qt::blue);
		pen.setWidth(2);
		break;
	case tap_finalDeleted:
		pen.setColor(Qt::black);
		pen.setWidth(2);
		break;
	default:
			assert(0);
	}
	
	painter->setPen(pen);
	const int delay_i = roundToInt(delay);
	const int decay_i = roundToInt(decay);
	if (type != tap_final)
	{
		painter->drawLine(0, 0, 0, decay_i);
	}
	painter->drawLine(0, decay_i, delay_i, decay_i);
	painter->drawLine(delay_i, decay_i, delay_i, 0);
	//draw text
	if (type == tap_selected || type == tap_deleted)
	{
		const QPoint delayPos = painter->xForm(QPoint(delay_i, 0));
		const QPoint decayPos = painter->xForm(QPoint(delay_i, decay_i));
		painter->save();
		painter->resetXForm();
		QFont sfont(font());
		//!! not good for all displays !!
		//sfont.setPixelSize(5);
		sfont.setPointSize(8);
		painter->setFont(sfont);
		painter->setClipRect(contentsRect());
        if (type == tap_deleted)
        {
            const QFontMetrics fm(sfont);

            const int msWidth = fm.width(QString("%1 ms").arg(delay, 0, 'f', 2));
        	painter->fillRect(delayPos.x() - 17, delayPos.y() - fm.height() + 9,
         					  msWidth + 10, fm.height() + 4, Qt::black);

            const int percWidth = fm.width(QString("%1 %").arg(decay, 0, 'f', 2));
            painter->fillRect(decayPos.x() - 17, decayPos.y() - fm.height() - 3,
            				  percWidth + 10, fm.height() + 4, Qt::black);
		}
		else
		{
    		painter->drawText(delayPos.x() - 12, delayPos.y() + 9,
    						  QString("%1 ms").arg(delay, 0, 'f', 2));
    		painter->drawText(decayPos.x() - 12, decayPos.y() - 3,
    						  QString("%1 %").arg(decay, 0, 'f', 2));
        }
		painter->restore();
	}
	//draw arrow
	const QPoint arrowPos = painter->xForm(QPoint(delay_i, 0));
	painter->save();
	painter->resetXForm();
	if (type == tap_final || type == tap_finalDeleted)
	{
		pen.setWidth(0);
		painter->setPen(pen);
	}
	painter->moveTo(arrowPos);
	painter->lineTo(arrowPos.x() - 2, arrowPos.y() - 2);
	painter->lineTo(arrowPos.x() + 2, arrowPos.y() - 2);
	painter->lineTo(arrowPos);
	painter->restore();
}

QSize CrEchoTapsView::sizeHint() const
{
	return QSize(300, WIDGET_HEIGHT);
}

void CrEchoTapsView::redrawAllTaps(QPainter* painter)
{
	m_finalFeedback = m_epar->finalFeedback;
	m_finalDelay = m_epar->finalDelay;
	if (m_finalFeedback)
	{
		drawEchoTap(m_finalDelay, m_finalFeedback, tap_final, painter);
	}
	(void)std::memcpy(&m_parallelEcho,
					  reinterpret_cast<void*>(
					  		const_cast<EchoParameters::ParallelEcho(*)[10]>(&m_epar->parallelEcho)),
					  sizeof(EchoParameters::ParallelEcho) * 10);
	bool drawSelected = false;
	for (int count=0; count < m_epar->parallelEchoCount; count++)
	{
		if (count == m_selectedEchoTap)
		{
			drawSelected = true;
			continue;
		}
		drawEchoTap(m_parallelEcho[count].delay, m_parallelEcho[count].decay, tap_normal, painter);
	}
	if (drawSelected)
	{
		drawEchoTap(m_parallelEcho[m_selectedEchoTap].delay,
					m_parallelEcho[m_selectedEchoTap].decay, tap_selected, painter);
	}
}

void CrEchoTapsView::slotChangeSelectedTap(int echoTapNum)
{
	QPainter painter(this);
	transformPainter(&painter);
	if (m_selectedEchoTap != -1)
	{
		drawEchoTap(m_parallelEcho[m_selectedEchoTap].delay,
					m_parallelEcho[m_selectedEchoTap].decay, tap_deleted, &painter);
		drawGrid(&painter);
	}
	m_selectedEchoTap = echoTapNum;
	redrawAllTaps(&painter);
#if 0
	std::cerr << "CrEchoTapsView::slotChangeSelectedTap | tap: " << echoTapNum << "\n";
#endif
}

void CrEchoTapsView::slotUpdateSelectedTap()
{
	assert(m_selectedEchoTap! = -1);
	QPainter painter(this);
	transformPainter(&painter);
	drawEchoTap(m_parallelEcho[m_selectedEchoTap].delay,
				m_parallelEcho[m_selectedEchoTap].decay, tap_deleted, &painter);
	drawGrid(&painter);
	redrawAllTaps(&painter);
}

void CrEchoTapsView::slotUpdateFinalTap()
{
	QPainter painter(this);
	transformPainter(&painter);
	if (m_finalFeedback)
	{
		drawEchoTap(m_finalDelay, m_finalFeedback, tap_finalDeleted, &painter);
	}
	drawGrid(&painter);
	redrawAllTaps(&painter);
}
