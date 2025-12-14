#ifndef TIMER
#define TIMER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint64_t get_time();

double get_elapsed_ms(uint64_t start_time);

void print_timestamp(double start_time);

#endif // TIMER