import subprocess
from pathlib import Path

def parse_args():
    from argparse import ArgumentParser
    parser = ArgumentParser(description="Run Fast Downward with specified parameters.")
    parser.add_argument("algorithm", type=str, help="The planning algorithm to use.")
    parser.add_argument("heuristic", type=str, help="The heuristic to use.")
    parser.add_argument("-o", "--output-dir", type=str, default="outputs", help="Directory to store output files.")
    return parser.parse_args()

def run_fast_downward(problem_file, algorithm, heuristic, output_file):
    # Set time and memory limit with ulimit
    subprocess.run("ulimit -t 60; ulimit -v 2000000", shell=True, executable="/bin/bash")

    # Run Fast Downward with the specified parameters
    try:
        if algorithm == 'astar':
            heuristic = f'{heuristic}()'
        else:
            heuristic = f'[{heuristic}()]'
        subprocess.check_output(
            f"./fast-downward/fast-downward.py castle/domain.pddl {problem_file} --search \"{algorithm}({heuristic})\" > {output_file}",
            shell=True
        )
    except Exception as e:
        print(e)

def main():
    args = parse_args()
    algorithm = args.algorithm
    heuristic = args.heuristic
    output_dir = Path(args.output_dir)

    output_dir.mkdir(parents=True, exist_ok=True)

    problem_files = []
    for file in Path("castle").glob("castle-*.pddl"):
        problem_files.append(file)

    for problem_file in problem_files:
        output_file = output_dir / problem_file.name.replace(".pddl", f"-{algorithm}-{heuristic}.out")
        run_fast_downward(problem_file, algorithm, heuristic, output_file)
        print(f"Output written to {output_file}")

if __name__ == "__main__":
    main()
