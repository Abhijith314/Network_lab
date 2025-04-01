#include <stdio.h>

// Global variables
int costMatrix[20][20];  // Stores the cost between all pairs of nodes
int n;                   // Number of nodes in the network

// Structure to represent each router's information
struct routers {
    int distance[20];    // Minimum distance to each destination node
    int adjNodes[20];    // Next hop node to reach each destination
} node[20];             // Array of routers (one for each node)

/* Function to read the cost matrix from user input */
void readCostMatrix() {
    int i, j;
    printf("\nEnter cost matrix\n");
    
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            scanf("%d", &costMatrix[i][j]);
            
            // Distance from a node to itself is always 0
            costMatrix[i][i] = 0;
            
            // Initialize router's distance table with direct link costs
            node[i].distance[j] = costMatrix[i][j];
            
            // Initially, next hop is the destination itself (direct connection)
            node[i].adjNodes[j] = j;
        }
    }
}

/* Function to calculate routing tables using Distance Vector algorithm */
void calcRoutingTable() {
    int i, j, k;
    
    // Bellman-Ford algorithm implementation
    for (i = 0; i < n; ++i) {          // For each source node
        for (j = 0; j < n; ++j) {       // For each destination node
            for (k = 0; k < n; ++k) {   // Check all possible intermediate nodes
                
                // If path through node k is better than current path
                if (node[i].distance[j] > costMatrix[i][k] + node[k].distance[j]) {
                    // Update with the new minimum distance
                    node[i].distance[j] = node[i].distance[k] + node[k].distance[j];
                    
                    // Update the next hop to be node k
                    node[i].adjNodes[j] = k;
                }
            }
        }
    }
}

/* Function to display the calculated routing tables */
void displayRoutes() {
    int i, j;
    
    for (i = 0; i < n; ++i) {      // For each router
        printf("\nRouter %d\n", i + 1);
        
        for (j = 0; j < n; ++j) {  // For each destination
            printf("Node %d via %d : Distance %d\n", 
                  j + 1, 
                  node[i].adjNodes[j] + 1,  // +1 for 1-based numbering
                  node[i].distance[j]);
        }
        printf("\n");
    }
}

int main() {
    int i, j;
    
    // Get number of nodes in the network
    printf("Number of nodes: ");
    scanf("%d", &n);
    
    // Read network topology (cost matrix)
    readCostMatrix();
    
    // Compute optimal routes using Distance Vector algorithm
    calcRoutingTable();
    
    // Display routing tables for all nodes
    displayRoutes();

    return 0;
}
