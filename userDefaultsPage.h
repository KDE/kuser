/**********************************************************************

	--- Qt Architect generated file ---

	File: userDefaultsPage.h
	Last generated: Mon Oct 12 00:56:45 1998

 *********************************************************************/

#ifndef userDefaultsPage_included
#define userDefaultsPage_included

#include <qlineedit.h>
#include <qcombobox.h>
#include <qcheckbox.h>

class userDefaultsPage : public QWidget
{
    Q_OBJECT

public:

    userDefaultsPage
    (
        QWidget* parent = NULL,
        const char* name = NULL
    );

  ~userDefaultsPage();

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

private:
  QComboBox *shell;
  QLineEdit *home;
  QCheckBox* createHomeDir;
  QCheckBox* copySkel;
  QCheckBox* usePrivateGroup;
};
#endif // userDefaultsPage_included
