#!/usr/bin/env python3

import subprocess
import re

#novo script de comparação, usar para dar uma olhada na média das heurísticas

def normalize_scientific_notation(number_str):
    """Normaliza números em notação científica para formato consistente"""
    # Verifica se é um número em notação científica
    if 'e' in number_str.lower():
        try:
            # Converte para float e depois para string com formato fixo
            num = float(number_str)
            # Usa formatação para evitar notação científica
            if abs(num) < 0.0001:
                return f"{num:.10f}".rstrip('0').rstrip('.')
            else:
                return f"{num:.8f}".rstrip('0').rstrip('.')
        except ValueError:
            return number_str
    return number_str

def normalize_line(line):
    """Normaliza todos os números em notação científica em uma linha"""
    if not line:
        return line
    
    cols = line.split(',')
    normalized_cols = []
    for col in cols:
        normalized_cols.append(normalize_scientific_notation(col.strip()))
    
    return ','.join(normalized_cols)

def run_test(algorithm, puzzles, expected_output, test_name):
    """Executa um teste e mostra os resultados obtidos e esperados"""
    print("=" * 60)
    print(f"TESTE: {test_name}")
    print(f"ALGORITMO: {algorithm}")
    print(f"PUZZLES: {puzzles}")
    print("-" * 60)

    # Executa o comando
    try:
        result = subprocess.run(
            ["./main", f"-{algorithm}"] + puzzles.split(),
            capture_output=True,
            text=True,
            timeout=30
        )
        obtained = result.stdout.strip()
    except Exception as e:
        print(f"❌ Erro ao executar: {e}")
        return

    # Processa resultados
    expected_lines = [line.strip() for line in expected_output.strip().split('\n') if line.strip()]
    obtained_lines = [line.strip() for line in obtained.split('\n') if line.strip()]

    print("📊 RESULTADOS OBTIDOS vs ESPERADOS:")
    print("-" * 100)
    print(f"{'':<4} {'Nós':<8} {'Sol.':<6} {'Tempo':<15} {'Heur. Média':<12} {'Heur. Ini.':<10}")
    print("-" * 100)

    # Primeiro mostra os resultados obtidos
    print("🔹 OBTIDOS:")
    for i, obt_line in enumerate(obtained_lines, 1):
        if not obt_line:
            continue

        obt_cols = obt_line.split(',')
        if len(obt_cols) >= 5:
            normalized_cols = [normalize_scientific_notation(col) for col in obt_cols]
            print(f"{i:<4} {normalized_cols[0]:<8} {normalized_cols[1]:<6} {
                  normalized_cols[2]:<15} {normalized_cols[3]:<12} {normalized_cols[4]:<10}")
        else:
            print(f"{i:<4} Formato inválido: {obt_line}")

    print()
    print("🔹 ESPERADOS:")
    for i, exp_line in enumerate(expected_lines, 1):
        if not exp_line:
            continue

        exp_cols = exp_line.split(',')
        if len(exp_cols) >= 5:
            normalized_cols = [normalize_scientific_notation(col) for col in exp_cols]
            print(f"{i:<4} {normalized_cols[0]:<8} {normalized_cols[1]:<6} {
                  normalized_cols[2]:<15} {normalized_cols[3]:<12} {normalized_cols[4]:<10}")
        else:
            print(f"{i:<4} Formato inválido: {exp_line}")

    print("-" * 100)
    print()


def main():
    """Função principal"""
    print("🎯 TESTES 8PUZZLES - VISUALIZAÇÃO DE RESULTADOS")
    print("Mostrando todos os dados obtidos e esperados (valores normalizados)")
    print("Colunas: 1-Nós, 2-Solução, 3-Tempo, 4-Heurística Média, 5-Heurística Inicial")
    print()
    # Teste A*
    run_test(
        "astar",
        "0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5",
        """91,16,0.0000479221,6.72561,10
612,21,0.000200987,9.81136,11
511,23,0.000167847,9.97655,15""",
        "A* Search"
    )

    # Teste BFS
    run_test(
        "bfs",
        "0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5",
        """5209,16,0.00154996,0,10
42704,21,0.011677,0,11
86288,23,0.022454,0,15""",
        "Breadth-First Search"
    )

    # Teste IDA*
    run_test(
        "idastar",
        "0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5",
        """165,16,0.0000319481,7.13781,10
439,21,0.0000369549,10.75,11
724,23,0.0000538826,11.7241,15""",
        "Iterative Deepening A*"
    )

    # Teste GBFS
    run_test(
        "gbfs",
        "0 6 1 7 4 2 3 8 5, 5 0 2 6 4 8 1 7 3, 2 4 7 0 3 6 8 1 5",
        """16,16,0.0000259876,5.5,10
96,27,0.0000660419,8.05422,11
90,57,0.0000729561,6.80982,15""",
        "Greedy Best-First Search"
    )
if __name__ == "__main__":
    main()