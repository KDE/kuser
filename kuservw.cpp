#include "misc.h"

#include "kuservw.h"

class KListViewUser : public QListViewItem
{
public:
  KListViewUser(QListView *parent, KUser *aku);
  virtual QString text ( int ) const;

  KUser *user;
};

KListViewUser::KListViewUser(QListView *parent, KUser *aku)
 : QListViewItem(parent), user(aku)
{
}

QString KListViewUser::text(int num) const
{
  switch(num)
  {
     case 0: return QString::fromLatin1("%1 ").arg(user->getUID(),6);
     case 1: return user->getName();
     case 2: return user->getFullName();
  }
  return QString::null;
}

KUserView::KUserView(QWidget *parent, const char *name) 
  : KListView( parent, name )
{
  init();
}

KUserView::~KUserView()
{
}

void KUserView::insertItem(KUser *aku) {
  KListViewUser *userItem = new KListViewUser(this, aku);
  KListView::insertItem(userItem);
  setSelected(userItem, true);
}

void KUserView::removeItem(KUser *aku) {
  KListViewUser *userItem = (KListViewUser *)firstChild();
  
  while(userItem)
  {
     if (userItem->user == aku)
     {
        delete userItem;
        return;
     }
     userItem = (KListViewUser*) userItem->nextSibling();
  }
}

void KUserView::init()
{
  setAllColumnsShowFocus(true);
  addColumn(i18n("UID"));
  setColumnAlignment(0, AlignRight);
  addColumn(i18n("User Login"));
  addColumn(i18n("Full Name"));
}

KUser *KUserView::getCurrentUser() {
  KListViewUser *userItem = (KListViewUser *)currentItem();
  if (!userItem) return 0;
  
  return userItem->user;
}

#include "kuservw.moc"
