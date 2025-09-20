#!/bin/bash
# Script: run_puzzles.sh
# Usage: ./run_puzzles.sh -astar puzzle_file.txt
#        ./run_puzzles.sh -bfs puzzle_file.txt
#        ./run_puzzles.sh -idastar puzzle_file.txt
#        ./run_puzzles.sh -idfs puzzle_file.txt
#        ./run_puzzles.sh -gbfs puzzle_file.txt

# Memory limit in KB (12GB = 12 * 1024 * 1024 KB)
GB_MEMORY_LIMIT=8
MEMORY_LIMIT=$(($GB_MEMORY_LIMIT * 1024 * 1024))

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
echo "Memory limit: $GB_MEMORY_LIMIT GB"
echo "================================================"

# Initialize counters
TOTAL_PUZZLES=0
SUCCESSFUL_PUZZLES=0
FAILED_PUZZLES=0
MEMORY_EXCEEDED=0

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
    
    # Execute the main program with memory limit using ulimit
    # Use timeout as an additional safety measure
    (ulimit -v $MEMORY_LIMIT && timeout 30 ./main "$ALGORITHM" $line)
    
    # Capture the exit status
    EXIT_STATUS=$?
    
    # Check the exit status
    if [ $EXIT_STATUS -eq 0 ]; then
        echo "✓ Puzzle $LINE_NUMBER: SUCCESS"
        SUCCESSFUL_PUZZLES=$((SUCCESSFUL_PUZZLES + 1))
    else
        # Other error codes
        echo "✗ Puzzle $LINE_NUMBER: FAILED - Exit code: $EXIT_STATUS"
        FAILED_PUZZLES=$((FAILED_PUZZLES + 1))
    fi
    
    TOTAL_PUZZLES=$((TOTAL_PUZZLES + 1))
    LINE_NUMBER=$((LINE_NUMBER + 1))
    echo "--------------------------------"
done < "$PUZZLE_FILE"

echo "================================================"
echo "SUMMARY:"
echo "Total puzzles processed: $TOTAL_PUZZLES"
echo "Successful puzzles: $SUCCESSFUL_PUZZLES"
echo "Failed puzzles: $FAILED_PUZZLES"
echo "================================================"
echo "COMPLETION: $SUCCESSFUL_PUZZLES out of $TOTAL_PUZZLES puzzles completed successfully"

# Return appropriate exit code
if [ $FAILED_PUZZLES -eq 0 ]; then
    exit 0
else
    exit 1
fi