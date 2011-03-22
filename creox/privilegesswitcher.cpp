/***************************************************************************
				privilegesswitcher.cpp  -  description
						 -------------------
		begin                : Tue Apr 10 2001
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
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <klocale.h>
#include "privilegesswitcher.h"

PrivilegesSwitcher* PrivilegesSwitcher::s_switcher = 0;

PrivilegesSwitcher::PrivilegesSwitcher()
{
	// Remember the real and effective user IDs.
	m_ruid = getuid();
	m_euid = geteuid();
	s_switcher = this;
}

void PrivilegesSwitcher::releasePrivileges() throw(Cr::CrException_runtimeError )
{
	int status;
#ifdef _POSIX_SAVED_IDS
	status = seteuid(m_ruid);
#else
	status = setreuid(m_euid, m_ruid);
#endif
	if(status < 0){
          //throw(Cr::CrException_runtimeError(i18n("Couldn't set uid!")));
	}
}

void PrivilegesSwitcher::getPrivileges() throw(Cr::CrException_runtimeError)
{
	int status;
#ifdef _POSIX_SAVED_IDS
	status = seteuid(m_euid);
#else
	status = setreuid(m_ruid, m_euid);
#endif
	if(status < 0){
          //throw(Cr::CrException_runtimeError(i18n("Couldn't set uid!")));
	}
}
