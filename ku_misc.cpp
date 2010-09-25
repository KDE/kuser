/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 or at your option version 3 as published by
 *  the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 **/

#include "globals.h"
#include <ku_config.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <errno.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif

#include <QFile>
#include <QDir>

#include <kmessagebox.h>
#include <krandom.h>
#include <kdebug.h>
#include <klocale.h>

#include "ku_misc.h"

bool backup(const QString & name)
{
  QString tmp = name + QString::fromLatin1(KU_BACKUP_EXT);
  QFile::remove( tmp );

  if (copyFile(QLatin1String( QFile::encodeName(name) ), QLatin1String( QFile::encodeName(tmp) )) == -1)
  {
    QString str;
    KMessageBox::error( 0, i18n("Can not create backup file for %1", name) );
    return false;
  }
  return true;
}

time_t now() {
  struct timeval tv;

  gettimeofday( &tv, NULL );
  return ( tv.tv_sec );
}

void copyDir(const QString &srcPath, const QString &dstPath, uid_t uid, gid_t gid)
{
  mode_t mode;
  QDir s(srcPath);
  QDir d(dstPath);

  QString dot = QString::fromLatin1(".");
  QString dotdot = QString::fromLatin1("..");

  s.setFilter( QDir::AllEntries | QDir::Hidden | QDir::System );

  for (uint i=0; i<s.count(); i++) {
    QString name(s[i]);

    if (name == dot)
      continue;
    if (name == dotdot)
      continue;

    QString filename(s.filePath(name));

    QFileInfo info(filename);
    mode = 0;
    if ( info.permission(QFile::ReadOwner) ) mode |=  S_IRUSR;
    if ( info.permission(QFile::WriteOwner) ) mode |=  S_IWUSR;
    if ( info.permission(QFile::ExeOwner) ) mode |=  S_IXUSR;
    if ( info.permission(QFile::ReadGroup) ) mode |=  S_IRGRP;
    if ( info.permission(QFile::WriteGroup) ) mode |=  S_IWGRP;
    if ( info.permission(QFile::ExeGroup) ) mode |=  S_IXGRP;
    if ( info.permission(QFile::ReadOther) ) mode |=  S_IROTH;
    if ( info.permission(QFile::WriteOther) ) mode |=  S_IWOTH;
    if ( info.permission(QFile::ExeOther) ) mode |=  S_IXOTH;

    if ( info.isSymLink() ) {
      QString link = info.readLink();

      if (symlink(QFile::encodeName(link),QFile::encodeName(d.filePath(name))) != 0) {
        KMessageBox::error( 0, i18n("Error creating symlink %1.\nError: %2",
                   d.filePath(s[i]), QString::fromLocal8Bit(strerror(errno))) );
      }
    } else if ( info.isDir() ) {
      QDir dir(filename);

      d.mkdir(name);
      copyDir(s.filePath(name), d.filePath(name), uid, gid);

      if (chown(QFile::encodeName(d.filePath(name)), uid, gid) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of folder %1.\nError: %2",
                   d.filePath(s[i]), QString::fromLocal8Bit(strerror(errno))) );
      }

      if (chmod(QFile::encodeName(d.filePath(name)), mode) != 0) {
        KMessageBox::error( 0, i18n("Cannot change permissions on folder %1.\nError: %2",
                   d.filePath(s[i]), QString::fromLocal8Bit(strerror(errno))) );
      }

    } else {
      if (copyFile(filename, d.filePath(name)) == -1) {
        continue;
      }

      if (chown(QFile::encodeName(d.filePath(name)), uid, gid) != 0) {
        KMessageBox::error( 0, i18n("Cannot change owner of file %1.\nError: %2",
                   d.filePath(s[i]), QString::fromLocal8Bit(strerror(errno))) );
      }

      if (chmod(QFile::encodeName(d.filePath(name)), mode) != 0) {
        KMessageBox::error( 0, i18n("Cannot change permissions on file %1.\nError: %2",
                   d.filePath(s[i]), QString::fromLocal8Bit(strerror(errno))) );
      }
    }
  }
}

#define BLOCK_SIZE 65536

int copyFile(const QString & from, const QString & to)
{
  QFile fi;
  QFile fo;
  char buf[BLOCK_SIZE];

  fi.setFileName(from);
  fo.setFileName(to);

  if (!fi.exists()) {
    KMessageBox::error( 0, i18n("File %1 does not exist.", from) );
    return (-1);
  }

  if (!fi.open(QIODevice::ReadOnly)) {
    KMessageBox::error( 0, i18n("Cannot open file %1 for reading.", from) );
    return (-1);
  }

  if (!fo.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    KMessageBox::error( 0, i18n("Cannot open file %1 for writing.", to) );
    return (-1);
  }

  while (!fi.atEnd()) {
    int len = fi.read(buf, BLOCK_SIZE);
    if (len <= 0)
      break;
    fo.write(buf, len);
  }

  fi.close();
  fo.close();

  return (0);
}

QStringList readShells()
{
    QStringList shells;

    FILE *f = fopen(SHELL_FILE,"r");
    if (f) {
      while (!feof(f)) {
        char s[200];

        fgets(s, 200, f);
        if (feof(f))
          break;

        s[strlen(s)-1]=0;
        if ((s[0])&&(s[0]!='#'))
          shells.append(QFile::decodeName(s));
      }
      fclose(f);
    }
    return shells;
}

void addShell(const QString &shell)
{
    QStringList shells = readShells();
    if (shells.contains(shell))
       return;

    FILE *f = fopen(SHELL_FILE,"a");
    if (f)
    {
        fputs(QFile::encodeName(shell).data(), f);
        fputc('\n', f);
    }
    fclose(f);
}

QByteArray genSalt( int len )
{
  QByteArray salt( len, 0 );
  const char * set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";

  salt[0] = set[getpid() % strlen(set)];
  for( int i = 1; i < len; i++ ) {
    salt[i] = set[KRandom::random() % strlen(set)];
  }
  return salt;
}

QString encryptPass( const QString &pass, bool md5 )
{
  QByteArray salt;
  char tmp[128];

  if ( md5 ) {
    salt = "$1$";
    salt += genSalt( 8 );
    salt += '$';

  } else {
    salt = genSalt( 2 );
  }
  strcpy( tmp, crypt( QFile::encodeName( pass ), salt ) );
  return QString::fromLocal8Bit( tmp );
}

int timeToDays(time_t time)
{
  return time < 0 ? -1 : time/(24*60*60);
}

time_t daysToTime(int days)
{
  return days*24*60*60;
}

void ku_add2ops( KLDAP::LdapOperation::ModOps &ops, const QString &attr, const QList<QByteArray> &vals, bool allownull )
{
  KLDAP::LdapOperation::ModOp op;
  op.type = KLDAP::LdapOperation::Mod_Replace;
  op.attr = attr;
  for ( int i = 0; i < vals.count(); ++i ) {
    if ( !vals[i].isEmpty() || allownull ) {
      op.values.append( vals[i] );
    }
  }
  ops.append( op );
}

void ku_add2ops( KLDAP::LdapOperation::ModOps &ops, const QString &attr, const QByteArray &val, bool allownull )
{
  QList<QByteArray> vals;
  kDebug() << "add2ops attr: " << attr << " value: '" << val << "'";
  vals.append( val );
  ku_add2ops( ops, attr, vals, allownull );
}
