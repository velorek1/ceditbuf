/*
========================================================================
- HEADER - 
Module to open a file by showing a dialogue that allows you to navigate 
through directories with a list with scroll.
@author : Velorek
@version : 1.0  
Last modified: 14/04/2019 Rename headers                                                                
========================================================================
*/

#ifndef _OPFILE_H_
#define _OPFILE_H_

/*====================================================================*/
/* COMPILER DIRECTIVES AND INCLUDES                                   */
/*====================================================================*/
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "scbuf.h"
#include "ui.h"

/*====================================================================*/
/* CONSTANTS */
/*====================================================================*/
#define MAX_ITEM_LENGTH 15
#define MAX 150
/*====================================================================*/
/* TYPEDEF STRUCTS DEFINITIONS */
/*====================================================================*/
typedef struct _dirdata {
  char   *item;
  char   *path;
  char    fullPath[MAX];
  unsigned isDirectory;		// Kind of item
  unsigned itemIndex;
} DIRDATA;


/*====================================================================*/
/* CONSTANT VALUES                                                    */
/*====================================================================*/

/*====================================================================*/
/* FUNCTION PROTOTYPES                                                */
/*====================================================================*/

//CONSOLE DISPLAY FUNCTIONS 
void    cleanLine(int line, int backcolor, int forecolor, int startx,
		  int numchars);

//DYNAMIC LINKED LIST FUNCTIONS
//void    deleteList(LISTBOX ** head);
//LISTBOX *addend(LISTBOX * head, LISTBOX * newp);
//LISTBOX *newelement(char *text, char *itemPath, unsigned itemType);

//LISTFILES FUNCTIONS
int     listFiles(LISTCHOICE ** listBox1, DIRDATA *openFileData);
int     addSpaces(char temp[MAX_ITEM_LENGTH]);
void    cleanString(char *string, int max);
//void    changeDir(SCROLLDATA * scrollData, char fullPath[MAX],
		  //char newDir[MAX]);
//OPEN FILE DIALOG
//void    openFileDialog(LISTCHOICE *listBox1, DIRDATA * openFileData, SCROLLDATA *scrollData);
//int	setFileName(char fileName[MAX]);
char replica(DIRDATA *datadir, SCROLLDATA *scrolld);



#endif
