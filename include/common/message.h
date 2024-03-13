#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

#include "common/linked_list.h"
#include "common/vector.h"
#include "common/entity.h"

#define MESSAGE_PROTOCOL_ID 0x8da685d9
#define MESSAGE_MAX_MTU 1200
#define MESSAGE_HEADER_SIZE (sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t))

#define MESSAGE_TYPE_PING 0x01
#define MESSAGE_TYPE_PONG 0x02
#define MESSAGE_TYPE_GET_SERVER_TIME 0x03
#define MESSAGE_TYPE_RETURN_SERVER_TIME 0x04
#define MESSAGE_TYPE_GET_LATENCY 0x05
#define MESSAGE_TYPE_RETURN_LATENCY 0x06
#define MESSAGE_TYPE_GET_ENTITIES 0x07
#define MESSAGE_TYPE_RETURN_ENTITIES 0x08
#define MESSAGE_TYPE_CREATE_ENTITY 0x09

typedef struct
{
    uint32_t protocol_id;
    uint8_t type;
    uint32_t length;
    char *data;
} message_t;

typedef struct
{
    uint32_t data_offset;
    message_t *message;

} incomming_message_t;

typedef struct
{
    uint32_t data_offset;
    uint32_t length;
    char *data;
} outgoing_message_t;

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

typedef struct
{
    int64_t timestamp;
    uint32_t entity_count;
    entity_t *entities;
} message_get_entities_response_t;

typedef struct
{
    vector_t position;
} message_create_entity_t;

message_t *message_init();
void message_cleanup(message_t **message);

incomming_message_t *incomming_message_init();
void incomming_message_cleanup(incomming_message_t **incomming_message);

void message_serialize(message_t *message, char *buffer, uint32_t *length);
message_t *message_deserialize(char *buffer, uint32_t length);

outgoing_message_t *outgoing_message_init();
void outgoing_message_cleanup(outgoing_message_t **outgoing_message);

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

message_t *message_init_get_entities();
void message_get_entities_cleanup(message_t **message);

message_t *message_init_return_entities(int64_t timestamp, uint32_t entity_count, entity_t *entities);
message_get_entities_response_t *message_return_entities_response_deserialize(message_t *message);
void message_return_entities_response_cleanup(message_get_entities_response_t **message);

message_t *message_init_create_entity(vector_t position);
message_create_entity_t *message_create_entity_deserialize(message_t *message);
void message_create_entity_cleanup(message_create_entity_t **message);

#endif // MESSAGE_H
