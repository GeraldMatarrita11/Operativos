#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 15

// Estructura para pasar los argumentos a la función de fusión paralela
struct MergeArgs {
    int *array;
    int start;
    int end;
};

// Función para generar números aleatorios en el rango [-1000, 1000]
int generarNumeroAleatorio() {
    return rand() % 2001 - 1000;
}

// Función para imprimir un vector
void imprimirVector(int *vector, int longitud) {
    for (int i = 0; i < longitud; i++) {
        printf("%d ", vector[i]);
    }
    printf("\n");
}

// Función para clonar un vector
int* clonarVector(int *vector, int longitud) {
    int *clon = (int*)malloc(longitud * sizeof(int));
    if (clon == NULL) {
        printf("Error: No se pudo asignar memoria para el clon del vector.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < longitud; i++) {
        clon[i] = vector[i];
    }
    return clon;
}

// Función para intercambiar dos elementos en un vector
void intercambiar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Función que fusiona dos subarreglos de arr[]
void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    // Creamos arreglos temporales
    int L[n1], R[n2];

    // Copiamos datos a los arreglos temporales L[] y R[]
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Combinamos los arreglos temporales en arr[l..r]
    i = 0; // Índice inicial del primer subarreglo
    j = 0; // Índice inicial del segundo subarreglo
    k = l; // Índice inicial del subarreglo fusionado
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copiamos los elementos restantes de L[], si hay alguno
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copiamos los elementos restantes de R[], si hay alguno
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Función recursiva para ordenar un subarreglo de arr[]
// utilizando el algoritmo de Merge Sort
void merge_sort(int arr[], int l, int r) {
    if (l < r) {
        // Encuentra el punto medio para dividir el arreglo en dos
        int m = l + (r - l) / 2;

        // Ordena la primera y la segunda mitad
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);

        // Fusiona las mitades ordenadas
        merge(arr, l, m, r);
    }
}

// Función de ordenamiento de burbuja
void bubbleSort(int arr[], int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                intercambiar(&arr[j], &arr[j+1]);
            }
        }
    }
}

// Función para dividir el trabajo entre los hilos y ordenar el vector
void* sort_thread(void* args) {
    struct MergeArgs* mergeArgs = (struct MergeArgs*)args;
    int* arr = mergeArgs->array;
    int start = mergeArgs->start;
    int end = mergeArgs->end;

    // Ordenar la parte asignada del arreglo
    bubbleSort(arr + start, end - start + 1);

    return NULL;
}

// Función para ordenar el vector usando Merge Sort de forma paralela
void merge_sort_parallel(int arr[], int n) {
    pthread_t threads[NUM_THREADS];
    struct MergeArgs args[NUM_THREADS];

    int size = n / NUM_THREADS;
    int remainder = n % NUM_THREADS;
    int start = 0;

    for (int i = 0; i < NUM_THREADS; ++i) {
        args[i].array = arr;
        args[i].start = start;
        args[i].end = start + size - 1 + (i < remainder ? 1 : 0);

        pthread_create(&threads[i], NULL, sort_thread, (void*)&args[i]);

        start = args[i].end + 1;
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Combinar los subvectores ordenados
    for (int i = 1; i < NUM_THREADS; ++i) {
        merge(arr, 0, (size * NUM_THREADS) - 1, n - 1);
    }
}

int main() {
    int N;
    printf("Ingrese la cantidad de numeros enteros aleatorios a generar: ");
    scanf("%d", &N);

    // Semilla para la generación de números aleatorios
    srand(time(0));

    // Crear un vector y generar números aleatorios
    int *vector = (int*)malloc(N * sizeof(int));
    if (vector == NULL) {
        printf("Error: No se pudo asignar memoria para el vector.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < N; i++) {
        vector[i] = generarNumeroAleatorio();
    }

//     Imprimir el vector original
//    printf("Vector original:\n");
//    imprimirVector(vector, N);

    // Clonar el vector original
    int *vectorClon = clonarVector(vector, N);

    // Ordenar el vector clonado usando Merge Sort y medir el tiempo
    clock_t inicio = clock();
    merge_sort(vectorClon, 0, N - 1);
    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

    // Imprimir el vector ordenado y el tiempo que tomó ordenarlo
//    printf("\nVector ordenado:\n");
//    imprimirVector(vectorClon, N);
    printf("\nTiempo de ordenamiento usando Merge Sort: %.6f segundos\n", tiempo);

    // Clonar el vector original
    int *vectorClon2 = clonarVector(vector, N);

    // Ordenar el vector utilizando Merge Sort y medir el tiempo
    clock_t inicio2 = clock();
    merge_sort_parallel(vectorClon2, N);
    clock_t fin2 = clock();
    double tiempo2 = (double)(fin2 - inicio2) / CLOCKS_PER_SEC;

    // Imprimir el vector ordenado y el tiempo que tomó ordenarlo
//    printf("\nVector ordenado:\n");
//    imprimirVector(vectorClon2, N);
    printf("\nTiempo de ordenamiento usando Merge Sort Paralelo: %.6f segundos\n", tiempo2);

    // Liberar memoria
    free(vector);
    free(vectorClon);

    return 0;
}