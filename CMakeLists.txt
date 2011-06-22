project(CREOX4)

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

# For now, we still need Qt3 supprt, should be disabled at some point
add_definitions(-DQT3_SUPPORT)

# The source files
add_subdirectory(creox)

# The pictures
add_subdirectory(pics)

# The icons
add_subdirectory(icons)
