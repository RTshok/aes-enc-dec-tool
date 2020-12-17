#include "aes256.h"

#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>

unsigned char *aes_encrypt(unsigned char *plaintext,
                           int            plaintext_len,
                           unsigned char *key,
                           unsigned char *iv,
                           size_t *       ciphertext_len)
{
  unsigned char *ciphertext   = NULL;
  int            padded_bytes = plaintext_len % AES_BLOCK_SIZE;

  // https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
  //allocating memory for future cipheredtext
  if (padded_bytes > 0) { // in case there are remained bytes - add them to have array multiple of AES_BLOCK_SIZE
    *ciphertext_len = plaintext_len - padded_bytes + AES_BLOCK_SIZE;
    ciphertext      = malloc(sizeof(unsigned char) * (plaintext_len - padded_bytes + AES_BLOCK_SIZE));
  }
  else if (padded_bytes == 0) { // in case there are no remained bytes - add AES_BLOCK_SIZE
    *ciphertext_len = plaintext_len + AES_BLOCK_SIZE;
    ciphertext      = malloc(sizeof(unsigned char) * (plaintext_len + AES_BLOCK_SIZE));
  }

  if (ciphertext == NULL) {
    printf("bad alloc !\n");
    return NULL;
  }

  EVP_CIPHER_CTX *ctx;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    goto error;
  }

  if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    goto error;
  }

  int len;
  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
    goto error;
  }

  if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
    goto error;
  }

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext;

error:
  ERR_print_errors_fp(stderr);
  free(ciphertext);
  return NULL;
}


unsigned char *aes_decrypt(unsigned char *ciphertext,
                           int            ciphertext_len,
                           unsigned char *key,
                           unsigned char *iv,
                           size_t         plaintext_len)
{
  unsigned char *plaintext = malloc(sizeof(unsigned char) * ciphertext_len);

  EVP_CIPHER_CTX *ctx;
  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    goto error;
  }

  if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
    goto error;
  }

  int len;
  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    goto error;
  }

  if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
    goto error;
  }

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext;

error:
  ERR_print_errors_fp(stderr);
  free(plaintext);
  return NULL;
}
