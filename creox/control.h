/***************************************************************************
													control.h  -  description
														 -------------------
		begin                : Mon Feb 7 2000
		copyright            : (C) 2000 by Jozef Kosoru
		email                : jozef.kosoru@pobox.sk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License version 2        *
 *   as published by the Free Software Foundation.                         *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H

#ifndef _DEBUG
	#undef DEBUG
	#undef QT_DEBUG
	#define NDEBUG
	#define NO_DEBUG
	#define QT_NO_DEBUG
	#define QT_NO_CHECK
#endif // _DEBUG

#ifdef _DEBUG
	#undef NDEBUG
	#undef QT_NO_DEBUG
	#undef QT_NO_CHECK
	#define DEBUG
	#define QT_DEBUG
	#define QT_CHECK_STATE
	#define QT_CHECK_RANGE
	#define QT_CHECK_MATH
	#define QT_CHECK_NULL
#endif // _DEBUG

#endif // CONTROL_H

