#include <sys/types.h>
#include <unistd.h>

pid_t get_pid(){ // pid_t = long int
    return getpid();
}
