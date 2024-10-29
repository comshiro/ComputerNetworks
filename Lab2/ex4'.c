#include <stdio.h>      
#include <stdlib.h>  
#include <unistd.h>     
#include <sys/wait.h>  
#include <signal.h>  


int main(){

    pid_t pid;
    int rv;

    switch(pid=fork()) {
case -1:
    perror("fork");
    exit(1);
case 0:
    char *arg_list[]={"ls", "-a", "-l", NULL};
    execvp("/bin/ls", arg_list);
    perror("execv");
    exit(2);
default:

wait(&rv);
if (WIFEXITED(rv)) {
                printf(" Fiul s-a terminat cu %d", WEXITSTATUS(rv));
            } else {
                printf("Fiul nu s-a terminat normal\n");
            }

            printf("Mesajul tatalui: Procesul fiu s-a terminat.\n");
    }

    return 0;
}
