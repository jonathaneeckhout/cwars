#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <endian.h>

#include "common/logging.h"
#include "common/utils.h"
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

incomming_message_t *incomming_message_init()
{
    incomming_message_t *incomming_message = calloc(1, sizeof(incomming_message_t));
    if (incomming_message == NULL)
    {
        return NULL;
    }

    incomming_message->data_offset = 0;
    incomming_message->message = NULL;

    return incomming_message;
}

void incomming_message_cleanup(incomming_message_t **incomming_message)
{
    if (incomming_message == NULL || *incomming_message == NULL)
    {
        return;
    }

    if ((*incomming_message)->message != NULL)
    {
        message_cleanup(&(*incomming_message)->message);
    }

    free(*incomming_message);
    *incomming_message = NULL;
}

outgoing_message_t *outgoing_message_init()
{
    outgoing_message_t *outgoing_message = calloc(1, sizeof(outgoing_message_t));
    if (outgoing_message == NULL)
    {
        return NULL;
    }

    outgoing_message->data_offset = 0;
    outgoing_message->length = 0;
    outgoing_message->data = NULL;

    return outgoing_message;
}

void outgoing_message_cleanup(outgoing_message_t **outgoing_message)
{
    if (outgoing_message == NULL || *outgoing_message == NULL)
    {
        return;
    }

    if ((*outgoing_message)->data != NULL)
    {
        free((*outgoing_message)->data);
    }

    free(*outgoing_message);
    *outgoing_message = NULL;
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

        message->data = calloc(1, message->length);
        if (message->data == NULL)
        {
            free(message);
            return NULL;
        }

        if (length < offset + message->length)
        {
            // Not enough data for the variable-size field, don't try to read it
            return message;
        }

        memcpy(message->data, buffer + offset, message->length);
        offset += message->length;
    }

    return message;
}

bool message_write_output(int sockfd, linked_list_t *message_queue, outgoing_message_t **outgoing_message, bool *error)
{
    if (*outgoing_message == NULL)
    {

        if (linked_list_is_empty(message_queue))
        {
            return false;
        }

        message_t *message = linked_list_pop(message_queue);

        *outgoing_message = outgoing_message_init();
        if (*outgoing_message == NULL)
        {
            log_error("Failed to initialize outgoing message");
            return false;
        }

        (*outgoing_message)->length = message->length + MESSAGE_HEADER_SIZE;
        (*outgoing_message)->data = calloc(1, (*outgoing_message)->length);

        message_serialize(message, (*outgoing_message)->data, &(*outgoing_message)->length);

        message_cleanup(&message);
    }

    if (*outgoing_message != NULL)
    {
        ssize_t bytes_sent = send(sockfd, (*outgoing_message)->data + (*outgoing_message)->data_offset, (*outgoing_message)->length - (*outgoing_message)->data_offset, 0);
        if (bytes_sent < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                // No data available
                return false;
            }
            else
            {
                log_warning("Failed to send message, disconnecting client");
                if (error != NULL)
                {
                    *error = true;
                }
                return false;
            }
        }
        else
        {
            (*outgoing_message)->data_offset += bytes_sent;
            if ((*outgoing_message)->data_offset == (*outgoing_message)->length)
            {
                outgoing_message_cleanup(outgoing_message);
            }
        }
    }

    return true;
}

static bool message_handle_receive_error(ssize_t bytes_received, bool *error)
{
    if (bytes_received < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // No data available
            return false;
        }
        else
        {
            log_warning("Failed to receive message, disconnecting client");
            if (error != NULL)
            {
                *error = true;
            }
            return false;
        }
    }
    else if (bytes_received == 0)
    {
        log_info("Peer disconnected");
        if (error != NULL)
        {
            *error = true;
        }
        return false;
    }
    return true;
}

static bool message_read_message_header(int sockfd, incomming_message_t **incomming_message, bool *error)
{
    char header_buffer[MESSAGE_HEADER_SIZE];
    ssize_t bytes_received = recv(sockfd, header_buffer, sizeof(header_buffer), MSG_PEEK);

    if (!message_handle_receive_error(bytes_received, error))
    {
        return false;
    }

    if (bytes_received < (ssize_t)MESSAGE_HEADER_SIZE)
    {
        // Not enough data to read the header
        return false;
    }

    *incomming_message = incomming_message_init();
    memset(header_buffer, 0, sizeof(header_buffer));

    bytes_received = recv(sockfd, header_buffer, sizeof(header_buffer), 0);
    if (!message_handle_receive_error(bytes_received, error))
    {
        return false;
    }

    (*incomming_message)->message = message_deserialize(header_buffer, sizeof(header_buffer));
    if ((*incomming_message)->message == NULL)
    {
        log_warning("Failed to deserialize message, disconnecting client");
        if (error != NULL)
        {
            *error = true;
        }
        return false;
    }

    return true;
}

static bool message_read_message_data(int sockfd, linked_list_t *message_queue, incomming_message_t **incomming_message, bool *error)
{
    ssize_t bytes_received = recv(sockfd, (*incomming_message)->message->data + (*incomming_message)->data_offset, (*incomming_message)->message->length - (*incomming_message)->data_offset, 0);

    if (!message_handle_receive_error(bytes_received, error))
    {
        return false;
    }

    (*incomming_message)->data_offset += bytes_received;

    if ((*incomming_message)->data_offset == (*incomming_message)->message->length)
    {
        linked_list_append(message_queue, (*incomming_message)->message);
        (*incomming_message)->message = NULL;
        incomming_message_cleanup(incomming_message);
    }

    return true;
}

bool message_read_input(int sockfd, linked_list_t *message_queue, incomming_message_t **incomming_message, bool *error)
{
    if (*incomming_message == NULL)
    {
        if (!message_read_message_header(sockfd, incomming_message, error))
        {
            return false;
        }
    }

    if (*incomming_message != NULL)
    {
        if ((*incomming_message)->message->length == 0)
        {
            linked_list_append(message_queue, (*incomming_message)->message);
            (*incomming_message)->message = NULL;
            incomming_message_cleanup(incomming_message);
            return true;
        }

        if (!message_read_message_data(sockfd, message_queue, incomming_message, error))
        {
            return false;
        }
    }

    return true;
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

    if (entity_count > 0 && entities == NULL)
    {
        return NULL;
    }

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

    network_position.x = htonll(position.x);
    network_position.y = htonll(position.y);
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
    response->position.x = ntohll(network_position.x);
    response->position.y = ntohll(network_position.y);

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
