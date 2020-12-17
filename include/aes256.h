#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define KEY_LENGTH 32
#define AES_BLOCK_SIZE 16

enum operations {
  DEFAULT = 0,
  ENCRYPT,
  DECRYPT
};

unsigned char *aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
                           unsigned char *iv, size_t *ciphertext_len);

unsigned char *aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                           unsigned char *iv, size_t plaintext_len);
void handleErrors(void);