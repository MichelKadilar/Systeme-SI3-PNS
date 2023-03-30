#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 256

int main(int argc, char *argv[]){
    int fd = open(argv[1], O_WRONLY | O_APPEND);
    if(fd == -1){
        printf("Erreur ouverture en écriture du tube nommé\n");
        exit(-1);
    }

    char buf[BUFFER_SIZE];
    int nb_char_read;
    while((nb_char_read = read(STDOUT_FILENO, buf, BUFFER_SIZE * sizeof(char))) != 0){
        write(fd, buf, nb_char_read * sizeof(char));
    }
    close(fd);
    exit(0);
}