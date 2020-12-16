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
    {"encrypt",  optional_argument, 0, 'e'},
    {"decrypt",  optional_argument, 0, 'd'},
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
  unsigned char iv[] = "1234569";
  unsigned char input_file_path[MAX_PATH_LENGTH];
  unsigned char output_file_path[MAX_PATH_LENGTH];

  if(argc <= 2) {
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
      if(strlen(optarg) > KEY_LENGTH)
      {
        printf("Key can't be longer than %d bytes! \n", (uint8_t) KEY_LENGTH);
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
      memcpy(input_file_path, optarg, strlen(optarg));
      break;

    case 'o':
      if(strlen(optarg) > MAX_PATH_LENGTH)
      {
        printf("Output path is too long!\n");
        return -1;
      }
      memcpy(output_file_path, optarg, strlen(optarg));
      break;

    case ':':

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

  unsigned char *in_data, out_data;
  uint32_t data_len, crc, cipher_len;
  switch (operation)
  {

    case ENCRYPT:
      data_len = file_read(input_file_path, in_data);
      crc = crc32((void *)in_data, data_len);
      out_data = aes_encrypt(in_data, data_len, key, iv, &cipher_len);
      append_header(out_data, crc, MAGIC_NUMBER, cipher_len);
      print_header(out_data, cipher_len);
      file_write(output_file_path, out_data, cipher_len);
      break;

    case DECRYPT:
      //aes_decrypt();
      break;

    case DEFAULT:

      break;

    default:

      break;

  }

  return 0;
}