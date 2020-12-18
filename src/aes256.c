#include "aes256.h"



EVP_CIPHER_CTX *create_context(unsigned char *key, unsigned char *iv, enum operations operation)
{
  EVP_CIPHER_CTX *ctx;

  /* Create and initialise the context */
  if (!(ctx = EVP_CIPHER_CTX_new())) {
    ERR_print_errors_fp(stderr);
    return NULL;
  }

  //EVP_CIPHER_CTX_set_padding(ctx, 0); // disable padding 
  if(ENCRYPT == operation) {
    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
      ERR_print_errors_fp(stderr);
      return NULL;
    }
  } else if (DECRYPT == operation) {
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
      ERR_print_errors_fp(stderr);
      return NULL;
    }
  }
  

  return ctx;
}

void delete_context(EVP_CIPHER_CTX **ctx)
{
  EVP_CIPHER_CTX_free(*ctx);
}

size_t aes_encrypt(EVP_CIPHER_CTX *ctx,
                   unsigned char *plaintext,
                   int            plaintext_len,
                   unsigned char *ciphertext,
                   bool last_block)
{
  size_t ciphertext_len;
  int len;
  if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) {
    goto error;
  }

  ciphertext_len = len;

  if(last_block) { // finalize encryption in case last block
    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) {
      goto error;
    }
    ciphertext_len += len;
  }

  return ciphertext_len;

error:
  ERR_print_errors_fp(stderr);
  return ciphertext_len;
}


size_t aes_decrypt(EVP_CIPHER_CTX *ctx,
                          unsigned char *ciphertext,
                           int            ciphertext_len,
                           unsigned char *plaintext,
                           bool last_block)
{

  size_t plaintext_len;
  int len;
  if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    goto error;
  }

  plaintext_len = len;
  if(last_block) { // finalize encryption in case last block
    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) {
      goto error;
    }
    plaintext_len += len;
  }

  return plaintext_len;

error:
  ERR_print_errors_fp(stderr);
  return plaintext_len;
}
