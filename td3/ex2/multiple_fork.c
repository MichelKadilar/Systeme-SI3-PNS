#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define P 10
#define N 10

int main(void) {
    for (int i = 0; i < P; i++) {
        switch (fork()) {
            case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
                perror("fork");
                exit(-1);
            case 0 : // execution du fils
                for (int n = 0; n < N; n++) {
                    printf("fils: %d, pid : %d\n", i, getpid());
                    sleep(1);
                }
                exit(0);
            default : // execution du parent
            {
                printf("parent\n");
                break;
            }
        }
    }

    int status;
    for (int j = 0; j < P; j++) {
        wait(&status);
    }
}