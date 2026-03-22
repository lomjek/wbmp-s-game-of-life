#ifndef UINTVAR
#define UINTVAR

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>


bool is_uintvar_done(uint8_t byte);

unsigned int uintvar_to_int(const uint8_t *number);

size_t int_to_uintvar(unsigned int value, uint8_t *number);

#endif // UINTVAR
