#include "io.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "aes256.h"
#include "crc32.h"
#include "utils.h"

int file_encrypt (const char *input_file_path, const char *output_file_path, const unsigned char *iv, const unsigned char *key, unsigned int magic_number, bool verbose, size_t block_size)
{
  FILE *input_file, *output_file;
  input_file = fopen(input_file_path, "rb");

  if (NULL == input_file) {
    printf("Can't open file at location : %s !\n", input_file_path);
    return EXIT_FAILURE;
  }

  output_file = fopen(output_file_path, "w+b");

  if (NULL == output_file) {
    printf("Can't open file at location : %s!\n", output_file_path);
    return EXIT_FAILURE;
  }

  //getting size of file
  fseek(input_file, 0L, SEEK_END);
  size_t in_file_size = ftell(input_file);
  fseek(input_file, 0L, SEEK_SET);

  unsigned int block_amount = block_amount = in_file_size / block_size;

  if(in_file_size % block_size > 0) {
    block_amount++;
  }

  if(in_file_size < block_size) {
    block_amount = 1;
  }

  unsigned char block[block_size];
  unsigned char ciphered_block[block_size + AES_BLOCK_SIZE]; // ciphered block could be padded by EVP up to 16 bytes
  size_t read_len, write_len, ciphered_len;
  unsigned int crc = 0;
  printf("file size : %ld \n", in_file_size);
  printf("block amount %d \n", block_amount);

  EVP_CIPHER_CTX *ctx = create_context(key, iv, ENCRYPT);

  for(unsigned int i = 0; i < block_amount; i++) {

    if(verbose)
      printf("block is %d \n", i);

    read_len = fread(block, sizeof(unsigned char), block_size, input_file);

    if(verbose)
      printf("read len : %ld\n", read_len);
    
    if(read_len == 0) {
      printf ("Bad fread ()!");
      fclose(input_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    crc = crc32_accum(crc, (const void *)block, read_len);
  
    ciphered_len = aes_encrypt(ctx, block, read_len, ciphered_block, (bool)(i == block_amount - 1)); // finilize in case last block

    if(verbose)
      printf("encrypted len %ld\n", ciphered_len);

    write_len = fwrite(ciphered_block, sizeof(unsigned char), ciphered_len, output_file);
    if (write_len != ciphered_len) {
      printf("Bad fwrite() \n");
      fclose(input_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    if(verbose)
      printf("written to file : %ld\n", write_len);
    fseek(output_file, 0L, SEEK_END);

    memset(block, 0, block_size);
    memset(ciphered_block, 0, block_size + AES_BLOCK_SIZE);
  }

  //writing header
  struct header header;
  init_header(&header, crc, magic_number, in_file_size);
  print_header(&header);
  fwrite((unsigned char *)&header, sizeof(unsigned char), sizeof(struct header), output_file);

  delete_context(&ctx);

  fclose(input_file);
  fclose(output_file);
  return EXIT_SUCCESS;
}

int file_decrypt (const char *input_file_path, const char *output_file_path, const unsigned char *iv, const unsigned char *key, unsigned int magic_number, bool verbose, size_t block_size)
{
  FILE *input_file, *output_file;
  input_file = fopen(input_file_path, "rb");

  if (NULL == input_file) {
    printf("Can't open file at location : %s !\n", input_file_path);
    return EXIT_FAILURE;
  }

  output_file = fopen(output_file_path, "w+b");

  if (NULL == output_file) {
    printf("Can't open file at location : %s!\n", output_file_path);
    return EXIT_FAILURE;
  }

  fseek(input_file, 0L, SEEK_END);
  size_t in_file_size = ftell(input_file) - sizeof(struct header); // aquiring actual size without header
  fseek(input_file, -sizeof(struct header), SEEK_END); // set position before last 12 bytes (header)

  struct header header;
  size_t ret_len = fread(&header, sizeof(unsigned char), sizeof(struct header), input_file); // getting header
  if(ret_len == 0) {
    printf("Bad fread()! \n");
    return EXIT_FAILURE;
  }

  fseek(input_file, 0L, SEEK_SET); // returning to beginning

  print_header(&header);

  if(header.magic_number != magic_number)
  {
    printf ("Bad magic number !\n");
    return EXIT_FAILURE;
  }

  size_t read_len, write_len, plaintext_len;
  unsigned int crc = 0;

  EVP_CIPHER_CTX *ctx = create_context(key, iv, DECRYPT);
  printf("size without header :%ld \n", in_file_size);

  // encrypted file should be already padded
  unsigned int block_amount = in_file_size / block_size; 

  if(in_file_size % block_size > 0) // in case there are more bytes but their amount less than block_size
  {
    block_amount++;
  }

  if(in_file_size < block_size) { // needed only one block to decrypt
    block_amount = 1;
  }

  unsigned char ciphered_block[block_size];
  unsigned char block[block_size + AES_BLOCK_SIZE];

  if(verbose)
    printf("block_amount: %d\n", block_amount);
  
  for(unsigned int i = 0; i < block_amount; i++)
  {
    if(verbose)
      printf("block: %d \n", i);

    read_len = fread(ciphered_block, sizeof(unsigned char), block_size, input_file);
    if(feof(input_file)) // in case header was read (last 12 bytes)
    {
      read_len = read_len - sizeof(struct header);
    }
    
    if(verbose)
      printf("read len : %ld\n", read_len);
    
    if(read_len == 0) {
      printf ("Bad fread ()!");
      fclose(input_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    plaintext_len = aes_decrypt(ctx, ciphered_block, read_len, block, (bool)(i == block_amount - 1));

    if(verbose)
      printf("plaintext len : %ld\n", plaintext_len);

    crc = crc32_accum(crc, block, plaintext_len);

    write_len = fwrite(block, sizeof(unsigned char), plaintext_len, output_file);
    if (write_len != plaintext_len) {
      printf("Bad fwrite() \n");
      fclose(input_file);
      fclose(output_file);
      return EXIT_FAILURE;
    }

    if(verbose)
      printf("write_len : %ld\n", write_len);

    memset(block, 0, block_size);
    memset(ciphered_block, 0, block_size + AES_BLOCK_SIZE);
  }

  printf("CRC from header : %08x \n", header.crc32);
  printf("calculated CRC : %08x\n", crc);

  fclose(input_file);
  fclose(output_file);
  delete_context(&ctx);

  if(header.crc32 == crc) {
    printf("crc32 is equal!\n");
    return EXIT_SUCCESS;
  } else  {
    printf("crc32 isn't equal!\n");
    return EXIT_FAILURE;
  }
  

}
