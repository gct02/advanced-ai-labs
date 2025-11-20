import numpy as np


def parse_results_file(filepath):
    with open(filepath, "r") as f:
        lines = f.readlines()

    instance_results = {}
    instance_index = 0

    for line in lines:
        if not line.strip():
            continue

        if line.startswith("Instance "):
            instance_index += 1

        if instance_index not in instance_results:
            instance_results[instance_index] = []

        instance_results[instance_index].append(line)

    results_dict = {
        "builtin": {
            "expanded": [],
            "search_time": [],
            "total_time": []
        },
        "planopt": {
            "expanded": [],
            "search_time": [],
            "total_time": []
        }
    }

    # Instance 1 with k=1000
    # Extracted PATTERN: [56, 57, 58, 59, 60, 61, 62, 63, 64]
    # Built-in implementation
    # [t=3.535239s, 90100 KB] Expanded until last jump: 943826 state(s).
    # [t=3.535239s, 90100 KB] Search time: 3.526239s
    # [t=3.535239s, 90100 KB] Total time: 3.535239s
    # PlanOpt implementation
    # [t=3.528553s, 90316 KB] Expanded until last jump: 943826 state(s).
    # [t=3.528553s, 90316 KB] Search time: 3.507553s
    # [t=3.528553s, 90316 KB] Total time: 3.528553s
    
    for instance_result_lines in instance_results.values():
        builtin_expanded_text_line = instance_result_lines[3]
        builtin_search_time_text_line = instance_result_lines[4]
        builtin_total_time_text_line = instance_result_lines[5]

        planopt_expanded_text_line = instance_result_lines[7]
        planopt_search_time_text_line = instance_result_lines[8]
        planopt_total_time_text_line = instance_result_lines[9]

        builtin_expanded = int(builtin_expanded_text_line.split(": ")[-1].split(" state")[0].strip())
        builtin_search_time = float(builtin_search_time_text_line.split(": ")[-1].split("s")[0].strip())
        builtin_total_time = float(builtin_total_time_text_line.split(": ")[-1].split("s")[0].strip())

        planopt_expanded = int(planopt_expanded_text_line.split(": ")[-1].split(" state")[0].strip())
        planopt_search_time = float(planopt_search_time_text_line.split(": ")[-1].split("s")[0].strip())
        planopt_total_time = float(planopt_total_time_text_line.split(": ")[-1].split("s")[0].strip())

        results_dict["builtin"]["expanded"].append(builtin_expanded)
        results_dict["builtin"]["search_time"].append(builtin_search_time)
        results_dict["builtin"]["total_time"].append(builtin_total_time)

        results_dict["planopt"]["expanded"].append(planopt_expanded)
        results_dict["planopt"]["search_time"].append(planopt_search_time)
        results_dict["planopt"]["total_time"].append(planopt_total_time)

    return results_dict


if __name__ == "__main__":
    from sys import argv

    results_file = argv[1]

    results_dict = parse_results_file(results_file)

    builtin_expanded_mean = np.mean(np.array(results_dict["builtin"]["expanded"]))
    builtin_search_time_mean = np.mean(np.array(results_dict["builtin"]["search_time"]))
    builtin_total_time_mean = np.mean(np.array(results_dict["builtin"]["total_time"]))

    builtin_expanded_std = np.std(np.array(results_dict["builtin"]["expanded"]))
    builtin_search_time_std = np.std(np.array(results_dict["builtin"]["search_time"]))
    builtin_total_time_std = np.std(np.array(results_dict["builtin"]["total_time"]))

    planopt_expanded_mean = np.mean(np.array(results_dict["planopt"]["expanded"]))
    planopt_search_time_mean = np.mean(np.array(results_dict["planopt"]["search_time"]))
    planopt_total_time_mean = np.mean(np.array(results_dict["planopt"]["total_time"]))

    planopt_expanded_std = np.std(np.array(results_dict["planopt"]["expanded"]))
    planopt_search_time_std = np.std(np.array(results_dict["planopt"]["search_time"]))
    planopt_total_time_std = np.std(np.array(results_dict["planopt"]["total_time"]))

    print("Built-in Implementation:")
    print(f"  Expanded States: Mean = {builtin_expanded_mean}, Std = {builtin_expanded_std}")
    print(f"  Search Time: Mean = {builtin_search_time_mean}, Std = {builtin_search_time_std}")
    print(f"  Total Time: Mean = {builtin_total_time_mean}, Std = {builtin_total_time_std}")

    print("\nPlanOpt Implementation:")
    print(f"  Expanded States: Mean = {planopt_expanded_mean}, Std = {planopt_expanded_std}")
    print(f"  Search Time: Mean = {planopt_search_time_mean}, Std = {planopt_search_time_std}")
    print(f"  Total Time: Mean = {planopt_total_time_mean}, Std = {planopt_total_time_std}")