#!/bin/bash

# Configuration for running tests on different implementations of the readers-writers problem
OUTPUT_DIR="output"

EXECUTABLES=(
    "le_semaphore"        # Writer priority with barrier synchronization
    "le_busy_wait"        # No priority, busy waiting
    "le_mutex_cond"       # Reader priority with mutex and condition variables
)

# Environment setup
# Check if OUTPUT_DIR exists, create if not.
if [ ! -d "$OUTPUT_DIR" ]; then
    mkdir -p "$OUTPUT_DIR"
fi

# Function to run a test case with the given parameters
# run_test_case(executable name, scenario name, number of readers, number of writers, round number)
run_test_case() {
    local exec_name="$1"
    local scenario_name="$2"
    local r="$3" # Readers number
    local w="$4" # Writers number
    local round_num="$5" # Current round number

    local SUMMARY_FILE="$OUTPUT_DIR/summary_metrics_${round_num}.csv"

    echo "    Scenario: $scenario_name, R=$r, W=$w"
    
    # Execute the program with the specified number of readers and writers using perf stat
    PROGRAM_FULL_OUTPUT=$(sudo perf stat -e 'cpu-cycles,task-clock' \
                            "../bin/$exec_name" "$r" "$w" 2>&1)


    # Parse the output to extract metrics
    program_exec_time=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Total execution time:" | awk '{print $4}')
    reader_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Readers Throughput:" | awk '{print $3}')
    writer_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Writers Throughput:" | awk '{print $3}')
    total_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Total Throughput:" | awk '{print $3}')

    # Parse perf output for CPU cycles and task clock
    perf_cpu_cycles=$(echo "$PROGRAM_FULL_OUTPUT" | grep "cpu-cycles" | awk -F'cycles' '{print $1}' | tr -d ' ' | tr -d ',' | tail -n 1)
    perf_task_clock_ms=$(echo "$PROGRAM_FULL_OUTPUT" | grep "task-clock" | awk -F'msec' '{print $1}' | tr -d ' ' | tr -d ',' | tail -n 1)


    # Save the results in the csv summary file
    echo "$exec_name,$scenario_name,$r,$w,$program_exec_time,$reader_throughput,$writer_throughput,$total_throughput,$perf_cpu_cycles,$perf_task_clock_ms" >> "$SUMMARY_FILE"
}

# Number of rounds for testing
NUM_ROUNDS=3

for round in $(seq 1 $NUM_ROUNDS); do
    echo "=== Starting Round $round ==="
    SUMMARY_FILE_ROUND="$OUTPUT_DIR/summary_metrics_${round}.csv"
    
    # Create or overwrite the CSV header for the current round's file
    echo "implementation,scenario,readers,writers,program_exec_time_sec,reader_throughput_ops_sec,writer_throughput_ops_sec,total_throughput_ops_sec,perf_cpu_cycles,perf_task_clock_ms" > "$SUMMARY_FILE_ROUND"

    for exec_name in "${EXECUTABLES[@]}"; do
        echo "--- Starting tests for: $exec_name (Round $round) ---"

        # Scenario 1: Same number of readers and writers
        # A representative combination is chosen, for example, 30 readers and 30 writers.
        run_test_case "$exec_name" "R_eq_W" 30 30 "$round"

        # Scenario 2: More writers than readers
        # A representative combination is chosen, for example, 30 readers and 50 writers.
        run_test_case "$exec_name" "W_gt_R" 30 50 "$round"

        # Scenario 3: More readers than writers
        # A representative combination is chosen, for example, 50 readers and 30 writers.
        run_test_case "$exec_name" "R_gt_W" 50 30 "$round"
    done
done

echo "All metrics are saved in: $OUTPUT_DIR/summary_metrics_1.csv, $OUTPUT_DIR/summary_metrics_2.csv, $OUTPUT_DIR/summary_metrics_3.csv"