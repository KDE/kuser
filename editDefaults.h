#ifndef editDefaults_included
#define editDefaults_included

#include <qtabdialog.h>

#include "userDefaultsPage.h"

class editDefaults : public QTabDialog {
  Q_OBJECT
public:
  editDefaults(QWidget* parent = NULL, const QString &name = NULL);
  virtual ~editDefaults();

  const QString &getShell() const;
  const QString &getHomeBase() const;
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
