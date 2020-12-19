#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <stdlib.h>

#define KEY_LENGTH     32
#define AES_BLOCK_SIZE 16
#define MAX_BLOCK_SIZE 8192 // low because of good fread/fwrite performance on low memory blocks

enum operations { DEFAULT = 0, ENCRYPT, DECRYPT };

size_t aes_encrypt(EVP_CIPHER_CTX *ctx,
                   unsigned char * plaintext,
                   int             plaintext_len,
                   unsigned char * ciphertext,
                   bool            last_block);

size_t aes_decrypt(EVP_CIPHER_CTX *ctx,
                   unsigned char * ciphertext,
                   int             ciphertext_len,
                   unsigned char * plaintext,
                   bool            last_block);

EVP_CIPHER_CTX *create_context(const unsigned char *key, const unsigned char *iv, enum operations operation);
void            delete_context(EVP_CIPHER_CTX **ctx);
