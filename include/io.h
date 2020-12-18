#include <stdbool.h>
#include <stdio.h>

#define MAX_PATH_LENGTH 255

int file_encrypt(const char *         input_file_path,
                 const char *         output_file_path,
                 const unsigned char *iv,
                 const unsigned char *key,
                 unsigned int         magic_number,
                 bool                 verbose);
int file_decrypt(const char *         input_file_path,
                 const char *         output_file_path,
                 const unsigned char *iv,
                 const unsigned char *key,
                 unsigned int         magic_number,
                 bool                 verbose);