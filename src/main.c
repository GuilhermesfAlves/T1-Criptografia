#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "manager.h"
#include "crypt.h"

int main(int argv, char* argc[]) {
    srand(time(NULL));

    // Entrada invalida
    if (argv != 3) {
        print_help(argc[0]);
        exit(1);
    }

    char textoOriginal[] = "criptografia eh massa, muito massa mesmo, acredite!";
    char *chaveCriptografia = generate_random_key();

    if (!is_key_valid(chaveCriptografia)) {
        if (chaveCriptografia)
            free(chaveCriptografia);

        printf("ERRO: Chave invalida\n");

        exit(1);
    }

    char *textoCriptografado = cipher(textoOriginal, chaveCriptografia);
    char *textoDescriptografado = decipher(textoCriptografado, chaveCriptografia);

    printf("Chave:\t\t\t%s\n", chaveCriptografia);
    printf("Original:\t\t%s\n", textoOriginal);
    printf("Criptografado:\t\t%s\n", textoCriptografado);
    printf("Descriptografado:\t%s\n", textoDescriptografado);

    // TODO: Leitura de arquivos + expandir strings até MAX_CIPHER_LEN
    // Expandir strings até MAX_CIPHER_LEN, ou seja se uma string lida tem 23 caracteres expandir ate 25 (se max == 25)

    free(textoCriptografado);
    free(textoDescriptografado);
    free(chaveCriptografia);

    return 0;
}
