#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERROR (-1)
#define WELL (1)

int main(void) {
    pid_t child_pid;
    printf("pid du processus père : %d\n", getpid());
    switch (child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork failed");
            exit(ERROR);
        case 0 : // execution du fils
            printf("pid de l'enfant : %d | ",
                   getpid()); // message s'affiche lorsque le fils prend fin. Buffer affiché puis flush.
            exit(WELL);
        default : // execution du parent
        {
            int child_status;
            wait(&child_status); // récupération du statut inutile car on ne fait rien avec.
            printf("Message qui ne va pas s'afficher car le buffer n'est pas encore plein");
            int res = execlp("./exec_prop-aux.exe", "exec_prop-aux.exe", "coucou", NULL);
            if (res == ERROR) {
                puts("Message qui ne va s'afficher que si execlp rencontre une erreur");
                break;
            }
            puts("Ce message va être écrasé par execlp");
        }
    }
    return 0;
}