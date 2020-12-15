#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define KEY_LENGTH 32

enum operations {
  DEFAULT = 0,
  ENCRYPT,
  DECRYPT
};

int aes_encrypt (const unsigned char *data, int data_len, const unsigned char *key,
                 const unsigned char *iv, unsigned char *ciphered_data);

int aes_decrypt (const unsigned char *data, int data_len, const unsigned char *key,
                 const unsigned char *iv, unsigned char *ciphered_data);