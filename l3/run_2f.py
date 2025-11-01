import subprocess
from pathlib import Path

input_dir = 'exercise-c/castle'
output_dir = 'exercise-c/output-2f'

fast_downward_path = 'exercise-c/fast-downward/fast-downward.py'

domain_path = 'exercise-c/castle/domain.pddl'

for instance_path in Path('exercise-c/castle').iterdir():
    if not instance_path.name.startswith('castle'):
        continue

    instance_name = instance_path.name.split('.')[0]
    planopt_ff_output_path = Path(output_dir) / (instance_name + '-planopt_ff.txt')
    ff_output_path = Path(output_dir) / (instance_name + '-ff.txt')

    subprocess.check_output(
        f"python3 {fast_downward_path} --translate {domain_path} {instance_path.as_posix()} --search \"eager_greedy(planopt_ff())\" > {planopt_ff_output_path.as_posix()}",
        shell=True
    )
    subprocess.check_output(
        f"python3 {fast_downward_path} --translate {domain_path} {instance_path.as_posix()} --search \"eager_greedy(ff())\" > {ff_output_path.as_posix()}",
        shell=True
    )