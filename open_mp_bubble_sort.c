#include <omp.h>
#include <stdio.h>

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
 
        // Perform Bubble sort on odd indexed element
        for (int i = 1; i <= n - 2; i = i + 2) {
            if (arr[i] > arr[i + 1]) {
                swap(&arr[i], &arr[i + 1]);
                isSorted = 0;
            }
        }
 
        // Perform Bubble sort on even indexed element
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
    int N = N;
    int a[N] = {6, 10, 3,2,1,4,5,};

    oddEvenSort(a, N);

    for (int i = 0; i < N; i++){
        printf("%d \n", a[i]);
    }
}