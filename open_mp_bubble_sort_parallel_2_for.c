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

void oddEvenSort(int* a, int n)
{
  /*
    Method 2: Uses 1 "parallel for" directive and 2 "for" directives
  */
  int phase, i, temp;
    #pragma omp parallel num_threads(NUM_THREADS) shared(a,n) private(i,temp,phase)
  for(phase=0;phase<n;++phase)
  {
    if(phase%2==0) //even phase
    {
    #pragma omp for
    for(i=1;i<n;i+=2)
	if(a[i-1] > a[i])
	{
	  temp = a[i];
	  a[i] = a[i-1];
	  a[i-1] = temp;
	}
    }
    else //odd phase
    {
    #pragma omp for
    for(i=1;i<n-1;i+=2)
	if(a[i] > a[i+1])
	{
      	  temp = a[i];
	  a[i] = a[i+1];
	  a[i+1] = temp;
	}
    }
  }
}

int main(){
    int n = 40000;
    int list[n];

    omp_set_num_threads(NUM_THREADS);
    
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