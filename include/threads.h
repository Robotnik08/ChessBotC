#ifndef THREADS_H
#define THREADS_H

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>

    typedef HANDLE Thread;
    typedef DWORD ThreadReturn;
    #define THREAD_CALLCONV WINAPI

    #define START_THREAD(thread, func, arg) \
        *(thread) = CreateThread(NULL, 0, func, arg, 0, NULL)

    #define JOIN_THREAD(thread) \
        WaitForSingleObject(thread, INFINITE); \
        CloseHandle(thread)

    #define SLEEP(seconds) Sleep((seconds) * 1000)

#else
    #include <pthread.h>
    #include <unistd.h>

    typedef pthread_t Thread;
    typedef void* ThreadReturn;
    #define THREAD_CALLCONV

    #define START_THREAD(thread, func, arg) \
        pthread_create(thread, NULL, func, arg)

    #define JOIN_THREAD(thread) \
        pthread_join(thread, NULL)

    #define SLEEP(seconds) sleep(seconds)
#endif

#endif // THREADS_H