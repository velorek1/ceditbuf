#include <stdio.h>
#include "listbox.h"
#include "rterm.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"
#include "scbuf.h"
#include "opfile.h"

SCROLLDATA datosLista;
DIRDATA datosFicheros;

int main(){
    char ch=0;
    init_term();
    initCEDIT();
     //if (screen2 != NULL) deleteList(&screen2);
     //Init 2 : Create 2 Screens for a double buffer  approach  
    //resetScrollData(&datosLista);
    //openFileDialog(listBo1, &datosFicheros, &datosLista);
    replica(listBox1, &datosLista);
//     removeList(&listBox1);
   close_term();
    //printf("%d\n", query_length(&listBox1));
}
