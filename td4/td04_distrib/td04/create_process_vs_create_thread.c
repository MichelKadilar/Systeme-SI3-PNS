
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include "timer.h"
#include <sys/wait.h>

#define NB_OF_INSTANCES 100

void *fun(void *arg) {
    return NULL;
}

int main(void) {
    pthread_t th, th2;
    int retour;
    //struct timespec vartime = timer_start();
    for (int i = 0; i < NB_OF_INSTANCES; i++) {
        pthread_create(&th, NULL, fun, NULL);
        pthread_join(th, (void *) &retour);
    }
    //long t = timer_end(vartime);


}
