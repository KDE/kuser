#ifndef _KUSERTBL_H
#define _KUSERTBL_H


#include <qpainter.h>
#include <qpixmap.h>
#include "krowtable.h"
#include "kuser.h"

class KUserRow : public KRow
{
public:
  KUserRow( KUser *aku, QPixmap *pUser);
  KUser *ku;

protected:
  virtual void paint( QPainter *painter, int col, int width );
  QPixmap *pmUser;
};

class KUserTable : public KRowTable
{
	Q_OBJECT

public:
	KUserTable( QWidget *parent = NULL, const char *name = NULL );

	void clear();
	void insertItem(KUser *aku);
	int currentItem();
	void setCurrentItem( int item );

private:
	QPixmap *pmUser;
	int fontpos;
	int	current;
};

#endif // _KUSERTBL_H
