include_directories(
  ${CMAKE_SOURCE_DIR}/creox
  ${CMAKE_SOURCE_DIR}/creox/cr_lib
  ${CMAKE_SOURCE_DIR}/creox/preset
  ${CREOX4_BINARY_DIR}/creox/dsp_flow
  ${KDE4_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES}
  )

set(dsp_flow_STAT_SRCS
  threadeffector.cpp
  effectkeeper.cpp)

kde4_add_kcfg_files(dsp_flow_STAT_SRCS ${CMAKE_SOURCE_DIR}/creox/settings.kcfgc)
kde4_add_library(dsp_flow STATIC ${dsp_flow_STAT_SRCS})
