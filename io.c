#include "io.h"

#include <malloc.h>

int file_read(const char *file_path, unsigned char *data)
{

  FILE* file;
  file = fopen(file_path, "rb");

  if(NULL == file) {
    printf("Can't open file at location : %s!", file_path);
    return -1;
  }

  fseek(file, 0L, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0L, SEEK_SET);

  data = malloc(size * sizeof(*data));

  if(NULL == data)
  {
    printf("Bad memory allocation!\n");
    fclose(file);
    return -1;
  }
  
  size_t ret_len = fread(data, sizeof(*data), size, file);
  if(ret_len != size)
  {
    printf("Bad fread()!\n");
    fclose(file);
    // free data ?
    return -1;
  }

  fclose(file);
  return size;
}

int file_write(const char *file_path, const unsigned char *data, int data_size)
{
  FILE* file;
  file = fopen(file_path, "wb");

  if(NULL == file) {
    printf("Can't open file at location : %s!", file_path);
    return -1;
  }

  size_t ret_len = fwrite(data, sizeof (*data), data_size, file);

  if(ret_len != data_size)
  {
    printf("Bad fwrite()!\n");
    fclose(file);
    return -1;
  }

  return ret_len;
}