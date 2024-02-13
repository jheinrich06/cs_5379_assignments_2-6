#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAX_VERTICES 10
#define INF 999999 // Infinity value for representing absence of edges

void generateRandomAdjacencyMatrix(int adjacencyMatrix[MAX_VERTICES][MAX_VERTICES], int dimension) {
    // Initialize adjacency matrix with zeroes
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            adjacencyMatrix[i][j] = 0;
        }
    }

    // Randomly generate edges for each type of density
    double densityTargets[3] = {0.75, 0.25, 0.5}; // Dense, Sparse, Varying density targets
    double currentDensity[3] = {0, 0, 0}; // Dense, Sparse, Varying current density
    int maxCost = 10; // Maximum cost for edge weights

    // Randomly generate edges for each type of density
    for (int d = 0; d < 3; d++) {
        int edges[MAX_VERTICES][2]; // Array to store generated edges
        int numEdges = 0; // Counter for number of generated edges

        // Continue generating edges until desired density is achieved
        while (currentDensity[d] < densityTargets[d]) {
            int row = rand() % dimension;
            int col = rand() % dimension;
            
            // Ensure generated edge is unique and not a self-loop
            if (row != col && adjacencyMatrix[row][col] == 0) {
                int weight = (d == 2) ? (rand() % (2 * maxCost + 1)) - maxCost : rand() % maxCost + 1;
                adjacencyMatrix[row][col] = weight;
                adjacencyMatrix[col][row] = weight;
                currentDensity[d] += 2.0 / (dimension * (dimension - 1)); // Update density
                edges[numEdges][0] = row;
                edges[numEdges][1] = col;
                numEdges++;
            }
        }

        // Print generated edges and current density
        printf("\nEdges for density type %d:\n", d+1);
        for (int i = 0; i < numEdges; i++) {
            printf("(%d, %d)\n", edges[i][0], edges[i][1]);
        }
        printf("Current density: %.2f\n", currentDensity[d]);
    }
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    int dimension = 6; // Dimension of the adjacency matrix
    int adjacencyMatrix[MAX_VERTICES][MAX_VERTICES];

    generateRandomAdjacencyMatrix(adjacencyMatrix, dimension);

    // Print the generated adjacency matrix
    printf("\nGenerated Adjacency Matrix:\n");
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            printf("%5d ", adjacencyMatrix[i][j]);
        }
        printf("\n");
    }
return 0;
}