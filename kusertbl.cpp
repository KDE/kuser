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
  //printf("KUserRow::paint(%p, %d, %d)\n", p, col, width);
  //printf("ku = %p\n", ku);
  //printf("ku->p_name = %s\n", (const char *)ku->p_name);
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
  //printf("KUserRow::paint end\n");
}

KUser *KUserRow::getData() {
  return (ku);
}

KUserTable::KUserTable(QWidget *parent, const char *name) : KRowTable(SelectRow, parent, name)
{
  QString pixdir = kapp->kdedir() + QString("/share/apps/kuser/pics/");
  pmUser = new QPixmap(pixdir + "user.xpm");

  setCellHeight( max( fontMetrics().lineSpacing(), pmUser->height()) );

  setNumCols( 2 );

  setAutoUpdate(TRUE);
  current = -1;
  sort = 0;
}

KUserTable::~KUserTable() {
  delete pmUser;
}

void KUserTable::setAutoUpdate(bool state) {
  QTableView::setAutoUpdate(state);
}

void KUserTable::sortBy(int num) {
  if ((sort > -2)&&(sort < 3))
    sort = num;
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
  setNumRows(numRows() + 1);
  if (sort == -1)
    insertRow(tmpUser, numRows()-1);
  else {
    bool isinserted = FALSE;

    for (int i=0;i<numRows();i++) {
       KUserRow *krow;

       krow = (KUserRow *)getRow(i);

       if (krow == NULL)
         break;

       if (isinserted)
         break;

       switch (sort) {
         case 0:
           if (krow->getData()->p_name > (const char *)aku->p_name) {
             insertBeforeRow(tmpUser, i);
             isinserted = TRUE;
           }
           break;
         case 1:
           if (krow->getData()->p_fname > (const char *)aku->p_fname) {
             insertBeforeRow(tmpUser, i);
             isinserted = TRUE;
           }
           break;
      }
    }

    if (!isinserted) {
      insertRow(tmpUser, numRows()-1);
    }
  }

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
