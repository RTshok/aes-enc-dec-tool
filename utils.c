#include "utils.h"

#include <malloc.h>
#include <string.h>

int append_header(unsigned char *in_data, uint32_t crc, uint32_t magic_number, uint32_t size)
{
  unsigned char *new_data = realloc(in_data, size + (uint8_t) sizeof(struct header)); // allocating new memory for header

  if(new_data == NULL) {
    printf("Bad memory allocation ! \n");
    return -1;
  }

  in_data = new_data;

  struct header header = {magic_number, size, crc};
  memcpy(&in_data[size + 1], (unsigned char *) &header, (uint8_t) sizeof(struct header)); //appending header

  return 0;
}


void print_header(const unsigned char *data, uint32_t size)
{
  struct header header = {};
  // perform validation of input data
  memcpy(&header, &data[size - sizeof(struct header)], sizeof(struct header));

  printf("magic number : %08x\n", header.magic_number);
  printf("size in bytes : %d\n", header.size);
  printf("crc32 : %08x\n", header.crc32);
}

struct header get_header (const unsigned char * data)
{
  uint32_t size = strlen(data);
  struct header header = {};
  // perform validation of input data
  memcpy(&header, &data[size - sizeof(struct header)], sizeof(struct header));

  return header;
}