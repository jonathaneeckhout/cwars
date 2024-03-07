#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <endian.h>

#include "common/logging.h"
#include "common/message.h"

message_t *message_init()
{
    message_t *message = calloc(1, sizeof(message_t));
    if (message == NULL)
    {
        return NULL;
    }

    message->protocol_id = MESSAGE_PROTOCOL_ID;
    message->type = 0;
    message->length = 0;
    message->data = NULL;

    return message;
}

void message_cleanup(message_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    if ((*message)->data != NULL)
    {
        free((*message)->data);
    }

    free(*message);
    *message = NULL;
}

void message_serialize(message_t *message, char *buffer, uint32_t *length)
{
    uint32_t offset = 0;
    uint32_t protocol_id_network = 0;
    uint32_t length_network = 0;

    protocol_id_network = htonl(message->protocol_id);
    memcpy(buffer + offset, &protocol_id_network, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &message->type, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    length_network = htonl(message->length);
    memcpy(buffer + offset, &length_network, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    if (message->length > 0)
    {
        memcpy(buffer + offset, message->data, message->length);
        offset += message->length;
    }

    *length = offset;
}

message_t *message_deserialize(char *buffer, uint32_t length)
{
    uint32_t offset = 0;
    message_t *message = NULL;
    uint32_t protocol_id_network = 0;
    uint32_t length_network = 0;

    if (length < MESSAGE_HEADER_SIZE)
    {
        log_info("Not enough data for the message header");
        return NULL;
    }

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    memcpy(&protocol_id_network, buffer + offset, sizeof(uint32_t));
    message->protocol_id = ntohl(protocol_id_network);
    offset += sizeof(uint32_t);

    if (message->protocol_id != MESSAGE_PROTOCOL_ID)
    {
        log_info("Invalid protocol id");
        free(message);
        return NULL;
    }

    memcpy(&message->type, buffer + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    memcpy(&length_network, buffer + offset, sizeof(uint32_t));
    message->length = ntohl(length_network);
    offset += sizeof(uint32_t);

    if (message->length > 0)
    {
        if (length < offset + message->length)
        {
            // Not enough data for the variable-size field
            free(message);
            return NULL;
        }

        message->data = calloc(1, message->length);
        if (message->data == NULL)
        {
            free(message);
            return NULL;
        }

        memcpy(message->data, buffer + offset, message->length);
        offset += message->length;
    }

    return message;
}

void message_send_non_blocking(int sockfd, linked_list_t *message_queue)
{
    if (message_queue == NULL)
    {
        return;
    }

    while (!linked_list_is_empty(message_queue))
    {
        char buffer[MESSAGE_MAX_MTU];
        uint32_t length = 0;
        ssize_t bytes_sent = 0;

        message_t *message = linked_list_pop(message_queue);
        if (message == NULL)
        {
            return;
        }

        memset(buffer, 0, sizeof(buffer));

        message_serialize(message, buffer, &length);

        bytes_sent = send(sockfd, buffer, length, MSG_DONTWAIT);
        if (bytes_sent < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // The socket is non-blocking and the send would block,
                // so re-queue the message and try again later.
                linked_list_push_front(message_queue, message);
                break;
            }
            else
            {
                // An actual error occurred
                log_error("Failed to send message");
                message_cleanup(&message);
                break;
            }
        }
        else
        {
            message_cleanup(&message);
        }
    }
}

void message_read_non_blocking(int sockfd, linked_list_t *message_queue, unsigned int max_messages_read, bool *connected)
{

    char buffer[MESSAGE_MAX_MTU];
    message_t *message = NULL;

    for (unsigned int i = 0; i < max_messages_read; i++)
    {
        ssize_t bytes_received = 0;
        ssize_t bytes_handled = 0;

        memset(buffer, 0, sizeof(buffer));

        bytes_received = recv(sockfd, buffer, sizeof(buffer), MSG_DONTWAIT);
        if (bytes_received < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No data available
                return;
            }
            else
            {
                log_error("Failed to receive message");
                // Something went wrong, better close the connection
                if (connected != NULL)
                {
                    *connected = false;
                }
                return;
            }
        }
        else if (bytes_received == 0)
        {
            log_info("Peer disconnected");
            if (connected != NULL)
            {
                *connected = false;
            }
            return;
        }

        if (message_queue == NULL)
        {
            continue;
        }

        while (bytes_handled < bytes_received)
        {
            message = message_deserialize(buffer + bytes_handled, bytes_received - bytes_handled);
            if (message == NULL)
            {
                log_error("Failed to deserialize message");
                break;
            }

            linked_list_append(message_queue, message);

            bytes_handled += MESSAGE_HEADER_SIZE + message->length;
        }
    }
}

message_t *message_init_ping()
{
    message_t *message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_PING;

    return message;
}

message_t *message_init_pong()
{
    message_t *message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_PONG;

    return message;
}

message_t *message_init_get_server_time(int64_t client_time)
{
    message_t *message = NULL;
    int64_t network_client_time = 0;

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_GET_SERVER_TIME;
    message->length = sizeof(int64_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_client_time = htobe64(client_time);
    memcpy(message->data, &network_client_time, message->length);

    return message;
}

message_get_server_time_response_t *message_get_server_time_response_deserialize(message_t *message)
{
    message_get_server_time_response_t *response = NULL;
    int64_t network_client_time = 0;

    if (message == NULL || message->type != MESSAGE_TYPE_GET_SERVER_TIME || message->length != sizeof(int64_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_get_server_time_response_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_client_time, message->data, sizeof(int64_t));
    response->client_time = be64toh(network_client_time);

    return response;
}

void message_get_server_time_response_cleanup(message_get_server_time_response_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    free(*message);
    *message = NULL;
}

message_t *message_init_return_server_time(int64_t server_time, int64_t client_time)
{
    message_t *message = NULL;
    int64_t network_server_time = 0;
    int64_t network_client_time = 0;

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_RETURN_SERVER_TIME;
    message->length = 2 * sizeof(int64_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_server_time = htobe64(server_time);
    memcpy(message->data, &network_server_time, sizeof(int64_t));

    network_client_time = htobe64(client_time);
    memcpy(message->data + sizeof(int64_t), &network_client_time, sizeof(int64_t));

    return message;
}

message_return_server_time_response_t *message_return_server_time_response_deserialize(message_t *message)
{
    message_return_server_time_response_t *response = NULL;
    int64_t network_server_time = 0;
    int64_t network_client_time = 0;

    if (message == NULL || message->type != MESSAGE_TYPE_RETURN_SERVER_TIME || message->length != 2 * sizeof(int64_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_return_server_time_response_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_server_time, message->data, sizeof(int64_t));
    response->server_time = be64toh(network_server_time);

    memcpy(&network_client_time, message->data + sizeof(int64_t), sizeof(int64_t));
    response->client_time = be64toh(network_client_time);

    return response;
}

void message_return_server_time_response_cleanup(message_return_server_time_response_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    free(*message);
    *message = NULL;
}

message_t *message_init_get_latency(int64_t client_time)
{
    message_t *message = NULL;
    int64_t network_client_time = 0;

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_GET_LATENCY;
    message->length = sizeof(int64_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_client_time = htobe64(client_time);
    memcpy(message->data, &network_client_time, message->length);

    return message;
}

message_get_latency_response_t *message_get_latency_response_deserialize(message_t *message)
{
    message_get_latency_response_t *response = NULL;
    int64_t network_client_time = 0;

    if (message == NULL || message->type != MESSAGE_TYPE_GET_LATENCY || message->length != sizeof(int64_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_get_latency_response_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_client_time, message->data, sizeof(int64_t));
    response->client_time = be64toh(network_client_time);

    return response;
}

void message_get_latency_response_cleanup(message_get_latency_response_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    free(*message);
    *message = NULL;
}

message_t *message_init_return_latency(int64_t client_time)
{
    message_t *message = NULL;
    int64_t network_client_time = 0;

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_RETURN_LATENCY;
    message->length = sizeof(int64_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_client_time = htobe64(client_time);
    memcpy(message->data, &network_client_time, message->length);

    return message;
}

message_return_latency_response_t *message_return_latency_response_deserialize(message_t *message)
{
    message_return_latency_response_t *response = NULL;
    int64_t network_client_time = 0;

    if (message == NULL || message->type != MESSAGE_TYPE_RETURN_LATENCY || message->length != sizeof(int64_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_return_latency_response_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_client_time, message->data, sizeof(int64_t));
    response->client_time = be64toh(network_client_time);

    return response;
}

void message_return_latency_response_cleanup(message_return_latency_response_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    free(*message);
    *message = NULL;
}

message_t *message_init_get_entities()
{
    message_t *message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_GET_ENTITIES;

    return message;
}

void message_get_entities_cleanup(message_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    message_cleanup(message);
}

message_t *message_init_return_entities(int64_t timestamp, uint32_t entity_count, entity_t *entities)
{
    message_t *message = NULL;
    int64_t network_timestamp = 0;
    uint32_t network_entity_count = 0;

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_RETURN_ENTITIES;
    message->length = sizeof(int64_t) + sizeof(uint32_t) + entity_count * sizeof(entity_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_timestamp = htobe64(timestamp);
    memcpy(message->data, &network_timestamp, sizeof(int64_t));

    network_entity_count = htonl(entity_count);
    memcpy(message->data + sizeof(int64_t), &network_entity_count, sizeof(uint32_t));

    memcpy(message->data + sizeof(int64_t) + sizeof(uint32_t), entities, entity_count * sizeof(entity_t));

    return message;
}

message_get_entities_response_t *message_return_entities_response_deserialize(message_t *message)
{
    message_get_entities_response_t *response = NULL;
    int64_t network_timestamp = 0;
    uint32_t network_entity_count = 0;

    if (message == NULL || message->type != MESSAGE_TYPE_RETURN_ENTITIES || message->length < sizeof(int64_t) + sizeof(uint32_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_get_entities_response_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_timestamp, message->data, sizeof(int64_t));
    response->timestamp = be64toh(network_timestamp);

    memcpy(&network_entity_count, message->data + sizeof(int64_t), sizeof(uint32_t));
    response->entity_count = ntohl(network_entity_count);

    if (message->length != sizeof(int64_t) + sizeof(uint32_t) + response->entity_count * sizeof(entity_t))
    {
        free(response);
        return NULL;
    }

    response->entities = calloc(response->entity_count, sizeof(entity_t));
    if (response->entities == NULL)
    {
        free(response);
        return NULL;
    }

    memcpy(response->entities, message->data + sizeof(int64_t) + sizeof(uint32_t), response->entity_count * sizeof(entity_t));

    return response;
}

void message_return_entities_response_cleanup(message_get_entities_response_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    if ((*message)->entities != NULL)
    {
        free((*message)->entities);
    }

    free(*message);
    *message = NULL;
}

message_t *message_init_create_entity(vector_t position)
{
    message_t *message = NULL;
    vector_t network_position = {0};

    message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_CREATE_ENTITY;
    message->length = sizeof(vector_t);
    message->data = calloc(1, message->length);
    if (message->data == NULL)
    {
        message_cleanup(&message);
        return NULL;
    }

    network_position.x = htobe64(position.x);
    network_position.y = htobe64(position.y);
    memcpy(message->data, &network_position, message->length);

    return message;
}

message_create_entity_t *message_create_entity_deserialize(message_t *message)
{
    message_create_entity_t *response = NULL;
    vector_t network_position = {0};

    if (message == NULL || message->type != MESSAGE_TYPE_CREATE_ENTITY || message->length != sizeof(vector_t))
    {
        return NULL;
    }

    response = calloc(1, sizeof(message_create_entity_t));
    if (response == NULL)
    {
        return NULL;
    }

    memcpy(&network_position, message->data, sizeof(vector_t));
    response->position.x = be64toh(network_position.x);
    response->position.y = be64toh(network_position.y);

    return response;
}

void message_create_entity_cleanup(message_create_entity_t **message)
{
    if (message == NULL || *message == NULL)
    {
        return;
    }

    free(*message);
    *message = NULL;
}
