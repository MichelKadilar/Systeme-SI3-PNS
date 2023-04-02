#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256
#define NB_ARG_MAX 50
#define separators " "

void system2(char args[]) {
    switch (fork()) {
        case -1: {
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
        }
        case 0: {
            printf("%d\n", system(args));
        }
        default: {
            wait(NULL);
        }
    }
}


int main() {
    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, stdin) != NULL) { // renvoie le pointeur buf
        // attention, fgets mets automatiquement un \n à la fin, on le retire
        buf[strlen(buf) - 1] = '\0';
        system2(buf);
    }
}
