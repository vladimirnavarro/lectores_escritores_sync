#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

// This program implements a solution to the readers-writers problem using semaphores.
// In this program, the writers are prioritized over the readers.

//The prgram uses semaphores to synchronize access to shared resources.
sem_t start_sem;
sem_t mutex;
sem_t write_sem;
sem_t read_sem;

//Global variables to track execution time and completed operations
struct timespec global_start_time, global_end_time;
double total_execution_time_sec;

int t_reads_completed;
int t_writes_completed;
int writing;
int writer_count;
int reader_count;
int num_readers;

//Reader and writer functions
void * reader_func(void* arg){
    sem_wait(&start_sem);

    int reader_id = *((int*)arg);
    free(arg);

    sem_wait(&mutex);
    while(writer_count > 0 || writing){
        sem_post(&mutex);
        sem_wait(&read_sem);
        sem_wait(&mutex);
    }
    reader_count++;
    sem_post(&mutex);

    //Simluate reading
    printf("Reader [%d] is reading...\n", reader_id);
    sleep(1 + rand() % 3);
    printf("Reader [%d] stop reading.\n", reader_id);

    sem_wait(&mutex);
    t_reads_completed++;
    reader_count--;
    if(reader_count == 0 || writer_count > 0){
        sem_post(&write_sem);
    }
    sem_post(&mutex);
    return NULL;
}

void * writer_func(void* arg){
    sem_wait(&start_sem);

    int writer_id = *((int*)arg);
    free(arg);

    sem_wait(&mutex);

    writer_count++;
    while(reader_count > 0 || writing){
        sem_post(&mutex);
        sem_wait(&write_sem);
        sem_wait(&mutex);
    }
    writing = 1;
    sem_post(&mutex);
    
    //Simulate writing
    printf("Writer [%d] is writing...\n", writer_id);
    sleep(1 + rand() % 3);
    printf("Writer [%d] stop writing.\n", writer_id);
    
    sem_wait(&mutex);
    writer_count--;
    t_writes_completed++;
    writing = 0;
    if(writer_count > 0){
        sem_post(&write_sem);
    } else {
        for (int i = 0; i < num_readers; i++){
            sem_post(&read_sem);
        }
    }
    sem_post(&mutex);

    return NULL;
}

int main(int argc, char const *argv[]){

    //Initialize global start time for execution time measurement
    clock_gettime(CLOCK_MONOTONIC, &global_start_time);

    //Cheack command line arguments for number of readers and writers
    if(argc < 3){
        printf("Usage: %s <num_readers> <num_writers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Parse the number of readers and writers from command line arguments
    num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    if(num_readers <= 0 || num_writers <= 0){
        fprintf(stderr, "Number of readers and writers must be positive integers.\n");
        return EXIT_FAILURE;
    }

    //Initialize semaphores
    int total_threads = num_readers + num_writers;
    if (sem_init(&start_sem, 0, 0) != 0) {
        fprintf(stderr, "Failed to initialize start semaphore.\n");
        return EXIT_FAILURE;
    }
    if (sem_init(&mutex, 0, 1) != 0) {
        fprintf(stderr, "Failed to initialize mutex semaphore.\n");
        sem_destroy(&start_sem);
        return EXIT_FAILURE;
    }
    if (sem_init(&write_sem, 0, 0) != 0) {
        fprintf(stderr, "Failed to initialize write semaphore.\n");
        sem_destroy(&mutex);
        sem_destroy(&start_sem);
        return EXIT_FAILURE;
    }
    if (sem_init(&read_sem, 0, 0) != 0) {
        fprintf(stderr, "Failed to initialize read semaphore.\n");
        sem_destroy(&write_sem);
        sem_destroy(&mutex);
        sem_destroy(&start_sem);
        return EXIT_FAILURE;
    }

    //Allocate memory for thread identifiers
    pthread_t *threads = malloc(total_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        sem_destroy(&read_sem);
        sem_destroy(&write_sem);
        sem_destroy(&mutex);
        sem_destroy(&start_sem);
        return EXIT_FAILURE;
    }

    //Seed the random number generator
    srand(time(NULL));

    //Initialize global variables
    t_reads_completed = 0;
    t_writes_completed = 0;
    writing = 0;
    writer_count = 0;
    reader_count = 0;

    //Variables to track the number of current readers and writers
    int current_writers = 0;
    int current_readers = 0;

    //Create threads for readers and writers randomly
    for (int i = 0; i < total_threads; i++){
        int *arg = malloc(sizeof(int));
        if (arg == NULL){
            fprintf(stderr, "Memory allocation failed.\n");
            for (int j = 0; j < i; j++) {
                pthread_cancel(threads[j]);
            }
            sem_destroy(&read_sem);
            sem_destroy(&write_sem);
            sem_destroy(&mutex);
            sem_destroy(&start_sem);
            free(threads);
            return EXIT_FAILURE;
        }

        if (current_readers < num_readers && (current_writers == num_writers || rand() % 2 == 0)) {
            *arg = current_readers;
            pthread_create(&threads[i], NULL, reader_func, (void*)arg);
            current_readers++;
        } else if (current_writers < num_writers) {
            *arg = current_writers;
            pthread_create(&threads[i], NULL, writer_func, (void*)arg);
            current_writers++;
        } else {
            free(arg);
            fprintf(stderr, "No more readers or writers can be created.\n");
            i--;
        }
    }

    //Release all threads to start
    for (int i = 0; i < total_threads; i++) {
        sem_post(&start_sem);
    }

    //Wait for all threads to finish
    for (int i = 0; i < total_threads; i++){
        pthread_join(threads[i], NULL);
    }

    //Record the end time for execution time measurement
    clock_gettime(CLOCK_MONOTONIC, &global_end_time);
    total_execution_time_sec = (global_end_time.tv_sec - global_start_time.tv_sec) +
                                (global_end_time.tv_nsec - global_start_time.tv_nsec) / 1e9;
    
    //Clean up resources
    sem_destroy(&read_sem);
    sem_destroy(&write_sem);
    sem_destroy(&mutex);
    sem_destroy(&start_sem);
    free(threads);

    //Results
    printf("\nReaders finished: %d\n", t_reads_completed);
    printf("Writers finished: %d\n", t_writes_completed);
    printf("Total execution time: %.4f seconds\n", total_execution_time_sec); 
    printf("Readers Throughput: %.2f ops/seg\n", (double)t_reads_completed /total_execution_time_sec);
    printf("Writers Throughput: %.2f ops/seg\n", (double)t_writes_completed / total_execution_time_sec);      
    printf("Total Throughput: %.2f ops/seg\n", 
        (double)(t_reads_completed + t_writes_completed) /total_execution_time_sec); 

    return EXIT_SUCCESS;
}