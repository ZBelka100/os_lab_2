#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
//тут вообще куча приколов

struct argum {
    int** arr;
    long long* res;
    int t;
    int N_ARRAYS;
    int NUM_FOR_ARRAYS;
    int NUM_THREADS;
};

void* thread_function(void* arg) {
    struct argum* id = ((struct argum*)arg);
    int incr = id->NUM_THREADS;
    int len = id->NUM_FOR_ARRAYS;
    for (int i = id->t; i < len; i += incr) {
        for (int j = 0; j < id->N_ARRAYS; ++j) {
            id->res[i] += id->arr[j][i];
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int N_ARRAYS;
    int NUM_FOR_ARRAYS;
    int NUM_THREADS;
    int** arrays = NULL; 
    long long* array_of_sums = NULL; 
    if (argc != 2) {
        printf("%s <number_of_threads>\n", argv[0]);
        return 1;
    }

    NUM_THREADS = atoi(argv[1]);
    scanf("%d", &N_ARRAYS);
    scanf("%d", &NUM_FOR_ARRAYS); 
    if (NUM_THREADS <= 0) {
        printf("Ну ты придумал конечно.\n");
        return 1;
    }
    arrays = (int**)realloc(arrays, N_ARRAYS * sizeof(int*));
    for (int i = 0; i < N_ARRAYS; ++i) {
        arrays[i] = (int*)realloc(arrays[i], NUM_FOR_ARRAYS * sizeof(int));
    }
    array_of_sums = (long long*)realloc(array_of_sums, NUM_FOR_ARRAYS * sizeof(long long));
    memset(array_of_sums, 0, NUM_FOR_ARRAYS * sizeof(long long));
    for (int i = 0; i < N_ARRAYS; ++i) {
        for (int j = 0; j < NUM_FOR_ARRAYS; ++j) {
            scanf("%d", &arrays[i][j]);
        }
    }
    if (arrays == NULL || array_of_sums == NULL) {
        printf("Ошибка выделения памяти.\n");
        return 1;
    }
    struct timespec start, end;

    struct argum* args_array = (struct argum*)malloc(NUM_THREADS * sizeof(struct argum));

    if (args_array == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        args_array[t].arr = arrays;
        args_array[t].res = array_of_sums;
        args_array[t].N_ARRAYS = N_ARRAYS;
        args_array[t].NUM_THREADS = NUM_THREADS;
        args_array[t].NUM_FOR_ARRAYS = NUM_FOR_ARRAYS;
        args_array[t].t = t;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t threads[NUM_THREADS];
    for (int t = 0; t < NUM_THREADS; ++t) {
        if(pthread_create(&threads[t], NULL, thread_function, (void*)&args_array[t])) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int t = 0; t < NUM_THREADS; ++t) {
        pthread_join(threads[t], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long long elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    printf("Затраченное время для %d потоков: %lld nanoseconds\n", NUM_THREADS, elapsed_time);
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_t thread;
    args_array[0].NUM_THREADS = 1;
    pthread_create(&thread, NULL, thread_function, (void*)&args_array[0]);

    pthread_join(thread, NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    printf("Затраченное время для одного потока: %lld nanoseconds\n", elapsed_time);

    pthread_exit(NULL);
}
