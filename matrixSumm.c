#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>


typedef struct {
    int n;
    int k;
    int p;
    double **a;
    int result;
} ThreadArgs;

double **readMatrixFromFile(char *fileName ,int n)
{
    FILE *fp = fopen(fileName, "r");
    double** matrix = (double**)malloc(n * sizeof(double*));

    for (int i = 0; i < n; i++)
    {
        matrix[i] = (double*)malloc(n * sizeof(double));
    }
    if (fp) 
    {
        printf("File data.txt has opened for reading\n");

        for (int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                fscanf(fp, "%lf", &matrix[i][j]);
                if (matrix[i][j] == EOF) 
                {
                    printf ("Зашёл в EOF\n"); 
                    break; 
                }                
            } 
        }
        if (fclose(fp) == 0) 
        {
            printf("data.txt has closed\n");
        } else 
        {
            printf("Error closing data.txt\n");
        }
    } else 
    {
        printf("Error opening data.txt\n");
    }
    return matrix;
}

double** makeTransponMatrix(double **matrix, int n)
{
    printf("tMatrix generation started\n");

    double** matrixT = (double**)malloc(n * sizeof(double*));

    for (int i = 0; i < n; i++)
    {
        matrixT[i] = (double*)malloc(n * sizeof(double));
    } 
    for (int i = 0; i < n;i++ )
    {
        for (int j = 0; j < n; j++)
        {
         matrixT[j][i] = matrix[i][j];   
        }
    }
    printf("tMatrix generation finished\n");
    return matrixT;
}

double** makeSumMatrix(double** matrix, double** matrixT, int n)
{
    printf("Matrix summ has started\n");

    double** sumMatrix = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++)
    {
        sumMatrix[i] = (double*)malloc(n * sizeof(double));
    }
    for (int i = 0; i < n; i ++ )
    {
        for (int j = 0; j < n; j++)
        {
            sumMatrix[i][j] = (matrix[i][j] + matrixT[i][j])/2;  
        }
    }
    printf("Matrix summ has finished\n");
    return sumMatrix;
}


void *makeMatrixSymmetryCheck(void * args)
{
    ThreadArgs * threadData = (ThreadArgs *) args;
    double **a = threadData->a;
    int n = threadData->n;
    int k = threadData->k;
    int p = threadData->p;

    int step = n / p;
    int start_row = k * step;
    int end_row = (k + 1) * step;     

    printf("Thread %d check has started\n", k);
    printf ("start_row = %d, end_row = %d\n ", start_row, end_row);
    for (int i = start_row ; i < end_row; i++) 
    {
        for (int j = k*step; j < n; j++) 
        {
            if (a[i][j] != a[j][i]) 
            {
                threadData->result = 0;
                pthread_exit(NULL);
            }
        }
    }
    threadData->result = 1;
    printf("Thread %d check has finished\n", k);
    pthread_exit(NULL);
}
 


int main()
{
    //------------------------Создание матрицы n*n----------------------------------//
    int n;
    char fileName[100];
    printf("Type size of matrix\n");
    scanf("%d", &n);
    printf("Type name of file with matrix to read\n");
    scanf("%s", fileName);
    

    double** matrix = readMatrixFromFile(n, fileName);
    //-----------------------Транспонирование матрицы-------------------------------//
    
    double** matrixT = makeTransponMatrix(matrix, n);

    //------------------------Сложение матриц. В итоге получаем симметричную матрицу----------------------------------------//
    
    double** sumMatrix = makeSumMatrix(matrix, matrixT, n);
    
    //------------------Инициаализация потоков----------------------//
    int p;
    printf ("Type count of threads: ");
    scanf("%d", &p);
    if (p > n)
    {
        printf("max threads count must be less than matrix size (p < n)");
        return 2;
    }

    pthread_t threads[p];
    ThreadArgs threadData[p];

    //-----------------Создание потоков------------------------------------//
    for(int k = 0; k < p; k++)
    {
        threadData[k].n = n;
        threadData[k].p = p;
        threadData[k].k = k;
        threadData[k].a = matrix;
        pthread_create(&threads[k], NULL, makeMatrixSymmetryCheck, (void *)&threadData[k]);
    
    }
    for (int k = 0; k < p; ++k) 
    {
        pthread_join(threads[k], NULL);
    }
    
    
    // Освобождение памяти после использования
    for (int i = 0; i < n; i++)
    {
        free(matrix[i]);
        free(matrixT[i]);
        free(sumMatrix[i]);
    }
    free(matrix);
    free(matrixT);
    free(sumMatrix);

    int flag = 0;
    for (int k = 0; k < p; ++k) 
    {
        if (threadData[k].result == 0) 
        {
            flag = 1;
        }
    }
    if (flag == 0) 
    {
        printf("\nMatrix is symmetric\n");
        return 0;
    } 
    else 
    {
        printf("\nMatrix is not symmetric\n");
        return 1;
    }
}