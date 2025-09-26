#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../include/manager.h"
#include "../include/crypt.h"

int main() {
    srand(time(NULL));

    char *plainText = read_stdin_to_string();

    if (!plainText){
        perror("erro não foi possível alocar a string\n");
        return 1;
    }

    // Usar argv para decidir se usa encrypt ou decrypt
    // Entrada invalida
    // if (argv != 3) {
    //     print_help(argc[0]);
    //     exit(1);
    // }

    char *key = generate_random_key();

    if (!is_key_valid(key)) {
        if (key)
            free(key);

        printf("ERRO: Chave invalida\n");

        exit(1);
    }

    char *encryptedText = encrypt(plainText, strlen(plainText), key);
    char *decryptedText = decrypt(encryptedText, strlen(encryptedText), key);

    printf("Chave:\t\t\t%s\n", key);
    printf("Original:\t\t%s\n", plainText);
    printf("Criptografado:\t\t%s\n", encryptedText);
    printf("Descriptografado:\t%s\n", decryptedText);

    // TODO: Leitura de arquivos + expandir strings até MAX_CRYPT_LEN
    // Expandir strings até MAX_CRYPT_LEN, ou seja se uma string lida tem 23 caracteres expandir ate 25 (se max == 25)

    free(encryptedText);
    free(decryptedText);
    free(plainText);
    free(key);
    return 0;
}
