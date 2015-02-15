/*
	simple_draw.c - Definition der Zeichenfunktionen fuer "SimpleDraw", Linux-Version

	Autor: Martin Graefe, martin.e.graefe@iem.thm.de
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LENGTH 56
#define SIMPLE_DRAW_KEY 0x44726177	// "Draw" (big endian encoding)

struct my_msg_struct {
	long mtype;
	char text[MAX_LENGTH];
};

int sd_msgid=-1;	// ID of message queue (initial value: no ID yet)


/******************************************************
	DDE-Verbindung zu "SimpleDraw" initialisieren
	Rueckgabewert: 0 = OK, 1 = Fehler
*******************************************************/
int msgqueue_init()
{
	int ret;
	struct stat file_stat;

	if (sd_msgid >= 0)	// Initialisierung schon durchgefuehrt?
		return 0;

	sd_msgid = msgget((key_t)SIMPLE_DRAW_KEY, 0666);
	if (sd_msgid >= 0)
		return 0;	// Queue found -> exit with OK.

			// Queue not found. SimpleDraw not yet started?
	if (stat("SimpleDraw", &file_stat) == 0)
		ret = system("./SimpleDraw &");
	else if (stat("../SimpleDraw", &file_stat) == 0)
		ret = system("../SimpleDraw &");
	else if (stat("../../SimpleDraw", &file_stat) == 0)
		ret = system("../../SimpleDraw &");
	else
		ret = system("SimpleDraw &");
	if (ret > 0)
	{
		perror("\n*** system() failed");
		fprintf(stderr, "Return code is %d.\n", ret);
		return 1;
	}
	usleep(500000);

	sd_msgid = msgget((key_t)SIMPLE_DRAW_KEY, 0666);
	if (sd_msgid >= 0)
		return 0;

	fprintf(stderr, "\n*** Can't find message queue of 'SimpleDraw'!\n\n");
	return 1;
}


/******************************************************
	Send data through message queue
*******************************************************/
void msgqueue_sendstring(char *cmd)
{
	static struct my_msg_struct message;

	if (sd_msgid < 0)
		if (msgqueue_init())
			return;
	message.mtype = 1;
	strncpy(message.text, cmd, MAX_LENGTH);
	if (msgsnd(sd_msgid, (void *)&message, strlen(cmd)+1, 0))
	{
		fprintf(stderr, "\n*** Can't send message to 'SimpleDraw'!\n\n");
		sd_msgid = -1;
	}
}


/******************************************************
	Loeschen der Grafik
	keine Parameter, kein Rückgabewert
*******************************************************/
void ClearGraphic(void)
{
	msgqueue_sendstring("0");
	return;
}


/******************************************************
	Grafik-Cursor positionieren
	Parameter: x, y
*******************************************************/
void MoveTo(int x, int y)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "1 %d %d", x, y);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Linie von Grafik-Cursor aus zu (x,y) ziehen
	Parameter: x, y
*******************************************************/
void DrawTo(int x, int y)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "2 %d %d", x, y);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Linie zeichnen
	Parameter: x1, y1, x2, y2
*******************************************************/
void DrawLine(int x1, int y1, int x2, int y2)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "3 %d %d %d %d", x1, y1, x2, y2);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Farbe und Strichstärke einstellen
	Parameter: Rot, Grün, Blau, Breite
*******************************************************/
void SetPen(int r, int g, int b, int w)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "4 %d %d %d %d", r, g, b, w);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Einzelnen Pixel setzen
	Parameter: x, y, Rot, Grün, Blau
*******************************************************/
void DrawPixel(int x, int y, int r, int g, int b)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "5 %d %d %d %d %d", x, y, r, g, b);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Text zeichnen
	Parameter: x, y, Text (UTF-8, max. 40 Bytes)
*******************************************************/
void PlaceText(int x, int y, char *text)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "6 %d %d '", x, y);
	strncat(buffer, text, 39);
	msgqueue_sendstring(buffer);
	return;
}


/******************************************************
	Fenstergröße ändern / Fenster verschieben
	Parameter: Position x, Position y, Breite, Höhe
*******************************************************/
void ResizeGraphic(int x, int y, int width, int height)
{
	static char buffer[MAX_LENGTH];

	sprintf(buffer, "7 %d %d %d %d", x, y, width, height);
	msgqueue_sendstring(buffer);
	return;
}
