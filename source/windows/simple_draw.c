/*
	simple_draw.c - Definition der Zeichenfunktionen für "Simple DDE Draw"

	Autor: Martin Gräfe, martin.e.graefe@iem.thm.de
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <Ddeml.h>

#define SERVICE_NAME "SimpleDraw"
#define TOPIC_NAME "Drawing"
#define ITEM_NAME "Command"

HCONV hConversation = NULL;		// no initial conversation happening
DWORD sd_idInst = 0;
HSZ   sd_hsz_service, sd_hsz_topic, sd_hsz_item;


/******************************************************
	DDE-Callback-Funktion für Nachrichten vom Server
	zum Client
*******************************************************/
HDDEDATA DDECallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2,
                     HDDEDATA hdata, ULONG_PTR dwData1, ULONG_PTR dwData2)
{
	switch (uType) {
		case XTYP_DISCONNECT:
			hConversation = NULL;
			break;
		default: break;
	}
	return (HDDEDATA)NULL;
}


/******************************************************
	DDE-Verbindung zu "Simple DDE Draw" initialisieren
	Rückgabewert: 0 = OK, 1 = Fehler
*******************************************************/
int sd_dde_init() {
	if (sd_idInst)		// Initialisierung schon durchgeführt?
		return 1;
	if (DdeInitialize(&sd_idInst, (PFNCALLBACK)DDECallback, APPCLASS_STANDARD | APPCMD_CLIENTONLY, 0L) != DMLERR_NO_ERROR)
	{
		fprintf(stderr, "\n*** DDE-Initialisierung fehlgeschlagen!\n");
		return 1;
	}

	sd_hsz_service = DdeCreateStringHandle(sd_idInst, (LPCWSTR) SERVICE_NAME, CP_WINANSI);
	sd_hsz_topic = DdeCreateStringHandle(sd_idInst, (LPCWSTR) TOPIC_NAME, CP_WINANSI);
	sd_hsz_item = DdeCreateStringHandle(sd_idInst, (LPCWSTR) ITEM_NAME, CP_WINANSI);
	if ((sd_hsz_service == NULL) || (sd_hsz_topic == NULL) || (sd_hsz_item == NULL))
	{
		fprintf(stderr, "\n*** DDE-String-Handle konnte nicht angelegt werden!\n");
		DdeUninitialize(sd_idInst);
		return 1;
	}

	hConversation = DdeConnect (sd_idInst, sd_hsz_service, sd_hsz_topic, NULL);
	if (hConversation == NULL) {
		system(" echo \"Simple DDE Draw.exe\" | cmd");		// versuche, Simple DDE Draw zu starten
		Sleep(1000);
		hConversation = DdeConnect (sd_idInst, sd_hsz_service, sd_hsz_topic, NULL);
		if (hConversation == NULL) {
			system(" echo \"..\\Simple DDE Draw.exe\" | cmd");	// versuche, Simple DDE Draw zu starten
			Sleep(1000);
			hConversation = DdeConnect (sd_idInst, sd_hsz_service, sd_hsz_topic, NULL);
			if (hConversation == NULL) {
				fprintf(stderr, "\n*** Keine Verbindung zu 'Simple DDE Draw'!\n");
				DdeFreeStringHandle(sd_idInst, sd_hsz_service);
				DdeFreeStringHandle(sd_idInst, sd_hsz_topic);
				DdeFreeStringHandle(sd_idInst, sd_hsz_item);
				DdeUninitialize(sd_idInst);
				return 1;
			}
		}
    }
	return 0;
}


/******************************************************
	Löschen der Grafik
	keine Parameter, kein Rückgabewert
*******************************************************/
void ClearGraphic(void) {
	if (!hConversation)
		if (sd_dde_init())
			return;
	DdeClientTransaction((LPBYTE)"0", 2, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Grafik-Cursor positionieren
	Parameter: x, y
*******************************************************/
void MoveTo(int x, int y) {
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "1 %d %d", x, y);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Linie von Grafik-Cursor aus zu (x,y) ziehen
	Parameter: x, y
*******************************************************/
void DrawTo(int x, int y) {
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "2 %d %d", x, y);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Linie zeichnen
	Parameter: x1, y1, x2, y2
*******************************************************/
void DrawLine(int x1, int y1, int x2, int y2) {
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "3 %d %d %d %d", x1, y1, x2, y2);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Farbe und Strichstärke einstellen
	Parameter: Rot, Grün, Blau, Breite
*******************************************************/
void SetPen(int r, int g, int b, int w) {
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "4 %d %d %d %d", r, g, b, w);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Einzelnen Pixel setzen
	Parameter: x, y, Rot, Grün, Blau
*******************************************************/
void DrawPixel(int x, int y, int r, int g, int b) {
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "5 %d %d %d %d %d", x, y, r, g, b);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Text zeichnen
	Parameter: x, y, Text (UTF-8, max. 40 Bytes)
*******************************************************/
void PlaceText(int x, int y, char *text) {
	static char buffer[56];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "6 %d %d '", x, y);
	strncat(buffer, text, 39);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1, hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}


/******************************************************
	Fenstergröße ändern / Fenster verschieben
	Parameter: Position x, Position y, Breite, Höhe
*******************************************************/
void ResizeGraphic(int x, int y, int width, int height)
{
	static char buffer[24];

	if (!hConversation)
		if (sd_dde_init())
			return;
	sprintf(buffer, "7 %d %d %d %d", x, y, width, height);
	DdeClientTransaction((LPBYTE)buffer, (DWORD)strlen(buffer)+1,
		hConversation, sd_hsz_item, CF_TEXT, XTYP_POKE, 3000, NULL);
	return;
}
