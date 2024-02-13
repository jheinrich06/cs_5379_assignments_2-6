#include <stdio.h> 
#include <stdlib.h> 
#include <mpi.h>


void HW2(int n, int **matrix, int *output);
int min(int a, int b);

MPI_Status status; 
int rank, process_count;

void main()
{
    //Initializing MPI
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    int n = 4;
    
    int **data_matrix = (int **)malloc(n * sizeof(int *)); 
    for (int i = 0; i < n; i++) {
        data_matrix[i] = (int *)malloc(n * sizeof(int));
    }
    int  *output = (int *)calloc(n, sizeof(int));

    //Matrix with zeros at diagonal positions - 4x4
    data_matrix[0][0] = 0;
    data_matrix[0][1] = 1;
    data_matrix[0][2] = 6;
    data_matrix[0][3] = 0;

    data_matrix[1][0] = 1;
    data_matrix[1][1] = 0;
    data_matrix[1][2] = 3;
    data_matrix[1][3] = 0;

    data_matrix[2][0] = 6;
    data_matrix[2][1] = 3;
    data_matrix[2][2] = 0;
    data_matrix[2][3] = 1;

    data_matrix[3][0] = 0;
    data_matrix[3][1] = 0;
    data_matrix[3][2] = 1;
    data_matrix[3][3] = 0;
    
    //Call to main function - all processes will enter
    HW2(n, data_matrix, output);

    //
    MPI_Barrier(MPI_COMM_WORLD);


    
    // Print results if you're the root process - zeros
    if (rank == 0) {
        printf("rank: %d outputs with zero: \n", rank);
        for (int i = 0; i < n; i++) {
            printf("Output %d: %d\n", i, output[i]);
        }
    }

    //Matrix with zero (except node zero) turned 999 - 4x4
    data_matrix[0][0] = 0;
    data_matrix[0][1] = 1;
    data_matrix[0][2] = 6;
    data_matrix[0][3] = 999;

    data_matrix[1][0] = 1;
    data_matrix[1][1] = 999;
    data_matrix[1][2] = 3;
    data_matrix[1][3] = 999;

    data_matrix[2][0] = 6;
    data_matrix[2][1] = 3;
    data_matrix[2][2] = 999;
    data_matrix[2][3] = 1;

    data_matrix[3][0] = 999;
    data_matrix[3][1] = 999;
    data_matrix[3][2] = 1;
    data_matrix[3][3] = 999;


    HW2(n, data_matrix, output);
    MPI_Barrier(MPI_COMM_WORLD);

    // Print results if you're the zero process
    if (rank == 0) {
        printf("rank: %d outputs with zero turned 999:\n", rank);
        for (int i = 0; i < n; i++) {
            printf("Output %d: %d\n", i, output[i]);
        }
    }

    MPI_Finalize();

}

int min(int a, int b){
    if (a<b) {
        return a;
    }
    return b;
}


void HW2(int n, int **matrix, int *output) {
    int i, j, count, tmp, leastVal, leastPos, *done, chunk_leastVal, chunk_leastPos;

    int chunk_size, p_used;
    if(process_count <= n)
        { chunk_size = n / process_count;
        p_used = process_count;
        }
    else {
        chunk_size = 1;
        p_used = n;
    }
    done = (int *) calloc( n, sizeof(int) );
    //printf("chunk size is %d", chunk_size);

    for(i=0; i<n; i++) {
        done[i]= 0;
        output[i] = matrix[0][i];
    }
    done[0] = 1 ;
    count = 1 ;
     
     if(rank >= n) {
        return;
     }

    //Outer loop is not parallelized since it is sequentially dependent.
    while( count < n ) {
        leastVal = 987654321 ;
        chunk_leastVal = leastVal;

        //Starting and ending points of the segments for parallelization
        int start = rank * chunk_size;
        int stop = (rank +1) * chunk_size;

        //This loop can be run in parallel since segments of any row can be analyzed in parallel and then merged.
        //We simply divided the number of iterations by the number of processes used and gave them a segment based on their rank
        for(i=start; i<stop && i < n; i++) { // <-- parallelize this loop
            tmp = output[i] ;
            if( (!done[i]) && (tmp < leastVal) ) {
                chunk_leastVal = tmp ;
                chunk_leastPos = i ;
            }
        }

        // Send data from non-zero processes to process zero
        if (rank != 0) { //Non-zero processes block
            // Send the smallest position and value found by each non-zero process to the zero process
            MPI_Send(&chunk_leastPos, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&chunk_leastVal, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Recv(&leastPos, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
            MPI_Recv(&leastVal, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);

        } else { //Process-zero block
            // Process zero's lowest value and corresponding position
            leastPos = chunk_leastPos;
            leastVal = chunk_leastVal;

            // Process zero takes in the lowes values and corresponding posiions form other non-zero processes
            for (int i = 1; i < p_used; i++) {
                MPI_Recv(&chunk_leastPos, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                MPI_Recv(&chunk_leastVal, 1, MPI_INT, i, 2, MPI_COMM_WORLD, &status);
                // As values are taken in they are compared with exiisting lowest value and only saved if lower than encumbant
                if (chunk_leastVal < leastVal) {
                    leastVal = chunk_leastVal;
                    leastPos = chunk_leastPos;
                }
            }

            // Send the global lowest value and corresponding position to all non-zero processes
            for (int i = 1; i < p_used; i++) {
                MPI_Send(&leastPos, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
                MPI_Send(&leastVal, 1, MPI_INT, i, 4, MPI_COMM_WORLD);

            }
        }
        // Make sure each process finishes calc before moving on.
        MPI_Barrier(MPI_COMM_WORLD);

        //Remove row of leastPos from candidates with done flag and increment count
        done[leastPos] = 1;
        count++ ;

        //int chunk_output[n];
        for(i=start; i<stop && i < n; i++) { // <-- parallelize this loop
            if( !(done[i]) )
                output[i] = min(output[i], leastVal + matrix[leastPos][i]);
        }


        // Send output updates to process 0 for compilation
        if (rank != 0) {
            MPI_Send(output + start, chunk_size, MPI_INT, 0, rank, MPI_COMM_WORLD);
        } else {
            for (int src = 1; src < process_count; src++) {
                int loop_start = src * chunk_size;
                MPI_Recv(output + loop_start, chunk_size, MPI_INT, src, src, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }


    } /*** End of while ***/
free(done) ;
}

