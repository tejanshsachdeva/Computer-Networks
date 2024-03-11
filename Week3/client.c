#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> // for threading

#define PORT 65432
#define ENCODING "UTF-8" // Specify encoding (e.g., UTF-8 or ASCII)

void *receive_messages(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[1024];

    while (1) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            exit(1);
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        }


        printf("Server: %s\n", buffer);
    }

    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char message[1024];
    pthread_t receive_thread;

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

    // Create a thread to receive messages from the server
    if (pthread_create(&receive_thread, NULL, receive_messages, (void *)&client_socket) != 0) {
        perror("Error creating thread");
        exit(1);
    }

    while (1) {
        fgets(message, sizeof(message), stdin); // Read message from user
        message[strcspn(message, "\n")] = '\0'; // Remove newline character

        // Send message to server
        if (send(client_socket, message, strlen(message), 0) == -1) {
            perror("Error sending message");
            exit(1);
        }
        printf("You: %s\n", message);
    }

    // Wait for receive thread to finish (optional)
    pthread_join(receive_thread, NULL);

    // Close the connection
    close(client_socket);

    return 0;
}
