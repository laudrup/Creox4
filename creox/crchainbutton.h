/***************************************************************************
						crchainbutton.h  -  description
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

#ifndef CRCHAINBUTTON_H
#define CRCHAINBUTTON_H

#include <qwidget.h>
#include "crbutton.h"

class CrEffectGui;
class CrChainView;

/**
	*@author Jozef Kosoru
	*/
class CrChainButton : public CrButton  {
	 Q_OBJECT
public:
	CrChainButton(CrEffectGui* effect, QWidget *parent=0, const char *name=0);
	~CrChainButton();
	void synchronize();
	CrEffectGui* getEffect() const { return m_effect; }
	CrChainView* getParentChainView() const { return m_parentChainView; }

protected:
	virtual void mouseMoveEvent(QMouseEvent* mouseEvent);
	virtual void mousePressEvent(QMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent(QMouseEvent* mouseEvent);
private:
	CrEffectGui* m_effect;
	CrChainView* m_parentChainView;
private slots:
	void toggleEffect(bool on);
};

#endif
