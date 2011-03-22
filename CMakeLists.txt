project(creox)

set( QT_USE_QT3SUPPORT TRUE )

find_package(KDE4 REQUIRED)

find_package(Qt4 REQUIRED)

find_package(Threads)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

include(KDE4Defaults)

include(${QT_USE_FILE})

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${QT_QT3SUPPORT_INCLUDE_DIR})

add_subdirectory(creox)

########### install files ###############




#original Makefile.am contents follow:

######## kdevelop will overwrite this part!!! (begin)##########
#
#EXTRA_DIST = creox.kdevprj admin AUTHORS COPYING ChangeLog INSTALL README TODO creox.lsm 
#
######## kdevelop will overwrite this part!!! (end)############
## not a GNU package. You can remove this line, if 
## have all needed files, that a GNU package needs
#AUTOMAKE_OPTIONS = foreign 
#
#$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;
#
#$(top_srcdir)/subdirs:
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs
#
#$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
#	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4
#
#MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files 
#
#package-messages:
#	$(MAKE) -f admin/Makefile.common package-messages
#	$(MAKE) -C po merge
#
#dist-hook:
#	cd $(top_distdir) && perl admin/am_edit -padmin
#	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
#SUBDIRS=$(TOPSUBDIRS)
