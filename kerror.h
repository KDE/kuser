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

#ifndef _KERROR_H
#define _KERROR_H

#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>

/** Class to mantain a Queue of errors that are displayed with KMessageBox::information in FIFO way. */
class KError: public QObject {
public:
  /** Constructor, do nothing. */
  KError();
  
  /** Destructor, do nothing. */
  ~KError();

  /**
   * Add a message to the queue of messages
   * \param amesg is (a QString containing) the message to be added.
   */
  void addMsg(QString amsg);
  
  /** Display the messages enqueued. This is done in FIFO mode. */
  void display();

private:
  /** List of strings that are the messages to be displayed */
  QStringList msgs;
};

#endif // _KERROR_H
