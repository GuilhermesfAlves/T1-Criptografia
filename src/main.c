#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/manager.h"
#include "include/crypt.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    // === Geração de chave ===
    if (strcmp(argv[1], "key") == 0) {
        unsigned char *key = generate_random_key();
        if (!key) {
            fprintf(stderr, "Erro ao gerar chave.\n");
            return 1;
        }
        printf("%s", key);
        free(key);
        return 0;
    }

    // === Modo encrypt / decrypt ===
    if (argc != 3) {
        print_help(argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    const char *keyfile = argv[2];

    unsigned char *key = read_file_to_string(keyfile);
    if (!key) {
        return 1;
    }

    if (!is_key_valid(key)) {
        fprintf(stderr, "Chave inválida.\n");
        free(key);
        return 1;
    }

    size_t in_len;
    unsigned char *inputText = read_stdin_to_string(&in_len);
    if (!inputText) {
        fprintf(stderr, "Erro ao ler entrada padrão.\n");
        free(key);
        return 1;
    }

    unsigned char *outputText = NULL;

    if (strcmp(mode, "encrypt") == 0) {
        outputText = encrypt(inputText, in_len, key);
    } else if (strcmp(mode, "decrypt") == 0) {
        outputText = decrypt(inputText, in_len, key);
    } else {
        print_help(argv[0]);
        free(inputText);
        free(key);
        return 1;
    }

    if (!outputText) {
        fprintf(stderr, "Erro na operação de %s.\n", mode);
        free(inputText);
        free(key);
        return 1;
    }

    fwrite(outputText, sizeof(unsigned char), in_len, stdout);

    free(outputText);
    free(inputText);
    free(key);

    return 0;
}
