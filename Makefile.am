INCLUDES	=	$(all_includes)

# May be -DLINUX -D_XU_QUOTA -DAIX -D_XU_SHADOW
SYSFLAGS = 	-D_KU_QUOTA -D_KU_SHADOW -D_KU_DEBUG # -g

# For Linux
OFLAGS =	-O2 -W -DLOCDIR=\"$(LOCALEDIR)\"
DEFS	+=	$(OFLAGS) $(SYSFLAGS) # -g
kuser_LDFLAGS	=	$(all_libraries)
kuser_LDADD	=	-lkdecore -lkdeui -lqt -L./ -lX11 -lXext -lintl $(LIBSOCKET)

####### Files

POFILES =	po/kuser.po
MOFILES =	po/kuser.mo
HTMLFILES =	doc/index.html
GIFFILES  =	doc/bugnet-member.gif
DOCFILES=	$(HTMLFILES) $(GIFFILES)
PICFILES=	pic/user.bmp
kuser_SOURCES =	kdatectl.cpp kuser.cpp main.cpp maindlg.cpp mnt.cpp propdlg.cpp quotatool.cpp pwdtool.cpp sdwtool.cpp misc.cpp usernamedlg.cpp pwddlg.cpp mrqpasswdlined.cpp
SRCMETA =       kdatectl.moc maindlg.moc propdlg.moc usernamedlg.moc pwddlg.moc mrqpasswdlined.moc
ALLHEADERS	=       kdatectl.h kuser.h maindlg.h   propdlg.h  mnt.h quotatool.h sdwtool.h pwdtool.h includes.h misc.h usernamedlg.h pwddlg.h mrqpasswdlined.h
EDITABLE =	$(POFILES) $(HTMLFILES) $(kuser_SOURCES) $(kuser_HEADERS) README Makefile.am

# Where to install binaries
PREFIX	=	$(prefix)
SBINDIR =	$(PREFIX)/bin
PICDIR  =       $(PREFIX)/lib/pic/kuser
DOCDIR  =	$(PREFIX)/share/doc/HTML/en/kuser
LOCALEDIR =	$(PREFIX)/lib/locale
MODIR   =	$(LOCALEDIR)/ru/LC_MESSAGES

sbin_PROGRAMS = kuser

# Version
VERSION =	0.1

srcdist:
	rm -rf ./kuser-$(VERSION) &>/dev/null
	mkdir ./kuser-$(VERSION)
	cp $(SOURCES) $(METASRC) $(HS) config.sub config.guess config.h.in acconfig.h acinclude.m4 Makefile.in configure.in configure install-sh ./kuser-$(VERSION)/
	cp -R po ./kuser-$(VERSION)/
	cp -R doc ./kuser-$(VERSION)/
	cp -R pic ./kuser-$(VERSION)/
	tar czvf ./kuser-$(VERSION).tgz ./kuser-$(VERSION)/
	rm -rf ./kuser-$(VERSION)

bindist: all
	rm -rf ./kuser-$(VERSION) &>/dev/null
	mkdir ./kuser-$(VERSION)
	cp $(TARGET) ./kuser-$(VERSION)/
	strip ./kuser-$(VERSION)/$(TARGET)
	tar czvf ./kuser-$(VERSION).bin.tgz ./kuser-$(VERSION)/
	rm -rf ./kuser-$(VERSION)

install-exec-local:
	$(INSTALL) -d $(SBINDIR)
	$(INSTALL) -s -m 750 ./$(TARGET) $(SBINDIR)

install-data-local:
	$(INSTALL) -d $(DOCDIR)
	$(INSTALL) -m 755 $(DOCFILES) $(DOCDIR)
	$(INSTALL) -d $(PICDIR)
	$(INSTALL) -m 644 $(PICFILES) $(PICDIR)
	$(INSTALL) -d $(MODIR)
	$(INSTALL) -m 644 $(MOFILES) $(MODIR)

showfiles:
	@echo $(HS) $(SOURCES) Makefile

updatepo:
	$(XGETTEXT) --default-domain=kuser -a -C -l ./ -n -s --keyword=_ $(SOURCES)
	mv kuser.po po/kuser.pot
	tupdate po/kuser.pot po/kuser.po>po/kuser.po.n
	mv po/kuser.po.n po/kuser.po

edit::
	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&

%.moc: %.h
	$(MOC) $< -o $@

%.mo: %.po
	$(MSGFMT) $< -o $@

