INCLUDES	= -I.. -I$(top_srcdir)/kuser $(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
#AM_CXXFLAGS = 	$(KU_NIS) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(LIBSHADOW) $(LIBGEN) $(LIB_KABC) -lkntlm

####### Files

kuser_SOURCES	=	\
	mainWidget.cpp editGroup.cpp kuservw.cpp \
	kgroupvw.cpp delUser.cpp \
	kuser.cpp kgroup.cpp main.cpp mainView.cpp \
	propdlg.cpp misc.cpp pwddlg.cpp \
	addUser.cpp editDefaults.cpp \
	kglobal.cpp sid.cpp\
	kuserfiles.cpp kgroupfiles.cpp kuserldap.cpp kgroupldap.cpp \
	kusersystem.cpp kgroupsystem.cpp selectconn.cpp\
	sha1.cpp kuserprefs.kcfgc \
	filessettings.ui generalsettings.ui ldapsettings.ui ldapsamba.ui passwordpolicy.ui

kuser_METASOURCES = AUTO

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	mainWidget.h editGroup.h kuservw.h \
	kuser.h mainView.h kgroup.h delUser.h \
	kgroupvw.h propdlg.h globals.h misc.h \
	pwddlg.h addUser.h \
	editDefaults.h selectconn.h sha1.h sid.h kglobal_.h \
	kusersystem.h kgroupsystem.h kuserfiles.h kgroupfiles.h kuserldap.h kgroupldap.h

kde_kcfg_DATA = kuser.kcfg

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
