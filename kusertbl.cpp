#include <kapp.h>

#include "kusertbl.h"
#include "kusertbl.moc"

#include "kuser.h"

#ifndef max
#define max(a,b) ((a>b) ? (a) : (b) )
#endif

KUserRow::KUserRow(KUser *aku, QPixmap *pUser)
{
  ku = aku;
  pmUser = pUser;
}

void KUserRow::paint( QPainter *p, int col, int width )
{
  int fontpos = (max( p->fontMetrics().lineSpacing(), pmUser->height()) - p->fontMetrics().lineSpacing())/2;
  switch(col) {
    case 0: {	// pixmap & Filename
      int start = 1 + pmUser->width() + 2;
      width -= 2 + pmUser->width();

      p->drawPixmap( 1, 0, *pmUser );
      p->drawText( start, fontpos, width, p->fontMetrics().lineSpacing(), AlignLeft, ku->p_name);
    }
      break;
    case 1:	// size
      p->drawText( 0, fontpos, width-2, p->fontMetrics().lineSpacing(), AlignLeft, ku->p_fname );
      break;
  }
}

KUserTable::KUserTable(QWidget *parent, const char *name) : KRowTable(SelectFixed, parent, name)
{
  QString pixdir = kapp->kdedir() + QString("/share/apps/kuser/pics/");
  pmUser = new QPixmap(pixdir + "user.xpm");

  setCellHeight( max( fontMetrics().lineSpacing(), pmUser->height()) );

  setNumCols( 2 );

  setAutoUpdate(TRUE);
  current = -1;
}

void KUserTable::clear()
{
  setTopCell( 0 );
  current = -1;
  setNumRows( 0 );
  updateScrollBars();
}

void KUserTable::insertItem(KUser *aku)
{
  KUserRow *tmpUser = new KUserRow(aku, pmUser);
  setNumRows( numRows() + 1 );
  insertRow( tmpUser, numRows()-1);
  if (autoUpdate())
    repaint();
}

int KUserTable::currentItem()
{
  return current;
}

void KUserTable::setCurrentItem(int item)
{
  int old = current;
  current = item;
  updateCell(old, 0);
  updateCell(current, 0, FALSE);
  emit highlighted(current, 0);
}
