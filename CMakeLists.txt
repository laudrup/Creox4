project(creox)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_package(JACK REQUIRED)

find_package(KDE4 REQUIRED)

find_package(Qt4 REQUIRED)

find_package(Threads)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_definitions(-DQT3_SUPPORT)

set( QT_USE_QT3SUPPORT TRUE )
# -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(${QT_USE_FILE})

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} $(JACK_INCLUDE_DIR) ${QT_INCLUDES} ${QT_QT3SUPPORT_INCLUDE_DIR})

add_subdirectory(creox)
