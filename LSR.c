#include <stdio.h>
#include <limits.h>

void main() {
    int n, i, j, adj[50][50], d, max = 1000; // max represents infinity

    // Input the number of nodes in the network
    printf("\nEnter the Number of Nodes: ");
    scanf("%d",&n);

    // Input the cost matrix between nodes
    printf("Enter the cost between Nodes:\n");
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j)
                adj[i][j] = 0;  // Cost from node to itself is 0
            else {
                printf("Cost from %d -> %d: ", i + 1, j + 1);
                scanf("%d", &adj[i][j]);
            }
        }
    }

    // Arrays for Dijkstra's algorithm
    int dist[n];     // Stores shortest distance from source to each node
    int visited[n];  // Tracks which nodes have been processed
    int round, v, src;

    // Initialize distances to infinity and visited to 0 (false)
    for (i = 0; i < n; i++) {
        dist[i] = max;      // Initially all distances are infinite
        visited[i] = 0;     // No nodes visited yet
    }

    // Get source node from user (convert to 0-based index)
    printf("\nEnter the source Node: ");
    scanf("%d", &src);
    src -= 1;
    dist[src] = 0;  // Distance from source to itself is 0

    // Array to store next hop information for routing
    int nxhop[n];
    for (i = 0; i < n; i++)
        nxhop[i] = 0;  // Initialize next hop to 0 (direct)

    // Dijkstra's Algorithm - main loop
    for (round = 0; round < n - 1; round++) {
        int min = max, min_index;

        // Step 1: Find the unvisited node with minimum distance
        for (v = 0; v < n; v++) {
            if (visited[v] == 0 && dist[v] < min) {
                min = dist[v];
                min_index = v;
            }
        }

        // Mark this node as processed
        visited[min_index] = 1;

        // Step 2: Update distances for adjacent nodes
        for (d = 0; d < n; d++) {
            // If node d is unvisited, has a connection from min_index, 
            // and a shorter path is found through min_index
            if (!visited[d] && adj[min_index][d] && dist[min_index] != max
                && dist[min_index] + adj[min_index][d] < dist[d]) {
                
                // Update the distance
                dist[d] = dist[min_index] + adj[min_index][d];
                
                // Update next hop information (unless it's directly connected)
                if (min_index != src)
                    nxhop[d] = min_index + 1;  // +1 for 1-based numbering
            }
        }

        // Display intermediate routing table for source node
        if (min_index == src) {
            printf("\nRouting Table of Node %d", src + 1);
            printf("\nDestination\tCost\tNext Hop\n");
            for (i = 0; i < n; i++) {
                if (dist[i] == 0)
                    printf("%d\t\t-\t-\n", i + 1);  // Source to itself
                else
                    printf("%d\t\t%d\t-\n", i + 1, dist[i]);
            }
        }
    }

    // Display final routing table
    printf("\nAfter Applying Dijkstra's Algorithm:\n");
    printf("\nRouting Table of Node %d", src + 1);
    printf("\nDestination\tCost\tNext Hop\n");
    for (i = 0; i < n; i++) {
        printf("%d\t\t%d\t\t", i + 1, dist[i]);
        if (nxhop[i] == 0)
            printf("-\n");  // Direct connection or unreachable
        else
            printf("%d\n", nxhop[i]);
    }

    // Display shortest path costs
    for (i = 1; i < n; i++)
        printf("The cost of the shortest path from router %d to %d is %d\n",
               src + 1, i + 1, dist[i]);
}
