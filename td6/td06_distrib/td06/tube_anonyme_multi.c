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
#define WRITING_SIZE 2
#define READING_SIZE 3
#define ARRAY_SIZE 10

void lecture(int fd);
void ecriture(int fd, const char* buf, int sleep_time);

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
            ecriture(fd[1], ALPHABET_MAJ, 1);
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
                    ecriture(fd[1], ALPHABET_MIN, 2);
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
    ssize_t nb_byte_read;
    while ((nb_byte_read = read(fd, buf, sizeof(buf))) != 0){
        write(STDOUT_FILENO, buf, nb_byte_read);
    }
}

void ecriture(int fd, const char *buf, int sleep_time){
    while(*buf != '\0'){
        write(fd, buf, WRITING_SIZE);
        buf += WRITING_SIZE;
        sleep(sleep_time);
    }
}