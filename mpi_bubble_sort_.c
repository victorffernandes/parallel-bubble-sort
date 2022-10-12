#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

#define COMPARE_MAX_TAG 1
#define COMPARE_MIN_TAG 2
#define EVEN_TRANSPOSITION 3
#define ODD_TRANSPOSITION 4
#define SYNC_LIST 5

void listGenerator(int* lista, int size){
  for (int i = 0 ; i < size; i++)
  {
    /* gerando valores aleatórios entre zero e tam*/
    lista[i] = rand() % (10 * size);  
  }
}

void compare_change(int *val1, int *val2)
{
    if (*val1 > *val2)
    {
        int aux = *val1;
        *val1 = *val2;
        *val2 = aux;
    }
}

void printlist(int * list, int n){
    printf("------ \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", list[i]);
    }
    printf("\n------ \n");
}

int isOdd(int i)
{
    return i % 2 == 1;
}

int isEven(int i)
{
    return i % 2 == 0;
}

int main(int argc, char **argv)
{
    void bubble_sort(int *list, int n);
    void sync_list(int * list, int send_initial_offset, int send_final_offset, int r_pid, int rec_initial_offset, int rec_final_offset, int id);
    int my_rank;
    int np;
    MPI_Status status;

    int n = 10000;
    int list[n];
    listGenerator(list, n);
   
    clock_t t;
    t = clock();

    bubble_sort(list, n);

    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("sequential time taken %f \n", time_taken);


    void odd_transposition(int *list, int n, int i_offset, int pid, int pairpid);
    void even_transposition(int *list, int n, int i_offset, int pid, int pairpid);
    void start_even_transposition(int rank, int neighbour_offset);
    void start_odd_transposition(int rank, int neighbour_offset);
    void sync_list_r(int * list, int initial_offset, int final_offset, int r_pid);
    void sync_list_s(int * list, int initial_offset, int final_offset, int r_pid);

    t = clock();
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int id = my_rank + 1;

    int initial_offset = (my_rank) * (n / np);
    int final_offset = isEven(n) ? initial_offset + (n / np) - 1 : initial_offset + (n / np);

    for (int i = 1; i <= np; i++)
    {
        if (isOdd(i)) {
            if (isOdd(id)){
                if (id < np)
                {
                    MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank + 1, COMPARE_MIN_TAG, MPI_COMM_WORLD);
                    int new_final_offset;
                    MPI_Status status;
                    MPI_Recv(&new_final_offset, 1, MPI_INTEGER, (my_rank + 1), COMPARE_MAX_TAG, MPI_COMM_WORLD, &status);
                    printf("id: %d initial_offset: %d final_offset: %d pairId: %d\n",id, initial_offset, new_final_offset, id + 1);
                    int new_n = (new_final_offset - initial_offset) + 1;
                    sync_list(list, initial_offset, final_offset, (my_rank + 1), final_offset + 1, new_final_offset, (my_rank + 1) );
                    // sync_list_s(list, initial_offset, final_offset, (my_rank + 1));
                    // sync_list_r(list, final_offset + 1, new_final_offset, (my_rank + 1));
                    // printf(" \n after iteration: %d id: %d", i, id);
                    for (int i = 0; i < (new_n); i++)
                    {
                        if (i % 2 == 1)
                        {
                            start_odd_transposition(my_rank, 1);
                            odd_transposition(&list, new_n, initial_offset, id, (my_rank + 1));
                        }
                        else
                        {
                            start_even_transposition(my_rank, 1);
                            even_transposition(&list, new_n, initial_offset, id, (my_rank + 1));
                        }
                    }
                }
            }
            else
                if (id > 1)
                {
                    MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank - 1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
                    int new_initial_offset;
                    MPI_Status status;
                    MPI_Recv(&new_initial_offset, 1, MPI_INTEGER, (my_rank - 1), COMPARE_MIN_TAG, MPI_COMM_WORLD, &status);
                    //printf("id: %d initial_offset: %d final_offset: %d pairId: %d\n",id, new_initial_offset, final_offset, id - 1);
                    int new_n = (final_offset - new_initial_offset) + 1;
                    sync_list(list, initial_offset, final_offset, (my_rank - 1),new_initial_offset, initial_offset - 1, (my_rank - 1) );
                    // sync_list_s(list, initial_offset, final_offset, (my_rank - 1));
                    // sync_list_r(list, new_initial_offset, initial_offset - 1, (my_rank - 1));
                   
                    //printf(" \n after iteration: %d id: %d", i, id);
                    for (int i = 0; i < (new_n); i++)
                    {                 
                        if (i % 2 == 1)
                        {
                            start_odd_transposition(my_rank, -1);
                            odd_transposition(&list, new_n, new_initial_offset, id, (my_rank - 1));
                        }
                        else
                        {
                            start_even_transposition(my_rank, -1);
                            even_transposition(&list, new_n, new_initial_offset, id, (my_rank - 1));
                        }
                    }
                }
        }

        if (isEven(i))
        {
            if (isEven(id))
            {
                if (id < np)
                {
                    MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank + 1, COMPARE_MIN_TAG, MPI_COMM_WORLD);
                    int new_final_offset;
                    MPI_Status status;
                    MPI_Recv(&new_final_offset, 1, MPI_INTEGER, (my_rank + 1), COMPARE_MAX_TAG, MPI_COMM_WORLD, &status);
                    //printf("id: %d initial_offset: %d final_offset: %d pairId: %d\n",id, initial_offset, new_final_offset, id + 1);
                    int new_n = (new_final_offset - initial_offset) + 1;
                    sync_list(list, 0, n/2, (my_rank + 1), n/2, n-1, (my_rank + 1));
                    // sync_list_s(&list, 0, n/2, (my_rank + 1));
                    // sync_list_r(&list, n/2, n-1, (my_rank + 1));
                    //printf(" \n after iteration: %d id: %d", i, id);
                    for (int i = 0; i < (new_n); i++)
                    {
                        if (i % 2 == 1)
                        {
                            start_odd_transposition(my_rank, 1);
                            odd_transposition(&list, new_n, initial_offset, id, (my_rank + 1));
                        }
                        else
                        {
                            start_even_transposition(my_rank, 1);
                            even_transposition(&list, new_n, initial_offset, id, (my_rank + 1));
                        }
                    }
                   
                }
            }
            else
            {
                if (id > 1)
                {
                    MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank - 1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
                    int new_initial_offset;
                    MPI_Status status;
                    MPI_Recv(&new_initial_offset, 1, MPI_INTEGER, (my_rank - 1), COMPARE_MIN_TAG, MPI_COMM_WORLD, &status);
                   
                    //printf("id: %d initial_offset: %d final_offset: %d pairId: %d\n",id, new_initial_offset, final_offset, id - 1);
                    sync_list(list, n/2, n-1, (my_rank - 1), 0, n/2, (my_rank - 1));
                    // sync_list_s(&list, n/2, n-1, (my_rank - 1));
                    // sync_list_r(&list, 0, n/2, (my_rank - 1));

                    int new_n = (final_offset - new_initial_offset) + 1;
                    //printf(" \n after iteration: %d id: %d", i, id);
                    for (int i = 0; i < (new_n); i++)
                    {
                        if (i % 2 == 1)
                        {
                            start_odd_transposition(my_rank, -1);
                            odd_transposition(&list, new_n, new_initial_offset, id, (my_rank - 1));
                        }
                        else
                        {
                            start_even_transposition(my_rank, -1);
                            even_transposition(&list, new_n, new_initial_offset, id, (my_rank - 1));
                        }
                    }
                   
                }
            }
        }
        // printf(" \n after iteration: %d id: %d", i, id);
    }

    MPI_Finalize();

    printf("----- \n");
    if(my_rank == 1)
    for (int i = 0; i < n; i++)
    {
        printf("%d \n", list[i]);
    }

    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("parallel time taken %f \n", time_taken);
    return 0;
}

void bubble_sort (int *vetor, int n) {
    int i, j, aux;

    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (vetor[j] > vetor[j + 1]) {
                aux = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

void start_even_transposition(int rank, int neighbour_offset)
{
    MPI_Status status;

    MPI_Send(NULL, 0, MPI_INTEGER, rank + neighbour_offset, EVEN_TRANSPOSITION, MPI_COMM_WORLD);
    MPI_Recv(NULL, 0, MPI_INTEGER, (rank + neighbour_offset), EVEN_TRANSPOSITION, MPI_COMM_WORLD, &status);
   
}

void start_odd_transposition(int rank, int neighbour_offset)
{
    MPI_Status status;
    MPI_Send(NULL, 0, MPI_INTEGER, rank + neighbour_offset, ODD_TRANSPOSITION, MPI_COMM_WORLD);
    MPI_Recv(NULL, 0, MPI_INTEGER, (rank + neighbour_offset), ODD_TRANSPOSITION, MPI_COMM_WORLD, &status);
   
}

void even_transposition(int *list, int n, int i_offset, int pid, int pairpid)
{
   
   
    if (isOdd(pid))
    {
       
        for (int j = 0; j < (n) / 4; j++)
        {
            compare_change(&list[i_offset + (2 * j)], &list[i_offset + (2 * j) + 1]);
        }
        sync_list(list, i_offset, i_offset + (2*(n / 4)) - 1, pairpid, i_offset + (2*(n/4)), i_offset + n - 1, pairpid);
        // sync_list_s(list, i_offset, i_offset + (2*(n / 4)) - 1, pairpid);
        // sync_list_r(list, i_offset + (2*(n/4)), i_offset + n - 1, pairpid);
    }
    else
    {
        for (int j = (n / 4); j < (n / 2); j++)
        {
            compare_change(&list[i_offset + (2 * j)], &list[i_offset + (2 * j) + 1]);
        }
        sync_list(list, i_offset + (2*(n/4)) , i_offset + n - 1, pairpid, i_offset, i_offset + (2*(n / 4)) - 1, pairpid);
        // sync_list_s(list, i_offset + (2*(n/4)) , i_offset + n - 1, pairpid);
        // sync_list_r(list, i_offset, i_offset + (2*(n / 4)) - 1, pairpid);
    }
}

void odd_transposition(int *list, int n, int i_offset, int pid, int pairpid)
{
    if (isOdd(pid))
    {
        for (int j = 0; j < (n) / 4; j++)
        {
            compare_change(&list[i_offset + (2 * j) + 1], &list[i_offset + (2 * j) + 2]);
        }
        sync_list(list, i_offset , i_offset + (2 * (n/4)), pairpid, i_offset + 2 * (n/4) + 1, i_offset + (n - 1) , pairpid);
        // sync_list_s(list, i_offset , i_offset + (2 * (n/4)), pairpid);
        // sync_list_r(list, i_offset + 2 * (n/4) + 1, i_offset + (n - 1) , pairpid);
       
    }
    else
    {
        for (int j = (n / 4); j < (n / 2) - 1; j++)
        {
            compare_change(&list[i_offset + (2 * j) + 1], &list[i_offset + (2 * j) + 2]);
        }
        if (isOdd(n))
        {
            compare_change(&list[i_offset + n - 2], &list[i_offset + n - 1]);
        }
        sync_list_s(list, i_offset + (2* (n/4)) + 1, i_offset + (n - 1) , pairpid);
        sync_list_r(list, i_offset , i_offset + (2 * (n / 4)), pairpid);
       
    }
}

void sync_list_s(int * list, int initial_offset, int final_offset, int r_pid){
   
    if(initial_offset == 0){
        MPI_Send(list,final_offset - initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, MPI_COMM_WORLD);
    } else{
        MPI_Send(&list[initial_offset],final_offset - initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, MPI_COMM_WORLD);
    }
    
}

void sync_list_r(int * list, int initial_offset, int final_offset, int r_pid){
    MPI_Status status;
   
    if(initial_offset == 0){
        MPI_Recv(list, final_offset - initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, MPI_COMM_WORLD, &status);
    } else{
        MPI_Recv(&list[initial_offset], final_offset - initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, MPI_COMM_WORLD, &status);
    }


    // printf("received");
   
}

void sync_list(int * list, int send_initial_offset, int send_final_offset, int r_pid, int rec_initial_offset, int rec_final_offset, int id){
    MPI_Status status;
   
   
    // if(send_final_offset == 0){
    //     MPI_Sendrecv(list, send_initial_offset - send_initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, &list[rec_initial_offset], rec_final_offset - rec_initial_offset + 1, MPI_INTEGER,id, SYNC_LIST, MPI_COMM_WORLD, &status);
    // } else if(rec_initial_offset == 0) {
    //     MPI_Sendrecv(&list[send_initial_offset], send_initial_offset - send_initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, list, rec_final_offset - rec_initial_offset + 1, MPI_INTEGER,id, SYNC_LIST, MPI_COMM_WORLD, &status);
    // }

    MPI_Sendrecv(&list[send_initial_offset], send_initial_offset - send_initial_offset + 1, MPI_INTEGER, r_pid, SYNC_LIST, &list[rec_initial_offset], rec_final_offset - rec_initial_offset + 1, MPI_INTEGER,id, SYNC_LIST, MPI_COMM_WORLD, &status);
    //printf("received");
   
}