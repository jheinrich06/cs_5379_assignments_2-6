#include <stdio.h> 
#include <stdlib.h> 
#include <mpi.h>


int min(int a, int b){
    if (a<b) {
        return a;
    }
    return b;
}


void HW2(int n, int **matrix, int *output) {
    int i, j, count, tmp, leastVal, leastPos, *done;

    //Sets up array that will indicate whether a row has been calculated 
    done = (int *) calloc( n, sizeof(int) );
    
    //This for loop only passes through outter loop once
    //Sets each entry in the output array equal to the corresponding entries with an adjacency to row[0], including 0's (non-connections)
    for(i=0; i<n; i++) {
        done[i]= 0;
        output[i] = matrix[0][i];
    }

    //printf("\n pre-second loop output 0 is %d",output[0]);
    //row[0] has been scanned so we discard to the done pile
    done[0] = 1 ;
    count = 1 ;

    //Sets up outter loop through adjacency matrix
    while( count < n ) {
        //Set prohibitively high leastVal
        leastVal = 987654321 ;
        
        //For each value in n, set tmp to current value of that iteration in the output array.
        //If the row[i] is not marked done, and tmp < leastVal, set leastVal to tmp and leastPos to index i
        //This succeeds in finding the smallest value in each row
        for(i=0; i<n; i++) { // <-- parallelize this loop
            tmp = output[i] ;
            printf("\n tmp is: %d", tmp);
            if( (!done[i]) && (tmp < leastVal) ) {
                leastVal = tmp ;
                leastPos = i ;
            }
        }
        printf("\n leastVal is: %d, leastPos is: %d ",leastVal, leastPos);

        //Remove row of leastPos from candidates with done flag and increment count
        done[leastPos] = 1;
        count++ ;
        
        //If row has not been removed, 
        for(i=0; i<n; i++) { // <-- parallelize this loop
            if( !(done[i]) )
                output[i] = min(output[i], leastVal + matrix[leastPos][i]);
        }
    } /*** End of while ***/
free(done) ;
}

void main()
{
    int n = 4;
    int **data_matrix = (int **)malloc(n * sizeof(int *)); 
    for (int i = 0; i < n; i++) {
        data_matrix[i] = (int *)malloc(n * sizeof(int));
    }
    int  *output = (int *)calloc(n, sizeof(int));

    printf("\n output 0 is %d",output[0]);
    printf("\n output 1 is %d",output[1]);
    printf("\n output 2 is %d",output[2]);

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
    printf("\n output 0 is %d",output[0]);
    printf("\n output 1 is %d",output[1]);
    printf("\n output 2 is %d",output[2]);
    printf("\n output 3 is %d",output[3]);

}