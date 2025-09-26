#include "manager.h"

void print_help (char *programName) {
    printf("\nUso do CRYPT: %s <arquivo_origem> <arquivo_saida>\n", programName);
}

char *read_stdin_to_string() {
    size_t capacity = 1024;   // tamanho inicial
    size_t length = 0;
    char *buffer = malloc(capacity);
    if (!buffer) {
        perror("malloc");
        return NULL;
    }

    int c;
    while ((c = getchar()) != EOF) {
        if (length + 1 >= capacity) { // +1 para o '\0'
            capacity *= 2;
            char *new_buffer = realloc(buffer, capacity);
            if (!new_buffer) {
                free(buffer);
                perror("realloc");
                return NULL;
            }
            buffer = new_buffer;
        }
        buffer[length++] = (char)c;
    }
    buffer[length] = '\0'; // termina string

    return buffer;
}