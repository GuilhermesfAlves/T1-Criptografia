#include "include/manager.h"

void print_help(const char *progname) {
    fprintf(stderr,
        "Uso:\n"
        "  %s key                          Gera uma chave aleatória e imprime na stdout\n"
        "  %s encrypt <keyfile> < input    Criptografa texto lido do stdin\n"
        "  %s decrypt <keyfile> < input    Descriptografa texto lido do stdin\n",
        progname, progname, progname
    );
}

// usado para receber a entrada do buffer que é o texto para criptografar/descriptografar
unsigned char *read_stdin_to_string(size_t *out_len) {
    size_t capacity = 1024;
    size_t length = 0;
    unsigned char *buffer = malloc(capacity);
    if (!buffer) {
        perror("malloc");
        return NULL;
    }

    int c;
    while ((c = getchar()) != EOF) {
        if (length >= capacity) {
            capacity *= 2;
            unsigned char *tmp = realloc(buffer, capacity);
            if (!tmp) { free(buffer); return NULL; }
            buffer = tmp;
        }
        buffer[length++] = (unsigned char)c;
    }
    *out_len = length;
    return buffer;
}

// usado somente para pegar a chave quando ela não é estática
unsigned char* read_file_to_string(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        perror("Erro ao abrir arquivo de chave");
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    unsigned char *buffer = malloc(size + 1);
    if (!buffer) {
        perror("Erro ao alocar buffer para chave");
        fclose(f);
        return NULL;
    }

    if (fread(buffer, 1, size, f) != (size_t)size) {
        perror("Erro ao ler chave do arquivo");
        free(buffer);
        fclose(f);
        return NULL;
    }

    buffer[size] = '\0';
    fclose(f);
    return buffer;
}

long long diff_ns(struct timespec start, struct timespec end) {
    long long s = (long long)(end.tv_sec - start.tv_sec) * 1000000000LL;
    long long ns = end.tv_nsec - start.tv_nsec;
    return s + ns;
}