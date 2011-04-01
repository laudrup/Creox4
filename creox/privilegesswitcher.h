/***************************************************************************
					privilegesswitcher.h  -  description
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

#ifndef PRIVILEGESSWITCHER_H
#define PRIVILEGESSWITCHER_H

#include <sys/types.h>

/**
 *@author Jozef Kosoru
 */
class PrivilegesSwitcher {
 public:
  PrivilegesSwitcher();
  void releasePrivileges();
  void getPrivileges();
  static PrivilegesSwitcher* getInstance() { return s_switcher; }

  uid_t m_euid;
  uid_t m_ruid;
  static PrivilegesSwitcher* s_switcher;
};

#endif
