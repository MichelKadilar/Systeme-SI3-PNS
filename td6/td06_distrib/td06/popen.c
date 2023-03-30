#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>

#define ERROR_EXIT 1
#define EXIT_OK 0
#define BUFFER_SIZE 256
#define EXEC_SHELL "/bin/sh"

int mypopen(int argc_without_program_name, char *args[]);

void lecture(int fd);


int main(int argc, char *argv[]) {
    if (argc > 1) {
        int fd = mypopen(argc - 1, &argv[1]); // à partir du premier argument après le nom du programme
        lecture(fd);
        close(fd);
    } else {
        printf("pas assez d'arguments\n");
    }
}

int mypopen(int argc_without_program_name, char *args[]) {
    int fd[2];
    pipe(fd);
    pid_t child_pid;
    switch (child_pid = fork()) {
        case -1 : // erreur de l'instruction fork, pas assez de mémoire pour cloner le processus
            perror("fork");
            exit(ERROR_EXIT);
        case 0 : // execution du premier fils
            close(fd[0]);
            char buf[BUFFER_SIZE * 2] = {};
            int cpt = 0;
            for (int i = 0; i < argc_without_program_name; i++) {
                for (int j = 0; args[i][j] != '\0'; j++) {
                    buf[cpt++] = args[i][j];
                }
                buf[cpt++] = ' ';
            }
            buf[cpt++] = '\0';
            dup2(fd[1], STDOUT_FILENO);
            execlp("/bin/sh", "sh", "-c", buf,
                   NULL); // path to executable, argv[0] of the new program, args of the new program
            exit(ERROR_EXIT); // Si on est là, c'est que execlp a échoué
            break;
        default : // execution du parent
        {
            close(fd[1]);
            wait(NULL);
            return fd[0];
        }
    }
}

void lecture(int fd) {
    char buf[BUFFER_SIZE];
    ssize_t nb_byte_read;
    while ((nb_byte_read = read(fd, buf, sizeof(buf))) != 0) {
        write(STDOUT_FILENO, buf, nb_byte_read);
    }
}