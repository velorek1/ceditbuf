/*====================================================================*/
/* OPEN FILE MODULE
   +ListFiles with double linked list and selection menu in C.
   +Scroll function added.
   +Integrated with C-EDIT
   A window is drawn to the buffer and all of the scroll animations
   are drawn to the terminal on raw mode to have a better scrolling
   animation. Once the file is selected, the window is closed and the
   previous screen is painted to the terminal again.
   Last modified : 11/1/2019 - Switch to readch() instead of getch()
                   06/04/2019 - Corrected all memory leaks
		   14/04/2019 - Rename headers
		   22/04/2020 - Added Set File to Open
   Coded by Velorek.
   Target OS: Linux.                                                  */
/*====================================================================*/

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES */
/*====================================================================*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "keyb.h"
#include "global.h"
#include "listbox.h"
//#include "uintf.h"
/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
#define CURRENTDIR "."
#define CHANGEDIR ".."
#define MAX_ITEM_LENGTH 15
#define DIRECTORY 1
#define FILEITEM 0
#define MAX 150

/*====================================================================*/
/* TYPEDEF STRUCTS DEFINITIONS */
/*====================================================================*/
typedef struct _dirdata {
  unsigned isDirectory;		// Kind of item
  char   *item;
  char   *path;
  char    fullPath[MAX];
  unsigned itemIndex;
 // LISTBOX *head;		//store head of the list
} DIRDATA;

/*====================================================================*/
/* GLOBAL VARIABLES */
/*====================================================================*/

int     window_x1 = 0, window_y1 = 0, window_x2 = 0, window_y2 = 0;
DIRDATA newdir;

/*====================================================================*/
/* CODE */
/*====================================================================*/

/* ------------------------------ */
/* Terminal manipulation routines */
/* ------------------------------ */
void cleanLine(int line, int backcolor, int forecolor, int startx,
	       int numchars) {
//Cleans line of console.
  int     i;
  for(i = startx; i < numchars; i++) {
    //clean line where path is displayed.
    outputcolor(forecolor, backcolor);
    gotoxy(i, line);
    printf("%c", FILL_CHAR);	//space
  }
}

/* ---------------- */
/* List files       */
/* ---------------- */

int addSpaces(char temp[MAX_ITEM_LENGTH]) {
  int     i;
  for(i = strlen(temp); i < MAX_ITEM_LENGTH; i++) {
    strcat(temp, " ");
  }
  return 0;
}

void cleanString(char *string, int max) {
  int     i;
  for(i = 0; i < max; i++) {
    string[i] = ' ';
  }
}

int listFiles(DIRDATA *newdir) {
 /* DIR    *d;
  struct dirent *dir;
  int     i;
  char    temp[MAX_ITEM_LENGTH];
  int     lenDir;		//length of directory

  printf("Here2\n");
  if(*listBox1 != NULL)
    removeList(listBox1);
 */
  listBox1 =
      addatend(listBox1, newitem("[CLOSE WINDOW]", -1,-1));
 listBox1 =
      addatend(listBox1, newitem("[SET FILENAME]", -1,-1));
   listBox1 =
      addatend(listBox1, newitem("[SET FILENAME]", -1,-1));
 listBox1 =
      addatend(listBox1, newitem("[SET FILENAME]", -1,-1));
 listBox1 =
      addatend(listBox1, newitem("[SET FILENAME]", -1,-1));
 
 //*listBox1 = addatend(*listBox1, newitem(CHANGEDIR, -1,DIRECTORY));	// ".."
/*
  //Start at current directory
  d = opendir(directory);
  //Find directories and add them to list first
  if(d) {
    while((dir = readdir(d)) != NULL) {
      if(dir->d_type == DT_DIR) {

	lenDir = strlen(dir->d_name);

	//Check length of directory
	//Directories are displayed between brackets [directory]
	if(lenDir > MAX_ITEM_LENGTH - 2) {
	  //Directory name is long. CROP
	  cleanString(temp, MAX_ITEM_LENGTH);
	  strcpy(temp, "[");
	  for(i = 1; i < MAX_ITEM_LENGTH - 1; i++) {
	    temp[i] = dir->d_name[i - 1];
	  }
	  temp[MAX_ITEM_LENGTH - 1] = ']';
	} else {
	  //Directory's name is shorter than display
	  //Add spaces to item string.
	  cleanString(temp, MAX_ITEM_LENGTH);
	  strcpy(temp, "[");
	  for(i = 1; i < lenDir + 1; i++) {
	    temp[i] = dir->d_name[i - 1];
	  }
	  temp[lenDir + 1] = ']';
	  addSpaces(temp);
	}
	//Add all directories except CURRENTDIR and CHANGEDIR
	if(strcmp(dir->d_name, CURRENTDIR) != 0
	   && strcmp(dir->d_name, CHANGEDIR) != 0)
	  *listBox1 =
	      addatend(*listBox1, newitem(temp, dir->d_name, -1,DIRECTORY));
      }
    }
  }
  closedir(d);

  //Find files and add them to list after directories
  d = opendir(directory);
  if(d) {
    while((dir = readdir(d)) != NULL) {
      if(dir->d_type == DT_REG) {
	//only list valid fiels
	if(strlen(dir->d_name) > MAX_ITEM_LENGTH) {
	  for(i = 0; i < MAX_ITEM_LENGTH; i++) {
	    temp[i] = dir->d_name[i];
	  }
	} else {
	  cleanString(temp, MAX_ITEM_LENGTH);
	  strcpy(temp, dir->d_name);
	  //Add spaces
	  addSpaces(temp);
	}
	*listBox1 =
	    addatend(*listBox1, newitem(temp, dir->d_name, FILEITEM));
      }
    }
    closedir(d);
  }
  */
  return 0;
}

void changeDir(SCROLLDATA * scrollData, char fullPath[MAX],
	       char newDir[MAX]) {
//Change dir
/*  char    oldPath[MAX];
  if(scrollData->isDirectory == DIRECTORY) {
    if(scrollData->itemIndex == 1) {
      //cd ..
      cleanString(fullPath, MAX);
      cleanString(oldPath, MAX);
      cleanString(newDir, MAX);
      chdir("..");
      getcwd(oldPath, sizeof(oldPath));
      strcpy(newDir, oldPath);
      strcpy(fullPath, oldPath);
    } else {
      //cd newDir
      cleanString(fullPath, MAX);
      cleanString(newDir, MAX);
      cleanString(oldPath, MAX);
      getcwd(oldPath, sizeof(oldPath));
      strcat(oldPath, "/");
      strcat(oldPath, scrollData->path);
      chdir(oldPath);
      strcpy(newDir, oldPath);
      strcpy(fullPath, oldPath);
    }
  }
}
int setFileName(char fileName[MAX]) {
  char    tempFile[MAX];
  int     ok, count;

  clearString(tempFile, MAX);

  ok = 0;
  count = inputWindow("Set file:", tempFile, "[-] C-EDIT INFO");
  if(count > 0) {
    //Check whether file exists and create file.
      strcpy(fileName, tempFile);
      ok = 1;
  }
  return ok;


*/
}
/* ---------------- */
/* Main             */
/* ---------------- */

/*========================================================================*/
/*  
  ListBox with Scroll: 
  ____________________
  
  Usage:
   
  listBox(headpointer, whereX, whereY, scrollData, backColor0, foreColor0,
backcolor1, forecolor1, displayLimit); */

/*========================================================================*/

char replica(DIRDATA *dirdata, SCROLLDATA *scrolld)
{
   char ch=0;
   listFiles(dirdata);
    ch = listBox(listBox1,  3,  1, scrolld,
		 MENU_PANEL, MENU_FOREGROUND0, MENU_SELECTOR, MENU_FOREGROUND1, 12, VERTICAL, 1); 
}


