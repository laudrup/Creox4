project(creox4)

# XXX: Not sure if this is actually the minimum required version
cmake_minimum_required(VERSION 2.8)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 3)
set(VERSION_PATCH 0)

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(JACK REQUIRED)
find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${JACK_DEFINITIONS})
add_definitions(-DCREOX_VERSION="${VERSION}")
include(${QT_USE_FILE})

# The kde headers sets lots of unused variables, so disable that warning
add_definitions(-Wno-unused-but-set-variable)

# For now, we still need Qt3 supprt, should be disabled at some point
add_definitions(-DQT3_SUPPORT)

# The source files
add_subdirectory(creox)

# The pictures
add_subdirectory(pics)

# The icons
add_subdirectory(icons)

# Rules for building packages
set(CPACK_GENERATOR "DEB")

set(CPACK_PACKAGE_DESCRIPTION "A real time sound effector for KDE 4")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}\rCreox is a real-time sound processor. You can plug your electric\rguitar or any other musical instrument directly to the PC's sound card\rand start experimenting with various sound effects. Creox has a nice\ruser-friendly GUI, a preset support, a low-latency DSP engine and each\reffect parameter can be altered on the fly.")
set(CPACK_PACKAGE_CONTACT "Kasper Laudrup <laudrup@stacktrace.dk>")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${VERSION})
set(VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

# Debian specific package rules.
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

include(CPack)
