cmake_minimum_required(VERSION 2.8.9)
project(kuser)

# search packages used by KDE
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (MacroLibrary)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

include_directories (${QDBUS_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

find_package(KdepimLibs REQUIRED)

include(CheckFunctionExists)
include(CheckIncludeFiles)
include(CheckLibraryExists)

include(TestBigEndian)

add_subdirectory(pics)
add_subdirectory(toolbar)
add_subdirectory(icon)
add_subdirectory(doc)

# use the project-specific _SOURCE_DIR variable (set by the project() call above), Alex
include_directories(${kuser_SOURCE_DIR} ${KDEPIMLIBS_INCLUDE_DIR})

set(KU_HOMEDIR_PERM 0700 CACHE STRING "Default home directory permissions")
set(KU_MAILBOX_GID 0 CACHE STRING "Default GID of the created mbox file")
set(KU_MAILBOX_PERM 0660 CACHE STRING "Default mbox file permissions")
set(KU_HOMETEMPLATE "/home/%u" CACHE STRING "Default home directory template")
set(KU_FIRSTUID 500 CACHE STRING "First UID of normal users")
set(KU_FIRSTGID 500 CACHE STRING "First GID of normal users")
set(KU_USERPRIVATEGROUP true CACHE STRING "User private groups by default")

check_include_files(shadow.h HAVE_SHADOW_H)
check_include_files(crypt.h HAVE_CRYPT_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(paths.h HAVE_PATHS_H)
check_library_exists(crypt crypt "" HAVE_CRYPT_LIBRARY)

set(CMAKE_REQUIRED_INCLUDES pwd.h)
check_function_exists(fgetpwent HAVE_FGETPWENT)
set(CMAKE_REQUIRED_INCLUDES grp.h)
check_function_exists(fgetgrent HAVE_FGETGRENT)
set(CMAKE_REQUIRED_INCLUDES shadow.h)
check_function_exists(fgetspent HAVE_FGETSPENT)

test_big_endian(WORDS_BIGENDIAN)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ku_config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/ku_config.h)


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
)

kde4_add_kcfg_files(kuser_SRCS ku_prefs.kcfgc)

kde4_add_ui_files(kuser_SRCS ku_filessettings.ui ku_generalsettings.ui ku_ldapsettings.ui ku_ldapsamba.ui ku_passwordpolicy.ui)

kde4_add_executable(kuser ${kuser_SRCS})

target_link_libraries(kuser ${KDE4_KIO_LIBS} ${KDE4_KLDAP_LIBS} ${KDE4_TARGET_PREFIX}kntlm)
if(HAVE_CRYPT_LIBRARY)
	target_link_libraries(kuser crypt)
endif(HAVE_CRYPT_LIBRARY)

install(TARGETS kuser ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install(PROGRAMS kuser.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES kuserui.rc DESTINATION ${DATA_INSTALL_DIR}/kuser)
install(FILES kuser.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
