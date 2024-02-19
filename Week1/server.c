#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 65432

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[1024];
    int addrlen = sizeof(client_address);

    // Create a TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Bind the socket to the address and port
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;  //ipv4
    server_address.sin_addr.s_addr = INADDR_ANY;  //accept in any interface
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {  //checks binding
        perror("Error binding socket");
        exit(1);
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept a connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);  //blocks server until server connects
    if (client_socket == -1) {
        perror("Error accepting connection");
        exit(1);
    }

    printf("Connected by %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Receive data from client
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("Error receiving data");
        exit(1);
    }

    // Check if the message is "hello"
    if (strncmp(buffer, "hello", 5) == 0) {  //check if first 5 char is hello
        // Print received message
        printf("Received: %s\n", buffer);

        // Send ACK back to client
        if (send(client_socket, "ACK", 3, 0) == -1) {
            perror("Error sending ACK");
            exit(1);
        }
        printf("Sent ACK to client\n");
    } else {
        printf("Unexpected message: %s\n", buffer);
    }

    // Close connections
    close(client_socket);
    close(server_socket);

    return 0;
}
