#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MAX_BUFFER_SIZE 1024

typedef struct
{
    int sockfd;
    SSL_CTX *ctx;
    SSL *ssl;
} Server;

Server server;

void server_init()
{
    // Create a UDP socket
    server.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server.sockfd < 0)
    {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(12345);

    // Bind the socket to the server address
    if (bind(server.sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind socket");
        close(server.sockfd);
        exit(EXIT_FAILURE);
    }

    // Set up DTLS context
    server.ctx = SSL_CTX_new(DTLS_server_method());
    if (server.ctx == NULL)
    {
        perror("Failed to create DTLS context");
        close(server.sockfd);
        exit(EXIT_FAILURE);
    }

    // Load server certificate and private key
    if (SSL_CTX_use_certificate_file(server.ctx, "server.crt", SSL_FILETYPE_PEM) <= 0)
    {
        perror("Failed to load server certificate");
        SSL_CTX_free(server.ctx);
        close(server.sockfd);
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(server.ctx, "server.key", SSL_FILETYPE_PEM) <= 0)
    {
        perror("Failed to load server private key");
        SSL_CTX_free(server.ctx);
        close(server.sockfd);
        exit(EXIT_FAILURE);
    }

    // Set up DTLS session
    server.ssl = SSL_new(server.ctx);
    if (server.ssl == NULL)
    {
        perror("Failed to create DTLS session");
        SSL_CTX_free(server.ctx);
        close(server.sockfd);
        exit(EXIT_FAILURE);
    }
    SSL_set_fd(server.ssl, server.sockfd);
}

void server_cleanup()
{
    // Clean up
    SSL_shutdown(server.ssl);
    SSL_free(server.ssl);
    SSL_CTX_free(server.ctx);
    close(server.sockfd);
}

// void run() {
//     // Initialize
//     init();

//     // Start listening for incoming connections
//     while (1) {
//         struct sockaddr_in client_addr;
//         socklen_t client_addr_len = sizeof(client_addr);
//         char buffer[MAX_BUFFER_SIZE];

//         // Receive data from client
//         ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);
//         if (recv_len < 0) {
//             perror("Failed to receive data");
//             break;
//         }

//         // Process received data
//         // ...

//         // Send response to client
//         ssize_t send_len = sendto(sockfd, buffer, recv_len, 0, (struct sockaddr*)&client_addr, client_addr_len);
//         if (send_len < 0) {
//             perror("Failed to send data");
//             break;
//         }
//     }

//     // Cleanup
//     cleanup();
// }
