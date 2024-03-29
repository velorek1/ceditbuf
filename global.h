/*
 * Global variables are intented to be placed here
 * Last modified: 9/7/2022
 * @author: velorek
 *
 */
#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include "scbuf.h"
#include "tm.h"
#include "edbuf.h" 
#include "listbox.h" 

#define TIMER_SPEED 150 //animation
#define TIMER_SPEED2 50 //cursor
#define END_LINE_CHAR 0x0A
#define FILL_CHAR 0x20
#define ENDSIGNAL -1
#define ESC_KEY '\e'
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80 


//USER-DEFINED MESSAGES
extern SCREENCELL *screen1;
extern SCREENCELL *screen2;
extern NTIMER cursor_timer1;
extern NTIMER timer2;
extern size_t animation;
extern int centerX;
//extern LISTCHOICE listBox1;
extern SCROLLDATA scrollData;
extern VLINES *edBuf1; //Buffer vector of lines(1022 chars)
extern VLINES tempLine;

//COLOR SCHEME
extern int EDITAREACOL;
extern int EDIT_FORECOLOR;
extern int STATUSBAR;
extern int STATUSMSG;
extern int MENU_PANEL;
extern int MENU2_PANEL;
extern int MENU_SELECTOR;
extern int MENU_FOREGROUND0;
extern int MENU2_FOREGROUND0;
extern int MENU_FOREGROUND1;
extern int EDITWINDOW_BACK;
extern int EDITWINDOW_FORE;
extern int SCROLLBAR_BACK;
extern int SCROLLBAR_FORE;
extern int SCROLLBAR_SEL;
extern int SCROLLBAR_ARR;
extern int WINDOW_TITLEB; // to be accessible from opfile.c
extern int WINDOW_TITLEF;

//GOODBYE MSG - ASCII ART
#define cedit_ascii_1 "  _____      ______    _ _ _   \n"
#define cedit_ascii_2 " / ____|    |  ____|  | (_) |  \n"
#define cedit_ascii_3 "| |   ______| |__   __| |_| |_ \n"
#define cedit_ascii_4 "| |  |______|  __| / _` | | __|\n"
#define cedit_ascii_5 "| |____     | |___| (_| | | |_ \n"
#define cedit_ascii_6 " \\_____|    |______\\__,_|_|\\__|\n"
#define TAB_SPACES 8

//ABOUT - ASCII ART
#define ABOUT_ASC_1 "        _   __       \n"
#define ABOUT_ASC_2 "       /  _|__ _|.___\n"
#define ABOUT_ASC_3 "       \\_  |__(_]| |  v0.1 \n       Coded   by  V3l0rek\n "
#define ANIMATION "||//--\\\\"

//USER-DEFINED MESSAGES
#define UNKNOWN "UNTITLED"
#define WINDOWMSG "DISPLAY IS TOO SMALL. PLEASE, RESIZE WINDOW"
#define STATUS_BAR_MSG1  " [C-Edit] | F2,CTRL+L: MENU | F1: HELP"
#define STATUS_BAR_MSG2 " [C-Edit] Press ESC to exit menu.             "
#define STATUS_BAR_MSG3 " ENTER: SELECT | <- -> ARROW KEYS             "
#define WLEAVE_MSG "\n       Are you sure\n    you want to quit?"
#define WSAVE_MSG ":\nFile saved successfully!"
#define WSAVELABEL_MSG "[-] File:"
#define WINFO_NOPEN "Error:\nThere isn't any file open!"
#define WINFO_SIZE "-File size: "
#define WINFO_SIZE2 "\n-No. of lines: "
#define WINFO_SIZE3 "\n-File name: "
#define WCHECKFILE2_MSG " File does not exist!  \n\n A new buffer will be created. \n"
#define WCHECKFILE_MSG " This file isn't a  \n text file. Program \n may crash. Open anyway?"
#define WINFONOTYET_MSG "Not implemented yet!"
#define WCHECKLINES_MSG " File longer than 3000 \n lines. You'll view those \n lines as read Mode! "
#define WMODIFIED_MSG " File has been modified\n Save current buffer?"
#define WFILEEXISTS_MSG " File exists. \n Overwrite?"
#define WFILEINREADMODE_MSG " File is on read mode. \n"

#define START_CURSOR_X 1
#define START_CURSOR_Y 2


//extern char aboutMSG[7][MAXLINE];

extern int     new_rows, new_columns, old_rows, old_columns;	// Terminal dimensions
extern int cursorX, cursorY; //position on screen X:[0, columns -2] Y:[0, rows-3]
extern int old_cursorX, old_cursorY;
extern long posBufX, posBufY; //position in the edit buffer X:[0,1022] Y:[0,13000]
extern long oldposBufX, oldposBufY; //position in the edit buffer X:[0,1022] Y:[0,13000]

int _animation();
int initCEDIT(); 


//TEXT BUFFER POINTERS
extern VLINES *edBuf1; //Buffer vector of lines(1022 chars)
extern VLINES tempLine;

extern int  hdisplayLimit; //horizontal scroll, last char position in line that allows scroll
extern int  currentColumn; //horizontal scroll, pointer to current char position in line
extern long linesinFile; //vertical scroll
extern long vdisplayLimit; //vertical scroll, last line that allows scroll
extern long currentLine; //verticall scroll, pointer to current top line in scroll
extern int vscrollActive; //vertical scroll, vertical scroll is posible
//extern int hscrollActive; //horizontal scroll, horizontal scroll is posible




#endif
