
include_directories(${CMAKE_SOURCE_DIR}/creox/effects_gui ${CMAKE_SOURCE_DIR}/creox/cr_lib ${CMAKE_SOURCE_DIR}/creox/effects_dsp ${CMAKE_SOURCE_DIR}/creox/dsp_flow ${CMAKE_SOURCE_DIR}/creox/preset ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(cr_lib)
add_subdirectory(dsp_flow)
add_subdirectory(effects_dsp)
add_subdirectory(effects_gui)
add_subdirectory(pics)
add_subdirectory(preset)


########### next target ###############

set(creox_SRCS
    privilegesswitcher.cpp
    crthreadeventdispatcher.cpp
    crsplashscreen.cpp
    croptionsdialog.cpp
    crchainview.cpp
    crchainpopupmenu.cpp
    crchainbutton.cpp
    crbutton.cpp
    creox.cpp
    main.cpp)

kde4_add_executable(creox ${creox_SRCS})

target_link_libraries(creox ${KDE4_KDECORE_LIBS} ${QT_QT3SUPPORT_LIBRARIES} effects_gui effects_dsp dsp_flow cr_lib preset pthread jack m kdeui)

install(TARGETS creox ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(FILES  creoxui.rc DESTINATION share/apps/creox)



#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#bin_PROGRAMS = creox
#
### INCLUDES were found outside kdevelop specific part
#
#creox_SOURCES = privilegesswitcher.cpp crthreadeventdispatcher.cpp crsplashscreen.cpp croptionsdialog.cpp crchainview.cpp crchainpopupmenu.cpp crchainbutton.cpp crbutton.cpp creox.cpp main.cpp 
#creox_LDADD   = ./effects_gui/libeffects_gui.a ./effects_dsp/libeffects_dsp.a ./dsp_flow/libdsp_flow.a ./cr_lib/libcr_lib.a ./preset/libpreset.a -lpthread -lm $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)
#
#SUBDIRS = cr_lib dsp_flow effects_dsp effects_gui pics preset 
#
#EXTRA_DIST = main.cpp creox.cpp creox.h creox.desktop control.h crbutton.cpp crbutton.h crchainbutton.cpp crchainbutton.h crchainpopupmenu.cpp crchainpopupmenu.h crchainview.cpp crchainview.h croptionsdialog.cpp croptionsdialog.h crsplashscreen.cpp crsplashscreen.h crthreadeventdispatcher.cpp crthreadeventdispatcher.h privilegesswitcher.cpp privilegesswitcher.h creoxui.rc crsplash.png hi16-app-creox.png hi32-app-creox.png hi48-app-creox.png hi64-app-creox.png 
#
#install-data-local:
#	$(mkinstalldirs) $(kde_appsdir)/Applications/
#	$(INSTALL_DATA) $(srcdir)/creox.desktop $(kde_appsdir)/Applications/creox.desktop
#	$(mkinstalldirs) $(kde_datadir)/creox/pics/
#	$(INSTALL_DATA) $(srcdir)/crsplash.png $(kde_datadir)/creox/pics/crsplash.png
#	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
#	$(INSTALL_DATA) $(srcdir)/hi16-app-creox.png $(kde_icondir)/hicolor/16x16/apps/creox.png
#	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
#	$(INSTALL_DATA) $(srcdir)/hi32-app-creox.png $(kde_icondir)/hicolor/32x32/apps/creox.png
#	$(mkinstalldirs) $(kde_icondir)/hicolor/48x48/apps/
#	$(INSTALL_DATA) $(srcdir)/hi48-app-creox.png $(kde_icondir)/hicolor/48x48/apps/creox.png
#	$(mkinstalldirs) $(kde_icondir)/hicolor/64x64/apps/
#	$(INSTALL_DATA) $(srcdir)/hi64-app-creox.png $(kde_icondir)/hicolor/64x64/apps/creox.png
#
#uninstall-local:
#	-rm -f $(kde_appsdir)/Applications/creox.desktop
#	-rm -f $(kde_datadir)/creox/pics/crsplash.png
#	-rm -f $(kde_icondir)/hicolor/16x16/apps/creox.png
#	-rm -f $(kde_icondir)/hicolor/32x32/apps/creox.png
#	-rm -f $(kde_icondir)/hicolor/48x48/apps/creox.png
#	-rm -f $(kde_icondir)/hicolor/64x64/apps/creox.png
#
######## kdevelop will overwrite this part!!! (end)############
## These paths are KDE specific. Use them:
## kde_appsdir         Where your application's menu entry (.desktop) should go to.
## kde_icondir         Where your icon should go to - better use KDE_ICON.
## kde_sounddir        Where your sounds should go to.
## kde_htmldir         Where your docs should go to. (contains lang subdirs)
## kde_datadir         Where you install application data. (Use a subdir)
## kde_locale          Where translation files should go to. (contains lang subdirs)
## kde_cgidir          Where cgi-bin executables should go to.
## kde_confdir         Where config files should go to (system-wide ones with default values).
## kde_mimedir         Where mimetypes .desktop files should go to.
## kde_servicesdir     Where services .desktop files should go to.
## kde_servicetypesdir Where servicetypes .desktop files should go to.
## kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
## kde_wallpaperdir    Where general wallpapers should go to.
## kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
## kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
## kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
## kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
## kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
## kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).
#
## set the include path for X, qt and KDE
#INCLUDES = $(all_includes) -I$(top_srcdir)/creox/effects_gui -I$(top_srcdir)/creox/cr_lib -I$(top_srcdir)/creox/effects_dsp -I$(top_srcdir)/creox/dsp_flow -I$(top_srcdir)/creox/preset
#
#METASOURCES = AUTO
#
## the library search path. 
#creox_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#
## Uncomment the following two lines if you add a ui.rc file for your application to make use of
## KDE´s XML GUI builing
#rcdir = $(kde_datadir)/creox
#rc_DATA = creoxui.rc
#
##WARNING: if you use a ui.rc file above, use:
#
## messages: rc.cpp
#
## instead of 
#
## messages:
#
#messages: 
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/creox.pot; \
#	fi
#
