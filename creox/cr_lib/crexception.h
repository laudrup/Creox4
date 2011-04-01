/***************************************************************************
					crexception.h  -  description
						 -------------------
		begin                : Sun Jun 17 2001
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

#ifndef CREXCEPTION_H
#define CREXCEPTION_H

#include <QString>

namespace Cr {

/**
	*@author Jozef Kosoru
	*/
class CrException {
public:
	CrException() { }
	virtual ~CrException() { }
	virtual const QString& what() const = 0;
};

/**
	*@author Jozef Kosoru
	*/
class CrException_logicError : public CrException {
	QString m_what;
public:
	CrException_logicError(const QString& what_arg) : m_what(what_arg) { m_what.prepend(QString::fromLatin1("[CrException_logicError] ")); }
	virtual const QString& what() const { return m_what; }
protected:
	CrException_logicError() : CrException() { }
};

/**
	*@author Jozef Kosoru
	*/
class CrException_runtimeError : public CrException {
	QString m_what;
public:
	CrException_runtimeError(const QString& what_arg) : m_what(what_arg) {m_what.prepend(QString::fromLatin1("[CrException_runtimeError] ")); }
	virtual const QString& what() const { return m_what; }
protected:
	CrException_runtimeError() : CrException() { }
};

}
#endif
