#ifndef _EDBUF_H_
#define _EDBUF_H_

#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_SIZE 1022
#define TRUE 1
#define FALSE 0
typedef int BOOL;

typedef struct _charbuf
{
    char ch;
    char specialChar;
    char attrib;    
}CHARBUF;

typedef struct _vlines 
{ 
	int  index;
   	CHARBUF linea[MAX_LINE_SIZE];
	struct _vlines *next;
} VLINES;


/* Adapted from Kernighan and Pike's "The Practice of Programming"  pp.46 et 
seq. (Addison-Wesley 1999) */

// create new list element of type VLINES from the supplied text string
VLINES *newline(VLINES temp);
VLINES *addfront(VLINES *head, VLINES *newp);
VLINES *addatend (VLINES *head, VLINES *newp);
VLINES *addmiddle (VLINES *head, VLINES *newp);
VLINES *update(VLINES *head, int index, VLINES temp);
VLINES *getObject(VLINES *head, int index);
void RemoveThing(VLINES **head, int index);
void deletetheList(VLINES **head);
VLINES *deleteline(VLINES *head, int index);
void deleteObject(VLINES **head,int index, BOOL sort);
int length(VLINES **head);
void printlist(VLINES **head);
void reindex(VLINES **head);
int dumpLine(VLINES *head, long index, VLINES *line);
int updateLine(VLINES *head, long index, VLINES *line);
CHARBUF getSingleChar(VLINES *head, long X, long Y );
#endif
