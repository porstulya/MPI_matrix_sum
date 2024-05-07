#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

double *readMatrixFromFile(char *fileName, int n)
{
    FILE *fp = fopen(fileName, "r");
    double* matrix = (double*)malloc(n * n * sizeof(double));
    if (fp) 
    {
        printf("File data.txt has opened for reading\n");

        for (int i = 0; i < n*n; i++)
        {
            fscanf(fp, "%lf", &(matrix[i]));              
        }
        if (fclose(fp) == 0) 
        {
            printf("data.txt has closed\n");
        } else 
        {
            printf("Error closing data.txt\n");
            return 0;
        }
    } else 
    {
        printf("Error opening data.txt\n");
        return 0;
    }
    return matrix;
}

double* makeTransponMatrix(double *matrix, int n)
{
    printf("tMatrix generation started\n");

    double* matrixT = (double*)malloc(n * n * sizeof(double));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            matrixT[j * n + i] = matrix[i * n + j];   
        }
    }
    printf("tMatrix generation finished\n");
    return matrixT;
}

void distributeElements(int matrixSize, int arraySize, int *displsArray) {
    int remainder = matrixSize % arraySize; 

    for (int i = 0; i < arraySize; i++) {
        displsArray[i] = matrixSize / arraySize; 
        if (remainder > 0) {
            displsArray[i]++; 
            remainder--;
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    int n;
    double *a;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3) {
        if (rank == 0) {
            printf("Usage: -np <process_count> %s <matrix_size> <data_file>\n", argv[0]);
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    if (size == 1)
    {
        if (rank == 0) {
            printf("Number of processes must be more than 1\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    n = atoi(argv[1]);

    int displsArray[size-1];
    int matrixSize = (n * n);
    int bufferVar = matrixSize / (size-1);
    char fileName[100];
    
    
    if (rank == 0) {
       
        double* matrix;
        double* matrixT;
        double* matrixResult = malloc(matrixSize*sizeof(double));

        matrix = readMatrixFromFile(argv[2], n);

        matrixT = makeTransponMatrix(matrix, n);

        distributeElements(matrixSize, size-1, displsArray);

        for (int i = 1; i < size; i++)
        { 
            if(size<4)
            {
                if(n%(size-1) == 0)
                {
                    MPI_Send(&displsArray[i-1], 1,MPI_INT, i, 'S', MPI_COMM_WORLD);

                    MPI_Send(matrix+(i-1)*(displsArray[i-1]), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);

                    MPI_Send(matrixT+(i-1)*(displsArray[i-1]), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);

                }else
                {
                    MPI_Send(&displsArray[i-1], 1,MPI_INT, i, 'S', MPI_COMM_WORLD);

                    MPI_Send(matrix+(i-1)*(displsArray[i-1]+i-1), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);
                    
                    MPI_Send(matrixT+(i-1)*(displsArray[i-1]+i-1), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);
                    
                }
            }else if(size >= 4)
            {
                if(n%(size-1) != 0)
                {
                    MPI_Send(&displsArray[i-1], 1,MPI_INT, i, 'S', MPI_COMM_WORLD);

                    MPI_Send(matrix+(i-1)*(displsArray[i-1])+(i>1), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);                

                    MPI_Send(matrixT+(i-1)*(displsArray[i-1])+(i>1), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);
        
                }else
                {
                    MPI_Send(&displsArray[i-1], 1,MPI_INT, i, 'S', MPI_COMM_WORLD);

                    MPI_Send(matrix+(i-1)*(displsArray[i-1]), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);
                
                    MPI_Send(matrixT+(i-1)*(displsArray[i-1]), displsArray[i-1],MPI_DOUBLE, i, 'S', MPI_COMM_WORLD);
                }    
            }
        }
        for(int i=1; i<size; i++)
        {
            if (size <4)
            {
                if(n%(size-1) == 0)
                {
                    MPI_Recv(matrixResult+(i-1)*(displsArray[i-1]), displsArray[i-1], MPI_DOUBLE, i,'R',MPI_COMM_WORLD, &status);
                    
                }
                else
                {
                    MPI_Recv(matrixResult+(i-1)*(displsArray[i-1]+i-1), displsArray[i-1], MPI_DOUBLE, i,'R',MPI_COMM_WORLD, &status);
                } 
            }else if(size >=4)
            {
                if (n%(size-1) != 0)
                {
                    MPI_Recv(matrixResult+(i-1)*(displsArray[i-1])+(i>1), displsArray[i-1], MPI_DOUBLE, i,'R',MPI_COMM_WORLD, &status);
                }else
                {
                    MPI_Recv(matrixResult+(i-1)*(displsArray[i-1]), displsArray[i-1], MPI_DOUBLE, i,'R',MPI_COMM_WORLD, &status);
                }
            }
        }
        printf("\n:::::FINAL RESULTS IS:::::\n");
        for(int i = 0; i < matrixSize; i++)
        {
            printf("%.2f ", matrixResult[i]);
            if ((i !=0) && ((i+1) % n == 0))
            {
                printf("\n");
            }
        }
        printf("\n");

        free(matrix);
        free(matrixT);
        free(matrixResult);
    }else
    {
        int arraySize;
        MPI_Recv(&arraySize, 1, MPI_INT, 0, 'S', MPI_COMM_WORLD, &status);

        double *arrayA = malloc(arraySize * sizeof(double));
        double *arrayT = malloc(arraySize * sizeof(double));
        double *arrayResult = malloc(arraySize * sizeof(double));

        MPI_Recv(arrayA, arraySize, MPI_DOUBLE, 0, 'S', MPI_COMM_WORLD, &status);
        MPI_Recv(arrayT, arraySize, MPI_DOUBLE, 0, 'S', MPI_COMM_WORLD, &status);
        

        for(int i = 0; i < arraySize; i++)
        {
            arrayResult[i] = (arrayA[i] + arrayT[i])/2;
        }
        
        MPI_Send(arrayResult, arraySize, MPI_DOUBLE, 0, 'R', MPI_COMM_WORLD);
        
        free(arrayA);
        free(arrayT);
        free(arrayResult);
        
    }
    
    MPI_Finalize();

    return EXIT_SUCCESS;
}
