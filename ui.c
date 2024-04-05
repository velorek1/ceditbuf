/* Module to create different widgets/gadgets 
 * for a Text User Interface
 * TextBox
 * Window
 * Last modified: 15/04/2023
 * @author:velorek
 */
#include <stdio.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
/*----------------------------*/
/* User Interface - Text Box. */
/*----------------------------*/
int textbox(SCREENCELL *newScreen,int wherex, int wherey, int displayLength,
        char *label, char text[MAX_TEXT], int backcolor,
        int labelcolor, int textcolor, BOOL raw, BOOL locked) {
  int     charCount = 0;
  int     exitFlag = 0;
  int     cursorON = 1;
  int     i;
  int     limitCursor = 0;
  int     positionx = 0;
  int     posCursor = 0;
  int     keypressed = 0;
  char    chartrail[5];
 // char    accentchar[2];
  char    displayChar;
  char    ch;
  NTIMER  cursorTime;
  SCREENCELL *aux = newScreen;
 
  init_timer(&cursorTime,150);
  positionx = wherex + strlen(label);
  limitCursor = wherex+strlen(label)+displayLength+1;
  write_str(aux,wherex, wherey, label, backcolor, labelcolor,raw);
  write_ch(aux,positionx, wherey, '[', backcolor, textcolor,raw);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    write_ch(aux,i, wherey, '.', backcolor, textcolor,raw);
  }
  write_ch(aux,positionx + displayLength + 1, wherey, ']', backcolor,
       textcolor,raw);
  if (raw != TRUE) dump_screen(aux);
  //Reset keyboard
  if(kbhit(1) == 1) ch = readch();
  ch = 0;

  do {
	 if (locked == 0) break;
      keypressed = kbhit(1);
    //Cursor Animation
   if (keypressed == 0){
    
    if (timerC(&cursorTime) == TRUE){
      switch (cursorON) {
    case 1:
      posCursor = positionx + 1;
          displayChar = '.';
          if (posCursor == limitCursor) {
            posCursor = posCursor - 1;
            displayChar = ch;
          }
          write_ch(aux,posCursor, wherey, displayChar, backcolor, textcolor,raw);
          //update_screen(aux);
          if (raw != TRUE) dump_screen(aux);
          cursorON = 0;
      break;
    case 0:
          posCursor = positionx + 1;
          if (posCursor == limitCursor) posCursor = posCursor - 1;
          write_ch(aux,posCursor, wherey, '|', backcolor, textcolor,raw);
          //update_screen(aux);
          if (raw != TRUE) dump_screen(aux);
          cursorON = 1;
      break;
      }
     }
      _animation();
    }
    //Process keys
    if(keypressed == 1) {
      ch = readch();
      keypressed = 0;

      //Read special keys
      if (ch==K_ESCAPE) {
               read_keytrail(chartrail);
      }
      //Read accents
      //if (ch==SPECIAL_CHARSET_1) read_accentchar(&ch, accentchar);
      //if (ch==SPECIAL_CHARSET_2) read_accentchar(&ch, accentchar);

      if(charCount < displayLength) {
     if(ch > 31 && ch < 127) {
      write_ch(aux,positionx + 1, wherey, ch, backcolor, textcolor,raw);
      text[charCount] = ch;
      positionx++;
      charCount++;
      //update_screen(aux);
      if (raw != TRUE) dump_screen(aux);
    }
      }
    }
    if (ch==K_CTRL_C){
	    return ENDSIGNAL;
    }
    if (ch==K_BACKSPACE){
      if (positionx>0 && charCount>0){
       ch=0;
       positionx--;
       charCount--;
       text[charCount] = '\0';
       write_ch(aux,positionx + 1, wherey, '.', backcolor, textcolor,raw);
       if (positionx < limitCursor-2) write_ch(aux,positionx + 2, wherey, '.', backcolor, textcolor,raw);
       //update_screen(aux);
       if (raw != TRUE) dump_screen(aux);
       //resetch();
      }
    }
    if(ch == K_ENTER || ch == K_TAB || ch == K_ESCAPE)
      exitFlag = 1;

    //ENTER OR TAB FINISH LOOP
  } while(exitFlag != 1);
  text[charCount] = '\0';
  //Clear field
  positionx = wherex + strlen(label);
  for(i = positionx + 1; i <= positionx + displayLength; i++) {
    write_ch(aux,i, wherey, '.', backcolor, textcolor,raw);
  }
  write_ch(aux,positionx + displayLength + 1, wherey, ']', backcolor,
       textcolor,raw);
 
  //resetch();
  return charCount;
}


void window(SCREENCELL *screen1, int x1, int y1, int x2, int y2, int backcolor,
         int bordercolor, int titlecolor, int border, int title, int shadow) {
/*
   Chars for drawing box-like characters will be passed as negative values.
   When the update_screen routine is called, it will check for negative
   values and map these chars to Unicode characters.
 */
  int     i, j;
  //char ch=0x20;
  wchar_t ch=0x20;
  i = x1;
  j = y1;
  //shadow
  if (shadow==1){
   for(j = y1 + 1; j <= y2 + 1; j++)
    for(i = x1 + 1; i <= x2 + 1; i++)
    {
      ch=read_char(screen1, i,j); //dynamic shadow
      if ((ch=='\0') || (ch==UNICODEBAR1)) ch=0x20;
      write_ch(screen1, i, j, ch, B_BLACK, F_WHITE,0);
    }
  }
  //window
  for(j = y1; j <= y2; j++)
    for(i = x1; i <= x2; i++)
      write_ch(screen1, i, j, ' ', backcolor, bordercolor,0);
  //borders
  if(border == 1) {
    //with borders. ANSI-ASCII 106-121
    for(i = x1; i <= x2; i++) {
      //upper and lower borders
      write_ch(screen1, i, y1, HOR_LINE, backcolor, bordercolor,0);   //horizontal line box-like char
      write_ch(screen1, i, y2, HOR_LINE, backcolor, bordercolor,0);
    }
    for(j = y1; j <= y2; j++) {
      //left and right borders
      write_ch(screen1,x1, j, VER_LINE, backcolor, bordercolor,0);   //vertical line box-like char
      write_ch(screen1,x2, j, VER_LINE, backcolor, bordercolor,0);
    }
    write_ch(screen1, x1, y1, UPPER_LEFT_CORNER, backcolor, bordercolor,0);   //upper-left corner box-like char
    write_ch(screen1, x1, y2, LOWER_LEFT_CORNER, backcolor, bordercolor,0);   //lower-left corner box-like char
    write_ch(screen1, x2, y1, UPPER_RIGHT_CORNER, backcolor, bordercolor,0);  //upper-right corner box-like char
    write_ch(screen1, x2, y2, LOWER_RIGHT_CORNER, backcolor, bordercolor,0);  //lower-right corner box-like char
  }
  if (title == 1) {
    for(i = x1; i <= x2; i++)
      write_ch(screen1, i, y1-1, ' ', titlecolor, titlecolor,0);
  }
 // dump_screen(screen1);
}
/*--------------------------------------------*/
/* Load current menu into circular linked list*/
/*--------------------------------------------*/

void loadmenus(int choice) {

  if(choice == HOR_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("File", 0, 1));
	listBox1 = addatend(listBox1, newitem("Options", 7, 1));
	listBox1 = addatend(listBox1, newitem("Help", 16, 1));
    }
  

  if(choice == FILE_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("New", -1, -1));
	listBox1 = addatend(listBox1, newitem("Open", -1, -1));
	listBox1 = addatend(listBox1, newitem("Save", -1, -1));
	listBox1 = addatend(listBox1, newitem("Save as...", -1, -1));
	listBox1 = addatend(listBox1, newitem("Exit", -1, -1));
 }
  if(choice == OPT_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("File Info", -1, -1));
	listBox1 = addatend(listBox1, newitem("Find...", -1, -1));
	listBox1 = addatend(listBox1, newitem("Colors", -1, -1));	
  }
  if(choice == HELP_MENU) {
 	if (listBox1 != NULL) removeList(&listBox1);
	listBox1 = addatend(listBox1, newitem("Help...", -1, -1));
	listBox1 = addatend(listBox1, newitem("About", -1, -1));	
 }
/*
  if(choice == YESNO_MENU) {
    add_item(mylist, "[YES]", (columns / 2) - 11, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[NO]", (columns / 2) - 3, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "[CANCEL]", (columns / 2) + 4, (rows / 2) + 2,  MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 2, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == OK_MENU2) {
    add_item(mylist, "[OK]", (columns / 2) - 1, (rows / 2) + 3, MENU2_PANEL, MENU2_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
  if(choice == COLORS_MENU) {
    add_item(mylist, "C-Edit Theme", (columns / 2) - 6, (rows / 2) - 2, 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Classic Theme", (columns / 2) - 6, (rows / 2) -1, 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
    add_item(mylist, "Dark Theme", (columns / 2) - 6, (rows / 2), 
        MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1);
  }
*/
}

/*---------------
 Menu loop
It depends on two values passed by reference stored in the function from where this routine is called:
-returnMenuChar
-menuCounter
It does some tricky modulo operation to cycle back
*/
void handlemenus(char *returnMenuChar, int *menuCounter, BOOL horizontalMenu)
{      
      if (horizontalMenu) *returnMenuChar= horizontal_menu();  
      do{
      switch (*returnMenuChar) {
	case 0: *returnMenuChar=filemenu(); break;
	case 1: *returnMenuChar=optionsmenu();  break;
	case 2: *returnMenuChar=helpmenu();  break;
	default:
      }       
      *menuCounter=*menuCounter + *returnMenuChar;  
      if (*returnMenuChar == K_ENTER) break;
      if (*returnMenuChar != ESC_KEY ) {
	      		//euclidian modulo, we circle back through the different switch cases
	                *returnMenuChar = ((*menuCounter % 3) + 3) % 3; 
	 }

     } while (*returnMenuChar != ESC_KEY);

}
//Entry menu- horizontal
char horizontal_menu() {
char ch=0;

         //Save current screen to screen2
        if (screen2 != NULL) deleteList(&screen2);
 	create_screen(&screen2);
        copy_screen(screen2,screen1);	       
        loadmenus(HOR_MENU);	
	//load menus from ui.c onto lisBox in (global.c)
        ch = listBox(listBox1, 0, 1 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  3, HORIZONTAL,0,1); 
        //delete listbox
	ch++; //to avoid warning
	removeList(&listBox1);
	xor_update(screen2,screen1);
  	copy_screen(screen1,screen2);
	if (screen2 != NULL) deleteList(&screen2);
        //return option passed in scrollData (global.c)
	return scrollData.itemIndex;
}
/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

char filemenu() {  
  char ch=0;
  if (screen2 != NULL) deleteList(&screen2);
  create_screen(&screen2);
  copy_screen(screen2,screen1);
  write_str(screen1,1, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  write_str(screen1,0, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1,1);
  loadmenus(FILE_MENU);
  draw_window(screen1,0, 2, 12, 8, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 3, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  if(scrollData.itemIndex == OPTION_1) {
    //New file option
    //newDialog(currentFile);
    //Update new global file name
    //refresh_screen(-1);
     //programStatus  = ENDSIGNAL;
  }
  if(scrollData.itemIndex == OPTION_2) {
    //External Module - Open file dialog.
    //openFileHandler();
    //flush_editarea();
  }
  if(scrollData.itemIndex == OPTION_3) {
    //Save option
  }
  if(scrollData.itemIndex == OPTION_4) {
    //Save as option  
      //saveasDialog(currentFile);
      //refresh_screen(-1);
  }

  if(scrollData.itemIndex == OPTION_5) {
    //Exit option
    //if(fileModified == 1)
      //exitp = confirmation();	//Shall we exit? Global variable!
    //else
      programStatus  = ENDSIGNAL;
  }
 //restore previous screen
  xor_update(screen2,screen1);
  copy_screen(screen1,screen2);
 //dump_screen(screen1);
	
 if (screen2 != NULL)
	deleteList(&screen2);
return ch;
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

char optionsmenu() {
  //int  setColor;
  char ch=0;
  if (screen2 != NULL) deleteList(&screen2);
  create_screen(&screen2);

  copy_screen(screen2,screen1);
  write_str(screen1,6, 1, "Options", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  loadmenus(OPT_MENU);
  draw_window(screen1,6, 2, 19, 6, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 9, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  
  if(scrollData.itemIndex == OPTION_1) {
    //File Info
    //fileInfoDialog();
  }
  if(scrollData.itemIndex == OPTION_3) {
    //Set Colors
  /*  setColor = colorsWindow(mylist,COLORSWTITLE);
    setColorScheme(setColor);
    checkConfigFile(setColor);	//save new configuration in config file
    *///refresh_screen(1);
  }
 //restore previous screen
  xor_update(screen2,screen1);
  copy_screen(screen1,screen2);
 

 if (screen2 != NULL)
	deleteList(&screen2);
return ch;

}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

char helpmenu() { 
  char ch= 0;
  if (screen2 != NULL)	deleteList(&screen2);
  create_screen(&screen2);

  copy_screen(screen2,screen1);
   write_str(screen1,15, 1, "Help", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 1, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  loadmenus(HELP_MENU);
  draw_window(screen1,15, 2, 25, 5, MENU_PANEL, MENU_FOREGROUND0, 0,1,0,1,1);
  ch = listBox(listBox1, 18, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
  if(scrollData.itemIndex == OPTION_1) {
    //About info
    //help_info();
  }
  if(scrollData.itemIndex == OPTION_2) {
    //About info
    //about_info();
  }
   xor_update(screen2,screen1);
  copy_screen(screen1,screen2);
  

 if (screen2 != NULL)
	deleteList(&screen2);
return ch;
}


