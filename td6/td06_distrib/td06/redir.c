/*							-*- coding: utf-8 -*-
 *
 * redir.c	-- Redirection fichiers
 *
 * Erick Gallesio (2012-03-09)
 * Stéphane Lavirotte (2017/03/23)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ 0
#define WRITE 1
#define ERROR_EXIT 1

void redirect(int std_fd, char *filename, char *const args[]);

void usage(char *progname) {
    fprintf(stderr, "Usage: %s direction file cmd arg ...\n", progname);
    fprintf(stderr, "       direction = R or W\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    int std_fd = -1;

    if (argc < 4) {
        usage(*argv);
    }

    /* Vérifier que le premier argument est bien un R ou un W */
    switch (argv[1][0]) {
        case 'R':
        case 'r':
            std_fd = READ;
            break;
        case 'W':
        case 'w':
            std_fd = WRITE;
            break;
        default:
            usage(*argv);
    }

    redirect(std_fd, argv[2], (char *const *) &argv[3]);
}

void redirect(int std_fd, char *filename, char *const args[]) {
    int fd;

    if (std_fd == READ) { // Si on veut lire depuis un fichier
        fd = open(filename, O_RDONLY); // lecture seule
        if (fd == -1) {
            printf("Erreur dans l'ouverture en lecture du fichier\n");
            exit(ERROR_EXIT);
        }
        dup2(fd, STDIN_FILENO); // STDIN_FILENO = 0 -> entrée standard
        close(fd);
        execvp(*args, args);
    } else { // Si on veut écrire dans un fichier
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC,
                  S_IWUSR); // écriture seule, si le fichier n'existe pas, on le crée
        if (fd == -1) {
            printf("Erreur dans l'ouverture en écriture du fichier\n");
            exit(ERROR_EXIT);
        }
        dup2(fd, STDOUT_FILENO); // STDOUT_FILENO = 1 -> sortie standard
        close(fd);
        execvp(*args, args);
    }
}
