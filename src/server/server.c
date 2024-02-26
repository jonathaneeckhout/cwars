#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "server/server.h"
#include <openssl/err.h>

#define MAX_BUFFER_SIZE 1024

Server *server_init(const char *key_path, const char *cert_path)
{
    Server *server = malloc(sizeof(Server));
    if (server == NULL)
    {
        perror("Failed to allocate memory for server");
        return NULL;
    }

    // Create a UDP socket
    server->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server->sockfd < 0)
    {
        perror("Failed to create socket");
        free(server); // Free the allocated memory
        return NULL;
    }

    // Set up server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(12345);

    // Bind the socket to the server address
    if (bind(server->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Failed to bind socket");
        close(server->sockfd);
        free(server); // Free the allocated memory
        return NULL;
    }

    // Set up DTLS context
    server->ctx = SSL_CTX_new(DTLS_server_method());
    if (server->ctx == NULL)
    {
        perror("Failed to create DTLS context");
        close(server->sockfd);
        free(server); // Free the allocated memory
        return NULL;
    }

    // Load server certificate and private key
    if (SSL_CTX_use_certificate_file(server->ctx, cert_path, SSL_FILETYPE_PEM) != 1)
    {
        ERR_print_errors_fp(stderr);
        perror("Failed to load server certificate");
        SSL_CTX_free(server->ctx);
        close(server->sockfd);
        free(server); // Free the allocated memory
        return NULL;
    }
    if (SSL_CTX_use_PrivateKey_file(server->ctx, key_path, SSL_FILETYPE_PEM) != 1)
    {
        ERR_print_errors_fp(stderr);
        perror("Failed to load server private key");
        SSL_CTX_free(server->ctx);
        close(server->sockfd);
        free(server); // Free the allocated memory
        return NULL;
    }

    // Set up DTLS session
    server->ssl = SSL_new(server->ctx);
    if (server->ssl == NULL)
    {
        perror("Failed to create DTLS session");
        SSL_CTX_free(server->ctx);
        close(server->sockfd);
        free(server); // Free the allocated memory
        return NULL;
    }
    SSL_set_fd(server->ssl, server->sockfd);

    return server;
}

void server_cleanup(Server **server)
{
    // Clean up
    SSL_shutdown((*server)->ssl);
    SSL_free((*server)->ssl);
    SSL_CTX_free((*server)->ctx);
    close((*server)->sockfd);
    free(*server);
    *server = NULL;
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
