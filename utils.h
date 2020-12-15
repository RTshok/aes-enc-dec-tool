#include <stdint.h>
#include <stddef.h>

// HEADER STRUCTURE
// [MAGIC_NUMBER - 4b] [DATA SIZE - 4b] [CRC32 - 4b]

struct header {
  uint32_t magic_number;
  uint32_t size;
  uint32_t crc32;
};

int append_header (unsigned char *in_data, uint32_t crc, uint32_t magic_number, uint32_t size);
void print_header(const unsigned char *data, uint32_t size);
struct header get_header (const unsigned char * data);