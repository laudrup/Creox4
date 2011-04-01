/***************************************************************************
						crchainview.h  -  description
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

#ifndef CRCHAINVIEW_H
#define CRCHAINVIEW_H

#include <qwidget.h>
#include <q3frame.h>
#include <q3ptrlist.h>
#include <QSizePolicy>
//Added by qt3to4:
#include <QResizeEvent>
#include "threadeffector.h"
#include "crchainbutton.h"

class EffectKeeper;
class QResizeEvent;
class QSize;

/**
	*@author Jozef Kosoru
	*/
class CrChainView : public Q3Frame  {
	 Q_OBJECT
public:
	CrChainView(EffectKeeper* effectKeeper, QWidget *parent=0, const char *name=0);
	~CrChainView();
	virtual QSizePolicy sizePolicy() const { return QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding); }
	virtual QSize sizeHint() const { return m_sizeHint; }
	/** must be called after EffectKeeper activation! */
	void activate();
	void synchronize();

	void moveUp(const CrChainButton* button);
	void moveDown(const CrChainButton* button);

	bool isFirstButton(const CrChainButton* button) const { return m_chainButtonArray[0] == button; }
	bool isLastButton(const CrChainButton* button) const { return m_chainButtonArray[m_threadEffector->getProcessorChainSize()-1] == button; }

protected:
	virtual void resizeEvent(QResizeEvent*);
private:
	void reorderChainButtons();
	void syncChainButtons();
	EffectKeeper* m_effectKeeper;
	ThreadEffector* m_threadEffector;
	Q3PtrList<CrChainButton> m_chainButtonList;
	CrChainButton** m_chainButtonArray;
	QSize m_sizeHint;
};

#endif
