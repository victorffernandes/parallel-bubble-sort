#include <stdio.h>
#include <mpi.h>

#define COMPARE_MAX_TAG 1
#define COMPARE_MIN_TAG 2
#define EVEN_TRANSPOSITION 3
#define ODD_TRANSPOSITION 4

void compare_change(int *val1, int *val2)
{
    if (*val1 > *val2)
    {
        printf("swapped: %d to %d \n", *val1, *val2);

        int aux = *val1;
        *val1 = *val2;
        *val2 = aux;
    }
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
    void bubble_sort(int *list, int i_offset, int f_offset);
    int my_rank;
    int np;
    MPI_Status status;

    int n = 10;
    int list[10] = {2, 5, 4, 10, 8, 30, 23, 47, 38, 12};
    int list[10] = {2, 4, 5, 8, 10, 12, 23, 30, 38, 47};



    void odd_transposition(int *list, int n, int i_offset, int pid);
    void even_transposition(int *list, int n, int i_offset, int pid);
    void start_even_transposition(int rank, int neighbour_offset);
    void start_odd_transposition(int rank, int neighbour_offset);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int id = my_rank + 1;

    int initial_offset = (my_rank) * (n / np);
    int final_offset = isEven(n) ? initial_offset + (n / np) - 1 : initial_offset + (n / np);

    printf("initial: %d final: %d id: %d \n", initial_offset, final_offset, id);

    bubble_sort(&list, initial_offset, final_offset);
    // for (int i = initial_offset; i <= final_offset; i++)
    // {
    //     printf("%d \n", list[i]);
    // }

    for (int i = 0; i < np; i++)
    {
        if (isOdd(i))
        { // Odd iteration
            if (isOdd(id))
            { // Odd process number                 // Compare-exchange with the right neighbor process
                if (id < np)
                {
                    MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank + 1, COMPARE_MIN_TAG, MPI_COMM_WORLD);
                    int new_final_offset;
                    MPI_Status status;
                    MPI_Recv(&new_final_offset, 1, MPI_INTEGER, (my_rank + 1), COMPARE_MAX_TAG, MPI_COMM_WORLD, &status);
                    printf("new_final_offset: %d\n", new_final_offset);

                    int new_n = (new_final_offset - initial_offset) + 1;
                    for (int i = 0; i < (new_n); i++)
                    {
                        if (i % 2 == 1) // switch between odd and even transpositions
                        {
                            start_odd_transposition(my_rank, 1);
                            odd_transposition(&list, new_n, initial_offset, id);
                        }
                        else
                        {
                            start_even_transposition(my_rank, 1);
                            even_transposition(&list, new_n, initial_offset, id);
                        }
                    }
                }
            }
            else
                // Compare-exchange with the left neighbor process
                if (id > 1)
                {
                    // printf("id: %d sending final_offset: %d\n", my_rank, final_offset);
                    MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank - 1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
                    int new_initial_offset;
                    MPI_Status status;
                    MPI_Recv(&new_initial_offset, 1, MPI_INTEGER, (my_rank - 1), COMPARE_MIN_TAG, MPI_COMM_WORLD, &status);
                    printf("new_initial_offset: %d\n", new_initial_offset);

                    int new_n = (final_offset - new_initial_offset) + 1;
                    for (int i = 0; i < (new_n); i++)
                    {
                        if (i % 2 == 1) // switch between odd and even transpositions
                        {
                            start_odd_transposition(my_rank, -1);
                            odd_transposition(&list, new_n, new_initial_offset, id);
                        }
                        else
                        {
                            start_even_transposition(my_rank, -1);
                            even_transposition(&list, new_n, new_initial_offset, id);
                        }
                    }
                }
        }

        // if (isEven(i))
        // { // even iteration
        //     if (isEven(id))
        //     { // Even process number
        //         if (id < np - 1)
        //         {
        //             MPI_Send(&initial_offset, 1, MPI_INTEGER, my_rank + 1, COMPARE_MIN_TAG, MPI_COMM_WORLD);
        //             int new_final_offset;
        //             MPI_Status status;
        //             MPI_Recv(&new_final_offset, 1, MPI_INTEGER, (my_rank + 1), COMPARE_MAX_TAG, MPI_COMM_WORLD, &status);
        //             printf("new_final_offset: %d\n", new_final_offset);

        //             int new_n = (new_final_offset - initial_offset) + 1;
        //             for (int i = 0; i < (new_n); i++)
        //             {
        //                 if (i % 2 == 1) // switch between odd and even transpositions
        //                 {
        //                     start_odd_transposition(my_rank, 1);
        //                     odd_transposition(&list, new_n, initial_offset, id);
        //                 }
        //                 else
        //                 {
        //                     start_even_transposition(my_rank, 1);
        //                     even_transposition(&list, new_n, initial_offset, id);
        //                 }
        //             }
        //         }
        //     }
        //     else
        //     {
        //         // Compare-exchange with the left neighbor process
        //         // printf("id: %d sending final_offset: %d\n", my_rank, final_offset);
        //         if (id > 1)
        //         {
        //             MPI_Send(&final_offset, 1, MPI_INTEGER, my_rank - 1, COMPARE_MAX_TAG, MPI_COMM_WORLD);
        //             int new_initial_offset;
        //             MPI_Status status;
        //             MPI_Recv(&new_initial_offset, 1, MPI_INTEGER, (my_rank - 1), COMPARE_MIN_TAG, MPI_COMM_WORLD, &status);
        //             printf("new_initial_offset: %d\n", new_initial_offset);

        //             int new_n = (final_offset - new_initial_offset) + 1;
        //             for (int i = 0; i < (new_n); i++)
        //             {
        //                 if (i % 2 == 1) // switch between odd and even transpositions
        //                 {
        //                     start_odd_transposition(my_rank, -1);
        //                     odd_transposition(&list, new_n, new_initial_offset, id);
        //                 }
        //                 else
        //                 {
        //                     start_even_transposition(my_rank, -1);
        //                     even_transposition(&list, new_n, new_initial_offset, id);
        //                 }
        //             }
        //         }
        //     }
        // }
    }

    MPI_Finalize();

    if (my_rank == 0)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%d \n", list[i]);
        }
    }
    return 0;
}

void bubble_sort(int list[], int i_offset, int f_offset)
{
    for (int i = 0; i < (f_offset - i_offset) + 1; i++)
    {
        for (int j = i_offset; j < f_offset; j++)
        {
            compare_change(&list[j], &list[j + 1]);
        }
    }
}

void odd_transposition(int list[], int n, int i_offset, int pid)
{
    // printf("calling odd transposition to pid: %d n: %d \n", pid, n);
    if (isOdd(pid))
    {
        // odd iteration: (a1,a2) (a2,a3),... (an/2-1, an/2)
        for (int j = 0; j < (n) / 4; j++)
        {
            // printf("pid: %d odd comparing: %d to %d \n", j, list[2 * j], list[2 * j + 1]);
            // printf("j: %d \n", j);
            // printf("odd comparing: %d to %d \n", i_offset + 2 * j + 1, i_offset + 2 * j + 2);
            // printf("value odd comparing: %d to %d \n", list[i_offset + 2 * j + 1], list[i_offset + 2 * j + 2]);
            compare_change(&list[i_offset + 2 * j + 1], &list[i_offset + 2 * j + 2]);
        }
    }
    else
    {
        // even iteration: odd iteration (a2,a3) (a4,a5),... (an+1/2-1, an+1/2)
        for (int j = (n / 4); j < (n / 2); j++)
        {
            // printf("even comparing: %d to %d \n", list[2 * j], list[2 * j + 1]);
            // printf("j: %d \n", j);
            // printf("value odd comparing: %d to %d \n", list[i_offset + 2 * j], list[i_offset + 2 * j + 1]);
            compare_change(&list[i_offset + 2 * j], &list[i_offset + 2 * j + 1]);
        }
        if (isOdd(n))
        {
            // printf("n: %d \n", i_offset + n);
            // printf("value odd comparing: %d to %d \n", list[i_offset + n - 2], list[i_offset + n - 1]);
            compare_change(&list[i_offset + n - 2],  &list[i_offset + n - 1]);
        }
    }
}

void start_even_transposition(int rank, int neighbour_offset)
{
    MPI_Status status;
    int sample = 0;
    MPI_Send(&sample, 1, MPI_INTEGER, rank + neighbour_offset, EVEN_TRANSPOSITION, MPI_COMM_WORLD);
    // printf("send even: message to_rank: %d \n", rank + neighbour_offset);
    MPI_Recv(&sample, 1, MPI_INTEGER, (rank + neighbour_offset), EVEN_TRANSPOSITION, MPI_COMM_WORLD, &status);
    // printf("received even: received message my_rank: %d \n", rank);
}

void start_odd_transposition(int rank, int neighbour_offset)
{
    MPI_Status status;
    int sample = 0;
    MPI_Send(&sample, 1, MPI_INTEGER, rank + neighbour_offset, ODD_TRANSPOSITION, MPI_COMM_WORLD);
    // printf("send odd: message to_rank: %d \n", rank + neighbour_offset);
    MPI_Recv(&sample, 1, MPI_INTEGER, (rank + neighbour_offset), ODD_TRANSPOSITION, MPI_COMM_WORLD, &status);
    // printf("received odd: received message my_rank: %d \n", rank);
}

void even_transposition(int list[], int n, int i_offset, int pid)
{
    // printf("calling even transposition to pid: %d \n", pid);
    if (isOdd(pid))
    {
        // odd iteration: (a1,a2) (a2,a3),... (an/2-1, an/2)
        for (int j = 0; j < (n) / 4; j++)
        {
            // printf("pid: %d odd comparing: %d to %d \n", j, list[2 * j], list[2 * j + 1]);
            // printf("value even comparing: %d to %d \n", list[i_offset + 2 * j + 1], list[i_offset + 2 * j + 2]);
            // printf("even comparing: %d to %d \n", i_offset + 2 * j + 1, i_offset + 2 * j + 2);
            compare_change(&list[i_offset + 2 * j + 1], &list[i_offset + 2 * j + 2]);
        }
    }
    else
    {
        // even iteration: odd iteration (a2,a3) (a4,a5),... (an+1/2-1, an+1/2)
        for (int j = (n / 4); j < (n / 2); j++)
        {
            // printf("even comparing: %d to %d \n", list[2 * j], list[2 * j + 1]);
            // printf("j: %d \n", j);
            // printf("even comparing: %d to %d \n", i_offset + 2 * j, i_offset + 2 * j + 1);
            // printf("value even comparing: %d to %d \n", list[i_offset + 2 * j], list[i_offset + 2 * j + 1]);
            compare_change(&list[i_offset + 2 * j], &list[i_offset + 2 * j + 1]);
        }
    }
}
