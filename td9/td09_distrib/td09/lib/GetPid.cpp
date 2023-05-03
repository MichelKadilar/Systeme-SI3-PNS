#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include "GetPid.h"

JNIEXPORT jlong JNICALL Java_GetPid_getPid(JNIEnv *, jclass) { // pid_t = long int
    return getpid();
}
