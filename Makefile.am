INCLUDES	=	$(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
DEFS		+= 	-D_KU_QUOTA -D_KU_SHADOW -D_KU_DEBUG 

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	-lkdeui -lkdecore -lqt -lX11 -lXext $(LIBSOCKET) $(LIBCRYPT)

####### Files

kuser_SOURCES	=	kdatectl.cpp kuser.cpp main.cpp maindlg.cpp mnt.cpp propdlg.cpp quotatool.cpp pwdtool.cpp sdwtool.cpp misc.cpp usernamedlg.cpp pwddlg.cpp mrqpasswdlined.cpp
BUILTSOURCE	=       kdatectl.moc maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc mrqpasswdlined.moc

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=       kdatectl.h kuser.h maindlg.h   propdlg.h  mnt.h quotatool.h sdwtool.h pwdtool.h includes.h misc.h usernamedlg.h pwddlg.h mrqpasswdlined.h
DISTFILES 	=	README AUTHORS
DISTCLEANFILES	= 	$(BUILTSOURCE) 
EDITABLE	=	$(DISTFILES) $(noinst_HEADERS) $(kuser_SOURCES)

# the programs get installed in $(prefix)/bin
bin_PROGRAMS = kuser

# the subdirs we have. Better would be to include pic here too
SUBDIRS = doc

# override the permissions
INSTALL_PROGRAMM = ${INSTALL} -m 0750 

# only install in your own directories
install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/System
	$(INSTALL) -m 644 kuser.kdelnk $(kde_appsdir)/System/
	$(mkinstalldirs) $(kde_datadir)/kuser/pics
	$(INSTALL) -m 644 pic/user.xpm $(kde_datadir)/kuser/pics/
	$(mkinstalldirs) $(kde_datadir)/kuser/toolbar
	$(INSTALL) -m 644 pic/profile_bw.xpm $(kde_datadir)/kuser/toolbar

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

kdatectl.cpp:kdatectl.moc
kdatectl.moc: kdatectl.h
	$(MOC) kdatectl.h -o kdatectl.moc

maindlg.cpp:maindlg.moc
maindlg.moc: maindlg.h
	$(MOC) maindlg.h -o maindlg.moc

mrqpasswdlined.cpp:mrqpasswdlined.moc
mrqpasswdlined.moc: mrqpasswdlined.h
	$(MOC) mrqpasswdlined.h -o mrqpasswdlined.moc

propdlg.cpp:propdlg.moc
propdlg.moc: propdlg.h
	$(MOC) propdlg.h -o propdlg.moc


usernamedlg.cpp:usernamedlg.moc
usernamedlg.moc: usernamedlg.h
	$(MOC) usernamedlg.h -o usernamedlg.moc

pwddlg.cpp:pwddlg.moc
pwddlg.moc: pwddlg.h
	$(MOC) pwddlg.h -o pwddlg.moc



