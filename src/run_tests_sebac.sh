#!/bin/bash

#Configuration for running tests on different implementations of the readers-writers problem
OUTPUT_DIR="mediciones_simplificadas"
SUMMARY_FILE="$OUTPUT_DIR/summary_metrics.csv"

EXECUTABLES=(
    "le_barrier"       #Writer priority with barrier synchronization
    "le_busy_wait"          #No priority, busy waiting
    "le_mutex_cond"         #Reader priority with mutex and condition variables
)

#Environment setup
rm -rf "$OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

echo "implementation,scenario,readers,writers,program_exec_time_sec,total_reads,total_writes,reader_throughput_ops_sec,writer_throughput_ops_sec,total_throughput_ops_sec,perf_cpu_cycles,perf_task_clock_ms" > "$SUMMARY_FILE"

#Function to run a test case with the given parameters
#run_test_case(executable name, scenario name, number of readers, number of writers)
run_test_case() {
    local exec_name="$1"
    local scenario_name="$2"
    local r="$3" #Readers number
    local w="$4" #Writers number

    echo "  Escenario: $scenario_name, R=$r, W=$w"
    
    #Execute the program with the specified number of readers and writers using perf stat
    PROGRAM_FULL_OUTPUT=$(sudo perf stat -e 'cpu-cycles,task-clock' \
                          "../bin/$exec_name" "$r" "$w" 2>&1)


    #Parse the output to extract metrics
    program_exec_time=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Total execution time:" | awk '{print $4}')
    total_reads=$(echo  "$PROGRAM_FULL_OUTPUT" | grep "Readers finished:" | awk '{print $3}')
    total_writes=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Writers finished:" | awk '{print $3}')
    reader_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Readers Throughput:" | awk '{print $3}')
    writer_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Writers Throughput:" | awk '{print $3}')
    total_throughput=$(echo "$PROGRAM_FULL_OUTPUT" | grep "Total Throughput:" | awk '{print $3}')

    #Parse perf output for CPU cycles and task clock
    perf_cpu_cycles=$(echo "$PROGRAM_FULL_OUTPUT" | grep "cpu-cycles" | awk -F',' '{print $1}')
    perf_task_clock_ms=$(echo "$PROGRAM_FULL_OUTPUT" | grep "task-clock" | awk -F',' '{print $1}')

    #Save the results in the csv summary file
    echo "$exec_name,$scenario_name,$r,$w,$program_exec_time,$total_reads,$total_writes,$reader_throughput,$writer_throughput,$total_throughput,$perf_cpu_cycles,$perf_task_clock_ms" >> "$SUMMARY_FILE"
}

for exec_name in "${EXECUTABLES[@]}"; do
    echo "--- Iniciando pruebas para: $exec_name ---"

    #Scenario 1: Same number of readers and writers
    #A representative combination is chosen, for example, 8 readers and 8 writers.
    run_test_case "$exec_name" "R_eq_W" 8 8

    #Scenario 2: More writers than readers
    #Eligiendo una combinación representativa, por ejemplo, 4 lectores y 16 escritores.
    run_test_case "$exec_name" "W_gt_R" 4 16

    #Scenario 3: More readers than writers
    #Eligiendo una combinación representativa, por ejemplo, 16 lectores y 4 escritores.
    run_test_case "$exec_name" "R_gt_W" 16 4

done

echo "All the metrics are saved in: $SUMMARY_FILE"