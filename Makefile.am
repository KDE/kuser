INCLUDES	=	$(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
DEFS		+= 	-g -D_KU_SHADOW -D_KU_DEBUG -D_KU_QUOTA

kuser_LDFLAGS	=	-g $(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	-lkdeui -lkdecore -lqt -lX11 -lXext $(LIBSOCKET) $(LIBCRYPT)

####### Files

kuser_SOURCES	=	kuservw.cpp kheader.cpp krowtable.cpp kusertbl.cpp kdatectl.cpp kuser.cpp main.cpp maindlg.cpp mnt.cpp propdlg.cpp quota.cpp misc.cpp usernamedlg.cpp pwddlg.cpp
BUILTSOURCE	=       kuservw.moc kheader.moc krowtable.moc kusertbl.moc kdatectl.moc maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=       kuservw.h kheader.h krowtable.h kusertbl.h kdatectl.h kuser.h maindlg.h   propdlg.h  mnt.h quota.h globals.h misc.h usernamedlg.h pwddlg.h
DISTFILES 	=	README AUTHORS THANKS
DISTCLEANFILES	= 	$(BUILTSOURCE) 
EDITABLE	=	$(DISTFILES) $(noinst_HEADERS) $(kuser_SOURCES) Makefile.am

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

pot::
	/usr/local/bin/xgettext -k_ -o kuser.pot -p ../po -d kuser -C -n *.cpp
	/usr/local/bin/msgmerge --strict -v -o ../po/ru/kuser.po.new ../po/ru/kuser.po ../po/kuser.pot
	mv ../po/ru/kuser.po.new ../po/ru/kuser.po

kdatectl.cpp:kdatectl.moc
kdatectl.moc: kdatectl.h
	$(MOC) kdatectl.h -o kdatectl.moc

maindlg.cpp:maindlg.moc
maindlg.moc: maindlg.h
	$(MOC) maindlg.h -o maindlg.moc

propdlg.cpp:propdlg.moc
propdlg.moc: propdlg.h
	$(MOC) propdlg.h -o propdlg.moc


usernamedlg.cpp:usernamedlg.moc
usernamedlg.moc: usernamedlg.h
	$(MOC) usernamedlg.h -o usernamedlg.moc

pwddlg.cpp:pwddlg.moc
pwddlg.moc: pwddlg.h
	$(MOC) pwddlg.h -o pwddlg.moc

kusertbl.cpp: kusertbl.moc
kusertbl.moc: kusertbl.h
	$(MOC) kusertbl.h -o kusertbl.moc

krowtable.cpp: krowtable.moc
krowtable.moc: krowtable.h
	$(MOC) krowtable.h -o krowtable.moc

kheader.cpp: kheader.moc
kheader.moc: kheader.h
	$(MOC) kheader.h -o kheader.moc

kuservw.cpp: kuservw.moc
kuservw.moc: kuservw.h
	$(MOC) kuservw.h -o kuservw.moc


