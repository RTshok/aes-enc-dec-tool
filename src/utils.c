#include "utils.h"

#include <malloc.h>
#include <string.h>

unsigned char *append_header(unsigned char **in_data,
                             uint32_t        crc,
                             uint32_t        magic_number,
                             uint32_t        ciphered_size,
                             uint32_t        plaintext_size)
{
  unsigned char *new_data = realloc(*in_data, ciphered_size + sizeof(struct header));  // allocating new memory for header

  if (new_data == NULL) {
    printf("Bad memory allocation ! \n");
    free(*in_data);
    *in_data = NULL;
    return NULL;
  }

  struct header header = {magic_number, plaintext_size, crc};
  memcpy(&new_data[ciphered_size], (unsigned char *)&header, sizeof(struct header));  //appending header

  return new_data;
}

unsigned char *remove_header(unsigned char **in_data, size_t *size)
{
  unsigned char *new_data = malloc(sizeof(unsigned char) * (*size) - sizeof(struct header));  // create new buffer without a header
  if (NULL == new_data) {
    printf("Can't allocate memory while removing header ! \n");
    return NULL;
  }

  memcpy(new_data, *in_data, (*size) - sizeof(struct header));
  *size = *size - sizeof(struct header);
  free(*in_data);  // free old data
  *in_data = NULL;
  return new_data;
}

void print_header(const unsigned char *in_data, size_t size)
{
  struct header header = {};
  memcpy(&header, &in_data[size - sizeof(struct header)], sizeof(struct header));

  printf("\n HEADER\n");
  printf("______________________________________________________\n\n");
  printf("magic number : %08x\n", header.magic_number);
  printf("size in bytes : %d\n", header.size);
  printf("crc32 : %08x\n", header.crc32);
  printf("______________________________________________________\n");
}

struct header *get_header(const unsigned char *in_data, size_t size)
{
  struct header *header = malloc(sizeof(struct header));
  if (NULL == header) {
    printf("Can't allocate memory for header !\n");
    return NULL;
  }
  memcpy(header, &in_data[size - sizeof(struct header)], sizeof(struct header));
  return header;
}
