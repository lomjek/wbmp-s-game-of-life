#ifndef DIR_IO
#define DIR_IO

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool is_valid_dir(const char *path);

bool create_dir(const char *path);

bool file_exists(const char *filename);

bool is_dir_empty(const char *path);

#endif //DIR_IO