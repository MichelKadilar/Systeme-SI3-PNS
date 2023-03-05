#include "sort.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

static void (*func)(int list[], int size);

void load_library(char *library_name) {
    void *ref = dlopen(library_name, RTLD_LAZY);
    if (!ref) {
        exit(1);
    }
    func = dlsym(ref, "sort");
    if ((dlerror()) != NULL || func == NULL) {
        exit(1);
    }
}

void sort(int list[], int size) {
    func(list, size);
}
