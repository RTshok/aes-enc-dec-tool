#include <stdio.h>

#define MAX_PATH_LENGTH 255

unsigned char *file_read(const char *file_path, size_t *file_len);
int            file_write(const char *file_path, const unsigned char *data, int data_size);