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

#ifndef nisDefaultsPage_included
#define nisDefaultsPage_included

#include <kcontainer.h>
#include <klocale.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <sys/types.h>

class nisDefaultsPage : public QWidget {
    Q_OBJECT

public:
    nisDefaultsPage(
        QWidget* parent = NULL,
        const char* name = NULL
    );

  ~nisDefaultsPage();

  void    addOne(KContainerLayout *layout, int row, bool homogeneous,
                 bool expand, bool fill);

  QString getPasswdSrc() const;
  QString getNISPasswdSrc() const;
  QString getGroupSrc() const;
  QString getNISGroupSrc() const;
  QString getMINGID() const;
  QString getMINUID() const;
  bool    getEnableNIS() const;

  void setPasswdSrc(const QString & data);
  void setNISPasswdSrc(const QString & data);
  void setGroupSrc(const QString & data);
  void setNISGroupSrc(const QString & data);
  void setMINGID(const QString & data);
  void setMINUID(const QString & data);
  void setEnableNIS(bool data);
  void sourcesHelp();

private:
  QLineEdit *passwdsrc;
  QLineEdit *nispasswdsrc;
  QLineEdit *groupsrc;
  QLineEdit *nisgroupsrc;
  QLineEdit *nismingid;
  QLineEdit *nisminuid;
  QCheckBox* enableNIS;
};
#endif // nisDefaultsPage_included

