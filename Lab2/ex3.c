#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]) {

    pid_t pid = fork(); 

    if (pid < 0) {
        perror("Eroare la fork()");
        exit(1);
    }

    if (pid > 0) {
        FILE *file = fopen(argv[1], "w");
        if (file == NULL) {
            perror("Eroare la deschiderea fișierului");
            exit(1);
        }

        char c[8]; 
        if (pid % 2 == 0) {
            strcpy(c, "fortune");
            fwrite(c, sizeof(char), strlen(c), file);
        } else {
            strcpy(c, "lost");
            fwrite(c, sizeof(char), strlen(c), file);
            fclose(file);
            exit(0);
        }

        fclose(file); 
        wait(NULL); 

    } else { 
        sleep(1);

        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Eroare la deschiderea fișierului");
            exit(1);
        }

        char str[13]; 
        if (fgets(str, sizeof(str), file) != NULL)
            printf("%s", str);
        else printf("eroare print");
        fclose(file);
    }

    return 0;
}

