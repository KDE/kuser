#include "misc.h"

#include "kgroupvw.h"

class KListViewGroup : public QListViewItem
{
public:
  KListViewGroup(QListView *parent, KGroup *aku);
  virtual QString text ( int ) const;

  KGroup *group;
};

KListViewGroup::KListViewGroup(QListView *parent, KGroup *aku)
 : QListViewItem(parent), group(aku)
{
}

QString KListViewGroup::text(int num) const
{
  switch(num)
  {
     case 0: return QString("%1 ").arg(group->getGID(),6);
     case 1: return group->getName();
  }
  return QString::null;
}


KGroupView::KGroupView(QWidget *parent, const char *name) 
  : KListView( parent, name ) 
{
  init();
}

KGroupView::~KGroupView() {
}

void KGroupView::insertItem(KGroup *aku) {
  KListViewGroup *groupItem = new KListViewGroup(this, aku);
  KListView::insertItem(groupItem);
  setSelected(groupItem, true);
}

void KGroupView::removeItem(KGroup *aku) {
  KListViewGroup *groupItem = (KListViewGroup *)firstChild();
  
  while(groupItem)
  {
     if (groupItem->group == aku)
     {
        delete groupItem;
        return;
     }
     groupItem = (KListViewGroup*) groupItem->nextSibling();
  }
}

void KGroupView::init() 
{
  setAllColumnsShowFocus(true);
  addColumn(i18n("GID"));
  setColumnAlignment(0, AlignRight);
  addColumn(i18n("Group name"));
}

KGroup *KGroupView::getCurrentGroup() {
  KListViewGroup *groupItem = (KListViewGroup *)currentItem();
  if (!groupItem) return 0;

  return groupItem->group;
}

#include "kgroupvw.moc"
