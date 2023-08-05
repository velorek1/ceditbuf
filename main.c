#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "listbox.h"
#include "scbuf.h"
#include "rterm.h"
#include "tm.h"
#include "global.h" 
#include "keyb.h"
#include "edbuf.h"

//Prototypes
void draw_screen();
void cursor_tick();
void editSection(char ac1, char ac2);
int process_input(char ch);
int special_keys();
void linetoScreen(long whereY, VLINES tempLine);
void cleanScreenLine(long whereY);
void cleanSection(long whereY, long start, int amount);
void buffertoScreen(long startPoint, long activeline);
wchar_t currentChar = 0;

void draw_screen(){
//BASE SCREEN IS STORED IN SCREEN 2
     int i=0;
     if (screen1 != NULL) deleteList(&screen1);
     if (screen2 != NULL) deleteList(&screen2);
     //Init 2 : Create 2 Screens for a double buffer  approach  
     old_rows=new_rows;
     old_columns=new_columns;
     create_screen(&screen1);
     create_screen(&screen2);
     //SCREEN 2
     screen_color(screen1, EDITAREACOL, EDITAREACOL, FILL_CHAR);
    //Failsafe just in case it can't find the terminal dimensions
    if(old_rows == 0)
      old_rows = ROWS_FAILSAFE;
    if(old_columns == 0)
    old_columns = COLUMNS_FAILSAFE;

  //Draw upper and lower bars
    for(i = 0; i < old_columns; i++) {
     write_ch(screen1, i, 1, FILL_CHAR, MENU_PANEL, MENU_PANEL,0);
    }

  for(i = 0; i < old_columns; i++) {
    write_ch(screen1, i, old_rows, FILL_CHAR, STATUSBAR, STATUSMSG,1);
  }
  // Text messages
  write_str(screen1, 0, 1, "File  Options  Help", MENU_PANEL, MENU_FOREGROUND0,0);
  write_str(screen1, 0, 1, "F", MENU_PANEL, F_RED,0);
  write_str(screen1, 7, 1, "p", MENU_PANEL, F_RED,0);
  write_str(screen1, 15, 1, "H", MENU_PANEL, F_RED,0);
  write_str(screen1, 0, old_rows, STATUS_BAR_MSG1, STATUSBAR, STATUSMSG,0);

  /* Frames */
  //window appearance and scroll bar
  for(i = 2; i < old_rows; i++) {
    write_ch(screen1,old_columns-1, i, ' ', SCROLLBAR_BACK, SCROLLBAR_FORE,0);	//Scroll bar
    write_ch(screen1,0, i, VER_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper vertical line box-like char 
  }
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, 2, HOR_LINE, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//horizontal line box-like char
    write_ch(screen1,i, old_rows - 1, ' ', EDITWINDOW_BACK, EDITWINDOW_FORE,0);
  }
  write_ch(screen1,0, 2, UPPER_LEFT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//upper-left box-like char
  //horizontal scroll bar
  for(i = 0; i < old_columns; i++) {
    write_ch(screen1,i, old_rows - 1, FILL_CHAR, SCROLLBAR_BACK, SCROLLBAR_FORE,0);
  }
  //Window-appearance
  write_ch(screen1,old_columns-1, 2, UPPER_RIGHT_CORNER, EDITWINDOW_BACK, EDITWINDOW_FORE,0);	//right window corner
  write_ch(screen1,old_columns-1, old_rows - 1, LOWER_RIGHT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);
  write_ch(screen1, 0, old_rows - 1, LOWER_LEFT_CORNER, EDITWINDOW_BACK,
	   EDITWINDOW_FORE,0);

  //Scroll symbols
  write_ch(screen1,old_columns-1, 3, '^', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, old_rows - 2, 'v', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns-1, 4, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 2, old_rows - 1, '*', SCROLLBAR_SEL, SCROLLBAR_FORE,0);
  write_ch(screen1, 1, old_rows - 1, '<', SCROLLBAR_ARR, SCROLLBAR_FORE,0);
  write_ch(screen1, old_columns - 2, old_rows - 1, '>', SCROLLBAR_ARR, SCROLLBAR_FORE,0);

  dump_screen(screen1);
  //Save screen for later
  copy_screen(screen2,screen1);
}

void cursor_tick(void){
//CURSOR ANIMATION
char drawChar=FILL_CHAR;
char drawChar0=FILL_CHAR;
int attrib = 0;
wchar_t code_point;
    
    //GET CHAR BEING POINTED AT FROM EDIT BUFFER
    if (edBuf1 != NULL)_dumpLine(edBuf1, posBufY, &tempLine);
    drawChar = tempLine.linea[posBufX].ch; 
    drawChar0 = tempLine.linea[posBufX].specialChar; 
    attrib =  tempLine.linea[posBufX].attrib;
    //CHECK FOR SPECIAL CHARACTERS AND CONVERT THEM TO UNICODE TO PRINT
    //IF CURSOR IS ON A CHAR HIGHLIGHT IT IN YELLOW
    if (drawChar0 != 0 && drawChar != '\0') code_point = ((drawChar0 & 0x1F) << 6) | (drawChar & 0x3F);
    else {
            if (drawChar == 0 || drawChar == END_LINE_CHAR) drawChar = FILL_CHAR;
	    code_point = drawChar;
    }
    if (cursor_timer1.ticks % 2 == 0) update_ch(cursorX, cursorY, ' ', B_WHITE, F_WHITE);
    else update_ch(cursorX, cursorY, code_point, EDITAREACOL, FH_YELLOW);
    
    //REST ANSI VALUES TO HAVE HIGH INTENSITY COLORS
    resetAnsi(0);

    //CLEAR LAST POSITION OF CURSOR WITH FILL_CHAR OR GO BACK TO PREVIOUS CHAR
    if ((old_cursorX != cursorX) || (old_cursorY != cursorY)){
	//drawChar = FILL_CHAR; 	
      //old?
      if (edBuf1 != NULL)_dumpLine(edBuf1, oldposBufY, &tempLine);
      drawChar = tempLine.linea[oldposBufX].ch; 
      drawChar0 = tempLine.linea[oldposBufX].specialChar; 
      attrib =  tempLine.linea[oldposBufX].attrib;
    if (drawChar0 != 0 && drawChar != '\0') code_point = ((drawChar0 & 0x1F) << 6) | (drawChar & 0x3F);
    else {
            if (drawChar == 0 || drawChar == END_LINE_CHAR) drawChar = FILL_CHAR;
	    code_point = drawChar;
    }
 
       update_ch(old_cursorX, old_cursorY, code_point, EDITAREACOL, attrib);
    } 
    if (old_cursorY != cursorY) {
         //Point to line in buffer when Y position changes
        if (edBuf1 != NULL) _dumpLine(edBuf1, posBufY, &tempLine);
    }
    //printf("\n");
    //write_num(screen1, 10,10, cursorX, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,11, cursorY, B_RED, F_WHITE,1);
    //write_num(screen1, 10,12, old_cursorX, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,12, old_cursorY, B_GREEN, F_WHITE,1);
    //write_num(screen1, 10,14, posBufX, B_RED, F_WHITE,1);
    //write_num(screen1, 10,15, posBufY, B_RED, F_WHITE,1);
    //write_num(screen1, 10,16, findEndline(tempLine), BH_BLUE, F_WHITE,1);
    //write_num(screen1, 10,17, _length(&edBuf1), B_RED, F_WHITE,1);
    //for (int i=0; i<findEndline(tempLine);i++) write_ch(screen1, 10+i,16, code_point, B_RED, F_WHITE,1);
    //dump_screen(screen1);
   //Buffer pointer position
    write_str(screen1, new_columns - 24, new_rows, "| R:        C:     ", STATUSBAR, STATUSMSG,1);
    write_num(screen1, new_columns - 10, new_rows, posBufX, STATUSBAR, STATUSMSG,1);
    write_num(screen1, new_columns - 20, new_rows, posBufY, STATUSBAR, STATUSMSG,1);
    write_str(screen1, new_columns - 39, new_rows, "| LINES:      ", STATUSBAR, STATUSMSG,1);
    write_num(screen1, new_columns - 31, new_rows, _length(&edBuf1), STATUSBAR, STATUSMSG,1);
}

int main(){

char ch=0;
int keypressed = 0;
int esc_key = 0;


    //Init Terminal
    init_term();
    initCEDIT();
    draw_screen();
    resetch();
    //tempLine.linea[0].ch = END_LINE_CHAR;
    //tempLine.linea[posBufX].ch = END_LINE_CHAR;
    //tempLine.linea[posBufX].specialChar = 0;
    //tempLine.linea[posBufX].attrib = EDIT_FORECOLOR; 
    do{    
	 //Time animation & resize window
	  if (timerC(&timer2) == TRUE){
           _animation();
	    //Refresh screen size buffers if terminal dimension changes
	    if (new_rows != old_rows || new_columns != old_columns)
            {
	      draw_screen();
            }
	  }
         //Cursor Animation	 
	 if (timerC(&cursor_timer1) == TRUE){
	      //Animation
             cursor_tick();
           }
	//PROCESS INPUT
    	 keypressed = kbhit(1);
	 if (keypressed == TRUE) {
	   ch=readch();
           if (ch == ESC_KEY) {esc_key = special_keys(); cursor_tick(); ch = 0;}       
   	   else {
		if (ch != 0) process_input(ch);
	   }
	 }
         else
          ch=0;
	 
	//Check for ESC-related keys
      } while (esc_key != ENDSIGNAL);     
     //free memory
     if (screen1 != NULL) deleteList(&screen1);
     if (screen2 != NULL) deleteList(&screen2);
     //restore terminal
     close_term();
  printf("\n");
  outputcolor(7, 0);
  _printlist(&edBuf1);
  printf("%ld:%ld\n", posBufX, posBufY);
  printf("\n%ld\n",sizeof(&edBuf1));
  _deletetheList(&edBuf1); //free edit Buffer
     
     return 0;
}

int special_keys() {
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
    old_cursorX = cursorX;
    old_cursorY = cursorY;
    oldposBufX = posBufX;
    oldposBufY = posBufY;


    strcpy(chartrail, "\0");
    read_keytrail(chartrail);   //Read trail after ESC key
    //only ESC key detected, finish program
    if (chartrail[0] == ESC_KEY && chartrail[1]==0) return ENDSIGNAL;
    
    //Check key trails for special keys starting with ESC.
    //FUNCTION KEYS : F1 - F4
    if(strcmp(chartrail, K_F2_TRAIL) == 0 ||
       strcmp(chartrail, K_F2_TRAIL2) == 0) {
       
      //  Drop-down menu loop 
      //drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_F3_TRAIL) == 0 ||
          strcmp(chartrail, K_F3_TRAIL2) == 0) {
    } else if(strcmp(chartrail, K_F1_TRAIL) == 0 ||
          strcmp(chartrail, K_F1_TRAIL2) == 0) {
    } else if(strcmp(chartrail, K_F4_TRAIL) == 0 ||
          strcmp(chartrail, K_F4_TRAIL2) == 0) {

      // ARROW KEYS
    } else if(strcmp(chartrail, K_LEFT_TRAIL) == 0) {
      //Left-arrow key
      if(cursorX > 1){
        cursorX = cursorX - 1;
        //editScroll.bufferX--;
      }
      if (posBufX>0) posBufX--;
    } else if(strcmp(chartrail, K_RIGHT_TRAIL) == 0) {
      //Right-arrow key
      if(cursorX < new_columns - 2){
        cursorX = cursorX + 1;
        //editScroll.bufferX++;
      }
      posBufX++;
    } else if(strcmp(chartrail, K_UP_TRAIL) == 0) {
      //Up-arrow key
      if(cursorY > 2) {
       cursorY = cursorY - 1;
        
      }
      if (posBufY > 0) posBufY--;
 
    } else if(strcmp(chartrail, K_DOWN_TRAIL) == 0) {
      //Down-arrow key
      if(cursorY < new_rows - 2) {
        cursorY = cursorY + 1;
        
      }
      posBufY++;
    } else if(strcmp(chartrail, K_PAGEDOWN_TRAIL) == 0) {
      //Page Down key
     } else if(strcmp(chartrail, K_PAGEUP_TRAIL) == 0) {
      //Page Down key
     }else if(strcmp(chartrail, K_HOME_TRAIL) == 0 || strcmp(chartrail, K_HOME_TRAIL2) == 0 ) {
    } else if(strcmp(chartrail, K_DELETE) == 0) {
      //delete button;
    } else if(strcmp(chartrail, K_ALT_F) == 0) {
      //data.index=FILE_MENU;
      //drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_P) == 0) {
      //data.index=OPT_MENU;
      //drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_H) == 0) {
      //data.index=HELP_MENU;
      //drop_down(&kglobal);  //animation
    } else if(strcmp(chartrail, K_ALT_O) == 0) {
      //openFileHandler();    //Open file Dialog
    } else if(strcmp(chartrail, K_ALT_N) == 0) {
      //newDialog(currentFile);   // New file
      //refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_A) == 0) {
      //saveasDialog(currentFile);    //Save as.. file
      //refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_D) == 0) {
      //fileInfoDialog();     //Info file
    } else if(strcmp(chartrail, K_ALT_W) == 0) {
      //if(strcmp(currentFile, UNKNOWN) == 0)
    //saveasDialog(currentFile);  //Write to file
      //else {
    //saveDialog(currentFile);
      //}
      //refresh_screen(-1);
    } else if(strcmp(chartrail, K_ALT_X) == 0) {
      //if(fileModified == 1)
    //exitp = confirmation(); //Shall we exit? Global variable!
      //else
    //exitp = EXIT_FLAG;
    } 
    esc_key = 1;

  return esc_key;
}

/*
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
*/

void linetoScreen(long whereY, VLINES tempLine){
//dump temporary Line to screen buffer - RAW MODE
   int i=0;
   int attrib = EDIT_FORECOLOR;

   for (i=0; i<findEndline(tempLine); i++){
	   attrib = tempLine.linea[i].attrib;  

	if(tempLine.linea[i].specialChar != 0) {
	  //Special char ? print the two values to screen buffer.
          gotoxy(i+START_CURSOR_X+1, whereY+1);
          outputcolor(attrib, EDITAREACOL);
          printf("%c%c", tempLine.linea[i].specialChar,tempLine.linea[i].ch);
	} else {
	  gotoxy(i+START_CURSOR_X+1, whereY+1);
          outputcolor(attrib, EDITAREACOL);
          printf("%c", tempLine.linea[i].ch);
	}
     }
}

void cleanScreenLine(long whereY)
{
   int i=0;
   for (i=0; i<new_columns-2; i++){
	 	gotoxy(i+START_CURSOR_X+1, whereY+1);
         	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	}
    resetAnsi(0);
   
}

void cleanSection(long whereY, long start, int amount)
{
   int i=0;
   for (i=start; i<start+amount; i++){
	 	gotoxy(i+START_CURSOR_X+1, whereY+1);
         	outputcolor(EDIT_FORECOLOR, EDITAREACOL);
          	printf("%c", FILL_CHAR);
	}
    resetAnsi(0);
   
}



void buffertoScreen(long startPoint, long activeline){
   long j=0;
   for (j=startPoint; j<_length(&edBuf1); j++){
	  _dumpLine(edBuf1, j , &tempLine);
 	  linetoScreen(j+START_CURSOR_Y, tempLine);
	}
    _dumpLine(edBuf1, activeline, &tempLine);
    resetAnsi(0);
}

int process_input(char ch){
char accentchar[2];
int insertMode=0;
VLINES *aux = NULL;
VLINES splitLine;
int i,j=0;
int attrib=EDIT_FORECOLOR;
char newch=0;
int endLine=0;
       
       //Check whether we are on Readmode
       // if (fileModified != FILE_READMODE) {
       //if ((ch > 31 && ch < 127) || ch < 0) {

       // A KEY IS PRESSED AND ADDED TO EDIT BUFFER
       // FIRST check for negative chars and special characters
        newch=ch;
        accentchar[0] = 0;
        accentchar[1] = newch;
       //Check whether we are on Readmode
       // if (fileModified != FILE_READMODE) {
     //Process normal printable chars first
     if ((ch > 31 && ch < 127) || ch < 0) {
	if (ch < 0) {
	    read_accent(&newch, accentchar);
	    newch = accentchar[1];
      }

      cursorX++;
      
      //SYNTAX HIGHLIGHTING DEMO
      //Highlight numbers in GREEN
      if ((accentchar[1] >= 48) && (accentchar[1] <=57)) attrib = FH_GREEN; 
      //Highlight special characters in CYAN
      
      if ((accentchar[1] >= 33) && (accentchar[1] <=47)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);
      if ((accentchar[1] >= 58) && (accentchar[1] <=64)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);
      if ((accentchar[1] >= 91) && (accentchar[1] <=96)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY); 
      if ((accentchar[1] >= 123) && (accentchar[1] <=126)) attrib = FH_CYAN; 
      aux = _getObject(edBuf1, posBufY);




      //FIRST TIME -> CREATE LINES IN BUFFER
      if (aux == NULL) {
	//FIRST CHAR - if not we create a new line in buffer

      	tempLine.index = posBufY;
	//Add spaces if cursor is not at (0,0)
 	if (posBufX != 0) {
		for (i=0; i<posBufX; i++) {
			tempLine.linea[i].ch = FILL_CHAR;
        		tempLine.linea[i].specialChar = 0;
			tempLine.linea[i].attrib = EDIT_FORECOLOR;
		}
	}	
	tempLine.linea[posBufX].ch = accentchar[1];
        tempLine.linea[posBufX].specialChar = accentchar[0];
        tempLine.linea[posBufX].attrib = attrib;
	//add end_line_char to line
	posBufX = posBufX + 1;
	tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = attrib;
	edBuf1 = _addatend(edBuf1, _newline(tempLine));
	linetoScreen(cursorY,tempLine);
     } else
	//LINE ALREADY EXISTS
	{
	//Locate the end of the line
	endLine = findEndline(tempLine);
	//Insert characters in the middle of other characters
	if (endLine > posBufX) {
	
	   tempLine.index = posBufY; //with every line index is incremented
	   if (insertMode == FALSE){
	     for (i=endLine; i>=posBufX; i--){ 
                 tempLine.linea[i+1].ch = tempLine.linea[i].ch;
                 tempLine.linea[i+1].specialChar = tempLine.linea[i].specialChar;
                 tempLine.linea[i+1].attrib = tempLine.linea[i].attrib;
              }
	    }
             tempLine.linea[posBufX].ch = accentchar[1];
             tempLine.linea[posBufX].specialChar = accentchar[0];
             tempLine.linea[posBufX].attrib = attrib;
              posBufX = posBufX + 1;
             _updateLine(edBuf1, posBufY, &tempLine);
	     linetoScreen(cursorY,tempLine);	
	     
	}    
	else {
	  // POSBUFX >= ENDLINE: Cursor is at the end or further away from latest text
          //ADD SPACES IF CURSOR IS NOT AT THE END OF THE LINE AND LINE ALREADY EXISTS
	  if(posBufX > endLine) {
	    for(i = endLine; i < posBufX; i++) {
	     tempLine.linea[i].ch = FILL_CHAR;
             tempLine.linea[i].specialChar = 0;
	     tempLine.linea[i].attrib = EDIT_FORECOLOR;
	   }
          } 
	  tempLine.linea[posBufX].ch = accentchar[1];
          tempLine.linea[posBufX].specialChar = accentchar[0];
          tempLine.linea[posBufX].attrib = attrib;
	  posBufX = posBufX + 1;
	  tempLine.linea[posBufX].ch = END_LINE_CHAR;
          tempLine.linea[posBufX].specialChar = 0;
          tempLine.linea[posBufX].attrib = attrib;
	  _updateLine(edBuf1, posBufY, &tempLine);  
	  linetoScreen(cursorY,tempLine);
         }
      }
    }
    old_cursorX = cursorX;
    old_cursorY = cursorY;
    oldposBufX = posBufX;
    oldposBufY = posBufY;

   //HANDLE ENTER KEY 
    if (ch == K_ENTER){
      //Add line to buffer
      //Display limit rows
      update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);
      if (cursorY<new_rows - 2) cursorY++;
      cursorX = START_CURSOR_X;
      //If buffer position pointer is at the end create a new empty line
      if (_length(&edBuf1) <= posBufY){
        //New line without chars in current line in edit buffer
        memset(&tempLine, '\0',sizeof(tempLine));
 	if (_length(&edBuf1)>0) tempLine.index = _length(&edBuf1);
	else tempLine.index = posBufY;
        tempLine.linea[posBufX].ch = END_LINE_CHAR;
        tempLine.linea[posBufX].specialChar = 0;
        tempLine.linea[posBufX].attrib = 0;
	edBuf1 = _addatend(edBuf1, _newline(tempLine));
      //  update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);
     } else{ 
        //SPLIT LINE IN TWO 
	//Locate the end of the line
  	endLine = findEndline(tempLine);
       // update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);
          //if (posBufX < endLine) {
           //Check if there are lines below and if so, move them.
	   //Move lines algorithm
	   //Create a new line to make room for the move
	   //Add current index to new line
           memset(&splitLine, '\0',sizeof(splitLine));
           memset(&tempLine, '\0',sizeof(tempLine));
	   tempLine.index = _length(&edBuf1);
	   edBuf1 = _addatend(edBuf1, _newline(tempLine));
	   //Move lines [j -> j+1] from bottom up until 1 line before active line
	   for (j=_length(&edBuf1); j>posBufY; j--)
		{
		     _dumpLine(edBuf1, j, &tempLine);
    	             _updateLine(edBuf1, j+1, &tempLine);
		     cleanSection(j+START_CURSOR_Y,0,findEndline(tempLine));
	   	}
	     _dumpLine(edBuf1, posBufY, &tempLine);
	     //Where are we on current line? -> Shall we move part of it?
              
             memset(&splitLine, '\0',sizeof(splitLine));
 	     //Split (1) and move(2) chars after posBufx to next line
	     //(1) Split and Update current line with chars that remain
	     for (i=0; i<posBufX; i++){
		  splitLine.linea[i].ch = tempLine.linea[i].ch;
		  splitLine.linea[i].specialChar = tempLine.linea[i].specialChar;
		  splitLine.linea[i].attrib = tempLine.linea[i].attrib;
	     } 
              if (isLineTerminated(splitLine)==FALSE) splitLine.linea[i].ch = END_LINE_CHAR;
	     _updateLine(edBuf1, posBufY, &splitLine);
	     j=0;
	     //(2) Move chars after posBufX to next line
             memset(&splitLine, '\0',sizeof(splitLine));
 	     for (i=posBufX; i<endLine; i++){
		   //j+1 line
		  splitLine.linea[j].ch = tempLine.linea[i].ch;
		  splitLine.linea[j].specialChar = tempLine.linea[i].specialChar;
		  splitLine.linea[j].attrib = tempLine.linea[i].attrib;
	   	  j++;
  	      }
	     if (isLineTerminated(splitLine)==FALSE) splitLine.linea[j].ch = END_LINE_CHAR;
	   //Update [J+1] line and write changes to screen
	   _updateLine(edBuf1, posBufY+1, &splitLine);
	   //cleanScreenLine(cursorY-1);
           cleanSection(cursorY-1,0,findEndline(tempLine));
           buffertoScreen(posBufY, posBufY+1);
	          
      } 
      //}
      //Move buffer pointer positions
      posBufY = posBufY + 1;
      posBufX = 0;
     }
      //fileModified = FILE_MODIFIED;
      
   if(ch == K_BACKSPACE) {
      //BACKSPACE key
      update_ch(cursorX, cursorY, ' ', EDITAREACOL, EDITAREACOL);
      if (posBufX == findEndline(tempLine)){
        tempLine.linea[posBufX-1].ch = 0;
        tempLine.linea[posBufX-1].specialChar = 0;
        tempLine.linea[posBufX-1].attrib = 0;
      } 
      else{
	//shift characters to the left if we are not at the end of the line 
       if (posBufX != 0){
          for(i=posBufX-1;i<findEndline(tempLine);i++){
            tempLine.linea[i].ch = tempLine.linea[i+1].ch;
            tempLine.linea[i].specialChar = tempLine.linea[i+1].specialChar;
            tempLine.linea[i].attrib = tempLine.linea[i+1].attrib;
          }
          tempLine.linea[i].ch = 0;
       }
      }
     //Remove line if we continue pressing backspace
     if(cursorX == START_CURSOR_X && cursorY > START_CURSOR_Y) {
 	//if (posBufY >= _length(&edBuf1)-1) _deleteObject(&edBuf1, posBufY, FALSE); //remove line from buffer
        //else { 
              memset(&tempLine, '\0',sizeof(tempLine));
              _dumpLine(edBuf1, posBufY-1, &tempLine);
	      if (findEndline(tempLine) < 1){

	       for (j=posBufY-1; j<=_length(&edBuf1); j++)
		{
                     _dumpLine(edBuf1, j+1, &tempLine);
		     _hardupdateLINE(&edBuf1, j, tempLine);
	             linetoScreen(j+START_CURSOR_Y,tempLine);
		     cleanScreenLine(j+START_CURSOR_Y+1);   
                     //cleanSection(cursorY, findEndline(tempLine), 2);
	      }
	      } else {
		      //merge two lines
		     endLine = findEndline(tempLine);
                      _dumpLine(edBuf1, posBufY, &splitLine);
		      for (i=0;i<findEndline(splitLine);i++) {
			     tempLine.linea[endLine+i].ch = splitLine.linea[i].ch; 
			     tempLine.linea[endLine+i].attrib = splitLine.linea[i].attrib; 
			     tempLine.linea[endLine+i].specialChar = splitLine.linea[i].specialChar; 		      
		      }
		      tempLine.linea[endLine+i].ch = END_LINE_CHAR;
		     _hardupdateLINE(&edBuf1, posBufY-1, tempLine);
	       for (j=posBufY; j<=_length(&edBuf1); j++)
		{
                     _dumpLine(edBuf1, j+1, &tempLine);
		     _hardupdateLINE(&edBuf1, j, tempLine);
	             linetoScreen(j+START_CURSOR_Y,tempLine);
		     cleanScreenLine(j+START_CURSOR_Y+1);   
                     //cleanSection(cursorY, findEndline(tempLine), 2);
	       }
               cleanScreenLine(posBufY+1);   
               cleanScreenLine(posBufY+2);   

	       }

		if (posBufY != _length(&edBuf1)) _deleteObject(&edBuf1, _length(&edBuf1)-1, FALSE);
	//}
	if (posBufY>0) posBufY--;
	_dumpLine(edBuf1, posBufY,&tempLine);
	posBufX = findEndline(tempLine)+1;
	cursorY = cursorY - 1;
	cursorX = findEndline(tempLine)+1 + START_CURSOR_X;
     }
      //cleanScreenLine(cursorY);
      if(cursorX > START_CURSOR_X){
        cursorX = cursorX - 1;
        posBufX--;
        _updateLine(edBuf1, posBufY, &tempLine);    
      } 


      cleanSection(cursorY, findEndline(tempLine), 2);
      linetoScreen(cursorY,tempLine);
     // cleanScreenLine(_length(&edBuf1)-1);   
    } 

    if(ch == K_TAB) {
      //TAB key sends spaces for convenience
      	      for (i=posBufX; i<posBufX+TAB_SPACES; i++)

		{
	            tempLine.linea[i].ch = FILL_CHAR;
                    tempLine.linea[i].specialChar = 0;
                    tempLine.linea[i].attrib = 0;
	      }      
	      posBufX=posBufX + TAB_SPACES;
	      if (cursorX < new_columns-8) {
                      for (i=0; i<TAB_SPACES; i++) update_ch(cursorX+i, cursorY, ' ', EDITAREACOL, EDITAREACOL);
		      cursorX = cursorX + TAB_SPACES;
	      }
        _updateLine(edBuf1, posBufY, &tempLine);    
     }

   return 0; 
}
