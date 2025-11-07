#!/usr/bin/env python3

"""
Parses one or more Fast Downward log files and appends the key
results to a CSV file named 'summary.csv'.

This script is designed to extract information from both the
translator and the search components of the log.

Usage:
    python parse_fd_log.py log1.txt log2.txt ... --output summary.csv
"""

import re
import csv
import os

def parse_log(log_content):
    """
    Extracts key information from a Fast Downward log file.

    Args:
        log_content (str): The full text content of the log file.

    Returns:
        dict: A dictionary containing all extracted data.
    """
    data = {
        'domain': None,
        'problem': None,
        'algorithm': None,
        'heuristic': None,
        'solution_found': False,
        'plan_length': None,
        'plan_cost': None,
        'expanded_states': None,
        'evaluated_states': None,
        'generated_states': None,
        'search_time_s': None,
        'total_time_s': None,
        'search_peak_memory_kb': None,
        'translator_variables': None,
        'translator_operators': None,
        'translator_goal_facts': None,
        'translator_time_s': None,
        'translator_peak_memory_kb': None,
    }

    # --- Extract Problem & Algorithm Info ---
    
    # Ex: ... translate.py castle/domain.pddl castle/castle-2-2-8-cards.pddl ...
    m = re.search(r'translate\.py\s+(?:.*?/)?(.+?)\.pddl\s+(?:.*?/)?(.+?)\.pddl', log_content)
    if m:
        data['domain'] = m.group(1)
        data['problem'] = m.group(2)

    # Ex: ... --search 'astar(ff())' ...
    m = re.search(r"--search\s+'(.+?)\((.*)\)'", log_content)
    if m:
        data['algorithm'] = m.group(1)
        # Clean up heuristic name, e.g., "ff()" -> "ff"
        data['heuristic'] = m.group(2).replace('()', '')

    # --- Extract Translator Stats ---
    m = re.search(r'Translator variables: (\d+)', log_content)
    if m: data['translator_variables'] = int(m.group(1))

    m = re.search(r'Translator operators: (\d+)', log_content)
    if m: data['translator_operators'] = int(m.group(1))

    m = re.search(r'Translator goal facts: (\d+)', log_content)
    if m: data['translator_goal_facts'] = int(m.group(1))

    m = re.search(r'Translator peak memory: (\d+) KB', log_content)
    if m: data['translator_peak_memory_kb'] = int(m.group(1))
    
    # Ex: Done! [0.010s CPU, 0.008s wall-clock]
    m = re.search(r'Done! \[([\d\.]+)s CPU', log_content)
    if m: data['translator_time_s'] = float(m.group(1))

    # --- Extract Search Stats ---
    if "Solution found." in log_content:
        data['solution_found'] = True

        m = re.search(r'Plan length: (\d+) step\(s\)\.', log_content)
        if m: data['plan_length'] = int(m.group(1))

        m = re.search(r'Plan cost: (\d+)', log_content)
        if m: data['plan_cost'] = int(m.group(1))

        m = re.search(r'Expanded (\d+) state\(s\)\.', log_content)
        if m: data['expanded_states'] = int(m.group(1))

        m = re.search(r'Evaluated (\d+) state\(s\)\.', log_content)
        if m: data['evaluated_states'] = int(m.group(1))

        m = re.search(r'Generated (\d+) state\(s\)\.', log_content)
        if m: data['generated_states'] = int(m.group(1))

        m = re.search(r'Search time: ([\d\.]+)s', log_content)
        if m: data['search_time_s'] = float(m.group(1))

        m = re.search(r'Total time: ([\d\.]+)s', log_content)
        if m: data['total_time_s'] = float(m.group(1))

        m = re.search(r'Peak memory: (\d+) KB', log_content)
        if m: data['search_peak_memory_kb'] = int(m.group(1))
    
    elif "Search failed" in log_content:
        data['solution_found'] = False
        # Try to get stats from failed runs if available
        m = re.search(r'Search time: ([\d\.]+)s', log_content)
        if m: data['search_time_s'] = float(m.group(1))
        m = re.search(r'Peak memory: (\d+) KB', log_content)
        if m: data['search_peak_memory_kb'] = int(m.group(1))

    return data

def parse_arguments():
    from argparse import ArgumentParser

    parser = ArgumentParser(description="Parse Fast Downward log files into a CSV summary.")
    parser.add_argument('log_files', nargs='+', help='Paths to Fast Downward log files.')
    parser.add_argument('--output', '-o', default='summary.csv', help='Output CSV file name (default: summary.csv).')
    return parser.parse_args()

def main():
    args = parse_arguments()
    log_files = args.log_files
    csv_file = args.output

    # Define the columns for the CSV
    fieldnames = [
        'problem', 'domain', 'algorithm', 'heuristic',
        'solution_found', 'plan_length', 'plan_cost',
        'expanded_states', 'evaluated_states', 'generated_states',
        'search_time_s', 'total_time_s', 'search_peak_memory_kb',
        'translator_variables', 'translator_operators', 'translator_goal_facts',
        'translator_time_s', 'translator_peak_memory_kb'
    ]

    # Check if file exists to decide whether to write the header
    file_exists = os.path.isfile(csv_file)

    print(f"Parsing {len(log_files)} log(s). Writing to {csv_file}...")

    with open(csv_file, 'a', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)

        if not file_exists:
            writer.writeheader()

        for log_file_path in log_files:
            try:
                with open(log_file_path, 'r') as log_f:
                    content = log_f.read()
                
                # Parse the content
                data = parse_log(content)
                
                # Write the row to the CSV
                writer.writerow(data)
                
                print(f"Successfully parsed: {log_file_path}")

            except Exception as e:
                print(f"Error parsing {log_file_path}: {e}")

    print("Done.")

if __name__ == "__main__":
    main()