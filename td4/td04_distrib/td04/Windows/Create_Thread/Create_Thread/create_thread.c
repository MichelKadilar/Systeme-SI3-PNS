#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <stddef.h>
#include <tchar.h>

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    char* str = (char*)lpParam;
    printf("%s\n", str);

    DWORD tid = GetCurrentThreadId();
    DWORD pid = GetCurrentProcessId();
    printf("Je suis le thread supplementaire d'id : %ul, je fais partie du processus : %d\n", tid, pid);

    Sleep(1000);
    ExitThread(0);
}

int main(void) {

    DWORD pid = GetCurrentProcessId();
    printf("PID du processus : %d\n", pid);

    DWORD tid = GetCurrentThreadId();
    printf("Je suis le thread principal d'id : %ul, je fais partie du processus : %d\n", tid, pid);

    char* str = "Quelle belle journee pour experimenter les threads sur Windows !";
    unsigned long thread_id;
    HANDLE thread_handler = CreateThread(
        NULL,                   // default security attributes
        0,                      // use default stack size  
        MyThreadFunction,       // thread function name
        str,          // argument to thread function 
        0,                      // use default creation flags 
        NULL);   // returns the thread identifier 


    if (thread_handler == NULL)
    {
        printf("ERROR IN THREAD CREATION\n");
        ExitProcess(1);
    }

    WaitForSingleObject(thread_handler, INFINITE);

    return 0;
}