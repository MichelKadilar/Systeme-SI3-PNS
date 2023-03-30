#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        printf("Erreur ouverture en lecture du tube nommé\n");
        exit(-1);
    }
    int i;
    while(read(fd, &i, 1) != 0){
        write(STDOUT_FILENO, &i, sizeof(int ));
    }
    close(fd);
    exit(0);
}