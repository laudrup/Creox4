/***************************************************************************
						crechotapsview.h  -  description
							 -------------------
		begin                : Sat Jan 13 2001
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

#ifndef CRECHOTAPSVIEW_H
#define CRECHOTAPSVIEW_H

#include <qwidget.h>
#include <q3frame.h>
#include <QSizePolicy>
#include <QSize>
#include "echoprocessor.h"

class QPainter;

/**
 *	@author Jozef Kosoru
 */
class CrEchoTapsView : public Q3Frame
{
  Q_OBJECT

    public:
  CrEchoTapsView(const EchoParameters* epar, const int selectedEchoTap = -1, QWidget *parent = 0,
                 const char *name = 0);

  ~CrEchoTapsView();

  virtual QSizePolicy sizePolicy() const
  {
    return QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  }

  virtual QSize sizeHint() const;

  //void setSelectedEchoTap(const int echoTapNum, const bool repaint=true);

  public slots:
  void slotChangeSelectedTap(int echoTapNum);
  void slotUpdateSelectedTap();
  void slotUpdateFinalTap();

 protected:
  virtual void drawContents(QPainter* painter);

 private:
  enum TapType { tap_normal, tap_selected, tap_deleted, tap_final, tap_finalDeleted };
  const EchoParameters* m_epar;
  EchoParameters::ParallelEcho m_parallelEcho[10];
  float m_finalFeedback;
  float m_finalDelay;
  int m_selectedEchoTap;

  void redrawAllTaps(QPainter* painter);

  static void transformPainter(QPainter* painter);
  static void drawGrid(QPainter* painter);
  void drawEchoTap(const float delay, const float decay, const TapType type,
                   QPainter* painter) const;

  static inline int roundToInt(float f)
  {
    int sign=1;
    if(f<0.0f){
      f=-f;
      sign=-1;
    }
    const int fi = static_cast<int>(f);
    if((f-static_cast<float>(fi))>=0.5f){
      return (fi+1)*sign;
    }
    else{
      return fi*sign;
    }
  }
};

#endif
