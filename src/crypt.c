#include "include/crypt.h"

// Headers de ordenacao
int compare_pairs(const void* a, const void* b);
unsigned int* columns_sequence_by_key (unsigned char* key, unsigned int start, unsigned int end);

// Headers de formatacao de strings
unsigned char* xor_strings (unsigned char *a, unsigned char *b, unsigned int size, unsigned int iteration);

// Cifra de Substituição
unsigned char* substitution_encrypt(unsigned char* input, unsigned char* key, unsigned int size);
unsigned char* substitution_decrypt(unsigned char* input, unsigned char* key, unsigned int size);

// Cifra de transposicao
unsigned char* transposition_encrypt (unsigned char* cryptedText, unsigned char* originalText, unsigned char* key, unsigned int len, unsigned int iteration);
unsigned char* transposition_decrypt (unsigned char* decryptedText, unsigned char* originalText, unsigned char* key, unsigned int len, unsigned int iteration);

unsigned char* generate_random_key () {
    unsigned char* newKey = malloc((KEY_LEN + 1) * sizeof(unsigned char));

    if (!newKey) {
        printf("ERRO: não foi possível alocar memória para a NOVA CHAVE");
        exit(1);
    }

    for(unsigned int i = 0; i < KEY_LEN; i++)
        newKey[i] = MIN_CHAR + (rand() % RANGE_CHAR); // Todos os caracteres da tabela ascii
    newKey[KEY_LEN] = '\0';

    return newKey;
}

// FIXME ainda há problema com '\n' e '\t', entre outros.
// Os acentos ficam corretos, por enquanto
bool is_printable_char(unsigned char i) {
    return (MIN_CHAR <= i && i <= MAX_CHAR);
}

bool is_key_valid (unsigned char* key) {
    for (unsigned int i = 0; i < KEY_LEN; i++)
        if (!is_printable_char(key[i]))
            return false;

    return true;
}

int compare_pairs(const void* a, const void* b) {
    KeyPair* pairA = (KeyPair*)a;
    KeyPair* pairB = (KeyPair*)b;

    // ordena com base no unsigned char
    if (pairA -> character < pairB -> character) return -1;
    if (pairA -> character > pairB -> character) return 1;

    // caso seja igual, ordena com base no index
    if (pairA -> original_index < pairB -> original_index) return -1;
    if (pairA -> original_index > pairB -> original_index) return 1;

    return 0;
}

unsigned int* columns_sequence_by_key (unsigned char* key, unsigned int start, unsigned int end) {
    if (key == NULL || start > end) {
        return NULL;
    }

    const unsigned int length = end - start;

    KeyPair* pairs = malloc(length * sizeof(KeyPair));
    unsigned int* result_indices = malloc(length * sizeof(unsigned int));

    if (!pairs || !result_indices) {
        free(pairs);
        free(result_indices);
        return NULL;
    }

    for (unsigned int i = 0; i < length; i++) {
        pairs[i].character = key[(start + i) % KEY_LEN];
        pairs[i].original_index = i;
    }

    qsort(pairs, length, sizeof(KeyPair), compare_pairs);

    for (unsigned int i = 0; i < length; i++) {
        result_indices[i] = pairs[i].original_index;
    }

    free(pairs);

    return result_indices;
}

unsigned char* xor_strings (unsigned char *a, unsigned char *b, unsigned int size, unsigned int iteration) {
    for (unsigned int i = 0; i < size; i++){
        const unsigned char x = a[i] ^ b[((i + (iteration * BLOCK_LEN)) % KEY_LEN)];
        if (is_printable_char(x))
            a[i] = x;
        // caso não seja caracter printável mantém
    }

    return a;
}

unsigned char* transposition_encrypt (unsigned char* cryptedText, unsigned char* originalText, unsigned char* key, unsigned int len, unsigned int iteration) {
    unsigned int transpositionIndex,
                 index = 0,
                 *columnSequence = columns_sequence_by_key(key, BLOCK_LEN * iteration, (BLOCK_LEN * (iteration + 1)));

    if (!columnSequence) {
        printf("ERRO: não foi possível alocar memória para a coluna de transposicao");
        return NULL;
    }

    int blocksInText = len / BLOCK_LEN;
    if (len % BLOCK_LEN)
        blocksInText++;
    // Decifra da transposicao
    for (int i = 0; i < blocksInText; i++) { // N blocos na string
        for (int j = 0; j < BLOCK_LEN; j++) { // BLOCK_LEN
            transpositionIndex = (columnSequence[j] + (i * BLOCK_LEN));
            cryptedText[index] = originalText[transpositionIndex];
            index++;
        }
    }

    free(columnSequence);

    return cryptedText;
}

unsigned char* transposition_decrypt (unsigned char* decryptedText, unsigned char* originalText, unsigned char* key, unsigned int len, unsigned int iteration) {
    unsigned int transpositionIndex,
                 index = 0,
                 *columnSequence = columns_sequence_by_key(key, BLOCK_LEN * iteration, (BLOCK_LEN * (iteration + 1)));

    if (!columnSequence) {
        printf("ERRO: não foi possível alocar memória para a coluna de transposicao");
        return NULL;
    }

    int blocksInText = len / BLOCK_LEN;
    if (len % BLOCK_LEN)
        blocksInText++;
    // Cifra da transposicao
    for (int i = 0; i < blocksInText; i++) { // N blocos na string
        for (int j = 0; j < BLOCK_LEN; j++) { // BLOCK_LEN
            transpositionIndex = (columnSequence[j] + (i * BLOCK_LEN));
            decryptedText[transpositionIndex] = originalText[index];
            index++;
        }
    }

    free(columnSequence);

    return decryptedText;
}

unsigned char* substitution_encrypt(unsigned char* input, unsigned char* key, unsigned int size) {
    unsigned int shift = 0;
    // Gera deslocamento a partir da chave
    for (unsigned int i = 0; i < KEY_LEN; i++)
        shift = (shift * 31 + key[i]) % RANGE_CHAR;

    for (unsigned int i = 0; i < size; i++) {
        const unsigned char c = input[i];

        if (is_printable_char(c)) {
            input[i] = MIN_CHAR + ((c - MIN_CHAR - shift + RANGE_CHAR) % RANGE_CHAR);
        }
    }

    return input;
}

// Cifra de César
unsigned char* substitution_decrypt(unsigned char* input, unsigned char* key, unsigned int size) {
    unsigned int shift = 0;
    // Gera deslocamento a partir da chave
    for (unsigned int i = 0; i < KEY_LEN; i++)
        shift = (shift * 31 + key[i]) % RANGE_CHAR;

    for (unsigned int i = 0; i < size; i++) {
        const unsigned char c = input[i];

        if (is_printable_char(c)) {
            input[i] = MIN_CHAR + ((c - MIN_CHAR + shift) % RANGE_CHAR);
        }
    }

    return input;
}

unsigned char* encrypt (unsigned char* originalText, int len, unsigned char* key) {
    unsigned char *input_buffer = malloc((len + 1) * sizeof(unsigned char)),
         *output_buffer = malloc((len + 1) * sizeof(unsigned char)),
         *temp;

    FILE* timeLog = fopen("out/crypt/encryptTimeLog.csv","a");

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    memcpy(input_buffer, originalText, len + 1);
    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    // Repeticoes de transposicao com cada chave
    for (unsigned int i = 0; i < N_REP_CRYPT; i++) {
        transposition_encrypt(output_buffer, input_buffer, key, len, i);
        substitution_encrypt(output_buffer, key, len);
        xor_strings(output_buffer, key, len, i);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    long long duracao_ns = diff_ns(inicio, fim);
    fprintf(timeLog, "%lld,", duracao_ns);
    fclose(timeLog);

    memcpy(output_buffer, input_buffer, len);
    output_buffer[len] = '\0';
    free(input_buffer);

    return output_buffer;
}

unsigned char* decrypt(unsigned char* cryptedText, int len, unsigned char* key) {
    unsigned char* input_buffer = malloc((len + 1) * sizeof(unsigned char));
    unsigned char* output_buffer = malloc((len + 1) * sizeof(unsigned char));
    unsigned char* temp;

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    // printf("LEN DECRYPT %d\n", len);

    FILE* timeLog = fopen("out/crypt/decryptTimeLog.csv","a");
    memcpy(input_buffer, cryptedText, len + 1);

    struct timespec inicio, fim;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (unsigned int i = N_REP_CRYPT; i > 0; i--) {
        xor_strings (input_buffer, key, len, i - 1);
        substitution_decrypt(input_buffer, key, len);
        transposition_decrypt(output_buffer, input_buffer, key, len, i - 1);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    long long duracao_ns = diff_ns(inicio, fim);
    fprintf(timeLog, "%lld,", duracao_ns);
    fclose(timeLog);

    memcpy(output_buffer, input_buffer, len);
    output_buffer[len] = '\0';

    free(input_buffer);

    return output_buffer;
}
