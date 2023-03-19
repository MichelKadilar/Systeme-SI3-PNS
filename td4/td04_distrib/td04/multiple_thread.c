#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include "timer.h"

#define NTHREAD 100

void wait_thread(int, pthread_t[]); // Définition du prototype de fonction pour l'utiliser
void *sleep_threads(void *); // Définition du prototype de fonction pour l'utiliser

        long create_thread(int n) {
    struct timespec vartime = timer_start(); /* Démarrage de la mesure temporelle */
    pthread_t th[n];
    /* Création de n threads s'exécutant en parallèle */
    for (int i = 0; i < n; i++) {
        th[i] = pthread_create(&(th[i]), NULL, sleep_threads, NULL);
    }
    /* On mesure le temps écoulé pour la création des n processus */
    long time = timer_end(vartime);

    /* On attend la fin des processus créés par mesure sociale et pour éviter un impact sur les prochaine mesures */
    wait_thread(n, th);

    return time; /* Retourne le temps écoulé pour réaliser la création des n processus */
}

void *sleep_threads(void *arg) {
    sleep(10);
    return NULL;
}

void wait_thread(int n, pthread_t th[]) {
    /* Mesure de salubrité sociale, le père attend la fin de tous ses fils */
    int retour;
    for (int i = 0; i < n; ++i)
        pthread_join(th[i], (void *) &retour);
}

int main(int argc, char *argv[]) {
    int n = NTHREAD;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            n = atoi(argv[i]);
        }
    }

    long time_processus = create_thread(n);

    printf("Time taken for creating %d threads (nanoseconds): %ld\n", n, time_processus);
    printf("Time taken for creating %d threads (milliseconds): %ld\n", n, time_processus / NANO_TO_MILLI);

    /* On flush la sortie standard */
    fflush(stdout);

    /* Fin du thread principal */
    exit(0);
}

