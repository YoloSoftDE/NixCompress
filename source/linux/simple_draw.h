/*
	simple_draw.h - Headerdatei f�r die Zeichenfunktionen zu "Simple DDE Draw"
	Autor: Martin Gr�fe, martin.e.graefe@iem.thm.de
*/

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
	L�schen der Grafik
	keine Parameter, kein R�ckgabewert
*******************************************************/
void ClearGraphic(void);


/******************************************************
	Grafik-Cursor positionieren
	Parameter: x, y
*******************************************************/
void MoveTo(int x, int y);


/******************************************************
	Linie von Grafik-Cursor aus zu (x,y) ziehen
	Parameter: x, y
*******************************************************/
void DrawTo(int x, int y);


/******************************************************
	Linie zeichnen
	Parameter: x1, y1, x2, y2
*******************************************************/
void DrawLine(int x1, int y1, int x2, int y2);


/******************************************************
	Farbe und Strichst�rke einstellen
	Parameter: Rot, Gr�n, Blau, Breite
*******************************************************/
void SetPen(int r, int g, int b, int w);


/******************************************************
	Einzelnen Pixel setzen
	Parameter: x, y, Rot, Gr�n, Blau
*******************************************************/
void DrawPixel(int x, int y, int r, int g, int b);


/******************************************************
	Text zeichnen
	Parameter: x, y, Text (UTF-8, max. 40 Bytes)
*******************************************************/
void PlaceText(int x, int y, char *text);


/******************************************************
	Fenstergr��e �ndern / Fenster verschieben
	Parameter: Position x, Position y, Breite, H�he
*******************************************************/
void ResizeGraphic(int x, int y, int width, int height);


#ifdef __cplusplus
}
#endif
