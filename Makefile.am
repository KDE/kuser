INCLUDES	=	$(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
DEFS	+= 	-D_KU_QUOTA -D_KU_SHADOW -D_KU_DEBUG # -g

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	-lkdecore -lkdeui -lqt -lX11 -lXext -lintl $(LIBSOCKET)

####### Files

kuser_SOURCES	=	kdatectl.cpp kuser.cpp main.cpp maindlg.cpp mnt.cpp propdlg.cpp quotatool.cpp pwdtool.cpp sdwtool.cpp misc.cpp usernamedlg.cpp pwddlg.cpp mrqpasswdlined.cpp
BUILTSOURCE	=       kdatectl.moc maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc mrqpasswdlined.moc
noinst_HEADERS	=       kdatectl.h kuser.h maindlg.h   propdlg.h  mnt.h quotatool.h sdwtool.h pwdtool.h includes.h misc.h usernamedlg.h pwddlg.h mrqpasswdlined.h
EDITABLE	=	doc/index.html $(kuser_SOURCES) $(noinst_HEADERS) README Makefile.am

noinst_PROGRAMS = kuser

SUBDIRS = doc

# Version
VERSION =	0.1

install-exec-local:
	$(mkinstalldirs) $(kde_bindir)
	$(INSTALL) -s -m 750 kuser $(kde_bindir)

install-data-local:
	$(mkinstalldirs) $(kde_datadir)
	$(INSTALL) -m 644 pic/user.xpm $(kde_datadir)/kuser/pics/
	$(mkinstalldirs) $(kde_toolbardir)
	$(INSTALL) -m 644 pic/profile_bw.xpm $(kde_toolbardir)

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

%.moc: %.h
	$(MOC) $< -o $@
