#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h> // for threading

#define PORT 65432
#define ENCODING "UTF-8" // Specify encoding (e.g., UTF-8 or ASCII)
#define BUFFER_SIZE 1024
#define AUTO_MESSAGE "Server received your message!" // Pre-defined message

pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for buffer access

void *receive_messages(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int expected_length; // Store expected message length (optional)

    while (1) {


        // Receive message content
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received == -1) {
            perror("Error receiving data");
            break;
        } else if (bytes_received == 0) {
            printf("Client disconnected.\n");
            break;
        }
        buffer[bytes_received] = '\0';

        pthread_mutex_lock(&buffer_mutex); // Lock mutex before accessing buffer
        printf("Client: %s\n", buffer);

        // Send automatic message to client
        if (send(client_socket, AUTO_MESSAGE, strlen(AUTO_MESSAGE), 0) == -1) {
            perror("Error sending message");
            break;
        }

        pthread_mutex_unlock(&buffer_mutex); // Unlock mutex after accessing buffer
    }

    close(client_socket); // Close the socket in the receive thread (optional)
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[BUFFER_SIZE];
    int addrlen = sizeof(client_address);
    pthread_t receive_thread;

    // Create a TCP socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // Bind the socket to the address and port
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(1);
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(1);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept a connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t*)&addrlen);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connected by %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        // Create a thread to receive messages from the client
        if (pthread_create(&receive_thread, NULL, receive_messages, (void *)&client_socket) != 0) {
            perror("Error creating thread");
            exit(1);
        }

        if (pthread_detach(receive_thread) != 0) {
            perror("Error detaching thread");
            exit(1);
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
