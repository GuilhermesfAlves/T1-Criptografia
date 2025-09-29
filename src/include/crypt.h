#ifndef PROJECT_CRYPT_H
#define PROJECT_CRYPT_H
#include "manager.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MIN_CHAR        1       // Exclui NULL (0)
#define MAX_CHAR        254     // Exclui DEL (255) e mantém UTF-8
#define RANGE_CHAR      (MAX_CHAR - MIN_CHAR + 1)
#define N_REP_CRYPT     10
#define BLOCK_LEN       6
#define KEY_LEN         128

typedef struct KeyPair_t{
    unsigned char character;
    unsigned int original_index;
} KeyPair;

unsigned char* generate_random_key();                               // Gera uam chave de tamanho KEY_LEN
bool is_key_valid(unsigned char* key);                              // Verifica validade da chave

unsigned char* encrypt(unsigned char* plainText, int len, unsigned char* key);        // Retorno: string cifrada
unsigned char* decrypt(unsigned char* encryptedText, int len, unsigned char* key);    // Retorno: string decifrada

#endif // PROJECT_CRYPT_H
