/***************************************************************************
						crbutton.h  -  description
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

#ifndef CRBUTTON_H
#define CRBUTTON_H

#include <qwidget.h>
#include <qbutton.h>
#include <qsizepolicy.h>

class QPainter;
class QSize;
class QString;

/**A special button used inside CrChainView.
	*@author Jozef Kosoru
	*/
class CrButton : public QButton  {
	 Q_OBJECT
public:
	CrButton(QWidget *parent=0, const char *name=0);
	~CrButton();
	void setIcon(const QString& iconName);

	virtual QSize sizeHint() const;
	virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed); }

protected:
	void setIntermediateState(const bool on);
	virtual void drawButton(QPainter* painter);
	virtual void drawButtonLabel(QPainter* painter);

private:
	QString m_iconName;
	int m_heightSizeHint;
	int m_iconWidth;
	bool m_intermediateState;
};

#endif
