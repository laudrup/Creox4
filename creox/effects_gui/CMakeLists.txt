include_directories(
  ${CMAKE_SOURCE_DIR}/creox
  ${CMAKE_SOURCE_DIR}/creox/cr_lib
  ${CMAKE_SOURCE_DIR}/creox/effects_dsp
  ${KDE4_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES})

set(effects_gui_STAT_SRCS
    crtremolo.cpp
    crphaser.cpp
    crflanger.cpp
    crechotapsview.cpp
    crecho.cpp
    crdistortion.cpp)

kde4_add_library(effects_gui STATIC ${effects_gui_STAT_SRCS})
