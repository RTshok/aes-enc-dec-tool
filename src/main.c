#include "aes256.h"
#include "io.h"
#include "crc32.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MAGIC_NUMBER 0xDEADBEEF

#define USAGE(argv) printf("Usage : %s -e (for encrypt) -d (for decrypt) -k <256 bit key> -i <input path> -o <output path>", argv[0]); \
                    printf(" -h - prints help -v - verbose\n"); \
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

  struct required_args {
    bool key;
    bool input_path;
    bool output_path;
  }req_args;
  memset(&req_args, 0, sizeof(struct required_args));

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
    return EXIT_FAILURE;
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
        return EXIT_FAILURE;
      }
      req_args.key = true;
      memcpy(key, optarg, strlen(optarg));
      break;

    case 'i':
      if(strlen(optarg) > MAX_PATH_LENGTH)
      {
        printf("Input path is too long!\n");
        return EXIT_FAILURE;
      }
      req_args.input_path = true;
      input_file_path = optarg;
      break;

    case 'o':
      if(strlen(optarg) > MAX_PATH_LENGTH)
      {
        printf("Output path is too long!\n");
        return EXIT_FAILURE;
      }
      req_args.output_path = true;
      output_file_path = optarg;
      break;

    case '?':
      printf("Command unknown, check if data passed with argument : -%c\n", optopt);
    case 'h':
      USAGE(argv);
      return EXIT_FAILURE; 

    default:
      printf("Unknown argument : -%c\n", optopt);
      USAGE(argv);
      return EXIT_FAILURE;
    }
  }

  if(!req_args.input_path)
  {
    printf("-i parameter is mandatory !\n");
    return EXIT_FAILURE;
  }

  if(!req_args.output_path)
  {
    printf("-o parameter is mandatory !\n");
    return EXIT_FAILURE;
  }

  if(!req_args.key)
  {
    printf("-k parameter is mandatory !\n");
    return EXIT_FAILURE;
  }



  unsigned char *in_data = NULL, *out_data = NULL;
  uint32_t crc = 0;
  size_t data_len = 0, cipher_len = 0;
  switch (operation)
  {

    case ENCRYPT:

      in_data = file_read((const char *)input_file_path, &data_len);
      if(NULL == in_data) {
        return EXIT_FAILURE;
      }

      if(verbose) 
        printf("input data length %ld\n", data_len);

      crc = crc32((const void *)in_data, data_len);

      out_data = aes_encrypt(in_data, data_len, key, iv, &cipher_len);
      if(NULL == out_data) {
        free(in_data);
        return EXIT_FAILURE;
      }

      free(in_data); // we don't need input data anymore

      if(verbose) 
        printf("ciphered package length : %ld\n", cipher_len);

      out_data = append_header(&out_data, crc, (uint32_t)MAGIC_NUMBER, cipher_len, data_len); // append real size
      if(NULL == out_data) {
        return EXIT_FAILURE;
      }

      if(verbose) 
        printf("Encryption completed !\n");

      print_header(out_data, cipher_len + sizeof(struct header));
      data_len = file_write((const char *)output_file_path, out_data, cipher_len + sizeof(struct header));

      if(verbose) 
        printf("package + header length : %ld \n", data_len);

      free(out_data);
      break;

    case DECRYPT:

      in_data = file_read((const char *)input_file_path, &data_len);
      if(NULL == in_data)
      {
        return EXIT_FAILURE;
      }

      if(verbose) 
        printf("input data length : %ld\n", data_len);

      struct header *header = get_header(in_data, data_len);
      if(NULL == header) {
        free(in_data);
        return EXIT_FAILURE;
      }

      if(header->magic_number != (uint32_t)MAGIC_NUMBER) { // header validation
        printf("Invalid header magic number! File isn't suitable! \n");
        return EXIT_FAILURE;
      }

      print_header(in_data, data_len);

      in_data = remove_header(&in_data, &data_len);
      if(verbose)
        printf("data length without header : %ld\n", data_len);
      
      if(NULL == in_data)
      {
        free(in_data);
        return EXIT_FAILURE;
      }

      out_data = aes_decrypt(in_data, data_len , key, iv, header->size);
      free(in_data); // we don't need input data anymore

      if(NULL == out_data)
      {
        return EXIT_FAILURE;
      }

      crc = crc32((const void *)out_data, header->size);
      if(verbose)
        printf("crc after decryption : %08x\n", crc);

      if(crc != header->crc32) {
        printf("Bad CRC! Decryption went wrong :( \n");
      } else {
        printf("Decryption completed ! CRC's are equal !\n");
      }
      
      data_len = file_write((const char *)output_file_path, out_data, header->size);
      if(verbose)
        printf("Bytes written to file: %ld\n", data_len);

      free(header);
      free(out_data);
      break;

    default:
      printf("No operation detected! Choose -d for decryption or -e for encryption! \n");
      break;

  }
  
  return 0;
}