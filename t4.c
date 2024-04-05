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
    init_term();
    initCEDIT();
    //printf("%d\n", query_length(&listBox1));
    replica(&datosFicheros, &datosLista);

    removeList(&listBox1);
    deleteList(&screen1);
   close_term();
    //printf("%d\n", query_length(&listBox1));
}
