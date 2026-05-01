#include<omp.h>
#include <iostream>

#define N 100000
#define CHUNKSIZE 100


int main(int argc, char *argv[]) {
    int i, chunk;
    float a[N], b[N], c[N];

    for (i = 0; i < N; i++)
        a[i] = b[i] = i * 1.0;
    chunk = CHUNKSIZE;

    double start = omp_get_wtime();

    #pragma omp parallel shared(a,b,c,chunk) private(i)
    {
        #pragma omp for schedule(static, chunk) nowait
        for (i = 0; i < N; i++)
            c[i] = a[i] + b[i];
    }

    double end = omp_get_wtime();

    std::cout << "Suma de vectores completada" << std::endl;
    std::cout << "Tiempo de ejecucion: " << end - start << " segundos" << std::endl;

    return 0;
}