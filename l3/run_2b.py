import subprocess
from pathlib import Path

input_dir = 'exercise-c/castle'
output_dir = 'exercise-c/output-2b'

fast_downward_path = 'exercise-c/fast-downward/fast-downward.py'

domain_path = 'exercise-c/castle/domain.pddl'

for instance_path in Path('exercise-c/castle').iterdir():
    if not instance_path.name.startswith('castle'):
        continue

    instance_name = instance_path.name.split('.')[0]
    astar_output_path = Path(output_dir) / (instance_name + '-astar.txt')
    blind_output_path = Path(output_dir) / (instance_name + '-blind.txt')

    subprocess.check_output(
        f"python3 {fast_downward_path} --translate {domain_path} {instance_path.as_posix()} --search \"astar(planopt_relaxed_task_graph())\" > {astar_output_path.as_posix()}",
        shell=True
    )
    subprocess.check_output(
        f"python3 {fast_downward_path} --translate {domain_path} {instance_path.as_posix()} --search \"blind(planopt_relaxed_task_graph())\" > {blind_output_path.as_posix()}",
        shell=True
    )