#ifndef editDefaults_included
#define editDefaults_included

#include <qtabdialog.h>

#include "userDefaultsPage.h"
#include "nisDefaultsPage.h"		

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
  QString getPasswdSrc() const;		
  QString getNISPasswdSrc() const;	
  QString getGroupSrc() const;		
  QString getNISGroupSrc() const;	
  QString getMINUID() const;		
  QString getMINGID() const;		
  bool getEnableNIS() const;		

  void setShell(const QString &data);
  void setHomeBase(const QString &data);
  void setCreateHomeDir(bool data);
  void setCopySkel(bool data);
  void setUserPrivateGroup(bool data);
  void setPasswdSrc(const QString &data);	
  void setNISPasswdSrc(const QString &data);	
  void setGroupSrc(const QString &data);	
  void setNISGroupSrc(const QString &data);	
  void setMINUID(const QString &data);		
  void setMINGID(const QString &data);		
  void setEnableNIS(bool data);			
  void sourcesHelp();				

protected:
  userDefaultsPage *page1;
  nisDefaultsPage *page2;		
};

#endif // editDefaults_included
