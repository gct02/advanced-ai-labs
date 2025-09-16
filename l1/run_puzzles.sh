#!/bin/bash

# Script: run_puzzles.sh
# Usage: ./run_puzzles.sh -astar puzzle_file.txt
#        ./run_puzzles.sh -bfs puzzle_file.txt
#        ./run_puzzles.sh -idastar puzzle_file.txt
#        ./run_puzzles.sh -idfs puzzle_file.txt
#        ./run_puzzles.sh -gbfs puzzle_file.txt

# Function to display usage
usage() {
    echo "Usage: $0 <algorithm> <puzzle_file>"
    echo "Algorithms: -astar, -bfs, -idastar, -idfs, -gbfs"
    echo "Example: $0 -astar puzzles.txt"
    exit 1
}

# Check if correct number of arguments provided
if [ $# -ne 2 ]; then
    usage
fi

ALGORITHM="$1"
PUZZLE_FILE="$2"

# Validate algorithm
case "$ALGORITHM" in
    -astar|-bfs|-idastar|-idfs|-gbfs)
        # Valid algorithm
        ;;
    *)
        echo "Error: Unknown algorithm '$ALGORITHM'"
        usage
        ;;
esac

# Check if puzzle file exists
if [ ! -f "$PUZZLE_FILE" ]; then
    echo "Error: Puzzle file '$PUZZLE_FILE' not found"
    exit 1
fi

# Check if main program exists
if [ ! -f "./main" ]; then
    echo "Error: main program not found. Please compile first."
    exit 1
fi

# Make sure main program is executable
chmod +x ./main

echo "Running $ALGORITHM on puzzles from $PUZZLE_FILE"
echo "================================================"

# Read each line from the puzzle file and execute the program
LINE_NUMBER=1
while IFS= read -r line; do
    # Skip empty lines
    if [ -z "$line" ]; then
        continue
    fi
    
    # Remove leading/trailing whitespace
    line=$(echo "$line" | xargs)
    
    # Skip comment lines (starting with #)
    if [[ "$line" == \#* ]]; then
        continue
    fi
    
    echo "Puzzle $LINE_NUMBER: $line"
    
    # Execute the main program with the algorithm and current puzzle
    ./main "$ALGORITHM" $line
    
    LINE_NUMBER=$((LINE_NUMBER + 1))
    echo "--------------------------------"
done < "$PUZZLE_FILE"

echo "Completed processing $((LINE_NUMBER - 1)) puzzles"
