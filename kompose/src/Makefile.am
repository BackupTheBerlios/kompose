# set the include path for X, qt and KDE
INCLUDES = $(all_includes) -I `imlib2-config --cflags`

# these are the headers for your project
noinst_HEADERS = kompose.h komposetaskmanager.h komposetask.h komposewidget.h \
	komposetaskwidget.h komposedesktopwidget.h komposelayout.h komposesystray.h \
	komposefullscreenwidget.h komposesettings.h komposepreferences.h komposetaskprefswidget.h \
	komposewidgetinterface.h komposeglobal.h komposetaskcontainerwidget.h komposetaskmgrdcopiface.h \
	komposeimage.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kompose.pot

KDE_ICON = kompose

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kompose

# the application source, library search path, and link libraries
kompose_SOURCES = main.cpp kompose.cpp komposetaskmanager.cpp komposetask.cpp \
	komposewidget.cpp komposetaskwidget.cpp komposedesktopwidget.cpp komposelayout.cpp \
	komposesystray.cpp komposefullscreenwidget.cpp komposesettings.cpp komposepreferences.cpp \
	komposetaskprefswidget.cpp komposewidgetinterface.cpp komposeglobal.cpp \
	komposetaskcontainerwidget.cpp komposetaskmgrdcopiface.skel komposeimage.cpp
#kompose_LDFLAGS = $(KDE_RPATH) $(all_libraries) -lX11 -lXext -lm `pkg-config xft xrender xcomposite xdamage xfixes --libs` `imlib2-config --libs`
kompose_LDFLAGS = $(KDE_RPATH) $(all_libraries) -lX11 -lXext -lm `imlib2-config --libs`
kompose_LDADD = $(LIB_KDEUI) $(LIB_XRENDER)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kompose.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kompose
shellrc_DATA = komposeui.rc

kde_icon_KDEICON = hi32-app-kompose.png hi16-app-kompose.png
