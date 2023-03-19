#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 5

void *func(void *arg)
{
    sleep(1 + rand() % 5);

    long n = *(long*) arg;

    printf("Dans la thread #%ld\n", n);

    return NULL;
}

int main()
{
    long i;
    pthread_t threads[MAX];
    long tab[MAX];

    /* Creation des threads */
    for (i = 0; i < MAX; i++) {
        tab[i] = i;
        pthread_create(&threads[i], NULL, func, (void *)&tab[i]);
    }

    /* On attend que ça se termine */
    for (i = 0; i < MAX; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
