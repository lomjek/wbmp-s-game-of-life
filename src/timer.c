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

void print_timestamp(const double milliseconds) {
    double number;
    const char *unit;

    // Determine value and unit
    if (milliseconds >= 1000.0) {
        number = milliseconds / 1000.0;
        unit = "s";
    }
    else {
        number = milliseconds;
        unit = "ms";
    }

    // Print the formatted string directly.
    // "%8.2f":
    // 8 is the minimum field width (4 digits before the dot + 1 dot + 2 digits after the dot + 1 for the space padding/sign/etc if needed, ensuring at least 4 digits before the dot are covered with padding)
    // .2 is for two digits after the decimal point
    // The default padding for floats is spaces, which meets the requirement.
    printf("%8.2f%s\n", number, unit);
}