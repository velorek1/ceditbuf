/* Module to display Menus on C·edit 
 * for a Text User Interface
 * TextBox
 * Window
 * Last modified: 6/4/2024
 * @author:velorek
 */
#include <stdio.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"
#include "menu.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "editor.h"
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

      copy_screen(screen2,screen1);
      if (horizontalMenu) *returnMenuChar= horizontal_menu();  
      do{
      switch (*returnMenuChar) {
	case 0: *menuCounter=FILE_MENU; *returnMenuChar=filemenu();  if (*returnMenuChar != DONT_UPDATE) xor_update(screen2,screen1); break;
     	case 1: *menuCounter=OPT_MENU; *returnMenuChar=optionsmenu();  xor_update(screen2,screen1); break;
	case 2: *menuCounter=HELP_MENU; *returnMenuChar=helpmenu(); xor_update(screen2,screen1);  break;
	default:
		break;
      }       
      *menuCounter=*menuCounter + *returnMenuChar;  
      if (*returnMenuChar == K_ENTER) break;
      if (*returnMenuChar == DONT_UPDATE) break;
      if (*returnMenuChar != ESC_KEY ) {
     			copy_screen(screen1,screen2);
	      		//euclidian modulo, we circle back through the different switch cases
	                *returnMenuChar = ((*menuCounter % 3) + 3) % 3; 
	 }

     } while (*returnMenuChar != ESC_KEY);
     //xor_update(screen2,screen1);
    if (*returnMenuChar != DONT_UPDATE){
      copy_screen(screen1,screen2);
      dump_screen(screen1);
   }
}
//Entry menu- horizontal
char horizontal_menu() {
char ch=0;

         //Save current screen to screen2
         loadmenus(HOR_MENU);	
	//load menus from ui.c onto lisBox in (global.c)
	//dump_screen(screen1);
        ch = listBox(listBox1, 0, 1 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  3, HORIZONTAL,0,1); 
        //delete listbox
	if(ch == ESC_KEY) scrollData.itemIndex = ch;; //to avoid warning
	removeList(&listBox1);
        //return option passed in scrollData (global.c)
	return scrollData.itemIndex;
}
/*-------------------------*/
/* Display File menu       */
/*-------------------------*/

char filemenu() {  
  char ch=0;
  write_str(screen1,0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
  write_str(screen1,0, 1, "File", MENU_SELECTOR, MENU_FOREGROUND1,1);
  loadmenus(FILE_MENU);
  draw_window(screen1,0, 2, 12, 8, MENU_PANEL, MENU_FOREGROUND0,0, 1,0,1,1);
  ch = listBox(listBox1, 3, 3 , &scrollData, MENU_PANEL, MENU_FOREGROUND0,  MENU_SELECTOR, MENU_FOREGROUND1,  -1, VERTICALWITHBREAK,0,1); 
     copy_screen(screen1,screen2);
     dump_screen(screen1);
 
  //return scrollData.itemIndex;
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
     inputWindow("File:", fileName,  "Quick load...");
     if (strlen(fileName)>0) {
	 filetoBuffer(fileName);
         flush_editarea(0);
         buffertoScreen(0, 0,0);
        dump_screen(screen1);
     }//buffertoScreen(0, 0, 0);
     return DONT_UPDATE;

  }
  if(scrollData.itemIndex == OPTION_3) {
    //Save option
    inputWindow("File:", fileName,  "Save file as...");
    if (strlen(fileName)>0) buffertoFile(fileName);
    return DONT_UPDATE;
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
 //dump_screen(screen1);
	
return ch;
}

/*--------------------------*/
/* Display Options menu     */
/*--------------------------*/

char optionsmenu() {
  //int  setColor;
  char ch=0;

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
return ch;

}

/*--------------------------*/
/* Display Help menu        */
/*--------------------------*/

char helpmenu() { 
  char ch= 0;

  write_str(screen1,15, 1, "Help", MENU_SELECTOR, MENU_FOREGROUND1,1);
  write_str(screen1, 0, new_rows, STATUS_BAR_MSG2, STATUSBAR, STATUSMSG,1);
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
  return ch;
}


