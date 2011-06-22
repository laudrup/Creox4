/***************************************************************************
					crvalidator.cpp  -  description
						 -------------------
		begin                : Fri Oct 6 2000
		copyright            : (C) 2000 by Jozef Kosoru
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

#include <cmath>
#include <iomanip>

#include "crvalidator.h"

const int CrValidator::s_validatorResolution = VALIDATOR_RESOLUTION;
const double CrValidator::s_validatorResolution_d = VALIDATOR_RESOLUTION;

CrValidator::CrValidator(const float bottom, const float top, const float slope, QWidget *parent, const char *name)
  : QDoubleValidator(static_cast<double>(bottom), static_cast<double>(top), 3, parent,name),
    m_bottom(bottom), m_top(top)
{
  //assert(top>bottom && slope>0.0f && slope<1.0f);

  if(slope == 0.5f){
    m_linear = true;
    m_m1 = s_validatorResolution_d / static_cast<double>(m_top - m_bottom);
    m_s1 = (static_cast<double>(-m_bottom) * s_validatorResolution_d) / static_cast<double>(m_top - m_bottom);

    m_m2 = 1.0 / m_m1;
    m_s2 = static_cast<double>(m_bottom);

    m_smallestVal = std::fabs( ((m_m2 * static_cast<double>(minValue())) + m_s2) -
                               ((m_m2 * (static_cast<double>(minValue())+0.9)) + m_s2) );
  } else{
    m_linear = false;
    m_exponent = std::log(static_cast<double>(slope)) / std::log(0.5);
    m_oneDivExponent = 1.0/m_exponent;

    m_m1 = 1.0 / static_cast<double>(m_top - m_bottom);
    m_s1 = static_cast<double>(-m_bottom) / static_cast<double>(m_top - m_bottom);

    m_m2 = static_cast<double>(m_top - m_bottom);
    m_s2 = static_cast<double>(m_bottom);

    if(slope < 0.5f){
      m_smallestVal = 0.0; //std::fabs( ((m_m2 * static_cast<double>(minValue())) + m_s2) -
      //((m_m2 * (static_cast<double>(minValue())+0.9)) + m_s2) );
    }
    else{
      m_smallestVal = 0.0; //std::fabs( ((m_m2 * static_cast<double>(minValue())) + m_s2) -
    }
  }

  m_prevIntValue = maxValue()+1;
  m_retFloatValue = 0.0f;
}

CrValidator::~CrValidator()
{
}

int CrValidator::getIntValue(const float variable) const
{
  if(m_linear){
    return roundToInt( (m_m1 * static_cast<double>(variable)) + m_s1 );
  }
  else{
    return roundToInt( std::pow((m_m1 * static_cast<double>(variable)) + m_s1, m_oneDivExponent) * s_validatorResolution_d );
  }

}

float CrValidator::getFloatValue(const int value) const
{
  if(value != m_prevIntValue){
    m_prevIntValue = value;
    if(m_linear){
      m_retFloatValue = (m_m2 * static_cast<double>(value)) + m_s2;
      if(std::fabs(m_retFloatValue)<m_smallestVal){
        m_retFloatValue = 0.0f;
      }
    }
    else{
      m_retFloatValue = (m_m2 * std::pow(static_cast<double>(value)/s_validatorResolution_d, m_exponent)) + m_s2;
      if(std::fabs(m_retFloatValue)<m_smallestVal){
        m_retFloatValue = 0.0f;
      }
    }
  }
  return static_cast<float>(m_retFloatValue);
}

int CrValidator::getStep() const
{
  if(m_linear){
    const double resolution = 0.01;
    double div = std::fabs( static_cast<double>(getFloatValue(minValue())) - static_cast<double>(getFloatValue(minValue()+1)) );
    if(!div){
      div = 0.01;
    }
    const int step = roundUpToInt( resolution / div );
    return (step>0) ? step : 1;
  }
  else{
    return s_validatorResolution/1000;
  }
}
