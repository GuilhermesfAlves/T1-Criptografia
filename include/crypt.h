#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef CRPYT_H
#define CRPYT_H

#define BLOCK_SIZE      6
#define MAX_KEY_SIZE    (BLOCK_SIZE * BLOCK_SIZE)
#define MAX_CIPHER_LEN  (BLOCK_SIZE * BLOCK_SIZE)

typedef struct {
    char character;
    unsigned int original_index;
} KeyPair;

// Algoritmos relacionados a chave de cifra
char* generate_random_key();
bool check_invalid_key(char* key);

// Algoritmos de cifra/decifra completos
char* cipher    (char* originalText, char* key);
char* decipher  (char* cryptedText,  char* key);

#endif // CRPYT_H
