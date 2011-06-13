include_directories(
  ${CMAKE_SOURCE_DIR}/creox
  ${KDE4_INCLUDES}
  ${KDE4_INCLUDE_DIR}
  ${QT_INCLUDES})

set(cr_lib_STAT_SRCS
    soundprocessor.cpp
    crvalidator.cpp
    crsliderarray.cpp
    crfloatspinbox.cpp
    creffectguitray.cpp
    creffectgui.cpp
    crdatastream.cpp)

kde4_add_library(cr_lib STATIC ${cr_lib_STAT_SRCS})
