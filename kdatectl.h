#ifndef KU_DATECTL_H
#define KU_DATECTL_H

//#include "includes.h"
#include <qobject.h>
#include <qlined.h>
#include <qlabel.h>
#include <qchkbox.h>
#include <qfont.h>

#include <kcombo.h>

class KDateCtl: public QObject
{
Q_OBJECT
public:
	int x;
	int y;

	int iday;
	int imonth;
	int iyear;

	QLineEdit *day;
	KCombo *month;
	QLineEdit *year;
	QCheckBox *isempty;
        QLabel *label;

	KDateCtl(QWidget *parent, const char *name, const char *checkTitle,
		 const char *title, long int adate, int ax, int ay);
	~KDateCtl();
 	void setDate(long int adate);
	long int getDate();	
	void setFont(const QFont &f);
private:
	void updateControls();
protected slots:
	void isEmptyToggled(bool);
	void dayChanged(const char *text);
	void monthChanged(int);
	void yearChanged(const char *);
signals:
	void textChanged();
};

#endif // K_DATECTL_H

