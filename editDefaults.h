#ifndef editDefaults_included
#define editDefaults_included

#include <qtabdialog.h>

#include "userDefaultsPage.h"

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

  void setShell(const QString &data);
  void setHomeBase(const QString &data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUserPrivateGroup(bool data);

protected:
  userDefaultsPage *page1;
};

#endif // editDefaults_included
