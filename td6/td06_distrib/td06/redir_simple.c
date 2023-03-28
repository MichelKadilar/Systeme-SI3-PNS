#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR_OPENING_FILE (-1)
#define ERROR_EXIT 1

int main(void){
    int fd = open("foo", O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR);
    if(fd==ERROR_OPENING_FILE){
        printf("erreur ouverture de foo\n");
        exit(ERROR_EXIT);
    }
    dup2(fd, STDOUT_FILENO); // STDOUT_FILENO = 1 -> sortie standard
    close(fd);
    execlp("ls", "ls", "-l", NULL);
    return ERROR_EXIT; // si on est là, c'est que le execlp a échoué
}