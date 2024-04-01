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
  *listBox1 = addatend(*listBox1, newitem("L >>LIST"));
  *listBox1 = addatend(*listBox1, newitem("Option 2"));
  *listBox1 = addatend(*listBox1, newitem("Option 3"));
  *listBox1 = addatend(*listBox1, newitem("Option 4"));
  *listBox1 = addatend(*listBox1, newitem("Option 5"));
  *listBox1 = addatend(*listBox1, newitem("Option 6"));
  *listBox1 = addatend(*listBox1, newitem("Option 7"));
  *listBox1 = addatend(*listBox1, newitem("Option 8"));
}

int main(){
    char ch=0;
    init_term();
    initCEDIT();
    //resetScrollData();
    addItems(&listBox1);   
    ch=listBox(listBox1, 3,3, &datosLista, B_WHITE,F_RED,B_BLUE,F_WHITE,3,HORIZONTAL,1); 
    printlist(listBox1, &datosLista, 3);

    // removeList(&listBox1);
    close_term();
    printf("%d\n", query_length(&listBox1));
}
