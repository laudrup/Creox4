project(CREOX4)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(JACK REQUIRED)
find_package(KDE4 REQUIRED)
find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${JACK_DEFINITIONS})
include(${QT_USE_FILE})

# For now, we still need Qt3 supprt, should be disabled at some point
add_definitions(-DQT3_SUPPORT)

# The source files
add_subdirectory(creox)

# The pictures
add_subdirectory(pics)

# The icons
add_subdirectory(icons)
