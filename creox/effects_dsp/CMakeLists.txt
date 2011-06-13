include_directories(
  ${CMAKE_SOURCE_DIR}/creox
  ${CMAKE_SOURCE_DIR}/creox/cr_lib
  ${KDE4_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES})

set(effects_dsp_STAT_SRCS
  tremoloprocessor.cpp
  phaserprocessor.cpp
  flangerprocessor.cpp
  echoprocessor.cpp
  distortionprocessor.cpp)

kde4_add_library(effects_dsp STATIC ${effects_dsp_STAT_SRCS})
