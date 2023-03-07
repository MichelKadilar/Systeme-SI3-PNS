#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(void) {
    pid_t child_pid;
    switch (child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork");
            exit(-1);
        case 0 : // execution du fils
            exit(0);
            break;
        default : // execution du parent
        {
            printf("parent\n");
            sleep(5);
            break;
        }
    }
}