
include_directories(${CMAKE_SOURCE_DIR}/creox ${CMAKE_SOURCE_DIR}/creox/cr_lib ${CMAKE_SOURCE_DIR}/creox/dsp_flow ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(preset_STAT_SRCS
    effectpreset.cpp
    crsavenewpresetdialog.cpp
    crpresetviewitem.cpp
    crpresetview.cpp
    crpresetpopupmenu.cpp
    crnewpresetfolderdialogimpl.cpp
    crnewpresetfolderdialog.cpp)

kde4_add_library(preset STATIC ${preset_STAT_SRCS})


########### install files ###############




#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#noinst_LIBRARIES = libpreset.a
#
### INCLUDES were found outside kdevelop specific part
#
#libpreset_a_METASOURCES = AUTO
#
#libpreset_a_SOURCES = effectpreset.cpp crsavenewpresetdialog.cpp crpresetviewitem.cpp crpresetview.cpp crpresetpopupmenu.cpp crnewpresetfolderdialogimpl.cpp crnewpresetfolderdialog.cpp 
#
#
#EXTRA_DIST = crnewpresetfolderdialog.cpp crnewpresetfolderdialog.h crnewpresetfolderdialogimpl.cpp crnewpresetfolderdialogimpl.h crpresetpopupmenu.cpp crpresetpopupmenu.h crpresetview.cpp crpresetview.h crpresetviewitem.cpp crpresetviewitem.h crsavenewpresetdialog.cpp crsavenewpresetdialog.h effectpreset.cpp effectpreset.h 
#
######## kdevelop will overwrite this part!!! (end)############
#INCLUDES = $(all_includes) -I$(top_srcdir)/creox -I$(top_srcdir)/creox/cr_lib -I$(top_srcdir)/creox/dsp_flow
