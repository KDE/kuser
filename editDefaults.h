#ifndef editDefaults_included
#define editDefaults_included

#include <qtabdialog.h>

#include "userDefaultsPage.h"

class editDefaults : public QTabDialog {
  Q_OBJECT
public:
  editDefaults(QWidget* parent = NULL, const char* name = NULL);
  virtual ~editDefaults();

  const char *getShell() const;
  const char *getHomeBase() const;
  bool getCreateHomeDir() const;
  bool getCopySkel() const;
  bool getUsePrivateGroup() const;

  void setShell(const char *data);
  void setHomeBase(const char *data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUsePrivateGroup(bool data);

protected:
  userDefaultsPage *page1;
};

#endif // editDefaults_included
