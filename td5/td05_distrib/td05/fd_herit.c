#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <wait.h>

#define ERROR (1)
#define WELL (0)
#define BUFFER_SIZE 256

void get_file_descriptor_table(pid_t pid) {
    char buf[BUFFER_SIZE];
    sprintf(buf, "/proc/%d/fd/", pid);

    DIR *dir = opendir(buf);
    struct dirent *d;
    while ((d = readdir(dir))) {
        if (!is_dot_dir(d->d_name)) {
            printf("%s\t", d->d_name);
        }
    }
    printf("\n");
    closedir(dir);
}

int main(void) {
    pid_t child_pid;
    printf("Pid du processus père : %d\n", getpid());
    int fd = open("toto.txt", O_WRONLY | O_CREAT | O_APPEND, S_IWUSR);
    if (fd == -1) {
        printf("Erreur dans l'ouverture du fichier\n");
        exit(ERROR);
    }

    DIR *dir = opendir("./Tests");
    struct dirent *d;

    char *buf;
    switch (child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork failed\n");
            exit(ERROR);
        case 0 : // execution du fils
            // message s'affiche lorsque le fils prend fin. Buffer affiché puis flush.
            printf("Pid de l'enfant : %d\n", getpid());

            printf("Affichage de la table de descripteurs de fichier au début du fils :\n");
            get_file_descriptor_table(getpid());

            /* Juste pour ajouter un descripteur de fichier dans la table du fils */
            int fd2 = open("titi.txt", O_WRONLY | O_CREAT | O_APPEND, S_IWUSR);
            if (fd2 == -1) {
                printf("Erreur dans l'ouverture du fichier\n");
                exit(ERROR);
            }
            /* fin de l'inutile */

            buf = "Exercice 4 - On teste l'écriture dans un fichier depuis le fils\n";
            if (write(fd, buf, strlen(buf)) <= 0) {
                printf("Erreur lors de l'écriture depuis le fils\n");
                exit(ERROR);
            }

            printf("Affichage des fichiers d'un répertoire dans le fils\n");
            while ((d = readdir(dir)) != NULL) { // on parcourt tous les fichiers du répertoire courant
                printf("%s\t", d->d_name);
            }
            printf("\n");

            printf("Affichage de la table de descripteurs de fichier à la fin du fils :\n");
            get_file_descriptor_table(getpid());

            closedir(dir);
            close(fd);
            exit(WELL);
        default : // execution du parent
        {
            sleep(1);
            printf("Affichage de la table de descripteurs de fichier au début du père :\n");
            get_file_descriptor_table(getpid());
            int child_status;
            wait(&child_status); // récupération du statut inutile car on ne fait rien avec.
            buf = "Exercice 4 - On teste l'écriture dans un fichier depuis le père\n";

            if (write(fd, buf, strlen(buf)) <= 0) {
                printf("Erreur lors de l'écriture depuis le père\n");
                exit(ERROR);
            }

            printf("Affichage des fichiers d'un répertoire dans le père\n");
            while ((d = readdir(dir)) != NULL) { // on parcourt tous les fichiers du répertoire courant
                printf("%s\t", d->d_name);
            }
            printf("\n");

            printf("Affichage de la table de descripteurs de fichier à la fin du père :\n");
            get_file_descriptor_table(getpid());
            closedir(dir);
            close(fd);
        }
    }
    return 0;
}