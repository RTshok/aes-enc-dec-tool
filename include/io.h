#include <stdbool.h>
#include <stdio.h>

#define MAX_PATH_LENGTH 255

unsigned char *file_read(const char *file_path, size_t *file_len);
int            file_write(const char *file_path, const unsigned char *data, size_t data_size);

int file_encrypt (const char *input_file_path, const char *output_file_path, const unsigned char *iv, const unsigned char *key, unsigned int magic_number, bool verbose, size_t block_size);
int file_decrypt (const char *input_file_path, const char *output_file_path, const unsigned char *iv, const unsigned char *key, unsigned int magic_number, bool verbose, size_t block_size);
