#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

void writeMatrixToFile(int n)
{
    if (remove("data.txt") == 0) 
    {
        printf("File data.txt has been removed\n");
    }
    FILE *fp = fopen("data.txt", "w");

    if (fp) 
    {
        printf("File data.txt has opened\n");

        for (int i = 0; i < n; i++) 
        {
            for (int j = 0; j < n; j++) 
            {
                double puttedNumber = (double)rand() / (RAND_MAX);
                puttedNumber = round(puttedNumber * 10000)/100;
                fprintf(fp, "%.2f ", puttedNumber);
            }
            fprintf(fp, "\n");
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
}


int main()
{
    time_t t;
    srand((unsigned) time(&t));

    printf("Type size of matrix\n");
    int n;
    scanf("%d", &n);
    writeMatrixToFile(n);
    
}