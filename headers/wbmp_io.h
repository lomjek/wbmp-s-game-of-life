#ifndef WBMP_IO
#define WBMP_IO

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "headers/uintvar.h"

size_t load_wbmp(char *file_path, unsigned int *width, unsigned int *height, uint8_t **buffer);

int save_wbmp(char *file_path, uint8_t *img_data, unsigned int width, unsigned int height, size_t data_length);

#endif //WBMP_IO
