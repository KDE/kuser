/*
 *  Copyright (c) 1998 Denis Perchine <dyp@perchine.com>
 *  Maintained by Adriaan de Groot <groot@kde.org>
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
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 **/

#include "globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

#ifdef HAVE_SYS_MNTENT_H
#include <sys/mntent.h>
#define BAD_GETMNTENT
#endif

#ifdef HAVE_SYS_MNTTAB_H
#include <sys/mnttab.h>
#endif

#include <qfile.h>

#include <kmessagebox.h>

#include "misc.h"
#include "kglobal_.h"

void backup(const QString & name)
{
  QString tmp = name + QString::fromLatin1(KU_BACKUP_EXT);
  unlink(QFile::encodeName(tmp));

  if (rename(QFile::encodeName(name), QFile::encodeName(tmp)) == -1)
  {
    QString str;
    str = i18n("Can't create backup file for %1").arg(name);
    KMessageBox::error(0, tmp);
    exit (1);
  }
}

long today() {
  return (time(NULL)/(24*60*60));
}

char *updateString(char *d, const char *t) {
  free(d);
  d = (char *)malloc(strlen(t)+1);  
  strcpy(d,t);
  return d;
}

int getValue(long int &data, const QString & text, const QString & msg) {
  bool ok;
  long int value = text.toLong(&ok);
  if (!ok) {
    KMessageBox::error(0, msg);
    return (-1);
  }
  data = value;

  return (0);
}

int getValue(int &data, const QString & text, const QString & msg) {
  bool ok;
  long int value = text.toLong(&ok);
  if (!ok) {
    KMessageBox::error(0, msg);
    return (-1);
  }
  data = value;

  return (0);
}

int getValue(unsigned int &data, const QString & text, const QString & msg) {
  bool ok;
  long int value = text.toLong(&ok);
  if (!ok) {
    KMessageBox::error(0, msg);
    return (-1);
  }
  data = value;

  return (0);
}

#define BLOCK_SIZE 65536

int copyFile(const QString & from, const QString & to) {
  QFile fi;
  QFile fo;
  char buf[BLOCK_SIZE];

#ifdef _KU_DEBUG
  printf("%s -> %s\n", from.local8Bit().data(), to.local8Bit().data());
#endif
  
  fi.setName(from);
  fo.setName(to);
  
  if (!fi.exists()) {
    err->addMsg(i18n("File %1 does not exist.").arg(from));
		err->display(); 
    return (-1);
  }

  if (!fi.open(IO_ReadOnly)) {
    err->addMsg(i18n("Cannot open file %1 for reading.").arg(from));
		err->display();
    return (-1);
  }

  if (!fo.open(IO_Raw | IO_WriteOnly | IO_Truncate)) {
    err->addMsg(i18n("Cannot open file %1 for writing.").arg(to));
		err->display();
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
