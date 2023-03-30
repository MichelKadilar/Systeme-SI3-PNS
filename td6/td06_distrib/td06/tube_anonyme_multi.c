#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define ERROR_EXIT 1
#define EXIT_OK 0
#define ALPHABET_MIN "abcdefghijklmnopqrstuvwxyz"
#define ALPHABET_MAJ "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ALPHABET_LENGTH 26
#define WRITING_SIZE 2
#define READING_SIZE 3
#define ARRAY_SIZE 10

void lecture(int fd);

int main(void) {
    int fd[2];
    if (pipe(fd) != 0) exit(ERROR_EXIT);
    // close(fd[0]); // Si on décommente, on n'écrit rien dans le tube car on ferme le lecteur avant l'écrivain.

    pid_t first_child_pid;
    switch (first_child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork");
            exit(ERROR_EXIT);
        case 0 : // execution du premier fils
            close(fd[0]); // on ferme le lecteur/la sortie du tube
            char *alph1 = ALPHABET_MIN;
            for (int i = 0; i < ALPHABET_LENGTH; i += WRITING_SIZE) {
                write(fd[1], &alph1[i], WRITING_SIZE); // la tête de lecture se déplace à chaque fois de 2 caractères
                //sleep(1);
            }
            exit(EXIT_OK);
            break;
        default : // execution du parent
        {
            pid_t second_child_pid;
            switch (second_child_pid = fork()) {
                case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
                    perror("fork");
                    exit(ERROR_EXIT);
                case 0 : // execution du second fils
                    close(fd[0]); // on ferme le lecteur/la sortie du tube
                    char *alph2 = ALPHABET_MAJ;
                    for (int i = 0; i < ALPHABET_LENGTH; i += WRITING_SIZE) {
                        write(fd[1], &alph2[i],
                              WRITING_SIZE); // la tête de lecture se déplace à chaque fois de 2 caractères
                        // sleep(1); première version
                        sleep(2);
                    }
                    exit(EXIT_OK);
                    break;
                default :
                    break; // execution du parent
            }

            close(fd[1]);
            lecture(fd[0]);
            close(fd[0]); // moyennement utile ici, puisque la fin du père va de toute façon réaliser cette action
            // a ma place juste après le sleep()
            break;
        }
    }
}

void lecture(int fd) {
    char buf[READING_SIZE];
    int really_read;
    while ((really_read = read(fd, buf, READING_SIZE) != 0)) { // on lit depuis le fichier pointé par fd
        // 3 caractères par 3 caractères, qu'on stocke dans "buf",
        write(STDOUT_FILENO, buf, really_read); // On écrit le caractère c sur la sortie standard du processus
    }
    close(fd); // on ferme le lecteur/la sortie du tube.
}