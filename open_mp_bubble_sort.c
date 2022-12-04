#include <omp.h>
#include <stdio.h>
#include <time.h>

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
 
    while (!isSorted) {
        isSorted = 1;
 
        #pragma parallel for
        for (int i = 1; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                isSorted = 0;
            }
        }
 
        #pragma parallel for
        for (int i = 0; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                isSorted = 0;
            }
        }
    }
 
    return;
}

int main(){
    int n = 40000;
    int list[n];

    omp_set_num_threads (1);
    
    FILE *ptr;

    ptr = fopen("40000.bin","wb");  // r for read, b for binary

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