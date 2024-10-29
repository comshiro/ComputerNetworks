/*
1. Scrieti un program in care se foloseste un singur pipe pentru
comunicare si in care:
• tatal scrie intr-un pipe un numar;
• fiul verifica daca numarul este prim si transmite prin pipe tatalui
raspunsul (yes, no);
• tatal va afisa raspunsul primit. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

bool isPrime(int n)
{
    if (n <= 1)
        return false;
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

int main()
{
    int pid, p[2];
    if (pipe(p) == -1)
        perror("Error in creating pipe");

        int number;
    printf("Enter a number: ");
    scanf("%d", &number); 
    
    pid = fork();

    switch (pid)
    {

    case -1:
    {
        perror("Error on forking");
        break;
    }
    case 0: 
    {
        if (read(p[0], &number, sizeof(int)) == -1) {
            perror("Error on reading number from parent");
            exit(1);
        }

        close(p[0]);
        
        char result[4];
        if (isPrime(number))
            strcpy(result, "yes");
        else
            strcpy(result, "no");

        if (write(p[1], result, strlen(result) + 1) == -1) {
            perror("Error on writing result to parent");
            exit(1);
        }

        exit(0);
    }
    default:
    {
            if (write(p[1], &number, sizeof(int)) == -1) 
            perror("Error on writing number to child");
        close(p[1]);

        char response[4];
        wait(NULL);
        if (read(p[0], response, 4) == -1)
            perror("Error on reading number");
        response[strlen(response)] = 0;
        printf("%s", response);
    }
    }

    return 0;
}