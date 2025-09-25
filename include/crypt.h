#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef CRYPT_H
#define CRYPT_H

#define MIN_CHAR        32
#define MAX_CHAR        126
#define RANGE_CHAR      (MAX_CHAR - MIN_CHAR + 1)
#define N_REP_CIPHER    10
#define BLOCK_LEN       6
#define KEY_LEN         (BLOCK_LEN * BLOCK_LEN)
#define STR_CIPHER_LEN  (BLOCK_LEN * BLOCK_LEN)

typedef struct KeyPair_t{
    char character;
    unsigned int original_index;
} KeyPair;

char* generate_random_key();        // Gera uam chave de tamanho KEY_LEN
bool is_key_valid(char* key);       // Verifica validade da chave

char* cipher    (char* originalText, char* key);    // Retorno: string cifrada
char* decipher  (char* cryptedText,  char* key);    // Retorno: string decifrada

#endif // CRYPT_H
