#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

static int aine_pid;
static int cadet_pid;

void handler_aine(int sig) {
    if (sig == SIGUSR2) {
        printf("SIGUSR2 received in the aine\n");
        printf("Aine is ending\n");
        exit(EXIT_SUCCESS);
    } else {
        printf("something received %d\n", sig);
    }
}

void handler_cadet(int sig) {
    if (sig == SIGUSR1) {
        kill(aine_pid, SIGUSR2);
        printf("Cadet is ending\n");
        exit(EXIT_SUCCESS);
    }
}

int main(void) {
    switch ((aine_pid = fork())) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
        {
            perror("fork failed\n");
            exit(EXIT_FAILURE);
        }
        case 0 : // execution du fils aine
        {
            struct sigaction sa;
            sa.sa_handler = handler_aine;
            sigaction(SIGUSR2, &sa, NULL);
            while (1) {};
            exit(EXIT_FAILURE);
        }
        default : // execution du parent
        {
            switch ((cadet_pid = fork())) {
                case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
                    perror("fork failed\n");
                    exit(EXIT_FAILURE);
                case 0 : // execution du fils cadet
                {
                    struct sigaction sa;
                    sa.sa_handler = handler_cadet;
                    sigaction(SIGUSR1, &sa, NULL);
                    while (1) {};
                    exit(EXIT_FAILURE);
                }
                default : // execution du parent
                {
                    sleep(1);
                    printf("Sending SIGUSR1 to cadet\n");
                    kill(cadet_pid, SIGUSR1);
                    printf("Waiting for children\n");
                    while (wait(NULL) < 0) {}
                    printf("Ending the program\n");
                }
            }
        }
    }
}