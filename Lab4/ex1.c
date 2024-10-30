#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int s[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) < 0) { 
        perror("Error creating socketpair"); 
        exit(1); 
    }

    int pid = fork();
    switch (pid) {
        case -1:
            perror("Error on fork");
            exit(2);

        case 0: { 
            close(s[1]); 

            char res[300];
            if (read(s[0], res, sizeof(res)) == -1) {
                perror("Error on read in child");
                exit(6);
            }

            strcat(res, " child");


            int size = strlen(res) + 1;  
            if (write(s[0], &size, sizeof(int)) == -1) {
                perror("Error on writing size in child");
                exit(7);
            }
            if (write(s[0], res, size) == -1) {
                perror("Error on writing string in child");
                exit(8);
            }

            close(s[0]); 
            break;
        }

        default: { 
            close(s[0]); 
            if (argc < 2) {
                fprintf(stderr, "Usage: %s <string>\n", argv[0]);
                exit(9);
            }

            if (write(s[1], argv[1], strlen(argv[1]) + 1) == -1) {
                perror("Error on writing in parent");
                exit(3);
            }

            wait(NULL);

            int size;
            if (read(s[1], &size, sizeof(int)) == -1) {
                perror("Error on reading size in parent");
                exit(4);
            }

            char res[size];
            if (read(s[1], res, size) == -1) {
                perror("Error on reading string in parent");
                exit(5);
            }

            printf("Result: %s\n", res);

            close(s[1]);
        }
    }

    return 0;
}
