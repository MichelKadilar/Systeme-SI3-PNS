#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
    printf("processus de pid %d exécutant le programme externe qui a pour nom %s\n", getpid(), argv[0]);
    return 0;
}