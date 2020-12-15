#include <stdio.h>

#define MAX_PATH_LENGTH 255

int file_read(const char *file_path, unsigned char *data);
int file_write(const char *file_path, const unsigned char *data, int data_size);