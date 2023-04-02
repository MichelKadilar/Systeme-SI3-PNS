#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 256
#define NB_ARG_MAX 50
#define separators " "

void get_tokenized_line(char *buf, char *delimiters, char *dest[NB_ARG_MAX]) {
    int i = 0;
    dest[i++] = strtok(buf, delimiters);
    while ((dest[i++] = strtok(NULL, delimiters)) != NULL) {
        if (i == NB_ARG_MAX - 1) exit(EXIT_FAILURE);
    }
    dest[i] = NULL;
}

void cd(char *arg) {
    int res;
    if (arg[0] == '\0') { // pas d'argument = "cd" = "cd ~" -> go HOME.
        if ((res = chdir(getenv("HOME"))) == -1) perror("chdir");
    } else {
        if ((res = chdir(arg)) == -1) perror("chdir");
    }
    printf("cd status : %d\n", res); // 0 = SUCCESS
}

void system2(char *args[]) {
    switch (fork()) {
        case -1: {
            perror("Fork failed\n");
            exit(EXIT_FAILURE);
        }
        case 0: {
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        }
        default: {
            wait(NULL);
        }
    }
}

void choose_cd_or_cmd(char *args[]) {
    printf("on entre dans le choix\n");
    //printf("toto : %s %d\n", args[0], strcmp("cd", args[0]));
    if (strcmp("cd", args[0]) == 0) {
        cd(args[1]);
    } else {
        system2(args);
    }
}


int main() {
    char buf[BUFFER_SIZE];
    while (fgets(buf, BUFFER_SIZE, stdin) != NULL) { // renvoie le pointeur buf
        // attention, fgets mets automatiquement un \n à la fin, on le retire
        buf[strlen(buf)-1] = '\0';
        char *tokens[NB_ARG_MAX] = {};  // args
        get_tokenized_line(buf, separators, tokens);
        choose_cd_or_cmd(tokens);
    }
}
