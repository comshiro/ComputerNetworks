#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

volatile sig_atomic_t afisari = 0;
volatile sig_atomic_t ignore_sigint = 1; 

void handle_sigusr1(int sig) {
    FILE *file = fopen("semnal.txt", "a");
    if (file == NULL) {
        perror("Eroare la deschiderea fișierului");
        exit(1);
    }
    fprintf(file, "Am primit semnal\n");
    fclose(file);
}

void handle_sigint(int sig) {
    if (ignore_sigint) {
        printf("SIGINT este ignorat în primele 60 de secunde.\n");
    } else {
        signal(SIGINT, SIG_DFL);
        raise(SIGINT);
    }
}

void allow_sigint(int sig) {
    ignore_sigint = 0;
    printf("Semnalul SIGINT nu mai este ignorat.\n");
}

int main() {
    signal(SIGUSR1, handle_sigusr1);
    signal(SIGINT, handle_sigint);

    alarm(60);
    signal(SIGALRM, allow_sigint);

    while (1) {
        afisari++;
        printf("Afisare #%d - PID: %d\n", afisari, getpid());
        sleep(3);
    }

    return 0;
}

