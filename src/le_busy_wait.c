#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// This program implements a solution to the readers-writers problem using busy wait and mutex.
// In this program, there is no priority between readers and writers, and they can run concurrently.

pthread_mutex_t t_mutex;

int writing;
int reader_count = 0;

int reader_finished = 0;
int writer_finished = 0;

void* reader_func(void* arg){
    int reader_id = *((int*)arg);
    free(arg);

    while(1){
        pthread_mutex_lock(&t_mutex);
        if(!writing){
            reader_count++;
            pthread_mutex_unlock(&t_mutex);
            break;
        }
        pthread_mutex_unlock(&t_mutex);
    }

    printf("Reader [%d] is reading...\n", reader_id);
    sleep(1 + rand() % 3);
    printf("Reader [%d] stop reading.\n", reader_id);

    pthread_mutex_lock(&t_mutex);
    reader_count--;
    reader_finished++;
    pthread_mutex_unlock(&t_mutex);

    return NULL;
}

void* writer_func(void* arg){
    int writer_id = *((int*)arg);
    free(arg);

    while(1){
        pthread_mutex_lock(&t_mutex);
        if(reader_count == 0 && !writing){
            writing = 1;
            pthread_mutex_unlock(&t_mutex);
            break;
        }
        pthread_mutex_unlock(&t_mutex);
    }

    printf("Writer [%d] is writing...\n", writer_id);
    sleep(1 + rand() % 3);
    printf("Writer [%d] stop writing.\n", writer_id);

    pthread_mutex_lock(&t_mutex);
    writer_finished++;
    writing = 0;
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

    pthread_mutex_init(&t_mutex, NULL);

    writing = 0;

    int total_threads = num_readers + num_writers;
    pthread_t threads[total_threads];

    
    srand(time(NULL));
    
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

    for (int i = 0; i < total_threads; i++){
        pthread_join(threads[i], NULL);
    }

    printf("\nReaders finished: %d\n", reader_finished);
    printf("Writers finished: %d\n", writer_finished);


    pthread_mutex_destroy(&t_mutex);

    double end_time = (double)clock() / CLOCKS_PER_SEC;
    printf("Total time (busy wait): %.4f seconds\n", end_time - start_time);

    return EXIT_SUCCESS;
}
