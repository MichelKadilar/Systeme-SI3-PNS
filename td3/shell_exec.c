#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERROR (-1) // ces macros ne servent en fait pas à grand chose puisque je ne fais rien des codes de sorties.
#define WELL (1)

int main(void) {
    int res;
    switch (fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork");
            exit(ERROR);
        case 0 : { // execution du fils
            res = execlp("who", "who", NULL);
            if (res == ERROR) {
                printf("Erreur du execlp\n");
                break;
            }
            break;
        }
        default : {// execution du parent
            wait(NULL);
            switch (fork()) {
                case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
                    perror("fork");
                    exit(ERROR);
                case 0 : { // execution du fils
                    int res = execlp("ls", "ls", "-ls", NULL);
                    if (res == ERROR) {
                        printf("Erreur du execlp\n");
                        exit(ERROR);
                    }
                    exit(WELL);
                }
                default : { // execution du parent
                    wait(NULL);
                    break;
                }
            }
        }
    }
    return 0;
}