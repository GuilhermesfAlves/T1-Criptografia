#include "crypt.h"

// Headers de ordenacao
int compare_pairs(const void* a, const void* b);
unsigned int* columns_sequence_by_key (char* key, unsigned int start, unsigned int end);

// Headers de formatacao de strings
char* fix_string (char* string, unsigned int size);
char* xor_strings (char *a, char *b, unsigned int size, unsigned int iteration);

// Cifra de transposicao
char* transposition_cipher (char* cryptedText, char* originalText, char* key, unsigned int iteration);
char* transposition_decipher (char* decryptedText, char* originalText, char* key, unsigned int iteration);

char* generate_random_key () {
    char* newKey = malloc((MAX_KEY_SIZE + 1) * sizeof(char));
    if(!newKey) {
        printf("ERRO: não foi possível alocar memória para a NOVA CHAVE");
        exit(1);
    }

    for(unsigned int i = 0; i < MAX_KEY_SIZE; i++)
        newKey[i] = 65 + (rand() % 26);

    newKey[MAX_KEY_SIZE] = '\0';

    return newKey;
}

bool check_invalid_key (char* key) {
    if (strlen(key) != MAX_KEY_SIZE) return true;

    for(unsigned int i = 0; key[i] != '\0'; i++)
        if ((key[i] < 'A') || (key[i] > 'Z'))
            return true;

    return false;
}

char* fix_string (char* string, unsigned int size) {
    for (unsigned int i = 0; i < size; i++) {
        // Letras em maisculo
        string[i] = toupper(string[i]);

        // Substituir espaços
        if (string[i] == ' ')
            string[i] = 'x';
    }

    return string;
}

int compare_pairs(const void* a, const void* b) {
    KeyPair* pairA = (KeyPair*)a;
    KeyPair* pairB = (KeyPair*)b;

    if (pairA->character < pairB->character) return -1;
    if (pairA->character > pairB->character) return 1;

    if (pairA->original_index < pairB->original_index) return -1;
    if (pairA->original_index > pairB->original_index) return 1;

    return 0;
}

unsigned int* columns_sequence_by_key (char* key, unsigned int start, unsigned int end) {
    if (key == NULL || strlen(key) == 0 || start > end) {
        return NULL;
    }

    const unsigned int length = end - start,
                       keyLength = strlen(key);

    KeyPair* pairs = malloc(length * sizeof(KeyPair));
    unsigned int* result_indices = malloc(length * sizeof(unsigned int));

    if (!pairs || !result_indices) {
        free(pairs);
        free(result_indices);
        return NULL;
    }

    for (unsigned int i = 0; i < length; i++) {
        pairs[i].character = key[(start + i) % keyLength];
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
    for (unsigned int i = 0; i < size; i++)
        a[i] = a[i] ^ b[((i + (iteration * BLOCK_SIZE)) % size)];

    return a;
}

char* transposition_cipher (char* cryptedText, char* originalText, char* key, unsigned int iteration) {
    unsigned int transpositionIndex,
                 index = 0,
                 *columnSequence = columns_sequence_by_key(key, BLOCK_SIZE * iteration, (BLOCK_SIZE * (iteration + 1)));

    if (!columnSequence) {
        printf("ERRO: não foi possível alocar memória para a coluna de transposicao");
        return NULL;
    }

    // Decifra da transposicao
    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            transpositionIndex = (columnSequence[i] + (j * BLOCK_SIZE));
            cryptedText[index] = originalText[transpositionIndex];
            index++;
        }
    }

    cryptedText[index] = '\0';

    free(columnSequence);

    return cryptedText;
}

char* transposition_decipher (char* decryptedText, char* originalText, char* key, unsigned int iteration) {
    unsigned int transpositionIndex,
                 index = 0,
                 *columnSequence = columns_sequence_by_key(key, BLOCK_SIZE * iteration, (BLOCK_SIZE * (iteration + 1)));

    if (!columnSequence) {
        printf("ERRO: não foi possível alocar memória para a coluna de transposicao");
        return NULL;
    }

    for (int i = 0; i < BLOCK_SIZE; i++) {
        for (int j = 0; j < BLOCK_SIZE; j++) {
            transpositionIndex = (columnSequence[i] + (j * BLOCK_SIZE));
            decryptedText[transpositionIndex] = originalText[index];

            index++;
        }
    }

    decryptedText[index] = '\0';

    free(columnSequence);

    return decryptedText;
}

char* cipher (char* originalText, char* key) {
    char *input_buffer = malloc((MAX_CIPHER_LEN + 1) * sizeof(char)),
         *output_buffer = malloc((MAX_CIPHER_LEN + 1) * sizeof(char)),
         *temp;

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    strncpy(input_buffer, originalText, MAX_CIPHER_LEN);
    input_buffer[MAX_CIPHER_LEN] = '\0';
    fix_string(input_buffer, MAX_CIPHER_LEN);

    // Repeticoes de transposicao com cada chave
    for (unsigned int i = 0; i < BLOCK_SIZE; i++) {
        transposition_cipher(output_buffer, input_buffer, key, i);
        xor_strings (output_buffer, key, MAX_CIPHER_LEN, i);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }

    memcpy(output_buffer, input_buffer, MAX_CIPHER_LEN);
    output_buffer[MAX_CIPHER_LEN] = '\0';

    free(input_buffer);

    return output_buffer;
}

char* decipher (char* cryptedText, char* key) {
    char* input_buffer = malloc((MAX_CIPHER_LEN + 1) * sizeof(char));
    char* output_buffer = malloc((MAX_CIPHER_LEN + 1) * sizeof(char));
    char* temp;

    if (!output_buffer || !input_buffer) {
        printf("ERRO: não foi possível alocar memória para o texto criptografado");
        free(input_buffer);
        free(output_buffer);
        return NULL;
    }

    memcpy(input_buffer, cryptedText, MAX_CIPHER_LEN);
    input_buffer[MAX_CIPHER_LEN] = '\0';

    for (unsigned int i = BLOCK_SIZE; i > 0; i--) {
        xor_strings (input_buffer, key, MAX_CIPHER_LEN, i - 1);
        transposition_decipher(output_buffer, input_buffer, key, i - 1);

        temp = input_buffer;
        input_buffer = output_buffer;
        output_buffer = temp;
    }

    memcpy(output_buffer, input_buffer, MAX_CIPHER_LEN);
    output_buffer[MAX_CIPHER_LEN] = '\0';

    free(input_buffer);

    return output_buffer;
}
