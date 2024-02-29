#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "common/logging.h"
#include "common/message.h"

message_t *message_init()
{
    message_t *message = malloc(sizeof(message_t));
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

    uint32_t protocol_id_network = htonl(message->protocol_id);
    memcpy(buffer + offset, &protocol_id_network, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(buffer + offset, &message->type, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    uint32_t length_network = htonl(message->length);
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

        message->data = malloc(message->length);
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

message_t *message_init_ping_response()
{
    message_t *message = message_init();
    if (message == NULL)
    {
        return NULL;
    }

    message->type = MESSAGE_TYPE_PING_RESPONSE;

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
        memset(buffer, 0, sizeof(buffer));

        ssize_t bytes_received = recv(sockfd, buffer, sizeof(buffer), MSG_DONTWAIT);
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

        message = message_deserialize(buffer, bytes_received);
        if (message == NULL)
        {
            log_error("Failed to deserialize message");
            continue;
        }

        linked_list_append(message_queue, message);
    }
}