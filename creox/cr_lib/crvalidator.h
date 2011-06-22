/***************************************************************************
					crvalidator.h  -  description
						 -------------------
		begin                : Fri Oct 6 2000
		copyright            : (C) 2000 by Jozef Kosoru
                                     : (C) 2011 by Kasper Laudrup
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CRVALIDATOR_H
#define CRVALIDATOR_H

#include <QWidget>
#include <QValidator>

#define VALIDATOR_RESOLUTION 100000

/**
 *@author Jozef Kosoru
 */
class CrValidator : public QDoubleValidator  {
  Q_OBJECT
 public:
  CrValidator(const float bottom, const float top, const float slope=0.5f, QWidget *parent=0, const char *name=0);
  ~CrValidator();

  int getIntValue(const float variable) const;
  float getFloatValue(const int value) const;
  int getStep() const;

  static int minValue() { return 0; }
  static int maxValue() { return s_validatorResolution; }

 private:
  bool m_linear;

  float m_bottom;
  float m_top;

  mutable int m_prevIntValue;
  mutable double m_retFloatValue;

  double m_exponent;
  double m_oneDivExponent;

  //y = m*x + s
  double m_m1;  //multiplier
  double m_m2;
  double m_s1;  //shift
  double m_s2;

  double m_smallestVal;

  static const int s_validatorResolution; // = VALIDATOR_RESOLUTION
  static const double s_validatorResolution_d; // = VALIDATOR_RESOLUTION

 private:
  static inline int roundUpToInt(double f)
  {
    int sign=1;
    if(f<0.0){
      f=-f;
      sign=-1;
    }
    const int fi = static_cast<int>(f);
    if((f-static_cast<double>(fi)) > 0.0){
      return (fi+1)*sign;
    }
    else{
      return fi*sign;
    }
  }

  static inline int roundToInt(double f)
  {
    int sign=1;
    if(f<0.0){
      f=-f;
      sign=-1;
    }
    const int fi = static_cast<int>(f);
    if((f-static_cast<double>(fi))>=0.5){
      return (fi+1)*sign;
    }
    else{
      return fi*sign;
    }
  }
};

#endif
