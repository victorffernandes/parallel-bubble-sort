#include <stdio.h>
#include <mpi.h>

#define COMPARE_MAX_TAG 1
#define COMPARE_MIN_TAG 2

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
    void bubble_sort(int *list, int i_offset, int f_offset);
    int my_rank;
    int np;
    float a = 0.0, b = 1.0;
    int dest = 0;
    int tag = 200;
    MPI_Status status;

    int n = 10;
    int list[10] = {2, 5, 4, 10, 8, 30, 23, 47, 38, 12};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int id = my_rank + 1;

    int initial_offset = (my_rank) * (n/np);
    int final_offset = initial_offset + (n/np) - 1 ;

    printf("initial: %d final: %d id: %d \n", initial_offset, final_offset, id);

    bubble_sort(&list, initial_offset, final_offset);
    // for(int i = initial_offset; i <= final_offset; i++){
    //     printf("%d \n", list[i]);
    // }

    for (int i = 0; i < np; i++)
    {
        if (isOdd(i))
        { // Odd iteration
            if (isOdd(id))
            { // Odd process number
                // Compare-exchange with the right neighbor process
                if (id < np){
                    MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank + 1, COMPARE_MIN_TAG, MPI_COMM_WORLD);
                    int new_final_offset;
                    MPI_Status status;
                    MPI_Recv(&new_final_offset, 1, MPI_INTEGER, (my_rank+1), COMPARE_MAX_TAG, MPI_COMM_WORLD, &status);
                    printf("new_final_offset: %d\n", new_final_offset);

                    int new_n = (new_final_offset-i_offset);
                    for (int i = 0; i < (new_n) + 1; i++)
                    {
                        if(i%2==1){ // odd iteration
                            for ( j=initial_offset; j<(new_n)/2-2; j++ ){
                                if(list[2*j+2] < list[2*j+1]){
                                    int aux = list[2*j+1];
                                    list[2*j+1] = list[2*j+2];
                                    list[2*j+2] = aux;
                                }
                            }
                            // if ((new_n)%2==1){
                            //     if(list[2*j+2] < list[2*j+1]){
                            //         int aux = list[2*j+1];
                            //         list[2*j+1] = list[2*j+2];
                            //         list[2*j+2] = aux;
                            //     }
                            // }
                        } else {
                            for ( j=1; j<new_n/2-1; j++ ){
                                if(list[2*j+1] < list[2*j]){
                                    int aux = list[2*j+1];
                                    list[2*j+1] = list[2*j];
                                    list[2*j] = aux;
                                }
                            }
                        }
                    }
                }
            }
            else
                // Compare-exchange with the left neighbor process
                if (id > 0){
                    printf("id: %d sending final_offset: %d\n", my_rank, final_offset);
                    MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank - 1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
                    int new_initial_offset;
                    MPI_Status status;
                    MPI_Recv(&new_initial_offset, 1, MPI_INTEGER, (my_rank-1), COMPARE_MIN_TAG, MPI_COMM_WORLD, &status);
                    printf("new_initial_offset: %d\n", new_initial_offset);
                }
        }
        // if (isEven(i))
        // { // Even iteration
        //     if (isEven(id))
        //     { // Even process number
        //         // Compare-exchange with the right neighbor process
        //         if (id < np - 1){
        //             MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank+1, 0, MPI_COMM_WORLD);
        //             // printf("ID:%d i: %d\n", id, i);
        //         }
        //     }
        // }
        // else
        // { // odd iteration
        //     // Compare-exchange with the left neighbor process
        //     MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank-1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
        //     // printf("ID:%d i: %d\n", id, i);
        // }
    }

    MPI_Finalize();
    return 0;
}

void bubble_sort(int list[], int i_offset, int f_offset)
{
    for (int i = 0; i < (f_offset-i_offset) + 1; i++)
    {
        int not_changed = 1;

        for (int j = i_offset; j < f_offset; j++)
        {
            if (list[j] > list[j+1])
            {
                int aux = list[j];
                list[j] = list[j+1];
                list[j+1] = aux;
                not_changed = 0;
            }
        }
        if (not_changed)
            break;
    }
}


