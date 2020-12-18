#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes256.h"
#include "io.h"

#define MAGIC_NUMBER 0xDEADBEEF

#define USAGE(argv)                                                                                                  \
  printf("Usage : %s -e (for encrypt) -d (for decrypt) -k <256 bit key> -i <input path> -o <output path>", argv[0]); \
  printf(" -h - prints help -v - verbose\n");                                                                        \
  printf("long arguments present:\n --encrypt \n --decrypt \n --key \n --input \n --output \n --verbose \n --help\n");


int main(int argc, char **argv)
{
  struct option longOptions[] = {{"verbose", no_argument, 0, 'v'},     {"help", no_argument, 0, 'h'},
                                 {"encrypt", no_argument, 0, 'e'},     {"decrypt", no_argument, 0, 'd'},
                                 {"key", required_argument, 0, 'k'},   {"input", required_argument, 0, 'i'},
                                 {"output", required_argument, 0, 'o'}};

  struct required_args {
    bool key;
    bool input_path;
    bool output_path;
    bool encrypt;
    bool decrypt;
  } req_args;
  memset(&req_args, 0, sizeof(struct required_args));

  int        index, longOptIndex = 0;
  extern int opterr, optopt;
  opterr                    = 0;
  bool            verbose   = false;
  enum operations operation = DEFAULT;

  unsigned char key[KEY_LENGTH];
  unsigned char iv[] = "0123456789012345";

  char *input_file_path  = NULL;
  char *output_file_path = NULL;

  if (argc < 2) {
    USAGE(argv);
    return EXIT_FAILURE;
  }

  while ((index = getopt_long(argc, argv, "vhedk:i:o:", longOptions, &longOptIndex)) != -1) {
    switch (index) {
      case 'v':
        verbose = true;
        break;

      case 'e':
        operation        = ENCRYPT;
        req_args.encrypt = true;
        break;

      case 'd':
        operation        = DECRYPT;
        req_args.decrypt = true;
        break;

      case 'k':
        if (strlen(optarg) != KEY_LENGTH) {
          printf("Key must be %d bytes long! \n", (uint8_t)KEY_LENGTH);
          return EXIT_FAILURE;
        }
        req_args.key = true;
        memcpy(key, optarg, strlen(optarg));
        break;

      case 'i':
        if (strlen(optarg) > MAX_PATH_LENGTH) {
          printf("Input path is too long!\n");
          return EXIT_FAILURE;
        }
        req_args.input_path = true;
        input_file_path     = optarg;
        break;

      case 'o':
        if (strlen(optarg) > MAX_PATH_LENGTH) {
          printf("Output path is too long!\n");
          return EXIT_FAILURE;
        }
        req_args.output_path = true;
        output_file_path     = optarg;
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

  if (req_args.decrypt == true && req_args.encrypt == true) {
    printf("Only one operation should be chosen ! Use only -e or only -d\n");
    return EXIT_FAILURE;
  }
  if (!req_args.input_path) {
    printf("-i parameter is mandatory !\n");
    return EXIT_FAILURE;
  }

  if (!req_args.output_path) {
    printf("-o parameter is mandatory !\n");
    return EXIT_FAILURE;
  }

  if (!req_args.key) {
    printf("-k parameter is mandatory !\n");
    return EXIT_FAILURE;
  }

  int ret = 0;
  switch (operation) {
    case ENCRYPT:
      ret = file_encrypt(input_file_path, output_file_path, iv, key, (unsigned int)MAGIC_NUMBER, verbose);
      if (ret != EXIT_SUCCESS) {
        printf("Encryption Failed ! \n");
        return EXIT_FAILURE;
      }
      break;

    case DECRYPT:
      ret = file_decrypt(input_file_path, output_file_path, iv, key, (unsigned int)MAGIC_NUMBER, verbose);
      if (ret != EXIT_SUCCESS) {
        printf("Decryption Failed ! \n");
        return EXIT_FAILURE;
      }
      break;

    default:
      printf("No operation detected! Choose -d for decryption or -e for encryption! \n");
      return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}