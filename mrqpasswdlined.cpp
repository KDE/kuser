
//
//
//



#include<qpainter.h>
#include<qdrawutl.h>
#include<qkeycode.h>
#include<qapp.h>
#include<qclipbrd.h>



#include "mrqpasswdlined.h"
#include "mrqpasswdlined.moc"



MrQPasswordLineEdit::MrQPasswordLineEdit(QWidget *parent, const char *name, WFlags f)
   : QWidget(parent, name, f)
{
   pixmap = new QPixmap();
   pixmap_valid = FALSE;
   
   anzahl = 0;
   textMaxLength=255;
   
   cursor_visible = FALSE;
   cursor_pos = 0;
   anzahl=0;
   connect( &timer, SIGNAL(timeout()), SLOT(cursor_switch()) );
   
   setFocusPolicy( QWidget::StrongFocus );
   setCursor( QCursor(ibeamCursor) );
   
   resize(150, 30);
}



void MrQPasswordLineEdit::pixmap_init( const QSize& groesse )
{
   if (pixmap_valid)
      return;
   
   // Pixmap auf die richtige groesse bringen
   pixmap->resize( groesse );
   pixmap->fill();
   pixmap_valid = TRUE;
   
   // Maximale Anzahl der Sternchen berechnen
   c_width = fontMetrics().width('*');
   if (!c_width)
      maxanzahl = 0;
   else
      maxanzahl = (groesse.width() - 6) / c_width -1;
   
   if (maxanzahl<0)
      maxanzahl = 0;
   
   if (maxanzahl <= 4)
      drittel=2;
   else
      drittel = maxanzahl/3;
   if (drittel > 10)
      drittel = 10;
   
   c_height = fontMetrics().height();
   
   // Rahmen malen
   QPainter p;
   p.begin( pixmap );
   qDrawWinButton( &p, 0, 0, groesse.width(), groesse.height(), colorGroup(), TRUE);
   p.end();
   
   // Sternchen wieder malen
   anzahl = _text.length();
   if (anzahl > maxanzahl)
      anzahl=maxanzahl;
   cursor_set(anzahl);
   pixmap_draw(0, anzahl);
}


void MrQPasswordLineEdit::setText(const char *t)
{
   QString zw(t);
   zw.truncate(textMaxLength);
   
   if ( strcmp(_text, zw) )
   {
      _text = zw;
      pixmap_valid=FALSE;
      repaint(FALSE);
      emit textChanged((const char *)_text);
   }
}


void MrQPasswordLineEdit::setMaxLength(const int wert)
{
   if (wert<=0)
      return;
   
   textMaxLength = wert;
   if ( (int)_text.length() > textMaxLength )
   {
      _text.truncate(textMaxLength);
      pixmap_valid=FALSE;
      repaint(FALSE);
      emit textChanged((const char *)_text);
   }
}




void MrQPasswordLineEdit::add(const int count)
{
   if (count <= 0)
      return;
   
   int neue_anzahl = anzahl + count;
   if (neue_anzahl > maxanzahl)
      neue_anzahl = maxanzahl - drittel;
   if (neue_anzahl < 0)
      neue_anzahl = 0;
   
   if (neue_anzahl < anzahl)
   {
      // Sternchen loeschen
      
      pixmap_clear(neue_anzahl, anzahl-neue_anzahl);
      cursor_set(neue_anzahl);
      change(neue_anzahl, anzahl-neue_anzahl +1);
      anzahl = neue_anzahl;
   }
   else
   {
      // Sternchen dazu
      cursor_set(neue_anzahl);
      pixmap_draw(anzahl, neue_anzahl-anzahl);
      change(anzahl, neue_anzahl-anzahl+1);
      anzahl = neue_anzahl;
   }
}


void MrQPasswordLineEdit::del(const int count)
{
   if (count <= 0)
      return;
   
   int neue_anzahl = anzahl - count;
   if (neue_anzahl <= 0)
      if ( (int)_text.length() <= (maxanzahl-drittel) )
         neue_anzahl = _text.length();
      else
         neue_anzahl = drittel;
   if (neue_anzahl > (int)_text.length() )
      neue_anzahl = _text.length();
   
   if (neue_anzahl < anzahl)
   {
      pixmap_clear(neue_anzahl, anzahl-neue_anzahl);
      cursor_set(neue_anzahl);
      change(neue_anzahl, anzahl-neue_anzahl +1);
      anzahl = neue_anzahl;
   }
   else
   {
      cursor_set(neue_anzahl);
      pixmap_draw(anzahl, neue_anzahl-anzahl);
      change(anzahl, neue_anzahl-anzahl+1);
      anzahl = neue_anzahl;
   }
}


void MrQPasswordLineEdit::pixmap_draw(const int start, const int anzahl)
{
   if (anzahl <= 0)
      return;
   
   static char *zeichen = "*";
   int x, h, i;
   
   // Startoffset berechnen
   x = c_width * start + 3;
   // Hoehe des Widgets - Rahmen
   h = size().height() - 6;
   
   // In einer Schleife 'anzahl' Sternchen malen
   QPainter p;
   p.begin(pixmap);
   for(i=0; i<anzahl; i++)
   {
      p.drawText(x, 3, c_width, h, AlignCenter, zeichen);
      x += c_width;
   }
   p.end();
}


void MrQPasswordLineEdit::pixmap_clear(const int start, const int anzahl)
{
   if (anzahl <= 0)
      return;
   
   int x = c_width * start + 3;
   int h = size().height() - 6;
   int w = c_width * anzahl;
   
   QPainter p;
   p.begin(pixmap);
   p.fillRect(x, 3, w, h, colorGroup().light() );
   p.end();
}


void MrQPasswordLineEdit::change(const int start, const int anzahl)
{
   if (anzahl <= 0)
      return;
   
   int h = size().height() -6;
   if (c_height < h)
      h = c_height;
   
   int x = c_width * start + 3;
   int y = ( size().height() - h ) / 2;
   int w = c_width * anzahl;
   
   repaint(x, y, w, h, FALSE);
}


void MrQPasswordLineEdit::pixmap_cursor(const int pos)
{
   int h = size().height() -6;
   if (c_height < h)
      h = c_height;
   
   int x = c_width * pos + 3;
   int y = ( size().height() - h ) / 2;
   
   QPainter p;
   p.begin( pixmap );
   p.fillRect(x, y, c_width, h, foregroundColor() );
   p.end();
}


void MrQPasswordLineEdit::cursor_on()
{
   if (cursor_visible)
      return;
   
   cursor_visible = TRUE;
   pixmap_cursor(cursor_pos);
}


void MrQPasswordLineEdit::cursor_off()
{
   if (!cursor_visible)
      return;
   
   cursor_visible = FALSE;
   pixmap_clear(cursor_pos, 1);
}


void MrQPasswordLineEdit::cursor_switch()
{
   if (cursor_visible)
      cursor_off();
   else
      cursor_on();
   
   change(cursor_pos, 1);
}


void MrQPasswordLineEdit::cursor_set(const int pos)
{
   if (cursor_visible)
   {
      // alten Cursor loeschen
      pixmap_clear(cursor_pos, 1);
      
      // neuen Cursor malen
      cursor_pos = pos;
      pixmap_cursor(cursor_pos);
   }
   else
   {
      // alte Stelle ist schon blank, neue Stelle vorsichtshalber loeschen
      cursor_pos = pos;
      pixmap_clear(cursor_pos, 1);
   }
}



QSize MrQPasswordLineEdit::sizeHint() const
{
   static QSize groesse;
   
   groesse.setHeight( fontMetrics().height() + 6 + 10);	// je 2 Pixel oben und unten vom 3D Rahmen,
   							// und je 1 Pixel oben & unten fuer den inneren Rahmen
   
   groesse.setWidth( fontMetrics().width('*') *16 +6 );	// Ungefaehr 16 Sternchen + Rahmen
   
   return(groesse);
}


void MrQPasswordLineEdit::resizeEvent(QResizeEvent *)
{
   pixmap_valid = FALSE;
}



void MrQPasswordLineEdit::paintEvent(QPaintEvent *event)
{
   pixmap_init( size() );
   
   bitBlt( this, event->rect().topLeft(), pixmap, event->rect() );
}


void MrQPasswordLineEdit::focusInEvent(QFocusEvent *)
{
   cursor_on();
   timer.start(500);
   change(cursor_pos, 1);
   // grabKeyboard();
}


void MrQPasswordLineEdit::focusOutEvent(QFocusEvent *)
{
   cursor_off();
   timer.stop();
   change(cursor_pos, 1);
   // releaseKeyboard();
}


void MrQPasswordLineEdit::keyPressEvent(QKeyEvent *event)
{
   if ( event->key()==Key_Enter || event->key()==Key_Return )
   {
      // Return
      emit returnPressed();
   }
   else if ( event->ascii() >= 32 && event->key() != Key_Delete )	// Checken, ob ASCII
   {
      // ASCII Code anhaengen
      char z = event->ascii();
      
      if ( (int)_text.length() < textMaxLength )
      {
         _text += z;
         cursor_on(); add(1); timer.start(500);
         emit textChanged((const char *)_text);
      }
   }
   else if ( event->state() & ControlButton )
   {
      // Control Taste gedrueckt
      switch(event->key())
      {
         case Key_H:			// CTRL-H  --> backspace
            backspace();
            break;
         
         case Key_Y:
         case Key_Z:
         case Key_K:
         case Key_X:			// CTRL-X  --> Alles loeschen
            _text.resize(1);
            cursor_on(); del(anzahl); timer.start(500);
            emit textChanged((const char *)_text);
            break;
         
         case Key_V:			// CTRL-V  --> Paste
            paste();
            break;
         
         default:
            event->ignore();		// Unbekannte Events zurueck geben
      }
   }
   else
   {
      // Normale Sodertasten
      switch(event->key())
      {
         case Key_Backspace:		// Backspace
            backspace();
            break;
         
         default:
            event->ignore();		// Unbekannte Events zurueck geben
      }
   }
}


void MrQPasswordLineEdit::backspace()
{
   if ( !_text.length() )
      return;
   
   _text.resize( _text.length() );
   cursor_on(); del(1); timer.start(500);
   emit textChanged((const char *)_text);
}


void MrQPasswordLineEdit::paste()
{
   QString t = QApplication::clipboard()->text();
   
   if ( !t.isEmpty() )
   {
      // Beim ersten Return cutten
      int l = t.find( '\n' );
      if ( l>=0 )
         t.truncate(l);
      
      // Sonderzeichen durch Leerzeichen ersetzen
      uchar *p;
      for( p=(uchar *)t.data(); *p; p++)
         if (*p < 32)
            *p = 32;
       
       // Text der schon im Widget steht, vorne einfuegen 
       t.insert(0, _text);
       
       // Neuen Text setzen
       setText(t);
   }
}



void MrQPasswordLineEdit::mousePressEvent(QMouseEvent *event)
{
   if ( event->button() == MidButton )
      paste();
}




