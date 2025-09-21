#include <stdio.h>
#include <stdlib.h>
// #include <time.h>

#include "crypt.h"

int main(int argv, char* argc[]) {
    srand(0);

    char textoOriginal[] = "criptografia eh massa, muito massa mesmo, acredite!";
    char *chaveCriptografia = generate_random_key();

    if (check_invalid_key(chaveCriptografia)) {
        if (chaveCriptografia)
            free(chaveCriptografia);

        printf("ERRO: Chave invalida\n");

        exit(1);
    }

    char *textoCriptografado = cipher(textoOriginal, chaveCriptografia);
    char *textoDescriptografado = decipher(textoCriptografado, chaveCriptografia);

    printf("Chave: %s\n\n", chaveCriptografia);
    printf("Original:\t\t\t%s\n", textoOriginal);
    printf("Criptografado:\t\t%s\n", textoCriptografado);
    printf("Descriptografado:\t%s\n", textoDescriptografado);

    free(textoCriptografado);
    free(textoDescriptografado);
    free(chaveCriptografia);

    return 0;
}
