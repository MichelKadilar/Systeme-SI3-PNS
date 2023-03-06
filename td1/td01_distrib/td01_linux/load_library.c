#include "sort.h"
#include <dlfcn.h>
#include <stdlib.h>

static void (*func)(int list[], int size);

static void *ref;

void load_library(char *library_name) {
    ref = dlopen(library_name, RTLD_LAZY);
    if (!ref) {
        exit(1);
    }
    func = dlsym(ref, "sort");
    if ((dlerror()) != NULL || func == NULL) {
        exit(1);
    }
}

void unload_library() {
    if (ref != NULL) {
        dlclose(ref);
    }
}

void sort(int list[], int size) {
    func(list, size);
}
