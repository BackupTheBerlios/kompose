# set the include path for X, qt and KDE
INCLUDES = $(all_includes) -I `imlib2-config --cflags`

# these are the headers for your project
noinst_HEADERS = kompose.h komposetaskmanager.h komposetask.h komposewidget.h \
	komposetaskwidget.h komposedesktopwidget.h komposelayout.h komposesystray.h \
	komposefullscreenwidget.h komposetaskprefswidget.h komposeglobal.h komposetaskcontainerwidget.h \
	komposetaskvisualizer.h komposeviewmanager.h komposedcopiface.h abstractviewwidget.h \
	komposeglwidget.h komposeglviewwidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp --language=C --from-code=UTF-8 -o $(podir)/kompose.pot

komposeiconsdir = $(kde_datadir)/kompose/icons
komposeicons_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kompose

# the application source, library search path, and link libraries
kompose_SOURCES = main.cpp kompose.cpp komposetaskmanager.cpp komposetask.cpp \
	komposewidget.cpp komposetaskwidget.cpp komposedesktopwidget.cpp komposelayout.cpp \
	komposesystray.cpp komposefullscreenwidget.cpp komposetaskprefswidget.cpp \
	komposeglobal.cpp komposetaskcontainerwidget.cpp komposetaskvisualizer.cpp \
	komposeviewmanager.cpp komposedcopiface.skel abstractviewwidget.cpp komposeglwidget.cpp \
	komposeglviewwidget.cpp komposesettings.kcfgc prefsbehaviour.ui
#kompose_LDFLAGS = $(KDE_RPATH) $(all_libraries) -lX11 -lXext -lm `pkg-config xft xrender xcomposite xdamage xfixes --libs` `imlib2-config --libs`
kompose_LDFLAGS = $(KDE_RPATH) $(all_libraries) -lX11 -lXext -lm `imlib2-config --libs` -lGLU -lGL
kompose_LDADD = $(LIB_KDEUI) $(LIB_XCOMPOSITE) $(LIB_XDAMAGE)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kompose.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kompose
shellrc_DATA = komposeui.rc

kde_kcfg_DATA = kompose.kcfg

