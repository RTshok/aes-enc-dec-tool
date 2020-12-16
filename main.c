#include "aes256.h"
#include "io.h"
#include "crc32.h"
#include "utils.h"

#include <stdio.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_NUMBER 0xDEADBEEF

#define USAGE(argv) printf("Usage : %s -e (for encrypt) -d (for decrypt) -k <256 bit key> -i <input path> -o <output path>", argv[0]); \
                    printf("-h for help -v for verbose\n"); \
                    printf("long arguments present:\n --encrypt \n --decrypt \n --key \n --input \n --output \n --verbose \n --help\n");


int main (int argc, char **argv)
{
  struct option longOptions[] = {
    {"verbose", no_argument, 0, 'v'},
    {"help",    no_argument, 0, 'h'},
    {"encrypt",  no_argument, 0, 'e'},
    {"decrypt",  no_argument, 0, 'd'},
    {"key",  required_argument, 0, 'k'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'}
  };

  int index, longOptIndex = 0;
  extern int opterr, optopt;
  opterr = 0;
  bool verbose = false;
  enum operations operation = DEFAULT;

  unsigned char key[KEY_LENGTH];
  unsigned char iv[] = "0123456789012345";
  char *input_file_path = NULL;
  char *output_file_path = NULL;

  if(argc < 2) {
    USAGE(argv);
    return -1;
  }

  while((index = getopt_long(argc, argv, "vhedk:i:o:", longOptions, &longOptIndex)) != -1) {

    switch(index) {
    case 'v':
      verbose = true;
      break;

    case 'e':
      operation = ENCRYPT;
      break;

    case 'd':
      operation = DECRYPT;
      break;
      
    case 'k':
      if(strlen(optarg) != KEY_LENGTH)
      {
        printf("Key must be %d bytes long! \n", (uint8_t) KEY_LENGTH);
        return -1;
      }
      memcpy(key, optarg, strlen(optarg));
      break;

    case 'i':
      if(strlen(optarg) > MAX_PATH_LENGTH)
      {
        printf("Input path is too long!\n");
        return -1;
      }

      input_file_path = optarg;
      break;

    case 'o':
      if(strlen(optarg) > MAX_PATH_LENGTH)
      {
        printf("Output path is too long!\n");
        return -1;
      }

      output_file_path = optarg;
      break;

    case ':':
      break;
    case '?':
      printf("Unknown argument :%c", index);
    case 'h':
      USAGE(argv);
      break;  

    default:
      printf("Unknown argument :%c", index);
      USAGE(argv);
      break;
    }
  }

  unsigned char *in_data = NULL, *out_data = NULL;
  uint32_t crc;
  size_t data_len, cipher_len;
  switch (operation)
  {

    case ENCRYPT:

      in_data = file_read((const char *)input_file_path, &data_len);
      if(NULL == in_data) {
        return -1;
      }

      if(verbose) 
        printf("data len %ld\n", data_len);

      crc = crc32((const void *)in_data, data_len);

      out_data = aes_encrypt(in_data, data_len, key, iv, &cipher_len);
      if(NULL == out_data) {
        return -1;
      }

      if(verbose) 
        printf("cipher len : %ld\n", cipher_len);

      out_data = append_header(out_data, crc, (uint32_t)MAGIC_NUMBER, cipher_len, data_len); // append real size
      if(NULL == out_data) {
        return -1;
      }

      if(verbose) 
        printf("Encryption completed !\n");

      print_header(out_data, cipher_len + sizeof(struct header));
      data_len = file_write((const char *)output_file_path, out_data, cipher_len + sizeof(struct header));

      if(verbose) 
        printf("data len : %ld \n", data_len);

      free(in_data);
      free(out_data);
      break;

    case DECRYPT:

      in_data = file_read((const char *)input_file_path, &data_len);
      if(NULL == in_data)
      {
        return -1;
      }

      if(verbose) 
        printf("file length : %ld\n", data_len);

      print_header(in_data, data_len);
      struct header *header = get_header(in_data, data_len);
      if(NULL == header) {
        return -1;
      }

      in_data = remove_header(in_data, &data_len);
      if(NULL == in_data)
      {
        return -1;
      }

      out_data = aes_decrypt(in_data, data_len , key, iv, header->size);
      if(NULL == out_data)
      {
        return -1;
      }
      crc = crc32((const void *)out_data, header->size);
      if(NULL == in_data)
      {
        return -1;
      }

      if(crc != header->crc32) {
        printf("Bad CRC! Decryption went wrong :( \n");
      } else {
        printf("Decryption completed ! CRC's are equal !\n");
      }
      
      free(header);
      free(out_data);
      free(in_data);
      break;

    case DEFAULT:

      break;

    default:

      break;

  }
  
  return 0;
}