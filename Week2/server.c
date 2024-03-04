#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h> // for close
#define ENCODING "UTF-8" // Specify encoding (e.g., UTF-8 or ASCII)

#define WINDOW_SIZE 5

int main()
{
    int sockid;
    int bindid;
    struct sockaddr_in myaddr;
    struct sockaddr_in client;
    int newsockid;
    int clientlen;
    int n;
    int msg_count = 0; // Track the message count
    char msg[1000];
    char ack[1000];
    int port_id = 6000;

    sockid = socket(AF_INET, SOCK_STREAM, 0);

    bzero((char*)&myaddr, sizeof(struct sockaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port_id);
    bindid = bind(sockid, (struct sockaddr*)&myaddr, sizeof(struct sockaddr_in));

    if (bindid < 0) {
        printf("error \n");
        return 1;
    }

    listen(sockid, 5);

    clientlen = sizeof(struct sockaddr_in);
    newsockid = accept(sockid, (struct sockaddr*)&client, &clientlen);
    if (newsockid < 0) {
        printf("error 2\n");
        return 1;
    }

    while (1) {
        if (msg_count % WINDOW_SIZE == 0 && msg_count > 0) {
            // Send acknowledgment when the window is full
            printf("Acknowledgement Sent\n");
            send(newsockid, "ACK", strlen("ACK"), 0);
        }

        if (recv(newsockid, msg, sizeof(msg), 0) > 0) {
            printf("Tejansh : %s\n", msg);
            msg_count++;
        }

        // Send acknowledgment for each message except the first five
        if (msg_count > WINDOW_SIZE && msg_count % WINDOW_SIZE != 0) {
            printf("ACK for %d\n", msg_count);
            send(newsockid, "ACK", strlen("ACK"), 0);
        }
    }

    close(newsockid);
    close(sockid);

    return 0;
}
