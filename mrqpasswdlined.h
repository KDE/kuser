
//
// Kleines LineEdit Widget fuer Passwoerter
//


#ifndef MRQPASSWORDLINEEDIT_H
#define MRQPASSWORDLINEEDIT_H


#include<qwidget.h>
#include<qpixmap.h>
#include<qtimer.h>



class MrQPasswordLineEdit : public QWidget
{
   Q_OBJECT
   
   public:
      MrQPasswordLineEdit(QWidget *parent=0, const char *name=0, WFlags f=0);
      virtual QSize sizeHint() const;				// Gibt die Empfohlene Groesse des Widgets zurueck
      
      const char *text() const { return (const char *)_text; };	// Gibt den Momentanen Text zurueck
      int maxLength() const {return(const int)textMaxLength; };	// Maximale Laenge des Textes
      void setMaxLength(const int length);			// Setzt die Maximale Laenge
   
   public slots:
      void setText(const char *);				// Setzt den Text des Widgets
   
   signals:
      void textChanged(const char *);
      void returnPressed();
      
   protected:
      virtual void paintEvent(QPaintEvent *event);		// Teile des Widgets zeichnen
      virtual void resizeEvent(QResizeEvent *event);		// Widget hat neue groesse
      virtual void focusInEvent(QFocusEvent *event);
      virtual void focusOutEvent(QFocusEvent *event);
      virtual void keyPressEvent(QKeyEvent *event);
      virtual void mousePressEvent(QMouseEvent *event);
      
   private:
      void pixmap_init(const QSize& groesse);			// Pixmap in angegebener Groesse malen
      void pixmap_draw(const int start, const int anzahl);	// Mal ab 'start' 'anzahl' Sternchen
      void pixmap_cursor(const int pos);			// Malt einen Cursor
      void pixmap_clear(const int start, const int anzahl);
      void cursor_on();
      void cursor_off();
      void cursor_set(const int pos);
      void change(const int start, const int anzahl);		// repaint erzeugen
      void add(const int count);				// 'anzahl' sternchen dazu
      void del(const int count);				// sternchen weg
      void backspace();
      void paste();
      
   private slots:
      void cursor_switch();
   
   private:
      QPixmap	*pixmap;					// Pixmap in der das komplette Widget drin ist
      int	anzahl;						// Anzahl der dargestellte Zeichen
      int	maxanzahl;					// Maximale Anzahl
      int	drittel;
      bool	pixmap_valid;					// Flag: Pixmap initialisiert
      int	c_width;					// Breite des '*'
      int	c_height;					// Hoehe
      bool	cursor_visible;					// Cursor an/aus
      int	cursor_pos;					// Aktuelle Position des Cursors
      QTimer	timer;						// Timer fuer das Cursor blinken
      QString	_text;
      int	textMaxLength;
};




#endif


