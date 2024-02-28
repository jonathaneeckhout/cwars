#ifndef CONFIG_H
#define CONFIG_H

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 9876

#define CERTIFICATE_FILE "../crt/cert.pem"
#define PRIVATE_KEY_FILE "../crt/private_key.pem"

#define FPS 10
#define MICROSECONDS_PER_FRAME 1000000 / FPS

#define CLIENT_FPS 10
#define CLIENT_MICROSECONDS_PER_FRAME 1000000 / FPS
#define CLIENT_RECONNECT_TIME 3

#endif // CONFIG_H
