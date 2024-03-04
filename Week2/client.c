#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h> // for close
#include <string.h>
#define ENCODING "UTF-8" // Specify encoding (e.g., UTF-8 or ASCII)

#define WINDOW_SIZE 5

int main()
{
    int sockid;
    int connectid;
    struct sockaddr_in servaddr;
    struct sockaddr_in client;
    int newsockid;
    int clientlen;
    int n;
    int msg_count = 0; // Track the message count
    char msg[1000];
    char ack[1000];
    int port_id = 6000;

    sockid = socket(AF_INET, SOCK_STREAM, 0);

    bzero((char*)&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port_id);
    connectid = connect(sockid, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));

    if (connectid < 0) {
        printf("error \n");
        return 1;
    }

    // Send first 5 messages
    for (int i = 0; i < WINDOW_SIZE; ++i) {
        printf("Enter message: ");
        fgets(msg, sizeof(msg), stdin);
        send(sockid, msg, strlen(msg), 0);
        msg_count++;
    }

    // Receive acknowledgment for the first 5 messages
    recv(sockid, ack, sizeof(ack), 0);
    printf("Received acknowledgment: %s\n", ack);

    while (1) {
        // Check if the client has sent a message per received acknowledgment
        //if (msg_count % WINDOW_SIZE == 0) {
            // Wait for acknowledgment before sending the next message
            printf("Enter message: ");
            fgets(msg, sizeof(msg), stdin);
            send(sockid, msg, strlen(msg), 0);
            msg_count++;
            // Receive acknowledgment for the current message
            recv(sockid, ack, sizeof(ack), 0);
            printf("Received acknowledgment: %s\n", ack);
        //} else {
            // Send messages without waiting for acknowledgment
            //printf("Enter message: ");
            //fgets(msg, sizeof(msg), stdin);
            //send(sockid, msg, strlen(msg), 0);
            //msg_count++;
        //}
    }

    close(sockid);

    return (0);
}
