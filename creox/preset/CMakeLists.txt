include_directories(
  ${CMAKE_SOURCE_DIR}/creox
  ${CMAKE_SOURCE_DIR}/creox/cr_lib
  ${CMAKE_SOURCE_DIR}/creox/dsp_flow
  ${KDE4_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES})

set(preset_STAT_SRCS
    effectpreset.cpp
    crsavenewpresetdialog.cpp
    crpresetviewitem.cpp
    crpresetview.cpp
    crpresetpopupmenu.cpp
    crnewpresetfolderdialogimpl.cpp
    crnewpresetfolderdialog.cpp)

kde4_add_library(preset STATIC ${preset_STAT_SRCS})
