#!/usr/bin/env python3

import subprocess

# script para testar corretude das soluções dos exemplos do pdf
# é basicamente o compare.py antigo


def run_test(algorithm, puzzles, expected_output, test_name):
    """Executa um teste e compara os resultados"""
    print("=" * 50)
    print(f"TESTE: {test_name}")
    print(f"ALGORITMO: {algorithm}")
    print(f"PUZZLES: {puzzles}")
    print("-" * 50)

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
    expected_lines = expected_output.strip().split('\n')
    obtained_lines = obtained.split('\n')

    print("📊 RESULTADOS (colunas 1, 2, 5):")
    print("-" * 40)
    print(f"{'':<6} {'Nós':<8} {'Sol.':<6} {'Heur. Ini.':<10} {'Status':<8}")
    print("-" * 40)

    all_correct = True

    for i, (exp_line, obt_line) in enumerate(zip(expected_lines, obtained_lines), 1):
        if not exp_line or not obt_line:
            continue

        # Extrai colunas 1, 2, 5
        exp_cols = exp_line.split(',')
        obt_cols = obt_line.split(',')

        if len(exp_cols) < 5 or len(obt_cols) < 5:
            print(f"Linha {i}: Formato inválido")
            all_correct = False
            continue

        exp_compare = (exp_cols[0], exp_cols[1], exp_cols[4])
        obt_compare = (obt_cols[0], obt_cols[1], obt_cols[4])

        status = "✅" if exp_compare == obt_compare else "❌"
        if status == "❌":
            all_correct = False

        print(f"{i:<6} {obt_cols[0]:<8} {obt_cols[1]:<6} {
              obt_cols[4]:<10} {status:<8}")

        # Mostra diferenças se houver
        if status == "❌":
            print(f"      Esperado: {exp_cols[0]}, {
                  exp_cols[1]}, {exp_cols[4]}")

    print("-" * 40)
    print(f"STATUS GERAL: {
          '✅ TODOS CORRETOS' if all_correct else '❌ ALGUM ERRO'}")
    print()


def main():
    """Função principal"""
    print("🎯 TESTES 8PUZZLES")
    print("Comparando colunas: 1 (nós), 2 (solução), 5 (heurística inicial)")
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