#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define P 10
#define N 10

int main(void) {
    for (int i = 0; i < P; i++) {
        pid_t child_pid;
        switch (child_pid = fork()) {
            case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
                perror("fork");
                exit(-1);
            case 0 : // execution du fils
                for (int n = 0; n < N; n++) {
                    printf("fils: %d\n", i);
                }
                sleep(1);
                exit(i);
                break;
            default : // execution du parent
            {
                int child_status;
                printf("parent\n");
                waitpid(child_pid, &child_status, 0);
                break;
            }
        }


    }
}