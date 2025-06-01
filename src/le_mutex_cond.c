#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>

// This program implements a solution to the readers-writers problem using mutexes and condition variables.
// In this program, the readers are prioritized over the writers.

pthread_mutex_t t_mutex;
pthread_cond_t cond;
int writing;
int reader_count = 0;

int reader_finished = 0;
int writer_finished = 0;

int finished;

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
    
    reader_finished++;
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
    
    writer_finished++;
    writing = 0;
    
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&t_mutex);

    return NULL;
}

int main(int argc, char const *argv[]){
    
    if (argc < 3) {
        printf("Usage: %s <num_readers> <num_writers>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    if (num_readers <= 0 || num_writers <= 0) {
        fprintf(stderr, "Number of readers and writers must be positive integers.\n");
        return EXIT_FAILURE;
    }
    double start_time = (double)clock() / CLOCKS_PER_SEC;


    if (pthread_mutex_init(&t_mutex, NULL) != 0) {
        fprintf(stderr, "Failed to initialize mutex.\n");
        return EXIT_FAILURE;
    }
    if (pthread_cond_init(&cond, NULL) != 0) {
        fprintf(stderr, "Failed to initialize condition variable.\n");
        pthread_mutex_destroy(&t_mutex);
        return EXIT_FAILURE;
    }
    srand(time(NULL));

    int total_threads = num_readers + num_writers;
    pthread_t *threads;
    threads = malloc(total_threads * sizeof(pthread_t));
    if (threads == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        pthread_mutex_destroy(&t_mutex);
        pthread_cond_destroy(&cond);
        return EXIT_FAILURE;
    }

    
    
    finished = 0;
    
    int current_writers = 0;
    int current_readers = 0;

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

    finished = 1;

    pthread_cond_broadcast(&cond);
    
    for (int i = 0; i < total_threads; i++){
        pthread_join(threads[i], NULL);
    }

    printf("\nReaders finished: %d\n", reader_finished);
    printf("Writers finished: %d\n", writer_finished);

    

    pthread_mutex_destroy(&t_mutex);
    pthread_cond_destroy(&cond);

    double end_time = (double)clock() / CLOCKS_PER_SEC;
    printf("Total time (mutex and condition variable): %.4f seconds\n", end_time - start_time);

    return EXIT_SUCCESS;
}
