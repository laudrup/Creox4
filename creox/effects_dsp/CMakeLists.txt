
include_directories(${CMAKE_SOURCE_DIR}/creox ${CMAKE_SOURCE_DIR}/creox/cr_lib ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(effects_dsp_STAT_SRCS
    tremoloprocessor.cpp
    phaserprocessor.cpp
    flangerprocessor.cpp
    echoprocessor.cpp
    distortionprocessor.cpp)

kde4_add_library(effects_dsp STATIC ${effects_dsp_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#noinst_LIBRARIES = libeffects_dsp.a
#
### INCLUDES were found outside kdevelop specific part
#
#libeffects_dsp_a_METASOURCES = AUTO
#
#libeffects_dsp_a_SOURCES = tremoloprocessor.cpp phaserprocessor.cpp flangerprocessor.cpp echoprocessor.cpp distortionprocessor.cpp 
#
#
#EXTRA_DIST = distortionprocessor.cpp distortionprocessor.h echoprocessor.cpp echoprocessor.h flangerprocessor.cpp flangerprocessor.h phaserprocessor.cpp phaserprocessor.h tremoloprocessor.cpp tremoloprocessor.h 
#
######## kdevelop will overwrite this part!!! (end)############
#INCLUDES = $(all_includes) -I$(top_srcdir)/creox -I$(top_srcdir)/creox/cr_lib
