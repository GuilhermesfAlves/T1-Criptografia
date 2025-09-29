import sys

# O padrão Unicode vai de 0x0 até 0x10FFFF
# 0x10FFFF em decimal é 1.114.111
MAX_CODEPOINT = 0x10FFFF

# Nome do arquivo de saída
output_filename = 'todos_os_caracteres_utf8.txt'

print(f"Gerando todos os {MAX_CODEPOINT + 1} pontos de código Unicode para o arquivo '{output_filename}'...")
print("Isso pode demorar um pouco.")

try:
    with open(output_filename, 'w', encoding='utf-8') as f:
        for i in range(MAX_CODEPOINT + 1):
            # Alguns pontos de código na faixa de "surrogates" (0xD800-0xDFFF)
            # não podem ser codificados sozinhos em UTF-16, e o Python pode
            # levantar um UnicodeEncodeError ao tentar convertê-los diretamente.
            # Portanto, é bom pular essa faixa.
            if 0xD800 <= i <= 0xDFFF:
                continue

            try:
                # Converte o número (ponto de código) para o caractere correspondente
                char = chr(i)
                # Escreve o caractere no arquivo
                f.write(char)
            except UnicodeEncodeError:
                # Ignora erros de caracteres que não podem ser codificados
                # (embora o 'if' acima já deva pegar a maioria dos casos problemáticos)
                pass

    print(f"Arquivo '{output_filename}' gerado com sucesso!")
    # Adiciona uma quebra de linha no final para terminais
    with open(output_filename, 'a', encoding='utf-8') as f:
        f.write('\n')

except IOError as e:
    print(f"Erro ao escrever no arquivo: {e}")
except Exception as e:
    print(f"Ocorreu um erro inesperado: {e}")
