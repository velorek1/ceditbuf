/*
======================================================================
PROGRAM C Editor - An editor with top-down menus.
@author : Velorek
@version : 1.0
Last modified : 05/08/2021 - Complete overhaul + Dynamic buffer added
======================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "rterm.h"
#include "listc.h"
#include "scbuf.h"
#include "opfile.h"
#include "uintf.h"
#include "fileb.h"
#include "keyb.h"
#include "about.h"
#include "tm.h"
#include "edbuf.h"

/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

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

//MISC. CONSTANTS
#define EXIT_FLAG -1
#define TAB_DISTANCE 8		//How many spaces TAB key will send.
#define START_CURSOR_X 2
#define START_CURSOR_Y 3
#define ROWS_FAILSAFE 25
#define COLUMNS_FAILSAFE 80
#define K_LEFTMENU -1		//Left arrow key pressed while in menu
#define K_RIGHTMENU -2		//Right arrow key pressed while in menu
#define MAX_TEXT 150
#define MAX_PATH 1024

//MENU CONSTANTS
#define HOR_MENU -1
#define FILE_MENU 0
#define OPT_MENU 1
#define HELP_MENU 2
#define YESNO_MENU 3
#define OK_MENU 4
#define MAX_FILENAME 100

//DROP-DOWN MENUS
#define OPTION_1 0
#define OPTION_2 1
#define OPTION_3 2
#define OPTION_4 3
#define OPTION_5 4
#define OPTION_NIL -1		//Reset option
#define CONFIRMATION 1

// DISPLAY CONSTANTS
#define FILL_CHAR 32
#define CHAR_NIL '\0'
#define END_LINE_CHAR 0x0A	// $0A

//FILE CONSTANTS
#define FILE_MODIFIED 1
#define FILE_UNMODIFIED 0
#define FILE_READMODE 2 
#define MAX_LINES 13000
/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

LISTCHOICE *mylist, data; //menus handler
SCREENCELL *my_screen; //display handler
SCROLLDATA openFileData; //openFile dialog
VLINES *edBuf1=NULL; //Buffer vector of lines(1022 chars)
VLINES tempLine;

FILE   *filePtr=NULL;
int     posBufY=0, posBufX=0; //position in the Edit buffer
int     rows = 0, columns = 0, old_rows = 0, old_columns = 0;	// Terminal dimensions
long    cursorX = START_CURSOR_X, cursorY = START_CURSOR_Y; //Cursor position
long    oldX = START_CURSOR_X, oldY = START_CURSOR_Y; //Cursor position blink
int     timerCursor = 0;	// Timer
char    kglobal = 0;		// Global variable for menu animation
char    currentFile[MAX_TEXT];
char    currentPath[MAX_PATH];
long    limitRow = 0, limitCol = 0;	// These variables account for the current limits of the buffer.
int     c_animation = 0;	//Counter for animation 
long    maxLineFile = 1;        //Last line of file
//FLAGS
int     exitp = 0;		// Exit flag for main loop
int     fileModified = FILE_UNMODIFIED;	//Have we modified the buffer?
char    timerOnOFF = 1;
int	forceBufferUpdate = 0; //To allow a smoother scroll animation.
NTIMER  _timer1; //Timer for animations and screen update
BOOL    insertMode = FALSE;

//FILE SCROLL POINTERS
long linesinFile =0;
int  hdisplayLength  =0; //horizontal scroll
int  currentColumn = 0; //horizontal scroll
int  displayLength = 0; //vertical scroll
long scrollLimit = 0; //vertical scroll
long currentLine = 0; //verticall scroll
int scrollActive = 0; //vertical scroll

/*====================================================================*/
/* PROTOTYPES OF FUNCTIONS                                            */
/*====================================================================*/

//Display prototypes
void    credits();
int     main_screen();
int     refresh_screen(int force_refresh);
void    flush_editarea();
void    cleanStatusBar();

//Timers
void    draw_cursor(long whereX, long whereY, long oldX, long oldY);
//int     timer_1(int *timer1, long whereX, long whereY, char onOff);
int     _tick();
void    update_indicators();

//Dialogs & menus
void    filemenu();
void    optionsmenu();
void    helpmenu();
int     confirmation();
int     about_info();
int     help_info();
void    drop_down(char *kglobal);
char    horizontal_menu();
int     newDialog(char fileName[MAX_TEXT]);
int     saveDialog(char fileName[MAX_TEXT]);
int     saveasDialog(char fileName[MAX_TEXT]);
int     openFileHandler();
int     fileInfoDialog();

//Keyhandling and input prototypes
int     process_input(long *whereX,
		      long *whereY, long *oldX, long *oldY, char ch);
int     special_keys(long *whereX, long *whereY, char ch);

//Edit prototypes
void    update_indicators();
int     findEndline(VLINES line);
BOOL    isLineTerminated(VLINES line);
void    linetoScreen(long whereY, VLINES tempLine, BOOL clean);
void    linetoScreenRAW(long whereY, VLINES tempLine, BOOL clean);
void    buffertoScreen(BOOL raw, long activeline);
void    cleanScreenLine(long whereY, BOOL raw);

//new
int handleopenFile(FILE ** filePtr, char *fileName, char *oldFileName);
void filetoDisplay(FILE *filePtr, int scupdate);
int check_arguments(int argc, char *argv[]);
long checkScrollValues();

/*====================================================================*/
/* MAIN PROGRAM - CODE                                                */
/*====================================================================*/

int main(int argc, char *argv[]) {
  char    ch = 0, oldchar = 0;
  int     esc_key = 0;		//To control key input and scan for keycodes.
  int     keypressed = 0;

  /*------------------------INITIAL VALUES----------------------------*/
  hidecursor();
  pushTerm();			//Save current terminal settings in failsafe
  create_screen();		//Create screen buffer to control display
  _timer1.ms = 30;  // Timer 1 - Clock & animation
  _timer1.ticks = 0; 
  main_screen();		//Draw screen
  update_screen();
  save_buffer();
  memset(&tempLine, '\0',sizeof(tempLine)); //Clear memory for temporary line
  resetch();			//Clear keyboard and sets ENTER = 13
  /*------------------------------------------------------------------*/
  
  
  /*------------------------CHECK ARGUMENTS----------------------------*/
  /*  

  */
  /*------------------------------------------------------------------*/
  
  /*------------------------MAIN PROGRAM LOOP-------------------------*/
  do {
    /* CURSOR is drawn in RAW MODE, i.e, directly to screen */
    draw_cursor(cursorX, cursorY, oldX, oldY);
    /* Query if screen dimensions have changed and if so, resize screen */
    refresh_screen(0);
    /* Timer for animation to show system time and update screen */
    if (timerC(&_timer1) == 1) { 
        _tick();
        if (screenChanged()){ 
		update_smart();
        	save_buffer();
	}
    }
   /* Wait for key_pressed to read key */
   keypressed = kbhit();
   if(keypressed == 1) {
      keypressed = 0;
      /* Process SPECIAL KEYS and other ESC-related issues */
      esc_key = special_keys(&cursorX, &cursorY, ch);

      //If arrow keys are used repeatedly. This avoids printing unwanted chars.   
      if(oldchar == K_ESCAPE)
	esc_key = 1;

      oldchar = ch;

      ch = readch();

      /* EDIT */
      if(esc_key == 0) {
	//Process input and get rid of extra characters
	process_input(&cursorX, &cursorY, &oldX, &oldY, ch);	//Edit
	keypressed = 0;
      }
    } else {
      //Keypressed = 0 - Reset values
     // oldX = cursorX;
      //oldY = cursorY;
      //resetch();
      esc_key = 0;
      ch = 0;
      oldchar = 0;
    }
  } while(exitp != EXIT_FLAG);	//exit flag for the whole program
  /*------------------------------------------------------------------*/
  
  //CREDITS
  if(filePtr != NULL) {
    closeFile(filePtr);
  }
  credits();
  return 0;
}

/*====================================================================*/
/* FUNCTION DEFINITIONS                                               */
/*====================================================================*/

/*-----------------------------------------*/
/* Draw cursor on screen and animate it    */
/*-----------------------------------------*/

void draw_cursor(long whereX, long whereY, long oldX, long oldY) {
/* CURSOR is drawn directly to screen and not to buffer */
  char    currentChar = FILL_CHAR;
  char    specialChar;
  char    lastChar=0;
  int     cursorFcolor, cursorBcolor;
  //Cursor colors
  cursorFcolor = EDIT_FORECOLOR;
  cursorBcolor = EDITAREACOL;

  //Remember previous char if we change lines
   if (oldY != whereY){
     lastChar = getSingleChar(edBuf1,oldX-START_CURSOR_X,oldY-START_CURSOR_Y).ch;
       gotoxy(oldX, oldY);
       outputcolor(cursorFcolor, cursorBcolor);
       if (abs(lastChar) > 31){ //make sure only printable chars are considered
       //does the characer have accents?
       if (lastChar > 0) printf("%c",lastChar);
       else
         printf("%c%c", getSingleChar(edBuf1,oldX-START_CURSOR_X, oldY-START_CURSOR_Y).specialChar, lastChar);
       }
    }

  //Remember previous char if we move on the line
   if (oldX != whereX){
     if(posBufX <= findEndline(tempLine)) {
       if (oldX<whereX) {currentChar = tempLine.linea[posBufX-1].ch; // current char;
			specialChar = tempLine.linea[posBufX-1].specialChar;
			}
       if (oldX>whereX) {currentChar = tempLine.linea[posBufX+1].ch; // current char;
			specialChar = tempLine.linea[posBufX+1].specialChar;
			}
     } else {
      currentChar = FILL_CHAR;
     }
   
   gotoxy(oldX, oldY);
   outputcolor(cursorFcolor, cursorBcolor);
  //Is it an accent or special char?
   if(currentChar < 0) {
      //specialChar = FILL_CHAR;
      printf("%c%c", specialChar,
	     currentChar);
    } else {
     //Careful with null char or enter or tab chars
     if(currentChar == 0 || currentChar == 10 || currentChar == 9) currentChar=FILL_CHAR;
      printf("%c", currentChar);
    }
   }

   
  if (insertMode == FALSE) 
	{ cursorBcolor = EDITAREACOL; cursorFcolor = EDIT_FORECOLOR;}
  else
	{ cursorBcolor = B_MAGENTA; cursorFcolor = F_WHITE;}
 
   //draw new cursor
    gotoxy(whereX, whereY);
    outputcolor(cursorFcolor, cursorBcolor);
    printf("|");
    //Is the cursor at the end or in the middle of text?
    if(posBufX < findEndline(tempLine)) {
      currentChar = tempLine.linea[posBufX].ch;
    } else {
      currentChar = FILL_CHAR;
    }
   //Display character in yellow.
    gotoxy(whereX, whereY);
    if (insertMode == FALSE) cursorFcolor = F_YELLOW;
    outputcolor(cursorFcolor, cursorBcolor);
    //Is it an accent or special char?
    specialChar = tempLine.linea[posBufX].specialChar;
    //currentChar = tempLine.linea[posBufX].ch;
    if(currentChar < 0) {
      printf("%c%c", specialChar,currentChar);
    } else {
     //Careful with null char or enter
     if(currentChar == '\0' || currentChar == 10 || currentChar == 32) {cursorBcolor = EDITAREACOL; currentChar=FILL_CHAR;}
      outputcolor(cursorFcolor, cursorBcolor);
      printf("%c", currentChar);
    }
    resetAnsi(0);  
}

/*-----------------------------------------*/
/* Timer 1 Animation. Clock and cursor     */
/*-----------------------------------------*/

int _tick() {
/* Timer for animations - Display time and clean cursor */
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
  char    temp[4];

  temp[0] = '[';
  temp[1] = ANIMATION[c_animation];
  temp[2] = ']';
  temp[3] = '\0';
  //save_buffer();
    time_str[strlen(time_str) - 1] = '\0';
    //display system time
    write_str(columns - strlen(time_str), 1, time_str, MENU_PANEL,
	      MENU_FOREGROUND0);
    write_str(columns - strlen(time_str) - 5, 1, temp, MENU_PANEL,
	      MENU_FOREGROUND0);
    //update only the screen bits that change 
    c_animation++;  //star animation
    update_indicators(); //current buffer pointer position

   if(c_animation > 6)
      c_animation = 0;
    return 0;
}

/*----------------------------------------- */
/* Update buffer position display on screen */
/*----------------------------------------- */

void update_indicators() {
//Buffer pointer position
  write_str(columns - 24, rows, "| L:        C:     ", STATUSBAR, STATUSMSG);
  write_num(columns - 10, rows, posBufX, 3, STATUSBAR, STATUSMSG);
  write_num(columns - 20, rows, posBufY, 4, STATUSBAR, STATUSMSG);
}


/*-----------------*/
/* Refresh screen  */
/*-----------------*/

int refresh_screen(int force) {
/* Query terminal dimensions again and check if resize 
   has been produced */
  get_terminal_dimensions(&rows, &columns);
  if (columns <55 && rows <17){
     //free_buffer();		//delete structure from memory 
     resetAnsi(0);
     gotoxy(1,1);
     printf("%s", WINDOWMSG);
     timerOnOFF=0;
  }
  else{
  if(rows != old_rows || columns != old_columns || force == 1 || force == -1) {
      if(force != -1) {
        timerOnOFF=1;
        free_buffer();		//delete structure from memory for resize
        create_screen();		//create new structure 
        main_screen();		//Refresh screen in case of resize
  	if (edBuf1 != NULL) buffertoScreen(FALSE, posBufY);
        update_screen();
       } else{
      //only update edit area to avoid flickering effect
       flush_editarea();
       if (edBuf1 != NULL) buffertoScreen(FALSE, posBufY);
       update_screen();
       return 1;
       }
  } else {
    return 0;
  }
  }
return 0;
}


/*--------------*/
/* EDIT Section */
/*------------- */
int findEndline(VLINES line) {
  char    ch = 0;
  int     i=0;
  int     result = 0;

  do {
    ch = line.linea[i].ch;
    //write_ch(i,1,ch,F_RED,B_WHITE);
    if(ch == CHAR_NIL || ch == END_LINE_CHAR)
      break;
    i++;
  } while(i < MAX_LINE_SIZE);
  result = i;
  ch = 0;
  return result;
}

BOOL isLineTerminated(VLINES line) {
  BOOL flag=FALSE;
  char ch=0;
  int i = 0;
  for (i=0; i< MAX_LINE_SIZE; i++){
    ch = line.linea[i].ch;
    if(ch == END_LINE_CHAR)
      flag = TRUE;
  }
  return flag;
}


void linetoScreen(long whereY, VLINES tempLine, BOOL clean){
//dump temporary Line to screen buffer
   int i,j=0;
   for (i=0; i<findEndline(tempLine); i++){
   	if(tempLine.linea[i].specialChar != 0) {
	  //Special char ? print the two values to screen buffer.
	  write_ch(i+START_CURSOR_X, whereY,  tempLine.linea[i].specialChar, EDITAREACOL,
		   EDIT_FORECOLOR);
	  write_ch(i+START_CURSOR_X, whereY,  tempLine.linea[i].ch, EDITAREACOL,   EDIT_FORECOLOR);
	} else {
	  write_ch(i+START_CURSOR_X, whereY, tempLine.linea[i].ch, EDITAREACOL,
		   EDIT_FORECOLOR);
	}
	if (clean == TRUE) 
	  for (j=findEndline(tempLine); j<columns -2; j++)
		write_ch(j+START_CURSOR_X, whereY, FILL_CHAR, EDITAREACOL, EDIT_FORECOLOR); 	  
	}
	write_ch(findEndline(tempLine)+START_CURSOR_X, whereY, '|', B_GREEN,
		   F_WHITE);
}	

void linetoScreenRAW(long whereY, VLINES tempLine, BOOL clean){
//dump temporary Line to screen buffer - RAW MODE
   int i,j=0;

   for (i=0; i<findEndline(tempLine); i++){
   	if(tempLine.linea[i].specialChar != 0) {
	  //Special char ? print the two values to screen buffer.
          gotoxy(i+START_CURSOR_X, whereY);
          outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          printf("%c%c", tempLine.linea[i].specialChar,tempLine.linea[i].ch);
	} else {
	  gotoxy(i+START_CURSOR_X, whereY);
          outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          printf("%c", tempLine.linea[i].ch);
	}
	if (clean == TRUE) 
	  for (j=findEndline(tempLine); j<columns -2; j++){
	 	gotoxy(j+START_CURSOR_X, whereY);
         	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	 }
	}
}
void cleanScreenLine(long whereY, BOOL raw)
{
   int i=0;
   for (i=0; i<columns-2; i++){
	write_ch(i+START_CURSOR_X, whereY,  FILL_CHAR, EDITAREACOL,
		   EDIT_FORECOLOR);
   if (raw == TRUE) {
	 	gotoxy(i+START_CURSOR_X, whereY);
         	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	}
   }
}


void buffertoScreen(BOOL raw, long activeline){
   long j=0;
   for (j=0; j<length(&edBuf1); j++){
	  dumpLine(edBuf1, j , &tempLine);
 	  linetoScreen(j+START_CURSOR_Y, tempLine,FALSE);
	  if (raw == TRUE) linetoScreenRAW(j+START_CURSOR_Y, tempLine,FALSE);
	}
    dumpLine(edBuf1, activeline, &tempLine);
    if (raw == FALSE) update_screen();
}

int process_input(long *whereX,
		  long *whereY, long *oldX, long *oldY, char ch) {
/* EDIT FUNCTIONS */
  char     accentchar[2];
  int i,j=0;
  VLINES   *aux = NULL;
  VLINES splitLine;
  *oldX = *whereX;
  *oldY = *whereY;
  if(ch != K_ESCAPE) {

    //Calculate position values 
   //Calculate position values 
    //limitCol = findEndline(editBuffer, editScroll.bufferY-1);
    
    //positionX = editScroll.bufferX-1;
    //positionY = editScroll.bufferY-1; 
    accentchar[0] = 0;
    accentchar[1] = 0;
  
    /* ---------------------------------------- */
    /* 
       READ CHARS WITH AND WITHOUT ACCENTS.
       Accent value is stored in the
       specialChar field of every item in the 
       buffer.
       Example:
       á : specialChar  = -61
       Char = -95
       a : specialChar = 0
       Char = 97                
     */

    /* ---------------------------------------- */
    //Check whether we are on Readmode
   if (fileModified != FILE_READMODE) {
    if((ch > 31 && ch < 127) || ch < 0) {
      //if a char has been read.
      read_accent(&ch, accentchar);

      //CHAR TO EDIT BUFFER
      //First we check if line exists in buffer
      aux = getObject(edBuf1, posBufY);
      if (aux == NULL) {
	//if not we create a new line in buffer
	tempLine.index = posBufY;
        tempLine.linea[posBufX].ch = accentchar[1];
        tempLine.linea[posBufX].specialChar = accentchar[0];
        tempLine.linea[posBufX].attrib = 21;
	//add end_line_char to line
        posBufX = posBufX + 1;
	tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = 0;
	edBuf1 = addatend(edBuf1, newline(tempLine));
     } else
	{
	if (posBufX == findEndline(tempLine)){
	//Insert at the end of the line
	//else we update the existing line
        tempLine.index = posBufY; //with every line index is incremented
        tempLine.linea[posBufX].ch = accentchar[1];
        tempLine.linea[posBufX].specialChar = accentchar[0];
        tempLine.linea[posBufX].attrib = 21;
	//add end_line_char to line
        posBufX = posBufX + 1;
	tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = 0;
 	updateLine(edBuf1, posBufY, &tempLine);
	//linetoScreen(*whereY,tempLine,1);	
	} else{
	   //insert characters in the middle
	   tempLine.index = posBufY; //with every line index is incremented
	   if (insertMode == FALSE){
	   for (i=findEndline(tempLine); i>=posBufX; i--){ 
               tempLine.linea[i+1].ch = tempLine.linea[i].ch;
               tempLine.linea[posBufX].specialChar = accentchar[0];
               tempLine.linea[posBufX].attrib = 21;
              }
	    }
             tempLine.linea[posBufX].ch = accentchar[1];
             tempLine.linea[posBufX].specialChar = accentchar[0];
             tempLine.linea[posBufX].attrib = 21;
              posBufX = posBufX + 1;
             updateLine(edBuf1, posBufY, &tempLine); 
	     linetoScreenRAW(*whereY,tempLine,FALSE);	
	}    
       }

      //ADD SPACES IF CURSOR IS NOT AT THE END OF THE LINE
      if(posBufX > findEndline(tempLine)) {
	for(i = findEndline(tempLine); i < posBufX-1; i++) {
	  tempLine.linea[i].ch = FILL_CHAR;
 	  updateLine(edBuf1, posBufY, &tempLine);    
	}
	  //linetoScreen(*whereY,tempLine,1);	
      }
     
      //CHAR TO SCREEN BUFFER - PRINT SINGLE CHAR
      //linetoScreen(*whereY,tempLine,1);	

	//Display limit (columns)
        
	if (*whereX<columns -1) *whereX = *whereX + 1;
      }

      fileModified = FILE_MODIFIED;
    }

    if (ch == K_ENTER){
      //Add line to buffer

      //Display limit rows
      if (*whereY<rows - 2) *whereY = *whereY +1;
      *whereX = START_CURSOR_X;

      //If buffer position pointer is at the end create a new empty line
      if (length(&edBuf1) <= posBufY){
        //new line without chars 
        memset(&tempLine, '\0',sizeof(tempLine));
 	if (length(&edBuf1)>0) tempLine.index = length(&edBuf1);
	else tempLine.index = posBufY;
        tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = 0;
	edBuf1 = addatend(edBuf1, newline(tempLine));
      } else {
      //Check if there are lines below and if so, move them.
      if  (posBufX <= findEndline(tempLine) || findEndline(tempLine) == 0 || posBufX > findEndline(tempLine)) 
	//Move lines algorithm
	{
	   //Create a new line to make room for the move
           memset(&tempLine, '\0',sizeof(tempLine));
	   tempLine.index = length(&edBuf1);
	   edBuf1 = addatend(edBuf1, newline(tempLine));
	   //Move lines [j -> j+1] from bottom up until 1 line before active line
	   for (j=length(&edBuf1); j>posBufY; j--)
		{
		     dumpLine(edBuf1, j, &tempLine);
    	             updateLine(edBuf1, j+1, &tempLine);
		     cleanScreenLine(j+START_CURSOR_Y, TRUE);
	   	}
             memset(&splitLine, '\0',sizeof(splitLine));
             dumpLine(edBuf1, posBufY, &tempLine);
	     //Equate pointers to avoid printing unnecessary cursor chars
	     *oldY = *whereY;
	     *oldX = *whereX;
	     //Clear previous line on the display buffer
	     cleanScreenLine(*whereY-1,TRUE);
	     //Where are we on current line? -> Shall we move part of it?
            if (posBufX < findEndline(tempLine)){
 	     //Split (1) and move(2) chars after posBufx to next line
	     //(1) Split and Update current line with chars that remain
	     for (i=0; i<posBufX; i++){
		  splitLine.linea[i].ch = tempLine.linea[i].ch;
		  splitLine.linea[i].specialChar = tempLine.linea[i].specialChar;
		  splitLine.linea[i].attrib = tempLine.linea[i].attrib;
	     }
	     splitLine.linea[i].ch = END_LINE_CHAR;
	     updateLine(edBuf1, posBufY, &splitLine);
	     j=0;
	     //(2) Move chars after posBufX to next line
             memset(&splitLine, '\0',sizeof(splitLine));
 	     for (i=posBufX; i<=findEndline(tempLine); i++){
		   //j+1 line
		  splitLine.linea[j].ch = tempLine.linea[i].ch;
		  splitLine.linea[j].specialChar = tempLine.linea[i].specialChar;
		  splitLine.linea[j].attrib = tempLine.linea[i].attrib;
	   	  j++;
  	      }
	   } else{
	      //Nothing to move in current line, because we are at the end; 
	      //We make sure to end it with 0x10 if the line is completely empty.
		if (isLineTerminated(splitLine) == FALSE) splitLine.linea[findEndline(splitLine)].ch = END_LINE_CHAR;
	   } 
	   //Update [J+1] line and write changes to screen
	   updateLine(edBuf1, posBufY+1, &splitLine);
           buffertoScreen(TRUE, posBufY+1);
	} else
	{
	  //No lines below posBufY to move -> clean next line
	  memset(&tempLine, '\0',sizeof(tempLine)); 
	}
      }
      //Move buffer pointer positions
      posBufY = posBufY + 1;
      posBufX = 0;
    }

    if(ch == K_BACKSPACE) {
      //BACKSPACE key
      //write_ch(*whereX, *whereY, ' ', EDITAREACOL, EDITAREACOL);
      tempLine.linea[posBufX-1].ch = END_LINE_CHAR;
      tempLine.linea[posBufX-1].specialChar = CHAR_NIL;
      tempLine.linea[posBufX-1].attrib = 0;

     //Remove line if we continue pressing backspace
      if(*whereX == START_CURSOR_X && *whereY > START_CURSOR_Y) {
 	deleteObject(&edBuf1, posBufY, FALSE); //remove line from buffer
        posBufY--;
	dumpLine(edBuf1, posBufY,&tempLine);
	posBufX = findEndline(tempLine)+1;
	*whereY = *whereY - 1;
	*whereX = findEndline(tempLine)+1 + START_CURSOR_X;
        //Update oldX/oldY To avoid cursor updating a leftout character
	*oldY = *whereY;
	*oldX = *whereX;
	//dump previous line into temporary buffer
        updateLine(edBuf1, posBufY, &tempLine);    
        linetoScreen(*whereY,tempLine,1);	
      }
      if(*whereX > START_CURSOR_X){
        *whereX = *whereX - 1;
        posBufX--;
        updateLine(edBuf1, posBufY, &tempLine);    
        linetoScreen(*whereY,tempLine,1);	
      }
    }

    if(ch == K_TAB) {
      //TAB key
     }
   
    if(ch == K_CTRL_L) {
      //Akin to F2
      buffertoScreen(FALSE,posBufY);
      if(horizontal_menu() == K_ESCAPE) {
	//Exit horizontal menu with ESC 3x
	kglobal = K_ESCAPE;
	//main_screen();
      }
      /*  Drop-down menu loop */
      drop_down(&kglobal);	//animation
    }
 
    if(ch == K_CTRL_C) {
      //Ask for confirmations CTRL-C -> Exit
      exitp = confirmation();	//Shall we exit? Global variable! 
    }
    if(ch == K_CTRL_H) {
      //Change color Scheme
      setColorScheme(colorsWindow(mylist, COLORSWTITLE));
      refresh_screen(1);
    }
  }
  return 0;
}

/*-----------------------------------------*/
/* Manage keys that send a ESC sequence    */
/*-----------------------------------------*/

int special_keys(long *whereX, long *whereY, char ch) {
/* MANAGE SPECIAL KEYS */
/* 
   New implementation: Trail of chars found in keyboard.c
   If K_ESCAPE is captured read a trail up to 5 characters from the console.
   This is to control the fact that some keys may change
   according to the terminal and expand the editor's possibilities.
   Eg: F2 can be either 27 79 81 or 27 91 91 82.  
*/
  int     esc_key = 0;
  char    chartrail[5];
  if(ch == K_ESCAPE) {
    read_keytrail(chartrail);	//Read trail after ESC key

    //Check key trails for special keys.

    //FUNCTION KEYS : F1 - F4
    if(strcmp(chartrail, K_F2_TRAIL) == 0 ||
       strcmp(chartrail, K_F2_TRAIL2) == 0) {
      //update screen
      buffertoScreen(FALSE,posBufY);
      if(horizontal_menu() == K_ESCAPE) {
	//Exit horizontal menu with ESC 3x
	kglobal = K_ESCAPE;
	//main_screen();
      }
      //  Drop-down menu loop */       
      drop_down(&kglobal);	//animation  
    } else if(strcmp(chartrail, K_F3_TRAIL) == 0 ||
	      strcmp(chartrail, K_F3_TRAIL2) == 0) {
      refresh_screen(1);
      buffertoScreen(TRUE,posBufY);
    } else if(strcmp(chartrail, K_F1_TRAIL) == 0 ||
	      strcmp(chartrail, K_F1_TRAIL2) == 0) {
      help_info();
    } else if(strcmp(chartrail, K_F4_TRAIL) == 0 ||
	      strcmp(chartrail, K_F4_TRAIL2) == 0) {
      about_info();
      // ARROW KEYS  
    } else if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      //Left-arrow key  
      if(*whereX > 2){
	*whereX = *whereX - 1;
        posBufX--;
      }
    } else if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      //Right-arrow key  
      if(*whereX < columns - 1){
	*whereX = *whereX + 1;
        posBufX++; 
      }
    } else if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      //Up-arrow key  
      if(*whereY > 3) {
	*whereY = *whereY - 1;
        posBufY--;
	dumpLine(edBuf1, posBufY, &tempLine);
      }
    } else if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
      //Down-arrow key  
      //if(*whereY < rows - 2 && editScroll.totalLines > *whereY - START_CURSOR_Y+1) {
	if (posBufY<length(&edBuf1) && *whereY < rows - 2){
	  *whereY = *whereY + 1;
          posBufY++;
	  dumpLine(edBuf1, posBufY, &tempLine);
	}	
    } else if(strcmp(chartrail, K_ALT_F) == 0) {
      data.index=FILE_MENU;
      drop_down(&kglobal);	//animation  
    } else if(strcmp(chartrail, K_ALT_P) == 0) {
      data.index=OPT_MENU;
      drop_down(&kglobal);	//animation  
    } else if(strcmp(chartrail, K_ALT_H) == 0) {
      data.index=HELP_MENU;
      drop_down(&kglobal);	//animation  
    } else if(strcmp(chartrail, K_ALT_O) == 0) {
      //openFileHandler();	//Open file Dialog
    } else if(strcmp(chartrail, K_ALT_N) == 0) {
      //newDialog(currentFile);	// New file
      refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_A) == 0) {
      //saveasDialog(currentFile);	//Save as.. file
      refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_D) == 0) {
      fileInfoDialog();		//Info file
    } else if(strcmp(chartrail, K_INSERT) == 0) {
      if (insertMode == FALSE) insertMode = TRUE;
      else insertMode = FALSE;		//TOGGLE INSERT MODE
    } else if(strcmp(chartrail, K_ALT_X) == 0) {
      if(fileModified == 1)
	exitp = confirmation();	//Shall we exit? Global variable!
      else
	exitp = EXIT_FLAG;
    }
     esc_key = 1;
  } else {
    //Reset esc_key
    esc_key = 0;
  }
  return esc_key;
}
/*-------------------*/
/* Draw main screen  */
/*-------------------*/

int main_screen() {
  int     i;

  //Save previous values  
  get_terminal_dimensions(&rows, &columns);
  old_rows = rows;
  old_columns = columns;
  //Failsafe just in case it can't find the terminal dimensions
  if(rows == 0)
    rows = ROWS_FAILSAFE;
  if(columns == 0)
    columns = COLUMNS_FAILSAFE;
  screen_color(EDITAREACOL);
  //Draw upper and lower bars
  for(i = 1; i <= columns; i++) {
    write_ch(i, 1, FILL_CHAR, MENU_PANEL, MENU_PANEL);
  }

  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, STATUSBAR, STATUSMSG);
  }
  // Text messages
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);

  /* Frames */
  //window appearance and scroll bar
  for(i = 2; i < rows; i++) {
    write_ch(columns, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE);	//Scroll bar
    write_ch(1, i, NVER_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);	//upper vertical line box-like char 
  }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, NHOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);	//horizontal line box-like char
    write_ch(i, rows - 1, ' ', EDITWINDOW_BACK, EDITWINDOW_FORE);
  }
  write_ch(1, 2, NUPPER_LEFT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE);	//upper-left box-like char
  //horizontal scroll bar
  for(i = 2; i < columns; i++) {
    write_ch(i, rows - 1, FILL_CHAR, SCROLLBAR_BACK, SCROLLBAR_FORE);
  }
  //Window-appearance
  write_ch(columns, 2, NUPPER_RIGHT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE);	//right window corner
  write_ch(columns, rows - 1, NLOWER_RIGHT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE);
  write_ch(1, rows - 1, NLOWER_LEFT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE);

  //Scroll symbols
  write_ch(columns, 3, '^', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns, rows - 2, 'v', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns, 4, '*', SCROLLBAR_SEL, SCROLLBAR_FORE);
  write_ch(3, rows - 1, '*', SCROLLBAR_SEL, SCROLLBAR_FORE);
  write_ch(2, rows - 1, '<', SCROLLBAR_ARR, SCROLLBAR_FORE);
  write_ch(columns - 1, rows - 1, '>', SCROLLBAR_ARR, SCROLLBAR_FORE);

  //checkScrollValues(editScroll.totalLines); //make calculations for scroll.
  //Write editBuffer
  //writeBuffertoDisplay(editBuffer);
  return 0;
}

/*-------------------------*/
/* Only refresh edit Area  */
/*-------------------------*/

void flush_editarea() {
int i,j;
  //Paint blue edit area
  for(j = START_CURSOR_Y; j < rows - 1; j++)
    for(i = START_CURSOR_X; i < columns - 1; i++)
    {
      gotoxy(i,j);
      outputcolor(EDITAREACOL,EDITAREACOL);
      printf("%c",FILL_CHAR);
      write_ch(i,j,FILL_CHAR,EDITAREACOL,EDITWINDOW_FORE);
     }
  for(i = 2; i < columns; i++) {
    write_ch(i, 2, NHOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE);	//horizontal line box-like char
  }
  //Center and diplay file name
  //write_str((columns / 2) - (strlen(currentFile) / 2), 2, currentFile,
//	    MENU_PANEL, MENU_FOREGROUND0);
  update_screen();
}
 
/*-------------------*/
/* Clean Status Bar  */
/*-------------------*/

 
//Clean Status bar
void cleanStatusBar(){
  int i;
  for(i = 1; i < columns; i++) {
    write_ch(i, rows, FILL_CHAR, STATUSBAR, STATUSMSG);
  }
}

/* --------------------------------------*/
//FILE OPERATIONS
/* --------------------------------------*/
int handleopenFile(FILE ** filePtr, char *fileName, char *oldFileName) {
  long    checkF = 0;
  long    linesinFile =0;
  int     ok = 0;
 currentLine = 0;
  currentColumn=0;
  scrollLimit = 0;

  openFile(filePtr, fileName, "r");
  //Check for binary characters to determine filetype
  checkF = checkFile(*filePtr);
  if(checkF > 5) {
    //File doesn't seem to be a text file. Open anyway?  
    ok = yesnoWindow(mylist, WCHECKFILE_MSG, OPENANYWAYWTITLE);
    if(ok == 1) {
     //filetoBuffer(*filePtr, editBuffer);
      refresh_screen(-1);
      //fileModified = FILE_READMODE; // Open as readmode
    } else {
      //Go back to previous file
      ok = -1;
      clearString(fileName, MAX_TEXT);
      strcpy(fileName, oldFileName);
      //Open file again and dump first page to buffer - temporary
      closeFile(*filePtr);
      openFile(filePtr, currentFile, "r");
      //filetoBuffer(*filePtr, editBuffer);
    }
  } else {
    //Check wheter file is bigger than buffer. 32700 lines
    linesinFile = countLinesFile(*filePtr);
    if (linesinFile > MAX_LINES) 
    ok=infoWindow(mylist, WCHECKLINES_MSG, FILETOBIGWTITLE);
    //Reset values
    //editScroll.scrollPointer = 0; //Set pointer to 0
    //editScroll.pagePointer =0; //set page Pointer to 0
    checkScrollValues(linesinFile); //make calculations for scroll.
    //editScroll.bufferX = 1;
    //editScroll.bufferY = 1;
    cursorX=START_CURSOR_X;
    cursorY=START_CURSOR_Y;
    //Dump file into edit buffer.
    //filetoBuffer(*filePtr, editBuffer);
    refresh_screen(-1);
  }
    //checkF = checkFile(*filePtr);
  
  if (linesinFile > displayLength) scrollActive = 1;
    else scrollActive = 0;

  return ok;
}
//this routine copies file content to screen buffer so that windows and dialogs
//can be displayed and the content they cover can be later retrieved
void filetoDisplay(FILE *filePtr, int scupdate){
  long     lineCounter = 0, i=1, whereinfile=0;
  double progress=0;
  int k=0;
  int scW = columns; 
  int scH = rows;
  int wherex = 0;
  char    ch;
  time_t  mytime = time(NULL);
  char   *time_str = ctime(&mytime);
//Update screen buffer 

if (filePtr != NULL) {
    rewind(filePtr);		//Make sure we are at the beginning
    //whereinfile=gotoLine(filePtr,currentLine);
    if (whereinfile>1) fseek(filePtr, whereinfile, 0);
    while (!feof(filePtr)) {
	  ch = getc(filePtr);	
	  wherex = labs(i-currentColumn);
          if (ch != END_LINE_CHAR && ch != '\0') {
		if (ch==9){ //for convenience TAB char is shown in green
			 //with horizontal scroll
			 if (i> currentColumn) write_ch(wherex,lineCounter+4,'>',BH_GREEN,F_WHITE);
			 i++;
		} else if (ch==13){
		//windows 0x0d is transform to 0x20 
		  ch=32;
		}
 		else{
		  if (i> currentColumn) write_ch(wherex,lineCounter+4,ch,BH_BLUE,F_WHITE);
		  i++;
		}
	  }
	  if(ch == END_LINE_CHAR) { 
		//next line
	    for (k=i; k<scW; k++){
		write_ch(k,lineCounter+4,' ',BH_BLUE,F_BLUE);
		}
	    lineCounter++;
	    i=1;
	  }
	//break when it reaches end of vertical displaying area
	  if (lineCounter > scH-6) break;
    }
    //to delete the last 0x0A character on screen
}
}

int check_arguments(int argc, char *argv[]){
int ok=0;
 if(argc > 1) {
    //Does the file exist? Open or create?
    if(file_exists(argv[1]) == 1) {
      //open
      clearString(currentFile, MAX_TEXT);
      strcpy(currentFile, argv[1]);
      ok = handleopenFile(&filePtr, currentFile, UNKNOWN);
    } else {
      //create
      clearString(currentFile, MAX_TEXT);
      strcpy(currentFile, argv[1]);
      //createnewFile(&filePtr, currentFile, 0);
    }
   }
   return ok;
}
long checkScrollValues(){
	return (linesinFile - displayLength);
}
/*--------------------------*/
/* Display horizontal menu  */
/*--------------------------*/

char horizontal_menu() {
  char    temp_char;
  kglobal=-1;
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG3, STATUSBAR, STATUSMSG);
  loadmenus(mylist, HOR_MENU);
  temp_char = start_hmenu(&data);
  free_list(mylist);
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  update_screen();
  return temp_char;
}

/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

void filemenu() {  
  cleanStatusBar();
  data.index = OPTION_NIL;
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, FILE_MENU);
  write_str(1, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(1, 2, 13, 8, MENU_PANEL, MENU_FOREGROUND0,0, 1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();
  free_list(mylist);

  if(data.index == OPTION_1) {
    //New file option
    //newDialog(currentFile);
    //Update new global file name
    refresh_screen(-1);
  }
  if(data.index == OPTION_2) {
    //External Module - Open file dialog.
    //openFileHandler();
    //flush_editarea();
  }
  if(data.index == OPTION_3) {
    //Save option
  }
  if(data.index == OPTION_4) {
    //Save as option  
      //saveasDialog(currentFile);
      refresh_screen(-1);
  }

  if(data.index == OPTION_5) {
    //Exit option
    if(fileModified == 1)
      exitp = confirmation();	//Shall we exit? Global variable!
    else
      exitp = EXIT_FLAG;
  }
  data.index = OPTION_NIL;
  cleanStatusBar();
   //Restore message in status bar
   write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

void optionsmenu() {
  int  setColor;
  data.index = OPTION_NIL;
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, OPT_MENU);
  write_str(7, 1, "Options", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(7, 2, 20, 6, MENU_PANEL, MENU_FOREGROUND0,0, 1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();

  free_list(mylist);
  if(data.index == OPTION_1) {
    //File Info
    //fileInfoDialog();
  }
  if(data.index == OPTION_3) {
    //Set Colors
    setColor = colorsWindow(mylist,COLORSWTITLE);
    setColorScheme(setColor);
    checkConfigFile(setColor);	//save new configuration in config file
    refresh_screen(1);
  }
  data.index = OPTION_NIL;
  //Restore message in status bar
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);

}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

void helpmenu() { 
  cleanStatusBar();
  data.index = OPTION_NIL;
  write_str(1, rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG);
  loadmenus(mylist, HELP_MENU);
  write_str(16, 1, "Help", MENU_SELECTOR, MENU_FOREGROUND1);
  draw_window(16, 2, 26, 5, MENU_PANEL, MENU_FOREGROUND0, 0,1,0);
  kglobal = start_vmenu(&data);
  close_window();
  write_str(1, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0);
  write_str(1, 1, "F", MENU_PANEL, F_RED);
  write_str(8, 1, "p", MENU_PANEL, F_RED);
  write_str(16, 1, "H", MENU_PANEL, F_RED);
  update_screen();
  free_list(mylist);
  if(data.index == OPTION_1) {
    //About info
    help_info();
  }
  if(data.index == OPTION_2) {
    //About info
    about_info();
  }
  data.index = -1;
  //Restore message in status bar
  cleanStatusBar();
  write_str(1, rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG);
}

/*-------------------------------*/
/* Display Confirmation Dialog   */
/*-------------------------------*/

/* Displays a window to asks user for confirmation */
int confirmation() {
  //int     ok = 0;
  exitp = EXIT_FLAG;
  return EXIT_FLAG;
}

/*--------------------------*/
/* Display About Dialog     */
/*--------------------------*/

int about_info() {
  int     ok = 0;
  char    msg[105];
  msg[0] = '\0';
  strcat(msg, ABOUT_ASC_1);
  strcat(msg, ABOUT_ASC_2);
  strcat(msg, ABOUT_ASC_3);
  strcat(msg, "\0");
  alertWindow(mylist, msg,ABOUTWTITLE);
  return ok;
}

/*--------------------------*/
/* Display About Dialog     */
/*--------------------------*/

int help_info() {
  int     ok = 0;
  char    msg[500];
 //writeBuffertoDisplay(editBuffer);
  msg[0] = '\0';
  strcat(msg, HELP1);		//located in user_inter.h
  strcat(msg, HELP2);		//located in user_inter.h
  strcat(msg, HELP3);		//located in user_inter.h
  strcat(msg, HELP4);		//located in user_inter.h
  strcat(msg, HELP5);		//located in user_inter.h
  strcat(msg, HELP6);		//located in user_inter.h
  strcat(msg, HELP7);		//located in user_inter.h
  strcat(msg, HELP8);		//located in user_inter.h
  strcat(msg, HELP9);		//located in user_inter.h
  strcat(msg, HELP10);		//located in user_inter.h
  strcat(msg, "\0");
  helpWindow(mylist, msg, HELPWTITLE);
  refresh_screen(0);
  return ok;
}

/*----------------------*/
/* Drop_Down Menu Loop  */
/*----------------------*/

void drop_down(char *kglobal) {
/* 
   Drop_down loop animation. 
   K_LEFTMENU/K_RIGHTMENU -1 is used when right/left arrow keys are used
   so as to break vertical menu and start the adjacent menu
   kglobal is changed by the menu functions.
*/
  do {
    if(*kglobal == K_ESCAPE) {
      //Exit drop-down menu with ESC           
      *kglobal = 0;
      break;
    }
    if(data.index == FILE_MENU) {
      filemenu();
      if(*kglobal == K_LEFTMENU) {
	data.index = OPT_MENU;
      }
      if(*kglobal == K_RIGHTMENU) {
	data.index = HELP_MENU;
      }
    }
    if(data.index == OPT_MENU) {
      optionsmenu();
      if(*kglobal == K_LEFTMENU) {
	data.index = HELP_MENU;
      }
      if(*kglobal == K_RIGHTMENU) {
	data.index = FILE_MENU;
      }
    }
    if(data.index == HELP_MENU) {
      helpmenu();
      if(*kglobal == K_LEFTMENU) {
	data.index = FILE_MENU;
      }
      if(*kglobal == K_RIGHTMENU) {
	data.index = OPT_MENU;
      }
    }
  } while(*kglobal != K_ENTER);
}

/*---------*/
/* Credits */
/*---------*/
void credits() {
/* Frees memory and displays goodbye message */
  //Free selected path item/path from opfiledialog 
  size_t i; //to be compatible with strlen
  char auth[27] ="Coded by v3l0r3k 2019-2021";  
  free_buffer();
  resetTerm();			//restore terminal settings from failsafe
  showcursor();
  resetAnsi(0);
  screencol(0);
  outputcolor(7, 0);
  printf(cedit_ascii_1);
  printf(cedit_ascii_2);
  printf(cedit_ascii_3);
  printf(cedit_ascii_4);
  printf(cedit_ascii_5);
  printf(cedit_ascii_6);

  outputcolor(0, 90);
  printf("\n%s",auth);
  outputcolor(0, 37);
  _timer1.ms=10;
  _timer1.ticks=0;
  i=0;

  outputcolor(0, 97);
  do{
    if (timerC(&_timer1) == 1) { 
       gotoxy(i,8);
       if (i==strlen(auth)) outputcolor(0,93);
       if (i<10 || i>16) 
	if (i<=strlen(auth)) printf("%c", auth[i-1]);
       printf("\n");
       i++;
     }
    } while (_timer1.ticks <30);

  printf("\n");
  outputcolor(7, 0);
  printlist(&edBuf1);
  printf("%d:%d\n", posBufX, posBufY);
  printf("\n%ld\n",sizeof(&edBuf1));
  deletetheList(&edBuf1); //free edit Buffer
}


int     fileInfoDialog(){
return 0;
}
