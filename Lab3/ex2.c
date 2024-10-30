/* Scrieri un program care sa simuleze comanda:
cat prog.c | grep "include" > prog.c , folosind fifo-uri si primitiva dup.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main()
{
    int p[2];
    if (pipe(p) == -1)
    {
        perror("Error on pipe");
        exit(1);
    }
    switch (fork())
    {
    case -1:
        perror("Error on fork");
        exit(1);
    case 0: /* copilul */
        close(1);
        if (dup(p[1]) != 1)
        {
            perror("Error on first dup");
            exit(2);
        }
        close(p[0]);
        close(p[1]);
        execlp("cat", "cat", "prog.c", NULL);
        perror("Error om exec");
        exit(3);
    }

    switch (fork())
    {
    case -1:
    {
        perror("Error on second fork");
        exit(4);
    }
    case 0:
        close(0);
        if (dup(p[0]) != 0)
        {
            perror("Error on second dup");
            exit(1);
        }
        close(p[0]);
        close(p[1]);
        char *res = "\"include\"";
                int output_fd = open("prog.c", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("Error opening prog.c for writing");
            exit(5);
        }
        dup2(output_fd, STDOUT_FILENO); // Redirecționăm stdout către fișier
        close(output_fd); // Închidem fișierul după duplicare
        execlp("grep", "grep", res, NULL);
        perror("Error on second exec");
        exit(1);
    }
    close(p[0]);
    close(p[1]);
    while (wait(NULL) != -1);
    return 0;
}
