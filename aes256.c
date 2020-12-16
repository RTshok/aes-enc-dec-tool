// encrypt / decrypt operations
#include "aes256.h"

unsigned char *aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, int *ciphertext_len)
{
    EVP_CIPHER_CTX *ctx;
    
    int len;

    unsigned char *ciphertext = NULL;
    int padded_bytes = plaintext_len % AES_BLOCK_SIZE;
    
    if(padded_bytes > 0) {
      *ciphertext_len = plaintext_len - padded_bytes + AES_BLOCK_SIZE;
      ciphertext = malloc(sizeof(unsigned char) * (plaintext_len - padded_bytes + AES_BLOCK_SIZE));
    } else if (padded_bytes == 0) {
      *ciphertext_len = plaintext_len + AES_BLOCK_SIZE;
      ciphertext = malloc (sizeof(unsigned char) * plaintext_len + AES_BLOCK_SIZE);
    }

    if(ciphertext == NULL)
    {
      printf("bad alloc !\n");
      return ciphertext;
    }
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    EVP_CIPHER_CTX_set_padding(ctx, 0);
    /*
     * Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    //*ciphertext_len = len;

    /*
     * Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext;
}


unsigned char *aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                           unsigned char *iv, int plaintext_len)
{
    EVP_CIPHER_CTX *ctx;
    
    int len;

    unsigned char *plaintext = malloc (sizeof(unsigned char) * plaintext_len);

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    EVP_CIPHER_CTX_set_padding(ctx, 0);
    /*
     * Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits
     */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /*
     * Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary.
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
  

    /*
     * Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext;
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}
