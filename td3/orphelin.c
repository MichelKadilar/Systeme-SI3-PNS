#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t child_pid;
    pid_t parent_pid = getpid();
    printf("%d\n", parent_pid);
    switch (child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork");
            exit(-1);
        case 0 : {
            while (getppid() == parent_pid) {
                sleep(5);
            }
            printf("%d\n", getppid());
            break;
        }// execution du fils
        default : // execution du parent
        {
            exit(0);
        }
    }

}
