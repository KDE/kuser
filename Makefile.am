INCLUDES	= -I.. -I$(top_srcdir)/kuser $(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
#AM_CXXFLAGS = 	$(KU_NIS) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(LIBSHADOW) $(LIBGEN) $(LIB_KABC) -lkntlm

####### Files

kuser_SOURCES	=	\
	ku_mainwidget.cpp ku_mainview.cpp ku_edituser.cpp ku_editgroup.cpp \
	kuservw.cpp kgroupvw.cpp ku_deluser.cpp \
	ku_user.cpp ku_group.cpp ku_main.cpp \
	ku_misc.cpp ku_pwdlg.cpp \
	ku_adduser.cpp ku_configdlg.cpp \
	ku_global.cpp sid.cpp\
	ku_userfiles.cpp ku_groupfiles.cpp ku_userldap.cpp ku_groupldap.cpp \
	ku_usersystem.cpp ku_groupsystem.cpp ku_selectconn.cpp\
	sha1.cpp ku_prefs.kcfgc \
	ku_filessettings.ui ku_generalsettings.ui ku_ldapsettings.ui ku_ldapsamba.ui ku_passwordpolicy.ui

kuser_METASOURCES = AUTO

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	globals.h       ku_edituser.h     ku_mainview.h    ku_userfiles.h \
	kgroupvw.h      ku_global.h       ku_mainwidget.h  ku_user.h \
	ku_adduser.h    ku_groupfiles.h   ku_misc.h        ku_userldap.h \
	ku_configdlg.h  ku_group.h        ku_pwdlg.h       ku_usersystem.h \
	ku_deluser.h    ku_groupldap.h    ku_selectconn.h  sha1.h \
	ku_editgroup.h  ku_groupsystem.h  kuservw.h        sid.h \

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
