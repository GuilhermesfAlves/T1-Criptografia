#include "../include/crypt.h"

// Headers de ordenacao
int compare_pairs(const void* a, const void* b);
unsigned int* columns_sequence_by_key (char* key, unsigned int start, unsigned int end);

// Headers de formatacao de strings
char* xor_strings (char *a, char *b, unsigned int size, unsigned int iteration);

// Cifra de Substituição
char* substitution_encrypt(char* input, char* key, unsigned int size);
char* substitution_decrypt(char* input, char* key, unsigned int size);

// Cifra de transposicao
char* transposition_encrypt (char* cryptedText, char* originalText, char* key, unsigned int len, unsigned int iteration);
char* transposition_decrypt (char* decryptedText, char* originalText, char* key, unsigned int len, unsigned int iteration);

char* generate_random_key () {
    char* newKey = malloc((KEY_LEN + 1) * sizeof(char));

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
bool is_printable_char(char i) {
    return (MIN_CHAR <= i && i <= MAX_CHAR);
}

bool is_key_valid (char* key) {
    for (unsigned int i = 0; i < KEY_LEN; i++)
        if (!is_printable_char(key[i]))
            return false;

    return true;
}

int compare_pairs(const void* a, const void* b) {
    KeyPair* pairA = (KeyPair*)a;
    KeyPair* pairB = (KeyPair*)b;

    // ordena com base no char
    if (pairA -> character < pairB -> character) return -1;
    if (pairA -> character > pairB -> character) return 1;

    // caso seja igual, ordena com base no index
    if (pairA -> original_index < pairB -> original_index) return -1;
    if (pairA -> original_index > pairB -> original_index) return 1;

    return 0;
}

unsigned int* columns_sequence_by_key (char* key, unsigned int start, unsigned int end) {
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

char* xor_strings (char *a, char *b, unsigned int size, unsigned int iteration) {
    for (unsigned int i = 0; i < size; i++){
        const char x = a[i] ^ b[((i + (iteration * BLOCK_LEN)) % KEY_LEN)];
        if (is_printable_char(x))
            a[i] = x;
        // caso não seja caracter printável mantém
    }

    return a;
}

char* transposition_encrypt (char* cryptedText, char* originalText, char* key, unsigned int len, unsigned int iteration) {
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

char* transposition_decrypt (char* decryptedText, char* originalText, char* key, unsigned int len, unsigned int iteration) {
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

char* substitution_encrypt(char* input, char* key, unsigned int size) {
    unsigned int shift = 0;
    // Gera deslocamento a partir da chave
    for (unsigned int i = 0; i < KEY_LEN; i++)
        shift = (shift * 31 + key[i]) % RANGE_CHAR;

    for (unsigned int i = 0; i < size; i++) {
        const char c = input[i];

        if (is_printable_char(c)) {
            input[i] = MIN_CHAR + ((c - MIN_CHAR - shift + RANGE_CHAR) % RANGE_CHAR);
        }
    }

    return input;
}

// Cifra de César
char* substitution_decrypt(char* input, char* key, unsigned int size) {
    unsigned int shift = 0;
    // Gera deslocamento a partir da chave
    for (unsigned int i = 0; i < KEY_LEN; i++)
        shift = (shift * 31 + key[i]) % RANGE_CHAR;

    for (unsigned int i = 0; i < size; i++) {
        const char c = input[i];

        if (is_printable_char(c)) {
            input[i] = MIN_CHAR + ((c - MIN_CHAR + shift) % RANGE_CHAR);
        }
    }

    return input;
}

char* encrypt (char* originalText, int len, char* key) {
    char *input_buffer = malloc((len + 1) * sizeof(char)),
         *output_buffer = malloc((len + 1) * sizeof(char)),
         *temp;

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    memcpy(input_buffer, originalText, len + 1);

    // Repeticoes de transposicao com cada chave
    for (unsigned int i = 0; i < N_REP_CRYPT; i++) {
        transposition_encrypt(output_buffer, input_buffer, key, len, i);
        substitution_encrypt(output_buffer, key, len);
        xor_strings(output_buffer, key, len, i);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }

    memcpy(output_buffer, input_buffer, len);
    output_buffer[len] = '\0';
    free(input_buffer);

    return output_buffer;
}

char* decrypt(char* cryptedText, int len, char* key) {
    char* input_buffer = malloc((len + 1) * sizeof(char));
    char* output_buffer = malloc((len + 1) * sizeof(char));
    char* temp;

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    memcpy(input_buffer, cryptedText, len + 1);

    for (unsigned int i = N_REP_CRYPT; i > 0; i--) {
        xor_strings (input_buffer, key, len, i - 1);
        substitution_decrypt(input_buffer, key, len);
        transposition_decrypt(output_buffer, input_buffer, key, len, i - 1);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }

    memcpy(output_buffer, input_buffer, len);
    output_buffer[len] = '\0';

    free(input_buffer);

    return output_buffer;
}
