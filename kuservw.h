#include <qwidget.h>

#include "kheader.h"
#include "kusertbl.h"

class KUserView : public QWidget
{
    Q_OBJECT

public:
  KUserView( QWidget* parent = NULL, const char* name = NULL );

  virtual ~KUserView();

  void clear();
  void insertItem(KUser *aku);
  int currentItem();
  void setCurrentItem( int item );
  void setAutoUpdate(bool state);

protected:
  virtual void KUserView::resizeEvent( QResizeEvent *rev );

signals:
  void selected(int item);
  void highlighted(int item);

private slots:
  void onSelect(int row, int);
  void onHighlight(int row, int);

private:
  void init();
  KHeader *m_Header;
  KUserTable *m_Users;
  int current;
};
