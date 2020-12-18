#include <stddef.h>
#include <stdint.h>

// HEADER STRUCTURE
// [MAGIC_NUMBER - 4b] [DATA SIZE - 4b] [CRC32 - 4b]

struct header {
  uint32_t magic_number;
  uint32_t size;
  uint32_t crc32;
};

unsigned char *append_header(unsigned char **in_data,
                             uint32_t        crc,
                             uint32_t        magic_number,
                             uint32_t        ciphered_size,
                             uint32_t        plaintext_size);
void init_header (struct header *header, uint32_t crc, uint32_t magic_number, uint32_t text_size);
void print_header(struct header *header);
struct header *get_header(const unsigned char *in_data, size_t size);
unsigned char *remove_header(unsigned char **in_data, size_t *size);