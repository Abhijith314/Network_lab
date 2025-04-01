#include <stdio.h>

int main() {
    int incoming, outgoing, buck_size, n, store = 0; // Declare variables for incoming packet size, outgoing rate, bucket size, number of packets, and current buffer size

    // Prompt the user to enter bucket size, outgoing rate, and number of packets
    printf("Enter the bucket size (Bytes): ");
    scanf("%d", &buck_size);
    
    printf("Enter the outgoing rate (Bytes per second): ");
    scanf("%d", &outgoing);
    
    printf("Enter the number of packets: ");
    scanf("%d", &n);
    
    printf("-----------\n"); // Divider for clarity
    
    // Loop through each packet
    while(n != 0) {
        // Prompt the user to enter the size of the incoming packet
        printf("Enter the incoming packet size (Bytes): ");
        scanf("%d", &incoming);
        
        // Check if the incoming packet can be accommodated in the buffer
        if(incoming <= (buck_size - store)) {
            // If yes, add the packet to the buffer
            store += incoming;
            printf("Current Bucket buffer size %d out of %d\n", store, buck_size);
        } else {
            // If not, drop the excess data and update buffer size
            printf("Dropped %d bytes of data from Packet\n", incoming - (buck_size - store));
            printf("Current Bucket buffer size %d out of %d\n", buck_size - store, buck_size);
            store = buck_size;
        }
        
        // Simulate outgoing data and update buffer size
        store = store - outgoing;
        
        // Ensure buffer size doesn't go negative
        if (store < 0) {
            store = 0;
        }
        
        // Display the remaining buffer size after outgoing data
        printf("After outgoing %d bytes left out of %d in buffer\n", store, buck_size);
        printf("----------------------------\n"); // Divider for clarity
        n--; // Decrement the number of packets
    }
    
    return 0;
}


// Output
// Enter the bucket size (Bytes): 100
// Enter the outgoing rate (Bytes per second): 50
// Enter the no of packets: 5
// Enter the incoming packet size (Bytes): 50
// Current Bucket buffer size 50 out of 100
// After outgoing 0 bytes left out of 100 in buffer
// Enter the incoming packet size (Bytes): 200
// Dropped 100 bytes of data from packet
// Current Bucket buffer size 100 out of 100
// After outgoing 50 bytes left out of 100 in buffer
// Enter the incoming packet size (Bytes): 50
// Current Bucket buffer size 100 out of 100
// After outgoing 50 bytes left out of 100 in buffer
// Enter the incoming packet size (Bytes): 20
// Current Bucket buffer size 70 out of 100
// After outgoing 20 bytes left out of 100 in buffer
// Enter the incoming packet size (Bytes): 10
// Current Bucket buffer size 30 out of 100
// After outgoing 0 bytes left out of 100 in buffer
