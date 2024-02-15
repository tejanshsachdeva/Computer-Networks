#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 65432

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[] = "hello";
    char buffer[1024];
    int bytes_received;

    // Create a TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Set up server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "localhost", &server_address.sin_addr);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    // Send "hello" message to server
    if (send(client_socket, message, strlen(message), 0) == -1) {
        perror("Error sending message");
        exit(1);
    }
    printf("Sent: %s\n", message); // Corrected printf statement

    // Receive response from server
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Error receiving data");
        exit(1);
    }

    // Check and print response
    if (strncmp(buffer, "ACK", 3) == 0) {
        printf("Received: %s\n", buffer);
    } else {
        printf("Unexpected message: %s\n", buffer);
    }

    // Close the connection
    close(client_socket);

    return 0;
}