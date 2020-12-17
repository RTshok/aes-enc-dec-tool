#include "io.h"

#include <stdlib.h>

unsigned char *file_read(const char *file_path, size_t *file_len)
{
  FILE *file;
  file = fopen(file_path, "rb");

  if (NULL == file) {
    printf("Can't open file at location : %s !\n", file_path);
    return NULL;
  }


  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  unsigned char *data = malloc(size * sizeof(*data));
  *file_len           = size;

  if (NULL == data) {
    printf("Bad memory allocation!\n");
    fclose(file);
    return NULL;
  }

  size_t ret_len = fread(data, sizeof(*data), size, file);

  if (ret_len != size) {
    printf("Bad fread()!\n");
    fclose(file);
    free(data);
    return NULL;
  }

  fclose(file);
  return data;
}

int file_write(const char *file_path, const unsigned char *data, size_t data_size)
{
  FILE *file;
  file = fopen(file_path, "w+b");

  if (NULL == file) {
    printf("Can't open file at location : %s!\n", file_path);
    return -1;
  }

  size_t ret_len = fwrite((const void *)data, sizeof(unsigned char), data_size, file);

  if (ret_len != data_size) {
    printf("Bad fwrite()!\n");
    fclose(file);
    return -1;
  }

  fclose(file);
  return ret_len;
}