
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#define NB_LOOP 5

void *thread_print_info(void *arg) {
    int sleep_time = *((int *) arg);
    if (sleep_time < 0) pthread_exit((void *) 1);

    for (int i = 0; i < NB_LOOP; i++) {
        printf("PID du processus courant : %d, identifiant du thread POSIX courant : %ld\n", getpid(), pthread_self());
        printf("Linux Thread ID: %ld\n", syscall(SYS_gettid));
        sleep(*((int *) arg));
    }

    pthread_exit((void *) 0);
    return NULL;
}

long not_simultaneous_create_join_thread(pthread_t pthread, void *(*fun)(void *arg), void *arg) {
    long retour;
    pthread_create(&pthread, NULL, fun, arg);
    pthread_join(pthread, (void *) &retour);
    return retour;
}

int main(int argc, char *argv[]) {
    printf("PID du processus courant : %d\n", getpid());
   // printf("TID du processus courant : %d\n", gettid()); // TID du thread principal = PID du processus lourd
    int n1, n2;
    int parallele;
    if(argc == 4){
        n1 = atoi(argv[1]);
        n2 = atoi(argv[2]);
        parallele = atoi(argv[3]);
    }

    pthread_t th, th2;
    long retour, retour2;

    if(parallele){
        retour = not_simultaneous_create_join_thread(th, thread_print_info, (void *) &n1);
        printf("Valeur renvoyée: %ld\n", retour);

        retour2 = not_simultaneous_create_join_thread(th2, thread_print_info, (void *) &n2);
        printf("Valeur renvoyée: %ld\n", retour2);
    }
    else {
        pthread_create(&th, NULL, thread_print_info, (void *) &n1);
        pthread_create(&th2, NULL, thread_print_info, (void *) &n2);
        pthread_join(th, (void *) &retour);
        pthread_join(th2, (void *) &retour2);

        printf("Valeur renvoyée: %ld\n", retour);
        printf("Valeur renvoyée: %ld\n", retour2);
    }


    printf("The end\n");

    return 0;
}