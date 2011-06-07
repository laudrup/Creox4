project(creox)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(JACK REQUIRED)

find_package(KDE4 REQUIRED)

find_package(Qt4 REQUIRED)

find_package(Threads)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS} ${JACK_DEFINITIONS})

# The kde headers sets lots of unused variables, so disable that warning
add_definitions(-Wno-unused-but-set-variable)

# For now, we still need Qt3 supprt, should be disabled at some point
add_definitions(-DQT3_SUPPORT)

set( QT_USE_QT3SUPPORT TRUE )
# -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(${QT_USE_FILE})

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${JACK_INCLUDE_DIR} ${QT_INCLUDES} ${QT_QT3SUPPORT_INCLUDE_DIR})

link_directories(${JACK_LIBRARIES})

add_subdirectory(creox)
