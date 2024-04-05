#include <stdio.h>
#include "listbox.h"
#include "rterm.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "scbuf.h"

//LISTCHOICE *listBox1;
SCROLLDATA datosLista;

void addItems(LISTCHOICE ** listBox1) {
//Load items into the list.  
  if(*listBox1 != NULL)
    removeList(listBox1);
  *listBox1 = addatend(*listBox1, newitem("Option 1",4,3));
  *listBox1 = addatend(*listBox1, newitem("Option 2",26,3));
  *listBox1 = addatend(*listBox1, newitem("Option 3",56,3));
  *listBox1 = addatend(*listBox1, newitem("Option 4",86,3));
/*  *listBox1 = addatend(*listBox1, newitem("Option 5",1,12));
  *listBox1 = addatend(*listBox1, newitem("Option 6",1,14));
  *listBox1 = addatend(*listBox1, newitem("Option 7",1,16));
  *listBox1 = addatend(*listBox1, newitem("Option 8",1,17));
*/}

int main(){
    char ch=0;
    init_term();
    initCEDIT();
    //resetScrollData();
    addItems(&listBox1);   
    ch=listBox(listBox1, 3,3, &datosLista, B_WHITE,F_RED,B_BLUE,F_WHITE,5,HORIZONTAL,1); 
    printlist(listBox1, &datosLista, 3);

    // removeList(&listBox1);
    close_term();
    printf("%d\n", query_length(&listBox1));
}
