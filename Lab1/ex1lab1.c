/*1. Scrieti un program C care citeste dintr-un fisier text
“test.in" (denumirea va fi data programului ca si parametru) doua
stringuri, le concateneaza, afiseaza la ecran “Hello World” si apoi scrie
stringurile concatenate intr-un fisier “test.out” (denumirea va fi data
programului ca si parametru).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]){
    char a;
    FILE *file = fopen(argv[1], "r");
       if (file == NULL) {
        perror("Eroare la deschiderea fișierului");
        return 1;
    }

    
    char str1[100], str2[100];
    if (fscanf(file, "%s %s", str1, str2) != 2) {
        fprintf(stderr, "Citire stringuri err\n");
        exit(1);
    }
    fclose(file);

    char str3[200];
    strcpy(str3, str1);
    strcat(str3, str2);
    
    printf("Hello World!\n");
    
    FILE *out_file = fopen(argv[2], "w");
    fprintf(out_file, "%s", str3);
    fclose(out_file);
    return 0;
}
