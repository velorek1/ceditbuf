#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "keyb.h"
#include "rterm.h"
//#include "global.h"

int main() {
    setlocale(LC_ALL, "");

    signed char charISO8859_1 = -67;
    wchar_t wideChar;

    if (mbtowc(&wideChar, &charISO8859_1, 1) == -1) {
        printf("Conversion failed\n");
    }
    
    printf("ISO-8859-1 (signed char): %d\n", charISO8859_1);
    printf("Unicode (wchar_t): %lc\n", wideChar);

    char ch1 = 0;
    char ch2[2];
    resetch();
    do{

    if (kbhit(1)) ch1 = readch();
    printf("%d:%c\n", ch1,ch1);
    if (ch1 == 27 | ch1 == 13) break;
    if (ch1<0) read_accent(&ch1,ch2);
      printf("%d:%d\n", ch2[0],ch2[1]);
      printf("%c%c\n", ch2[0],ch2[1]);
    } while (ch1 != 0x27);
    return 0;
}
