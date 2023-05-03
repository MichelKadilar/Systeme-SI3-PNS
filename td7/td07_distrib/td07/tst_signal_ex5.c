#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define NB_SIGINT_WANTED 5
#define TRUE 1

static int nb_sigint_received = 0;

void on_signal(int sig){
    printf("%d ***signal\n", sig);
    if(sig == SIGINT){
        nb_sigint_received++;
    }
    if(nb_sigint_received == NB_SIGINT_WANTED){
        kill(getpid(), SIGKILL);
    }
    signal(sig, on_signal);
}


int main(void){
    signal(SIGSEGV, on_signal);
    signal(SIGINT, on_signal);

    while(TRUE) {
        /* boucle infinie volontaire pour avoir le temps de lancer des signaux */
    }
    return 0;
}