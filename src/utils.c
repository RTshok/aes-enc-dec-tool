#include "utils.h"

void init_header (struct header *header, uint32_t crc, uint32_t magic_number, uint32_t text_size)
{
  header->crc32 = crc;
  header->magic_number = magic_number;
  header->size = text_size;
}

void print_header(struct header *header)
{
  printf("\n HEADER\n");
  printf("______________________________________________________\n\n");
  printf("magic number : %08x\n", header->magic_number);
  printf("size in bytes : %d\n", header->size);
  printf("crc32 : %08x\n", header->crc32);
  printf("______________________________________________________\n");
}

