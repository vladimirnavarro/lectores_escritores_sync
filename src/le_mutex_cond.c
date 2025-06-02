#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<time.h>

//This program implements a solution to the readers-writers problem using mutexes and condition variables.
//In this program, the readers are prioritized over the writers.

//The program uses a mutex to manage access to shared resources and condition variables to signal when readers or writers can proceed.
pthread_mutex_t t_mutex;
pthread_cond_t cond;

//Global variables to track execution time and completed operations
struct timespec global_start_time, global_end_time;
double total_execution_time_sec;

int writing;
int reader_count;
int t_reads_completed;
int t_writes_completed;
int finished;

//Reader and writer functions
void* reader_func(void* arg){
    int reader_id = *((int*)arg);
    free(arg);

    pthread_mutex_lock(&t_mutex);

    while(!finished){
        pthread_cond_wait(&cond, &t_mutex); 
    }

    while(writing){
        pthread_cond_wait(&cond, &t_mutex);
    }
    reader_count++;
    pthread_mutex_unlock(&t_mutex);

    //Simluate reading
    printf("Reader [%d] is reading...\n", reader_id);
    sleep(1 + rand() % 3);
    printf("Reader [%d] stop reading.\n", reader_id);
    
    
    pthread_mutex_lock(&t_mutex);
    
    t_reads_completed++;
    reader_count--;
    if(reader_count == 0){
        pthread_cond_signal(&cond);
    }

    pthread_mutex_unlock(&t_mutex);

    return NULL;
}

void* writer_func(void* arg){
    int writer_id = *((int*)arg);
    free(arg);

    pthread_mutex_lock(&t_mutex);

    while(!finished){
        pthread_cond_wait(&cond, &t_mutex);
    }

    while(writing || reader_count > 0){
        pthread_cond_wait(&cond, &t_mutex);
    }
    writing = 1;
    pthread_mutex_unlock(&t_mutex);

    //Simulate writing
    printf("Writer [%d] is writing...\n", writer_id);
    sleep(1 + rand() % 3);
    printf("Writer [%d] stop writing.\n", writer_id);
    
    pthread_mutex_lock(&t_mutex);
    
    t_writes_completed++;
    writing = 0;
    
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&t_mutex);

    return NULL;
}

int main(int argc, char const *argv[]){

    //Initialize the global start time for execution time measurement
    clock_gettime(CLOCK_MONOTONIC, &global_start_time);
    
    //Check command line arguments for number of readers and writers
    if (argc < 3) {
        printf("Usage: %s <num_readers> <num_writers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Parse the number of readers and writers from command line arguments
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    if (num_readers <= 0 || num_writers <= 0) {
        fprintf(stderr, "Number of readers and writers must be positive integers.\n");
        return EXIT_FAILURE;
    }

    //Initialize the mutex and condition variable
    if (pthread_mutex_init(&t_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to initialize mutex.\n");
        return EXIT_FAILURE;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        fprintf(stderr, "Failed to initialize condition variable.\n");
        pthread_mutex_destroy(&t_mutex);
        return EXIT_FAILURE;
    }

    //Seed the random number generator
    srand(time(NULL));

    //Initialize global variables
    writing = 0;
    reader_count = 0;
    t_reads_completed = 0;
    t_writes_completed = 0;

    //Allocate memory for threads
    int total_threads = num_readers + num_writers;
    pthread_t *threads;
    threads = malloc(total_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        pthread_mutex_destroy(&t_mutex);
        pthread_cond_destroy(&cond);
        return EXIT_FAILURE;
    }

    //Initialize the finished flag
    finished = 0;
    
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
            pthread_mutex_destroy(&t_mutex);
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
            fprintf(stderr, "Error lógico: No se pudo crear un hilo.\n");
            i--;
        }
    }

    //Set the finished flag to indicate that threads can start processing
    finished = 1;

    //Signal all threads to start processing
    pthread_cond_broadcast(&cond);
    
    //Wait for all threads to finish
    for (int i = 0; i < total_threads; i++){
        pthread_join(threads[i], NULL);
    }

    //Record the end time and calculate total execution time
    clock_gettime(CLOCK_MONOTONIC, &global_end_time);
    total_execution_time_sec = (global_end_time.tv_sec - global_start_time.tv_sec) + 
                                (global_end_time.tv_nsec - global_start_time.tv_nsec) / 1e9;
    
    //Clean up resources
    finished = 0;
    pthread_mutex_destroy(&t_mutex);
    pthread_cond_destroy(&cond);
    free(threads);

    //Results
    printf("\nReaders finished: %d\n", t_reads_completed);
    printf("Writers finished: %d\n", t_writes_completed);
    printf("Total execution time: %.4f seconds\n", total_execution_time_sec); 
    printf("Readers Throughput: %.2f ops/seg\n", (double)t_reads_completed / total_execution_time_sec);
    printf("Writers Throughput: %.2f ops/seg\n", (double)t_writes_completed / total_execution_time_sec);      
    printf("Total Throughput: %.2f ops/seg\n", 
        (double)(t_reads_completed + t_writes_completed) /total_execution_time_sec);                   
    
    return EXIT_SUCCESS;
}
