#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

wchar_t convertChar(char c1, char c2) {
    // Given the two-byte representation for a Char get the wchar convrsion
    setlocale(LC_ALL, "");

    // Combine the bytes into a single string
    char temp[3];
    temp[0] = c1;
    temp[1] = c2;
    temp[2] = '\0';

    wchar_t wchar;

    mbstowcs(&wchar, temp, 2);

    return wchar;
}

int main(){
    // Print the result
    printf("Wide character representation: %lc\n", convertChar(-61,-111));

}

