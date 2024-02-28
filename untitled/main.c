#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX_NUM_THREADS 8

// Estructura para pasar los argumentos a la función de fusión paralela
struct MergeArgs {
    int *array;
    int start;
    int mid;
    int end;
};

// Función de fusión para el algoritmo Merge Sort
void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Función recursiva para el algoritmo Merge Sort
void* merge_sort_recursive(void* args) {
    struct MergeArgs* mergeArgs = (struct MergeArgs*)args;
    int *arr = mergeArgs->array;
    int start = mergeArgs->start;
    int end = mergeArgs->end;

    if (start < end) {
        int mid = start + (end - start) / 2;

        // Dividir y ordenar recursivamente las dos mitades
        struct MergeArgs leftArgs = { arr, start, mid };
        struct MergeArgs rightArgs = { arr, mid + 1, end };

        pthread_t tid[2];
        pthread_create(&tid[0], NULL, merge_sort_recursive, (void*)&leftArgs);
        pthread_create(&tid[1], NULL, merge_sort_recursive, (void*)&rightArgs);

        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);

        // Combinar las dos mitades ordenadas
        merge(arr, start, mid, end);
    }
    pthread_exit(0);
}

// Función para ordenar el vector usando Merge Sort de forma paralela
void merge_sort_parallel(int arr[], int n) {
    struct MergeArgs args = { arr, 0, n - 1 };
    merge_sort_recursive((void*)&args);
}

// Función para calcular la moda del vector
int find_mode(int arr[], int n) {
    int max_count = 0, mode = -1;
    for (int i = 0; i < n; i++) {
        int count = 0;
        for (int j = 0; j < n; j++) {
            if (arr[j] == arr[i])
                count++;
        }
        if (count > max_count) {
            max_count = count;
            mode = arr[i];
        }
    }
    return mode;
}

int main() {
    int N;

    // Solicitar al usuario el número de elementos
    printf("Ingrese el número de elementos (N): ");
    scanf("%d", &N);

    // Crear un vector de tamaño N
    int vector[N];

    // Generar y almacenar números aleatorios en el vector
    for (int i = 0; i < N; ++i) {
        vector[i] = rand() % 2001 - 1000; // Números aleatorios en el rango [-1000, 1000]
    }

    // Clonar el vector
    int cloned_vector[N];
    for (int i = 0; i < N; i++) {
        cloned_vector[i] = vector[i];
    }

    // Ordenar el vector utilizando Merge Sort (versión tradicional)
    clock_t start_time = clock();
    merge_sort_parallel(cloned_vector, N);
    clock_t end_time = clock();
    printf("Tiempo de ordenamiento usando Merge Sort (versión tradicional): %.6f segundos\n",
           (double)(end_time - start_time) / CLOCKS_PER_SEC);

    // Clonar el vector nuevamente
    for (int i = 0; i < N; i++) {
        cloned_vector[i] = vector[i];
    }

    // Ordenar el vector utilizando Merge Sort (paralelo)
    start_time = clock();
    merge_sort_parallel(cloned_vector, N);
    end_time = clock();
    printf("Tiempo de ordenamiento usando Merge Sort (paralelo): %.6f segundos\n",
           (double)(end_time - start_time) / CLOCKS_PER_SEC);

    // Calcular la moda del vector (paralelo)
    start_time = clock();
    int mode = find_mode(vector, N);
    end_time = clock();
    printf("Moda del vector: %d\n", mode);
    printf("Tiempo de cálculo de la moda: %.6f segundos\n",
           (double)(end_time - start_time) / CLOCKS_PER_SEC);

    return 0;
}