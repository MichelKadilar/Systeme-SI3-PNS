#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#define NB_SIGINT_WANTED 5
#define TRUE 1

static int nb_sigint_received = 0;

void on_signal(int sig) {
    printf("%d ***signal\n", sig);
    if (sig == SIGINT) {
        nb_sigint_received++;
    }
    while(TRUE) {
        // boucle infinie volontaire pour avoir le temps de lancer des signaux.
    }
    if (nb_sigint_received == NB_SIGINT_WANTED) {
        kill(getpid(), SIGKILL);
    }
}

int main(void) {
    struct sigaction sigact;
    sigset_t msk_int, msk_segv;

    // On gère notre ensemble de masques de signaux
    sigemptyset(&msk_int);
    sigaddset(&msk_int, SIGINT);
    sigemptyset(&msk_segv);
    sigaddset(&msk_segv, SIGSEGV);
    // Fin de la gestion des masques des signaux

    // Gestion des signaux à leur reception
    sigact.sa_handler = on_signal; // Règle : Quand on recevra un signal défini, on ira dans la fonction on_signal
    sigact.sa_mask = msk_segv; // Règle : Quand on recevra un signal défini, on ignorera le signal SIGSEGV pendant
    // la réception et l'exécution du handler du signal défini
    sigaction(SIGINT, &sigact, NULL); // On applique les deux règles précédentes à la réception d'un signal SIGINT
    // Quand on va recevoir un signal SIGINT, on va aller dans on_signal et on va ignorer les signaux SIGSEGV
    sigact.sa_mask = msk_int; // Règle : Quand on recevra un signal défini, on ignorera le signal SIGINT pendant
    // la réception et l'exécution du handler du signal défini
    sigaction(SIGSEGV, &sigact, NULL); // On applique la règle précédente et sa_handler pour le signal SIGSEGV

    unsigned int i = sleep(60);
    printf("time remaining : %d\n", i);
    return 0;
}
