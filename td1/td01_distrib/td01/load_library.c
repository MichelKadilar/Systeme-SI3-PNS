#include "sort.h"
#include <dlfcn.h>
#include <stdlib.h>

void *load_library(char *library_name) {
    void *ref = dlopen(library_name, RTLD_LAZY);
    if (!ref) {
        exit(1);
    }
    void *function_reference = dlsym(ref, "sort");
    if ((dlerror()) != NULL || function_reference == NULL) {
        exit(1);
    }
    return function_reference;
}

void sort(int list[], int size){
    void (*sort)(int list[], int size) = load_library(argv[1]);
    // mais cela ne correspond pas au tri qui est dans sort.h
    sort(list, size);

}

int main(int argc, char *argv[]) {
    void (*sort)(int list[], int size); // mais cela ne correspond pas au tri qui est dans sort.h
    void *sort_function_reference = load_library(argv[1]);
    sort = sort_function_reference;

}


