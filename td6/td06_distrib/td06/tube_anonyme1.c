#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define READ 0
#define WRITE 1
#define ERROR_EXIT 1
#define ARRAY_SIZE 10

void lecture(int fd);

int main(void) {
    int fd[2];
    if (pipe(fd) != 0) exit(ERROR_EXIT);
    char numbers[ARRAY_SIZE] = "0123456789";
    // close(fd[0]); // Si on décommente, on n'écrit rien dans le tube car on ferme le lecteur avant l'écrivain.
    if (write(fd[1], numbers, strlen(numbers)) == 0) exit(ERROR_EXIT); // On écrit "0123456789" dans le tube grâce au
    // descripteur de fichier fd[1] qui pointe vers l'entrée du tube.
    close(fd[1]); // on ferme l'écrivain/l'entrée du tube
    lecture(fd[0]);
    return 0;
}

void lecture(int fd) {
    char c;
    while (read(fd, &c, 1) != 0) { // on lit depuis le fichier pointé par fd
        // 1 caractère par 1 caractère, qu'on stocke dans "c",
        write(STDOUT_FILENO, &c, 1); // On écrit le caractère c sur la sortie standard du processus
    }
    close(fd); // on ferme le lecteur/la sortie du tube.
}