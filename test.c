#include <stdio.h>
#include "edbuf.h"
VLINES *mibuffer=NULL;
VLINES mydata;

int myrandom(int num)
{
  return rand() & num;
}
int main(){
int i=0, j=0;
	srand(1);
	for (i=0;i<19998;i++){
	  mydata.index = i;
         for (j=0; j<MAX_LINE_SIZE; j++){
	  	mydata.linea[j].ch = 65+ myrandom(50);
 	 	mydata.linea[j].specialChar = myrandom(125);
	  	mydata.linea[j].attrib = myrandom(125);
	  }
	  	mibuffer = addatend(mibuffer, newline(mydata));	
	}
	printlist(&mibuffer);
	//mydata.linea[0].ch ='7';
	//updateLine(mibuffer, 0, &mydata);
	//printlist(&mibuffer);
	printf("\n%ld\n",sizeof(&mibuffer));
	deletetheList(&mibuffer);
}



