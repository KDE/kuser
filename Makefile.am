INCLUDES	= -I.. -I$(top_srcdir)/kuser $(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
AM_CXXFLAGS = 	$(KU_NIS) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG -D_SVID_SOURCE -D_GNU_SOURCE -DQT_NO_ASCII_CAST -DQT_NO_CAST_ASCII

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(LIBSHADOW) $(LIBGEN)

####### Files

kuser_SOURCES	=	\
	mainWidget.cpp editGroup.cpp kuservw.cpp \
	kgroupvw.cpp delUser.cpp \
	kuser.cpp kgroup.cpp main.cpp mainView.cpp mnt.cpp \
	propdlg.cpp quota.cpp misc.cpp pwddlg.cpp \
	grpnamedlg.cpp kerror.cpp addUser.cpp editDefaults.cpp \
	nisDefaultsPage.cpp userDefaultsPage.cpp kglobal.cpp kcontainer.cpp

kuser_METASOURCES = AUTO

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	mainWidget.h editGroup.h kuservw.h \
	kuser.h mainView.h kgroup.h delUser.h \
	kgroupvw.h propdlg.h mnt.h quota.h globals.h misc.h \
	pwddlg.h grpnamedlg.h kerror.h addUser.h \
	nisDefaultsPage.h userDefaultsPage.h editDefaults.h \
	kglobal_.h

EDITABLE	=	$(noinst_HEADERS) $(kuser_SOURCES) Makefile.am

EXTRA_DIST = README AUTHORS THANKS kuser.desktop

rcdir = $(kde_datadir)/kuser
rc_DATA = kuserui.rc

# the programs get installed in $(prefix)/bin
bin_PROGRAMS = kuser

# the subdirs we have. Better would be to include pic here too
SUBDIRS = pic toolbar icon

xdg_apps_DATA = kuser.desktop

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kuser.pot

pot::
	/usr/local/bin/xgettext -ki18n -o kuser.pot -p ../po -d kuser -C -n *.cpp
	/usr/local/bin/msgmerge --strict -v -o $(podir)/kuser.pot
	mv $(podir)/ru/kuser.po
