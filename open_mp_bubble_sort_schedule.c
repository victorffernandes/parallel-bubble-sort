#include <omp.h>
#include <stdio.h>
#include <time.h>

#define NUM_THREADS 2

/* swaps the elements */
void swap(int * x, int * y)
{
    int temp;
 
    temp = *x;
    *x = *y;
    *y = temp; 
}

// A function to sort the algorithm using Odd Even sort
void oddEvenSort(int arr[], int n)
{
    int isSorted = 0; // Initially array is unsorted
    int i;
 
    while (!isSorted) {
        isSorted = 1;
 
        #pragma omp parallel for schedule(dynamic, 1000) shared(arr) private(i)
        for (i = 1; i <= n - 2; i = i + 2) { // odd (1,2) (3,4) (5,6)
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                isSorted = 0;
            }
        }
 
        #pragma omp parallel for schedule(dynamic, 1000) shared(arr) private(i)
        for (i = 0; i <= n - 2; i = i + 2) { // even (0 1) (2, 3) (4,5)
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                isSorted = 0;
            }
        }
    }
 
    return;
}

int main(){
    int n = 200000;
    int list[n];

    omp_set_num_threads(NUM_THREADS);
    
    FILE *ptr;

    ptr = fopen("200000.bin","wb");  // r for read, b for binary

    fread(list,sizeof(int) * n,n,ptr); // read 10 bytes to our buffer

    clock_t t;
    t = clock();
    double time_taken;

    oddEvenSort(list, n);

    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("time taken %f \n", time_taken);

    // for (int i = 0; i < n; i++){
    //     printf("%d \n", list[i]);
    // }
}