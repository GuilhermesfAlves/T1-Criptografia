#include "include/manager.h"
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define AES_KEY_SIZE 32
#define AES_IV_SIZE  16

// ===========================
// Chave e IV fixos
// ===========================
static const unsigned char STATIC_KEY[AES_KEY_SIZE] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
    0x36, 0x37, 0x38, 0x39, 0x30, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x31
};

static const unsigned char STATIC_IV[AES_IV_SIZE] = {
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35
};

// ===========================
// Utils
// ===========================
void handleErrors(void) {
    ERR_print_errors_fp(stderr);
    abort();
}

// ===========================
// Criptografia AES CBC 256
// ===========================
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    FILE* timeLog = fopen("out/aes/encryptTimeLog.csv","a");
        struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
        handleErrors();
    ciphertext_len += len;

    clock_gettime(CLOCK_MONOTONIC, &fim);
    long long duracao_ns = diff_ns(inicio, fim);
    fprintf(timeLog, "%lld,", duracao_ns);
    fclose(timeLog);

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        handleErrors();

    FILE* timeLog = fopen("out/aes/decryptTimeLog.csv","a");
    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);

    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
        handleErrors();
    plaintext_len += len;

    clock_gettime(CLOCK_MONOTONIC, &fim);
    long long duracao_ns = diff_ns(inicio, fim);
    fprintf(timeLog, "%lld,", duracao_ns);
    fclose(timeLog);

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}

// ===========================
// Help e Diff opcional
// ===========================
static void print_help_aes(const char *prog) {
    fprintf(stderr,
        "Uso:\n"
        "  %s encrypt < input > output\n"
        "  %s decrypt < input > output\n",
        prog, prog
    );
}

// ===========================
// MAIN
// ===========================
int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_help_aes(argv[0]);
        return 1;
    }

    const char *cmd = argv[1];
    size_t in_len;
    unsigned char *input = read_stdin_to_string(&in_len);
    if (!input) {
        fprintf(stderr, "Erro ao ler stdin\n");
        return 1;
    }

    // Buffer com sobra para padding
    unsigned char *out = malloc(in_len + AES_KEY_SIZE);
    if (!out) { perror("malloc"); free(input); return 1; }

    int out_len = 0;

    if (strcmp(cmd, "encrypt") == 0) {
        out_len = encrypt(input, (int)in_len,
                          (unsigned char*)STATIC_KEY,
                          (unsigned char*)STATIC_IV,
                          out);
        fwrite(out, 1, out_len, stdout);
    }
    else if (strcmp(cmd, "decrypt") == 0) {
        out_len = decrypt(input, (int)in_len,
                          (unsigned char*)STATIC_KEY,
                          (unsigned char*)STATIC_IV,
                          out);
        fwrite(out, 1, out_len, stdout);
    }
    else {
        print_help_aes(argv[0]);
        free(input);
        free(out);
        return 1;
    }

    free(input);
    free(out);
    return 0;
}
