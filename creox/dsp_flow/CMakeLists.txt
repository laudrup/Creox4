
include_directories(${CMAKE_SOURCE_DIR}/creox ${CMAKE_SOURCE_DIR}/creox/cr_lib ${CMAKE_SOURCE_DIR}/creox/preset ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(dsp_flow_STAT_SRCS threadeffector.cpp effectkeeper.cpp)

kde4_add_library(dsp_flow STATIC ${dsp_flow_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#noinst_LIBRARIES = libdsp_flow.a
#
### INCLUDES were found outside kdevelop specific part
#
#libdsp_flow_a_METASOURCES = AUTO
#
#libdsp_flow_a_SOURCES = threadeffector.cpp effectkeeper.cpp 
#
#
#EXTRA_DIST = effectkeeper.cpp effectkeeper.h threadeffector.cpp threadeffector.h 
#
######## kdevelop will overwrite this part!!! (end)############
#INCLUDES = $(all_includes) -I$(top_srcdir)/creox -I$(top_srcdir)/creox/cr_lib -I$(top_srcdir)/creox/preset
