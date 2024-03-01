#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "common/linked_list.h"

#define MESSAGE_PROTOCOL_ID 0x8da685d9
#define MESSAGE_MAX_MTU 1200
#define MESSAGE_HEADER_SIZE (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t))

#define MESSAGE_TYPE_PING 0x01
#define MESSAGE_TYPE_PONG 0x02
#define MESSAGE_TYPE_GET_SERVER_TIME 0x03
#define MESSAGE_TYPE_RETURN_SERVER_TIME 0x04
#define MESSAGE_TYPE_GET_LATENCY 0x05
#define MESSAGE_TYPE_RETURN_LATENCY 0x06

typedef struct
{
    uint32_t protocol_id;
    uint8_t type;
    uint32_t length;
    char *data;
} message_t;

typedef struct
{
    int64_t client_time;
} message_get_server_time_response_t;

typedef struct
{
    int64_t server_time;
    int64_t client_time;
} message_return_server_time_response_t;

typedef struct
{
    int64_t client_time;
} message_get_latency_response_t;

typedef struct
{
    int64_t client_time;
} message_return_latency_response_t;

message_t *message_init();
void message_cleanup(message_t **message);
void message_serialize(message_t *message, char *buffer, uint32_t *length);
message_t *message_deserialize(char *buffer, uint32_t length);

void message_read_non_blocking(int sockfd, linked_list_t *message_queue, unsigned int max_messages_read, bool *connected);
void message_send_non_blocking(int sockfd, linked_list_t *message_queue);

message_t *message_init_ping();
message_t *message_init_pong();

message_t *message_init_get_server_time(int64_t client_time);
message_get_server_time_response_t *message_get_server_time_response_deserialize(message_t *message);
void message_get_server_time_response_cleanup(message_get_server_time_response_t **message);

message_t *message_init_return_server_time(int64_t server_time, int64_t client_time);
message_return_server_time_response_t *message_return_server_time_response_deserialize(message_t *message);
void message_return_server_time_response_cleanup(message_return_server_time_response_t **message);

message_t *message_init_get_latency(int64_t client_time);
message_get_latency_response_t *message_get_latency_response_deserialize(message_t *message);
void message_get_latency_response_cleanup(message_get_latency_response_t **message);

message_t *message_init_return_latency(int64_t client_time);
message_return_latency_response_t *message_return_latency_response_deserialize(message_t *message);
void message_return_latency_response_cleanup(message_return_latency_response_t **message);

#endif // MESSAGE_H
