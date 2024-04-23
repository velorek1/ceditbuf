#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "rterm.h"
#include "scbuf.h"
#include "listbox.h"
#include "tm.h"
#include "keyb.h"
#include "global.h"

#define MAX_ITEM_LENGTH 120
int ndirs=0, nfiles=0;

int listFiles(LISTCHOICE ** listBox1, char *directory) {
  DIR    *d;
  struct dirent *dir;
  int     i;
  char    temp[MAX_ITEM_LENGTH];
  int     lenDir;       //length of directory
  ndirs=0;
  nfiles=0;
  if (*listBox1 != NULL) removeList(listBox1);
  *listBox1 = addatend(*listBox1, newitem("[..]",-1,-1,F_RED,B_WHITE));    // ".."

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
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, "[");
      for(i = 1; i < MAX_ITEM_LENGTH - 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[MAX_ITEM_LENGTH - 1] = ']';
    } else {
      //Directory's name is shorter than display
      //Add spaces to item string.
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, "[");
      for(i = 1; i < lenDir + 1; i++) {
        temp[i] = dir->d_name[i - 1];
      }
      temp[lenDir + 1] = ']';
    }
    //Add all directories except CURRENTDIR and CHANGEDIR
    if(strcmp(dir->d_name, ".") != 0
       && strcmp(dir->d_name, "..") != 0)
    { ndirs++;
      *listBox1 =
          addatend(*listBox1, newitem(temp, -1,-1,F_RED,B_WHITE));}
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
      memset(&temp, '\0',sizeof(temp)); //Clear memory for temporary line
      strcpy(temp,"\0");
      strcpy(temp, dir->d_name);
    }
    nfiles++;
    *listBox1 =
        addatend(*listBox1, newitem(temp, -1,-1,-1,-1));
      }
    }
    closedir(d);
  }
  
  return 0;
}

void addItems(LISTCHOICE **listBox1)
{
//Load items into the list.  
        //if (listBox1 != NULL) removeList(listBox1);
	for (int h = 0; h <10; h++) {
		//*ch = textarray[h];
		*listBox1 = addatend(*listBox1, newitem("Test",-1,-1,-1,-1));
	}
}

int main(){
char ch=0;
char path[MAXFILENAME];
char bit[MAXFILENAME];
char ndirstr[100];
char nfilestr[100];
char currentPath[4] = "./\0";
int window_y1, window_y2, window_x1,window_x2;

    init_term();
    initCEDIT();
    resetch(); 
    strcpy(path,"\0");
    strcpy(path, "./");
    setselectorLimit(23);
    window_y1 = (new_rows / 2) - 10;
    window_y2 = (new_rows / 2) + 10;
    window_x1 = (new_columns / 2) - 13;
    window_x2 = (new_columns / 2) + 13;    
   do{
     draw_window(screen1,window_x1, window_y1, window_x2, window_y2,B_WHITE,F_BLACK,B_BLUE,1,1,1,0);
     draw_window(screen1,window_x1, window_y2-3, window_x2, window_y2,B_BLUE,F_WHITE,B_BLACK,1,0,0,0);
     dump_screen(screen1);
      listFiles(&listBox1,currentPath);
      sprintf(ndirstr, "[%d] Directories", ndirs);
      sprintf(nfilestr, "[%d] Files", nfiles);
      write_str(screen1,window_x1+2,window_y2-2,ndirstr,B_BLUE,FH_WHITE,1);
      write_str(screen1,window_x1+2,window_y2-1,nfilestr,B_BLUE,F_WHITE,1);
      write_str(screen1,window_x1+1,window_y2-4, "   PRESS [ESC] TO EXIT   ",B_BLACK,FH_WHITE,1);
      write_str(screen1,window_x1+2,window_y1,"Select file...",B_BLUE,FH_WHITE,1);
      ch = listBox(listBox1, window_x1+3,window_y1+1, &scrollData, B_WHITE, F_BLACK,B_CYAN, FH_WHITE, 15, VERTICAL,1,LOCKED);
      
      if (ch == K_ENTER){

	    printf("Char %c\n", scrollData.item[0]);
        if (scrollData.item[0] == '[') {
	 //directory   
            memset(&path, '\0',sizeof(path)); //Clear memory for temporary line
            memset(&bit, '\0',sizeof(bit)); //Clear memory for temporary line
    	    strcpy(bit,"\0");
            strcpy(path,"\0");
            for (size_t i=1; i<strlen(scrollData.item)-1; i++) bit[i-1] = scrollData.item[i];
	    getcwd(path, sizeof(path));
	    strcat(path, "/");
	    strcat(path, bit);
	    chdir(path);
	    //printf("Directory %s:\n",path);
	    //break;
	    ch=0;
      }
      else {
	     break;
      }	 
    } 
   } while (ch!=ESC_KEY);   
    setselectorLimit(15);
   
    close_term();
    
    printf("Key %d:%d\n", ch,K_ENTER);
    if (ch!=27){
	    printf("File selected: %s\n", scrollData.item);
    }
	    printf("Directory %s:\n",path);
	    printf("Directory %s:\n",bit);
    if (listBox1 != NULL) removeList(&listBox1);
}
