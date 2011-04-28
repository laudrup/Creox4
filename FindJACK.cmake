# - Try to find jack-2.6
# Once done this will define
#
#  JACK_FOUND - system has jack
#  JACK_INCLUDE_DIR - the jack include directory
#  JACK_LIBRARIES - Link these to use jack
#  JACK_DEFINITIONS - Compiler switches required for using jack
#
#  Copyright (c) 2008 Andreas Schneider <mail@cynapses.org>
#  Modified for other libraries by Lasse Kärkkäinen <tronic>
#
#  Redistribution and use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

if (JACK_LIBRARIES AND JACK_INCLUDE_DIR)
  # in cache already
  set(JACK_FOUND TRUE)
else (JACK_LIBRARIES AND JACK_INCLUDE_DIR)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_JACK jack)
  endif (PKG_CONFIG_FOUND)

  find_path(JACK_INCLUDE_DIR
    NAMES
      jack/jack.h
    PATHS
      ${_JACK_INCLUDEDIR}
      /usr/include
      /usr/local/include
      /opt/local/include
  )
  
  find_library(JACK_LIBRARY
    NAMES
      jack
    PATHS
      ${_JACK_LIBDIR}
      /usr/lib
      /usr/local/lib
      /opt/local/lib
  )

  if (JACK_LIBRARY)
    set(JACK_FOUND TRUE)
  endif (JACK_LIBRARY)

  set(JACK_INCLUDE_DIR
    ${JACK_INCLUDE_DIR}
  )

  if (JACK_FOUND)
    set(JACK_LIBRARIES
      ${JACK_LIBRARIES}
      ${JACK_LIBRARY}
    )
  endif (JACK_FOUND)

  if (JACK_INCLUDE_DIR AND JACK_LIBRARIES)
     set(JACK_FOUND TRUE)
  endif (JACK_INCLUDE_DIR AND JACK_LIBRARIES)

  if (JACK_FOUND)
    if (NOT JACK_FIND_QUIETLY)
      message(STATUS "Found jack: ${JACK_LIBRARY} ${JACK_INCLUDE_DIR}")
    endif (NOT JACK_FIND_QUIETLY)
  else (JACK_FOUND)
    if (JACK_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find JACK")
    endif (JACK_FIND_REQUIRED)
  endif (JACK_FOUND)

  # show the JACK_INCLUDE_DIR and JACK_LIBRARIES variables only in the advanced view
  mark_as_advanced(JACK_INCLUDE_DIR JACK_LIBRARIES)

endif (JACK_LIBRARIES AND JACK_INCLUDE_DIR)

 
