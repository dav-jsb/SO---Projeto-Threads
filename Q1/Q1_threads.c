#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 5 // Número de threads;
#define NUM_ARRAY 10 // Número de elementos do vetor;

typedef struct{
    int pos_init;
    int partition;
    int* data;
} _subarray;

void* Sum(void* subarray){
    _subarray array = *(_subarray*) subarray;
    int* p_result = NULL;
    int result = 0;
    int pos_init = array.pos_init;
    int partition = array.partition;

    for (int j = pos_init; j < (pos_init + partition); j++){
        result += array.data[j];
    }

    p_result = &result;

    pthread_exit((void*) p_result);
}

int* SetToZero(int* array){
    for (int i = 0; i < NUM_ARRAY; i++){
        array[i] = 0;
    }

    return array;
}

//No Scheduler, vamos designar para cada uma das threads quantos elementos elas vão processar;
int* Scheduler(){ 
    float partition = NUM_ARRAY / N;
    int i = 0;
    int nat_partition = partition;

    int* schedule = NULL;

    if (!(schedule = (int*) malloc(N * sizeof(int)))){
        printf("Erro de alocação de memória no scheduler!\n");
        return NULL;
    }

    while (i < N){
        if ((i == (N - 1)) && (nat_partition < partition)) schedule[i] = nat_partition + 1;
        else schedule[i] = nat_partition;
    }

    return schedule;
}

int main(){
    pthread_t thread_list[N];
    int rc;
    int* numeric_array = NULL;
    int pos = 0;
    int* schedule = Scheduler();
    int part_results[N];
    int total = 0;
    int* partial = NULL;

    if (!(numeric_array = (int*) malloc(NUM_ARRAY * sizeof(int)))){
        printf("Erro de alocação de memória para array numérica!\n");
        exit(1);
    }

    numeric_array = SetToZero(numeric_array);

    for (int i = 0; i < N; i++){
        _subarray __subarray;
        __subarray.data = numeric_array;
        __subarray.partition = schedule[i];
        __subarray.pos_init = pos;

        rc = pthread_create(&thread_list[i], NULL, Sum, (void*) &__subarray);
        if (rc){
            printf("Erro de criação da thread; Código de erro: %d\n", rc);
            exit(1);
        }

        pos += schedule[i];
    }

    for (int j = 0; j < N; j++){
        rc = pthread_join(thread_list[j], (void**) partial);
        if (rc){
            printf("Erro de join da thread; Código de erro: %d\n", (void**) partial);
            exit(1);
        }

        total += *partial;
    }

    printf("Total: %d\n", total);
}