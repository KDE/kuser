INCLUDES	=	$(all_includes)

# May be -D_XU_QUOTA -D_XU_SHADOW
DEFS		+= 	@KU_NIS@ @KU_SHADOW@ @KU_QUOTA@ @QUOTACTL@ -D_KU_DEBUG

kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
kuser_LDADD	=	     $(LIB_KDEUI) $(LIBCRYPT) $(PAMLIBS) $(LIBPWDB) $(LIBGEN)

####### Files

kuser_SOURCES	=	\
	mainWidget.cpp editGroup.cpp editGroupData.cpp kuservw.cpp \
	kheader.cpp krowtable.cpp kusertbl.cpp kgrouptbl.cpp kgroupvw.cpp \
	kdatectl.cpp kuser.cpp kgroup.cpp main.cpp maindlg.cpp mnt.cpp \
	propdlg.cpp quota.cpp misc.cpp usernamedlg.cpp pwddlg.cpp \
	grpnamedlg.cpp kerror.cpp addUser.cpp editDefaults.cpp \
	userDefaultsPage.cpp userDefaultsPageData.cpp

BUILTSOURCE	=	\
	mainWidget.moc editGroup.moc editGroupData.moc kuservw.moc kheader.moc \
	krowtable.moc kusertbl.moc kgrouptbl.moc kgroupvw.moc kdatectl.moc \
	maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc grpnamedlg.moc \
	addUser.moc userDefaultsPageData.moc userDefaultsPage.moc \
	editDefaults.moc

# the following are only useful, if you want to use "make dist"
noinst_HEADERS	=	\
	mainWidget.h editGroup.h editGroupData.h kuservw.h kheader.h \
	krowtable.h kusertbl.h kdatectl.h kuser.h maindlg.h kgroup.h \
	kgrouptbl.h kgroupvw.h propdlg.h mnt.h quota.h globals.h misc.h \
	usernamedlg.h pwddlg.h grpnamedlg.h kerror.h addUser.h \
	userDefaultsPageData.h userDefaultsPage.h editDefaults.h

#DISTFILES 	=	README AUTHORS THANKS
DISTCLEANFILES	= 	$(BUILTSOURCE)
EDITABLE	=	$(DISTFILES) $(noinst_HEADERS) $(kuser_SOURCES) Makefile.am

EXTRA_DIST = README AUTHORS THANKS kuser.kdelnk

# the programs get installed in $(prefix)/bin
bin_PROGRAMS = kuser

# the subdirs we have. Better would be to include pic here too
SUBDIRS = doc pic toolbar icon mini-icon lsm

# only install in your own directories
install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/System
	$(INSTALL_DATA) kuser.kdelnk $(kde_appsdir)/System/

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kuser_SOURCES) && mv messages.po ../po/kuser.pot

pot::
	/usr/local/bin/xgettext -ki18n -o kuser.pot -p ../po -d kuser -C -n *.cpp
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

grpnamedlg.cpp:grpnamedlg.moc
grpnamedlg.moc: grpnamedlg.h
	$(MOC) grpnamedlg.h -o grpnamedlg.moc

pwddlg.cpp:pwddlg.moc
pwddlg.moc: pwddlg.h
	$(MOC) pwddlg.h -o pwddlg.moc

krowtable.cpp: krowtable.moc
krowtable.moc: krowtable.h
	$(MOC) krowtable.h -o krowtable.moc

kheader.cpp: kheader.moc
kheader.moc: kheader.h
	$(MOC) kheader.h -o kheader.moc

kusertbl.cpp: kusertbl.moc
kusertbl.moc: kusertbl.h
	$(MOC) kusertbl.h -o kusertbl.moc

kuservw.cpp: kuservw.moc
kuservw.moc: kuservw.h
	$(MOC) kuservw.h -o kuservw.moc

kgrouptbl.cpp: kgrouptbl.moc
kgrouptbl.moc: kgrouptbl.h
	$(MOC) kgrouptbl.h -o kgrouptbl.moc

kgroupvw.cpp: kgroupvw.moc
kgroupvw.moc: kgroupvw.h
	$(MOC) kgroupvw.h -o kgroupvw.moc

editGroup.cpp: editGroup.moc editGroupData.moc
editGroup.moc: editGroup.h
	$(MOC) editGroup.h -o editGroup.moc

editGroupData.cpp: editGroupData.moc
editGroupData.moc: editGroupData.h
	$(MOC) editGroupData.h -o editGroupData.moc

mainWidget.cpp: mainWidget.moc
mainWidget.moc: mainWidget.h
	$(MOC) mainWidget.h -o mainWidget.moc

addUser.cpp: addUser.moc
addUser.moc: addUser.h
	$(MOC) addUser.h -o addUser.moc

editDefaults.cpp: editDefaults.moc
editDefaults.moc: editDefaults.h
	$(MOC) editDefaults.h -o editDefaults.moc

userDefaultsPage.cpp: userDefaultsPage.moc userDefaultsPageData.moc
userDefaultsPage.moc: userDefaultsPage.h
	$(MOC) userDefaultsPage.h -o userDefaultsPage.moc

userDefaultsPageData.cpp: userDefaultsPageData.moc
userDefaultsPageData.moc: userDefaultsPageData.h
	$(MOC) userDefaultsPageData.h -o userDefaultsPageData.moc


