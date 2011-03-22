
include_directories(${CMAKE_SOURCE_DIR}/creox ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(cr_lib_STAT_SRCS
    soundprocessor.cpp
    crvalidator.cpp
    crsliderarray.cpp
    crfloatspinbox.cpp
    creffectguitray.cpp
    creffectgui.cpp
    crdatastream.cpp)

kde4_add_library(cr_lib STATIC ${cr_lib_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#noinst_LIBRARIES = libcr_lib.a
#
### INCLUDES were found outside kdevelop specific part
#
#libcr_lib_a_METASOURCES = AUTO
#
#libcr_lib_a_SOURCES = soundprocessor.cpp crvalidator.cpp crsliderarray.cpp crfloatspinbox.cpp creffectguitray.cpp creffectgui.cpp crdatastream.cpp 
#
#
#EXTRA_DIST = crdatastream.cpp crdatastream.h creffectgui.cpp creffectgui.h creffectguitray.cpp creffectguitray.h crexception.h crfloatspinbox.cpp crfloatspinbox.h crsliderarray.cpp crsliderarray.h crvalidator.cpp crvalidator.h soundprocessor.cpp soundprocessor.h 
#
######## kdevelop will overwrite this part!!! (end)############
#INCLUDES = $(all_includes) -I$(top_srcdir)/creox
