#include "headers/timer.h"

#ifdef _WIN32

#include <windows.h>

// Returns time point in microseconds (us)
uint64_t get_time() {
    LARGE_INTEGER count;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&count);
    QueryPerformanceFrequency(&frequency);
    return (uint64_t)((count.QuadPart * 1000000) / frequency.QuadPart);
}

#else

#include <time.h>

// Returns time point in microseconds (us)
uint64_t get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // Calculate microseconds: (seconds * 1,000,000) + (nanoseconds / 1000)
    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

#endif

// Calculates elapsed time in milliseconds (ms) from a starting time point (us)
double get_elapsed_ms(uint64_t start_time) {
    uint64_t current_time = get_time();
    uint64_t elapsed_us = current_time - start_time;
    // Convert microseconds to milliseconds
    return (double)elapsed_us / 1000.0;
}