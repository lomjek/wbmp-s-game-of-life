#include "headers/timer.h"

#ifdef _WIN32

#include <windows.h>

uint64_t get_time() {
    LARGE_INTEGER count;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&count);
    QueryPerformanceFrequency(&frequency);
    return (uint64_t)((count.QuadPart * 1000000) / frequency.QuadPart);
}

#else

#include <time.h>

uint64_t get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint64_t)ts.tv_sec * 1000000 + (uint64_t)ts.tv_nsec / 1000;
}

#endif

double get_elapsed_ms(uint64_t start_time) {
    uint64_t current_time = get_time();
    uint64_t elapsed_us = current_time - start_time;
    return (double)elapsed_us / 1000.0;
}

void print_timestamp(const double milliseconds) {
    double number;
    const char *unit;

    if (milliseconds >= 1000.0) {
        number = milliseconds / 1000.0;
        unit = "s";
    }
    else {
        number = milliseconds;
        unit = "ms";
    }

    printf("%8.2f%s\n", number, unit);
}