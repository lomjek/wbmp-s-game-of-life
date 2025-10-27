#ifndef DIR_IO
#define DIR_IO

#include <stdbool.h>
#include <string.h>

bool is_valid_dir(const char *path);

bool create_dir(const char *path);

#endif //DIR_IO