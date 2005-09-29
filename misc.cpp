/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Copyright (c) 2004 Szombathelyi György <gyurco@freemail.hu>
 *  Former maintainer: Adriaan de Groot <groot@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software Foundation.
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

#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif
#include <qfile.h>

#include <kmessagebox.h>

#include "misc.h"
#include "kglobal_.h"

bool backup(const QString & name)
{
  QString tmp = name + QString::fromLatin1(KU_BACKUP_EXT);
  QFile::remove( tmp );

  if (copyFile(QFile::encodeName(name), QFile::encodeName(tmp)) == -1)
  {
    QString str;
    KMessageBox::error( 0, i18n("Can't create backup file for %1").arg(name) );
    return false;
  }
  return true;
}

time_t now() {
  struct timeval tv;
  
  gettimeofday( &tv, NULL );
  return ( tv.tv_sec );
}

#define BLOCK_SIZE 65536

int copyFile(const QString & from, const QString & to) 
{
  QFile fi;
  QFile fo;
  char buf[BLOCK_SIZE];

  fi.setName(from);
  fo.setName(to);
  
  if (!fi.exists()) {
    KMessageBox::error( 0, i18n("File %1 does not exist.").arg(from) );
    return (-1);
  }

  if (!fi.open(IO_ReadOnly)) {
    KMessageBox::error( 0, i18n("Cannot open file %1 for reading.").arg(from) );
    return (-1);
  }

  if (!fo.open(IO_Raw | IO_WriteOnly | IO_Truncate)) {
    KMessageBox::error( 0, i18n("Cannot open file %1 for writing.").arg(to) );
    return (-1);
  }
  
  while (!fi.atEnd()) {
    int len = fi.readBlock(buf, BLOCK_SIZE);
    if (len <= 0)
      break;
    fo.writeBlock(buf, len);
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

QCString genSalt( int len )
{
  QCString salt( len + 1 );
  const char * set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789./";
    
  salt[0] = set[getpid() % strlen(set)];
  for( int i = 1; i < len; i++ ) {
    salt[i] = set[kapp->random() % strlen(set)];
  }
  return salt;
}

QString encryptPass( const QString &pass, bool md5 )
{
  QCString salt;
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
