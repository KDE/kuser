
project(kuser)

add_subdirectory( pic ) 
add_subdirectory( toolbar ) 
add_subdirectory( icon ) 

include_directories( ${CMAKE_SOURCE_DIR}/kuser  )

set(KU_HOMEDIR_PERM 0700 CACHE STRING "Default home directory permissions")
set(KU_MAILBOX_GID 0 CACHE STRING "Default GID of the created mbox file")
set(KU_MAILBOX_PERM 0660 CACHE STRING "Default mbox file permissions")
set(KU_HOMETEMPLATE "/home/%u" CACHE STRING "Default home directory template")
set(KU_FIRSTUID 500 CACHE STRING "First UID of normal users")
set(KU_FIRSTGID 500 CACHE STRING "First GID of normal users")
set(KU_USERPRIVATEGROUP true CACHE STRING "User private groups by default")

check_include_files(shadow.h HAVE_SHADOW_H)
check_include_files(crypt.h HAVE_CRYPT_H)

set(CMAKE_REQUIRED_INCLUDES pwd.h)
check_function_exists(fgetpwent HAVE_FGETPWENT)
set(CMAKE_REQUIRED_INCLUDES grp.h)
check_function_exists(fgetpwent HAVE_FGETGRENT)
set(CMAKE_REQUIRED_INCLUDES shadow.h)
check_function_exists(fgetspent HAVE_FGETSPENT)

add_definitions(-DHAVE_KU_CONFIG_H=1)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ku_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ku_config.h )

########### next target ###############

set(kuser_SRCS 
	ku_mainwidget.cpp 
	ku_mainview.cpp 
	ku_edituser.cpp 
	ku_editgroup.cpp 
	ku_usermodel.cpp 
	ku_groupmodel.cpp 
	ku_deluser.cpp 
	ku_user.cpp 
	ku_group.cpp 
	ku_main.cpp
	ku_misc.cpp 
	ku_pwdlg.cpp 
	ku_adduser.cpp 
	ku_configdlg.cpp 
	ku_global.cpp 
	sid.cpp
	ku_userfiles.cpp 
	ku_groupfiles.cpp 
	ku_userldap.cpp 
	ku_groupldap.cpp 
	ku_usersystem.cpp 
	ku_groupsystem.cpp 
	ku_selectconn.cpp
	sha1.cpp
)

kde4_add_ui_files(kuser_SRCS ku_filessettings.ui ku_generalsettings.ui ku_ldapsettings.ui ku_ldapsamba.ui ku_passwordpolicy.ui)

kde4_add_kcfg_files(kuser_SRCS ku_prefs.kcfgc)

kde4_automoc(${kuser_SRCS})

kde4_add_executable(kuser ${kuser_SRCS})

target_link_libraries(kuser  ${KDE4_KDECORE_LIBS} crypt ${KDE4_KDEUI_LIBS} ${KDE4_KABC_LIBS} kntlm)

install(TARGETS  kuser  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kuser.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kuserui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kuser )
install( FILES kuser.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )




#original Makefile.am contents follow:

#INCLUDES	= -I.. -I$(top_srcdir)/kuser $(all_includes)
#
## May be -D_XU_QUOTA -D_XU_SHADOW
##AM_CXXFLAGS = 	$(KU_NIS) $(KU_QUOTA) $(QUOTACTL) -D_KU_DEBUG
#
#kuser_LDFLAGS	=	$(all_libraries) $(KDE_RPATH)
#kuser_LDADD	=	$(LIB_KDEUI) $(LIBCRYPT) $(LIBSHADOW) $(LIBGEN) $(LIB_KABC) -lkntlm
#
######## Files
#
#kuser_SOURCES	=	\
#	ku_mainwidget.cpp ku_mainview.cpp ku_edituser.cpp ku_editgroup.cpp \
#	kuservw.cpp kgroupvw.cpp ku_deluser.cpp \
#	ku_user.cpp ku_group.cpp ku_main.cpp \
#	ku_misc.cpp ku_pwdlg.cpp \
#	ku_adduser.cpp ku_configdlg.cpp \
#	ku_global.cpp sid.cpp\
#	ku_userfiles.cpp ku_groupfiles.cpp ku_userldap.cpp ku_groupldap.cpp \
#	ku_usersystem.cpp ku_groupsystem.cpp ku_selectconn.cpp\
#	sha1.cpp ku_prefs.kcfgc \
#	ku_filessettings.ui ku_generalsettings.ui ku_ldapsettings.ui ku_ldapsamba.ui ku_passwordpolicy.ui
#
#kuser_METASOURCES = AUTO
#
## the following are only useful, if you want to use "make dist"
#noinst_HEADERS	=	\
#	globals.h       ku_edituser.h     ku_mainview.h    ku_userfiles.h \
#	kgroupvw.h      ku_global.h       ku_mainwidget.h  ku_user.h \
#	ku_adduser.h    ku_groupfiles.h   ku_misc.h        ku_userldap.h \
#	ku_configdlg.h  ku_group.h        ku_pwdlg.h       ku_usersystem.h \
#	ku_deluser.h    ku_groupldap.h    ku_selectconn.h  sha1.h \
#	ku_editgroup.h  ku_groupsystem.h  kuservw.h        sid.h \
#
#kde_kcfg_DATA = kuser.kcfg
#
#EDITABLE	=	$(noinst_HEADERS) $(kuser_SOURCES) Makefile.am
#
#EXTRA_DIST = README AUTHORS THANKS kuser.desktop
#
#rcdir = $(kde_datadir)/kuser
#rc_DATA = kuserui.rc
#
## the programs get installed in $(prefix)/bin
#bin_PROGRAMS = kuser
#
## the subdirs we have. Better would be to include pic here too
#SUBDIRS = pic toolbar icon
#
#xdg_apps_DATA = kuser.desktop
#
#edit::
#	xemacs -fn "-edict-fixed-medium-r-*-*-*-120-*-*-*-*-koi8-r" -geometry 142x41+0+0 $(EDITABLE)&
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kuser.pot
#
#pot::
#	/usr/local/bin/xgettext -ki18n -o kuser.pot -p ../po -d kuser -C -n *.cpp
#	/usr/local/bin/msgmerge --strict -v -o $(podir)/kuser.pot
#	mv $(podir)/ru/kuser.po
