#ifndef nisDefaultsPage_included
#define nisDefaultsPage_included

#include <kcontainer.h>
#include <klocale.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <sys/types.h>

#define NONNISPWD I18N_NOOP("Local passwd source:")
#define NONNISGRP I18N_NOOP("Local group source:")
#define NISPWD    I18N_NOOP("NIS passwd source:")
#define NISGRP    I18N_NOOP("NIS group source:")
#define NISMINUID I18N_NOOP("NIS minimum UID:")
#define NISMINGID I18N_NOOP("NIS minimum GID:")

class nisDefaultsPage : public QWidget {
    Q_OBJECT

public:
    nisDefaultsPage(
        QWidget* parent = NULL,
        const char* name = NULL
    );

  ~nisDefaultsPage();

  void    addOne(KContainerLayout *layout, int row, bool homogeneous,
                 bool expand, bool fill);

  QString getPasswdSrc() const;
  QString getNISPasswdSrc() const;
  QString getGroupSrc() const;
  QString getNISGroupSrc() const;
  QString getMINGID() const;
  QString getMINUID() const;
  bool    getEnableNIS() const;

  void setPasswdSrc(const QString & data);
  void setNISPasswdSrc(const QString & data);
  void setGroupSrc(const QString & data);
  void setNISGroupSrc(const QString & data);
  void setMINGID(const QString & data);
  void setMINUID(const QString & data);
  void setEnableNIS(bool data);
  void sourcesHelp();

private:
  QLineEdit *passwdsrc;
  QLineEdit *nispasswdsrc;
  QLineEdit *groupsrc;
  QLineEdit *nisgroupsrc;
  QLineEdit *nismingid;
  QLineEdit *nisminuid;
  QCheckBox* enableNIS;
};
#endif // nisDefaultsPage_included

