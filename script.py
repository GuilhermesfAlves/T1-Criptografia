#!/usr/bin/env python3
import subprocess
import time
import difflib
import csv
import argparse
import os
import glob
from pathlib import Path

# Caminhos para os binários
CRYPT_BIN = Path("bin/crypt")
AES_BIN = Path("bin/aes")

# Diretórios de dados e saída
DATA_DIR = Path("data")
OUT_DIR = Path("out")

def run_command(cmd, input_data=None):
    """Executa um comando e retorna (stdout, stderr, tempo_em_segundos)"""
    start = time.perf_counter()
    proc = subprocess.run(
        cmd,
        input=input_data,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False
    )
    end = time.perf_counter()
    return proc.stdout, proc.stderr, end - start

def ensure_dirs(algo):
    """Garante que os diretórios de saída existam"""
    algo_dir = OUT_DIR / algo
    encrypted_dir = algo_dir / "encrypted"
    decrypted_dir = algo_dir / "decrypted"
    
    for d in [algo_dir, encrypted_dir, decrypted_dir]:
        d.mkdir(parents=True, exist_ok=True)
    
    return encrypted_dir, decrypted_dir

def show_diff(original_bytes, decrypted_bytes, filename):
    """Mostra um diff legível entre os textos original e decifrado"""
    try:
        original_text = original_bytes.decode('utf-8')
        decrypted_text = decrypted_bytes.decode('utf-8')
    except UnicodeDecodeError:
        # Se não for UTF-8, mostra como hex
        original_text = original_bytes.hex()
        decrypted_text = decrypted_bytes.hex()

    diff = difflib.unified_diff(
        original_text.splitlines(keepends=True),
        decrypted_text.splitlines(keepends=True),
        fromfile=f"{filename} (original)",
        tofile=f"{filename} (decrypted)"
    )

    diff_text = ''.join(diff)
    if diff_text.strip():
        print("  Diferenças encontradas:")
        for line in diff_text.splitlines():
            if line.startswith("+"):
                print(f"    \033[32m{line}\033[0m")  # verde
            elif line.startswith("-"):
                print(f"    \033[31m{line}\033[0m")  # vermelho
            else:
                print(f"    {line}")

def benchmark(csv_mode=False):
    # Lista de algoritmos para testar
    algorithms = [
        ("crypt", CRYPT_BIN),
        ("aes", AES_BIN)
    ]

    files = sorted(DATA_DIR.glob("*"))
    if not files:
        print(f"Nenhum arquivo encontrado em {DATA_DIR}/")
        return

    results = []

    for algo_name, algo_bin in algorithms:
        print(f"=== ALGORITMO: {algo_name.upper()} ===")
        
        encrypted_dir, decrypted_dir = ensure_dirs(algo_name)

        for file in files:
            if not file.is_file():
                continue

            print(f"Arquivo: {file.name}")

            # === 1. Encrypt ===
            plain_data = file.read_bytes()

            if algo_name == "crypt":
                # Para crypt: gerar chave e usar nos comandos
                keys_dir = (OUT_DIR / algo_name / "keys")
                keys_dir.mkdir(parents=True, exist_ok=True)
                
                key_path = keys_dir / f"{file.stem}.key"
                stdout, stderr, _ = run_command([str(algo_bin), "key"])
                if stderr:
                    print(f"  Erro ao gerar chave: {stderr.decode()}")
                    continue
                key_path.write_bytes(stdout)

                # Comandos com chave
                encrypt_cmd = [str(algo_bin), "encrypt", str(key_path)]
                decrypt_cmd = [str(algo_bin), "decrypt", str(key_path)]
            else:
                # Para aes: comandos sem chave
                encrypt_cmd = [str(algo_bin), "encrypt"]
                decrypt_cmd = [str(algo_bin), "decrypt"]

            enc_path = encrypted_dir / f"{file.stem}.enc"
            enc_stdout, enc_stderr, enc_time = run_command(
                encrypt_cmd,
                input_data=plain_data
            )
            if enc_stderr:
                print(f"  Erro ao criptografar: {enc_stderr.decode()}")
                show_diff(plain_data, enc_stdout, file.name)
                continue

            enc_path.write_bytes(enc_stdout)

            # === 2. Decrypt ===
            dec_stdout, dec_stderr, dec_time = run_command(
                decrypt_cmd,
                input_data=enc_stdout
            )
            if dec_stderr:
                print(f"  Erro ao descriptografar: {dec_stderr.decode()}")
                show_diff(plain_data, dec_stdout, file.name)
                continue

            dec_path = decrypted_dir / f"{file.stem}.dec"
            dec_path.write_bytes(dec_stdout)

            # === 3. Comparar ===
            ok = plain_data == dec_stdout
            status = "OK" if ok else "Falha"

            print(f"  Tempo encrypt: {enc_time*1000:.2f} ms")
            print(f"  Tempo decrypt: {dec_time*1000:.2f} ms")
            print(f"  Verificação: {status}")

            if not ok:
                show_diff(plain_data, dec_stdout, file.name)

            # === 4. Registrar resultados ===
            results.append({
                "algoritmo": algo_name,
                "arquivo": file.name,
                "tam_bytes": len(plain_data),
                "tempo_encrypt_ms": enc_time * 1000,
                "tempo_decrypt_ms": dec_time * 1000,
                "status": "OK" if ok else "FALHA"
            })

            print()
        print()

    # === Exportar CSV ===
    if csv_mode and results:
        csv_path = OUT_DIR / "benchmark.csv"
        with open(csv_path, "w", newline="") as csvfile:
            writer = csv.DictWriter(csvfile, fieldnames=results[0].keys())
            writer.writeheader()
            writer.writerows(results)
        print(f"Resultados salvos em {csv_path}")

    print("Benchmark concluído.")

def limpar_csv():
    pastas = ["out/crypt", "out/aes"]
    for pasta in pastas:
        padrao = os.path.join(pasta, "*.csv")
        for arquivo in glob.glob(padrao):
            try:
                with open(arquivo, "w", encoding="utf-8") as f:
                    pass 
            except OSError as e:
                print(f"Erro ao remover {arquivo}: {e}")

def pular_linha_csv():
    pastas = ["out/crypt", "out/aes"]
    for pasta in pastas:
        padrao = os.path.join(pasta, "*.csv")
        for arquivo in glob.glob(padrao):
            with open(arquivo, "a", encoding="utf-8") as f:
                f.write("\n")

def log_tamanhos_csv():
    pastas = ["out/crypt", "out/aes"]
    for pasta in pastas:
        arquivos = list(Path(pasta).glob("*.csv"))
        if not arquivos:
            continue

        data_files = sorted(Path("data").glob("*"))

        nomes_str = ','.join([str(f.name) for f in data_files])
        tamanhos_str = ','.join([str(f.stat().st_size) for f in data_files])

        for f in arquivos:
            try:
                with open(f, "a", encoding="utf-8") as file:
                    file.write(f"{nomes_str}\n{tamanhos_str}\n")
            except OSError as e:
                print(f"Erro ao escrever em {f}: {e}")

if __name__ == "__main__":
    limpar_csv()
    parser = argparse.ArgumentParser(description="Benchmark de criptografia")
    parser.add_argument("--csv", action="store_true", help="Exporta resultados para out/benchmark.csv")
    args = parser.parse_args()
    log_tamanhos_csv()
    for i in range(20):        
        benchmark(csv_mode=args.csv)
        pular_linha_csv()