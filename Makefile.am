INCLUDES	=	$(all_includes) -I.. -I$(top_srcdir)/kuser

# May be -D_XU_QUOTA -D_XU_SHADOW
AM_CXXFLAGS = 	$(KU_NIS) $(KU_SHADOW) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(PAMLIBS) $(LIBPWDB) $(LIBGEN)

####### Files

kuser_SOURCES	=	\
	mainWidget.cpp editGroup.cpp editGroupData.cpp kuservw.cpp \
	kheader.cpp krowtable.cpp kusertbl.cpp kgrouptbl.cpp kgroupvw.cpp \
	kdatectl.cpp kuser.cpp kgroup.cpp main.cpp mainView.cpp mnt.cpp \
	propdlg.cpp quota.cpp misc.cpp usernamedlg.cpp pwddlg.cpp \
	grpnamedlg.cpp kerror.cpp addUser.cpp editDefaults.cpp \
	userDefaultsPage.cpp kglobal.cpp

kuser_METASOURCES =	USE_AUTOMOC

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	mainWidget.h editGroup.h editGroupData.h kuservw.h kheader.h \
	krowtable.h kusertbl.h kdatectl.h kuser.h mainView.h kgroup.h \
	kgrouptbl.h kgroupvw.h propdlg.h mnt.h quota.h globals.h misc.h \
	usernamedlg.h pwddlg.h grpnamedlg.h kerror.h addUser.h \
	userDefaultsPage.h editDefaults.h kglobal.cpp

EDITABLE	=	$(DISTFILES) $(noinst_HEADERS) $(kuser_SOURCES) Makefile.am

EXTRA_DIST = README AUTHORS THANKS kuser.desktop

rcdir = $(kde_datadir)/kuser
rc_DATA = kuserui.rc

# the programs get installed in $(prefix)/bin
bin_PROGRAMS = kuser

# the subdirs we have. Better would be to include pic here too
SUBDIRS = pic toolbar icon lsm

appsdir   = $(kde_appsdir)/System
apps_DATA = kuser.desktop

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

messages: rc.cpp
	$(XGETTEXT) rc.cpp $(kuser_SOURCES) -o $(podir)/kuser.pot

pot::
	/usr/local/bin/xgettext -ki18n -o kuser.pot -p ../po -d kuser -C -n *.cpp
	/usr/local/bin/msgmerge --strict -v -o $(podir)/kuser.pot
	mv $(podir)/ru/kuser.po

