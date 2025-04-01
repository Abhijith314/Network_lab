#include <stdio.h>

int main() {
    // Variable declarations:
    int incoming;       // Size of incoming packet (bytes)
    int outgoing;       // Outgoing transmission rate (bytes/sec)
    int buck_size;      // Maximum bucket/buffer size (bytes)
    int n;              // Number of packets to process
    int store = 0;      // Current buffer fill level (bytes)

    // Get user input for bucket parameters
    printf("Enter the bucket size (Bytes): ");
    scanf("%d", &buck_size);    // Maximum capacity of the bucket
    
    printf("Enter the outgoing rate (Bytes per second): ");
    scanf("%d", &outgoing);     // Rate at which data leaves the bucket
    
    printf("Enter the number of packets: ");
    scanf("%d", &n);            // Total packets to be processed
    
    printf("-----------\n");     // Visual separator

    // Process each packet
    while(n != 0) {
        // Get incoming packet size
        printf("Enter the incoming packet size (Bytes): ");
        scanf("%d", &incoming);

        // Check if packet fits in remaining bucket space
        if(incoming <= (buck_size - store)) {
            // Packet fits - add to bucket
            store += incoming;
            printf("Current Bucket buffer size %d out of %d\n", store, buck_size);
        } else {
            // Packet doesn't fit - drop excess bytes
            int dropped = incoming - (buck_size - store);
            printf("Dropped %d bytes of data from Packet\n", dropped);
            
            // Fill bucket to capacity
            store = buck_size;
            printf("Current Bucket buffer size %d out of %d\n", store, buck_size);
        }

        // Simulate outgoing transmission
        store = store - outgoing;
        
        // Prevent negative buffer size
        if (store < 0) {
            store = 0;
        }

        // Display status after transmission
        printf("After outgoing %d bytes left out of %d in buffer\n", store, buck_size);
        printf("----------------------------\n");  // Visual separator
        
        n--;  // Decrement packet counter
    }
    
    return 0;
}
