#define N 4
#define M N
#define K N
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    int i, j, k, rank, size, tag = 99, blksz, sum = 0;
    int a[N][M];
    int b[M][K];
    //int a[N][N] = { {1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7} };
    //int b[N][N] = { {1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7} };
    int c[N][K];
    int aa[N];
    int cc[N];
    double starttime;
    double elapsedTime;
    double totalTime;

    //Инициализируем MPI, отмечаем время начала работы 
    MPI_Init(&argc, &argv);
    starttime = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL) + rank);

    //Создаем матрицы из рандомных чисел
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            a[i][j] = rand() % 10;
        }
    }

    for (i = 0; i < M; i++) {
        for (j = 0; j < K; j++) {
            b[i][j] = rand() % 10;
        }
    }

    //if (rank == 0) {
    //    printf("Matrix A: \n");
    //    for (i = 0; i < N; i++) {
    //        for (j = 0; j < M; j++) {
    //            printf(" %d", a[i][j]);
    //        }
    //        printf("\n");
    //    }
    //    printf("______________________________________\n");
    //    printf("\nMatrix B: \n");
    //    for (i = 0; i < M; i++) {
    //        for (j = 0; j < K; j++) {
    //            printf(" %d", b[i][j]);
    //        }
    //        printf("\n");
    //    }
    //}

    //Отправляем строчки матрицы A по процессам     
    MPI_Scatter(a, N * N / size, MPI_INT, aa, N * N / size, MPI_INT, 0, MPI_COMM_WORLD);

    //Даем матрицу B всем процессам
    MPI_Bcast(b, M * K, MPI_INT, 0, MPI_COMM_WORLD);
    
    //Барьер для синхронизации
    MPI_Barrier(MPI_COMM_WORLD);

    //Перемножаем
    for (i = 0; i < K; i++)
    {
        cc[i] = 0;
        for (j = 0; j < N; j++)
        {
           cc[i] += aa[j] * b[j][i];
        }
    }

    //Собираем полученные данные в матрицу С
    MPI_Gather(cc, K * N / size, MPI_INT, c, K * N / size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    //Считаем время суммарно потраченное всеми процессами
    elapsedTime = MPI_Wtime() - starttime;
    MPI_Reduce(&elapsedTime, &totalTime, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Finalize();

    //Выводим матрицу
    if (rank == 0) {
        for (i = 0; i < N; i++) {
            for (j = 0; j < K; j++) {
                printf("%d ", c[i][j]);
            }
            printf("\n");
        }
        printf("\nTotal: %.4f sec\n", totalTime);
    }
    return 0;
}
