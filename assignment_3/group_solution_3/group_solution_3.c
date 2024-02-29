#include <stdio.h> 
#include <stdlib.h> 
#include <math.h>
#include <time.h>
#include <mpi.h>

// Implements binary divide and conquer to distribute message that barrier is clear
void mybarrier(MPI_Comm comm) {
    int rank, size, tag = 0;
    MPI_Status status;

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);
    printf("rank: %d inside barrier \n", rank);

    //Initialize round
    int round = 1;

    int parent, child_1, child_2;

    // Instuctions for Root Process
    if(rank == 0) {
        // Set child 1
        if(size >= 2) {
            child_1 = 1;
        }
        else {
            child_1 = 0;
        }
        
        //Set child 2
        if(size >= 3) {
            child_2 = 2;
        }
        else {
            child_2 = 0;
        }

        // Root Receive messages from below
        // No children case
        if(child_1 == 0 && child_2 == 0) {
            printf("Node %d exiting barrier after %d rounds \n", rank, round);

        }
        // Else recieve messages from one or 2 children below and return
        else if(child_1 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            round = round +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);

        }
        else if(child_2 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            round = round +1;
            tag = tag +1;

            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
        }
        else {
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            round = round +1;
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d from node %d \n", rank, child_1);
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d from node %d \n", rank, child_2);
        }
    }

    // Instructions for Process 1
    else if(rank == 1) {
        parent = 0;
        if(size >= 4) {
            child_1 = 3;
        }
        else {
            child_1 = 0;
        }
        
        if(size >= 5) {
            child_2 = 4;
        }
        else {
            child_2 = 0;
        }


        // Send messages from bottom
        if(child_1 == 0 && child_2 == 0) {
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);

        }
        // Else recieve messages from one or 2 children below and send up
        else if(child_1 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            round = round +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d from node %d \n", rank, parent);

        }
        else if(child_2 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            round = round +1;
            tag = tag +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);

            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d from node %d \n", rank, parent);
        }
        else {
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            round = round +1;
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d from node %d \n", rank, parent);
        }

        // Recieve confirmation from parent all nodes are complete
        MPI_Recv(&round, 1, MPI_INT, parent, 1, comm, &status);
        printf("Message 1 recieved by node %d from node %d \n", rank, parent);
        round = round +1;

        // Distribute message to any children and exit
        // No children case
        if(child_1 == 0 && child_2 == 0) {
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        // Else distribute to one or 2 children and exit
        else if(child_1 == 0){
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        else if(child_2 == 0){
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        else {
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        
    }

    // Instructions for Even Processes
    else if(rank % 2 == 0 && rank / 2 > 0 ) {
        parent = (rank / 2) -1;
        
        //Set child 2
        if(size >= (rank * 2) +2) {
            child_1 = (rank * 2) +1;
        }
        else {
            child_1 = 0;
        }

        //Set child 1
        if(size >= (rank * 2) +3) {
            child_2 = (rank * 2) +2;
        }
        else {
            child_2 = 0;
        }
        
        // Send messages from bottom
        if(child_1 == 0 && child_2 == 0) {
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);

        }
        // Else recieve messages from one or 2 children below and send up
        else if(child_1 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            round = round +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);

            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);

        }
        else if(child_2 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            round = round +1;
            tag = tag +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);

            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);
        }
        else {
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            round = round +1;
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);
        }

        // Recieve confirmation from parent all nodes are complete
        MPI_Recv(&round, 1, MPI_INT, parent, 1, comm, &status);
        printf("Message 1 recieved by node %d from node %d \n", rank, parent);
        round = round +1;

        // Distribute message to any children and exit
        // No children case
        if(child_1 == 0 && child_2 == 0) {
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
        }
        // Else distribute to one or 2 children and exit
        else if(child_1 == 0){
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
        }
        else if(child_2 == 0){
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
        }
        else {
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
        }
    }

    // Instructions for Odd Processes greater than 1
    else if(rank % 2 == 1 && rank / 2 >= 1) {
        parent = rank / 2;
        
        //Set child 1
        if(size >= (rank * 2) +2) {
            child_1 = (rank * 2) +1;
        }
        else {
            child_1 = 0;
        }
        
        // Set child 2
        if(size >= (rank * 2) +3) {
            child_2 = (rank * 2) +2;
        }
        else {
            child_2 = 0;
        }

        // Send messages from bottom
        if(child_1 == 0 && child_2 == 0) {
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);

        }
        // Else recieve messages from one or 2 children below and send up
        else if(child_1 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            round = round +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);

            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);

        }
        else if(child_2 == 0){
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            round = round +1;
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);

            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);
        }
        else {
            MPI_Recv(&round, 1, MPI_INT, child_1, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_1);
            MPI_Recv(&round, 1, MPI_INT, child_2, 0, comm, &status);
            printf("Message 0 recieved by node %d from node %d \n", rank, child_2);
            round = round +1;
            MPI_Send(&round, 1, MPI_INT, parent, 0, comm);
            printf("Message 0 sent by node %d to node %d \n", rank, parent);
        }

        // Recieve confirmation from parent all nodes are complete
        MPI_Recv(&round, 1, MPI_INT, parent, 1, comm, &status);
        printf("Message 0 recieved by node %d from node %d \n", rank, parent);
        round = round +1;

        // Distribute message to any children and exit
        // No children case
        if(child_1 == 0 && child_2 == 0) {
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        // Else distribute to one or 2 children and exit
        else if(child_1 == 0){
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        else if(child_2 == 0){
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        else {
            MPI_Send(&round, 1, MPI_INT, child_1, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_1);
            MPI_Send(&round, 1, MPI_INT, child_2, 1, comm);
            printf("Message 1 sent by node %d to node %d \n", rank, child_2);
            printf("Node %d exiting barrier after %d rounds \n", rank, round);
            //return;
        }
        
    }
}

int main(int argc, char **argv) {
    // Initialize MPI
    MPI_Init(&argc, &argv);

    int rank;
    // Grab the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Do a process
    for (int i = 0; i < 10000; i++) {
         //placeholder code to simulate some processing
        int x = i % 2;
    }

    double timestamp1 = MPI_Wtime();
    printf("Task 1 complete for node %d. Entering the barrier at time: %f\n", rank, timestamp1 );
    fflush(stdout);

    // Call the our barrier function to ensure all "Before barrier"
    // messages are printed before any "After barrier" messages.
    mybarrier(MPI_COMM_WORLD);

    double timestamp2 = MPI_Wtime();
    printf("Node %d leaves the barrier at time: %f \n", rank, timestamp2);
    fflush(stdout);

    // Do another process
    for (int i = 0; i < 10000; i++) {
        // placeholder code to simulate some processing
        int x = i % 2;
    }

    double timestamp3 = MPI_Wtime();
    printf("Task 2 complete for Node %d. Process exit at time: %f \n", rank, timestamp3 );
    fflush(stdout);

    
    // Finalize the MPI environment
    MPI_Finalize();
    return 0;
}


