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

#ifndef editDefaults_included
#define editDefaults_included

#include <qtabdialog.h>

#include "userDefaultsPage.h"
#include "nisDefaultsPage.h"		

class editDefaults : public QTabDialog {
  Q_OBJECT
public:
  editDefaults(QWidget* parent = 0, const char * name = 0);
  virtual ~editDefaults();

  QString getShell() const;
  QString getHomeBase() const;
  bool getCreateHomeDir() const;
  bool getCopySkel() const;
  bool getUserPrivateGroup() const;
  QString getPasswdSrc() const;		
  QString getNISPasswdSrc() const;	
  QString getGroupSrc() const;		
  QString getNISGroupSrc() const;	
  QString getMINUID() const;		
  QString getMINGID() const;		
  bool getEnableNIS() const;		

  void setShell(const QString &data);
  void setHomeBase(const QString &data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUserPrivateGroup(bool data);
  void setPasswdSrc(const QString &data);	
  void setNISPasswdSrc(const QString &data);	
  void setGroupSrc(const QString &data);	
  void setNISGroupSrc(const QString &data);	
  void setMINUID(const QString &data);		
  void setMINGID(const QString &data);		
  void setEnableNIS(bool data);			
  void sourcesHelp();				

protected:
  userDefaultsPage *page1;
  nisDefaultsPage *page2;		
};

#endif // editDefaults_included
