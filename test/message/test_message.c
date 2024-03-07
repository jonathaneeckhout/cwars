#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/utils.h"
#include "common/vector.h"
#include "common/message.h"

static void test_message_init(void UNUSED **state)
{
    message_t *message = message_init();
    assert_non_null(message);
    message_cleanup(&message);
}

static void test_message_cleanup(void UNUSED **state)
{
    message_t *message = message_init();
    message_cleanup(&message);
    assert_null(message);
}

static void test_message_serialize(void UNUSED **state)
{
    message_t *message = message_init();
    message->protocol_id = MESSAGE_PROTOCOL_ID;
    message->type = MESSAGE_TYPE_PING;
    message->length = 0;
    message->data = NULL;

    char buffer[MESSAGE_HEADER_SIZE];
    uint32_t buffer_length = MESSAGE_HEADER_SIZE;
    message_serialize(message, buffer, &buffer_length);

    assert_int_equal(buffer_length, MESSAGE_HEADER_SIZE);
    assert_int_equal(buffer[0] & 0xFF, (MESSAGE_PROTOCOL_ID >> 24) & 0xFF);
    assert_int_equal(buffer[1] & 0xFF, (MESSAGE_PROTOCOL_ID >> 16) & 0xFF);
    assert_int_equal(buffer[2] & 0xFF, (MESSAGE_PROTOCOL_ID >> 8) & 0xFF);
    assert_int_equal(buffer[3] & 0xFF, MESSAGE_PROTOCOL_ID & 0xFF);
    assert_int_equal(buffer[4] & 0xFF, MESSAGE_TYPE_PING);
    assert_int_equal(buffer[5] & 0xFF, 0);
    assert_int_equal(buffer[6] & 0xFF, 0);
    assert_int_equal(buffer[7] & 0xFF, 0);

    message_cleanup(&message);
}

static void test_message_deserialize(void UNUSED **state)
{
    message_t *message = message_init();
    message->protocol_id = MESSAGE_PROTOCOL_ID;
    message->type = MESSAGE_TYPE_PING;
    message->length = 0;
    message->data = NULL;

    char buffer[MESSAGE_HEADER_SIZE];
    memset(buffer, 0, MESSAGE_HEADER_SIZE);
    uint32_t buffer_length = MESSAGE_HEADER_SIZE;
    message_serialize(message, buffer, &buffer_length);

    message_t *deserialized_message = message_deserialize(buffer, buffer_length);
    assert_non_null(deserialized_message);

    assert_int_equal(deserialized_message->protocol_id, MESSAGE_PROTOCOL_ID);
    assert_true(deserialized_message->type == MESSAGE_TYPE_PING);
    assert_int_equal(deserialized_message->length, 0);
    assert_null(deserialized_message->data);

    message_cleanup(&message);
    message_cleanup(&deserialized_message);
}

static void test_message_deserialize_invalid(void UNUSED **state)
{
    char buffer[MESSAGE_HEADER_SIZE];
    memset(buffer, 0, MESSAGE_HEADER_SIZE);
    uint32_t buffer_length = MESSAGE_HEADER_SIZE;

    message_t *deserialized_message = message_deserialize(buffer, buffer_length);
    assert_null(deserialized_message);
}

static void test_message_deserialize_invalid_protocol_id(void UNUSED **state)
{
    message_t *message = message_init();
    message->protocol_id = MESSAGE_PROTOCOL_ID;
    message->type = MESSAGE_TYPE_PING;
    message->length = 0;
    message->data = NULL;

    char buffer[MESSAGE_HEADER_SIZE];
    memset(buffer, 0, MESSAGE_HEADER_SIZE);
    uint32_t buffer_length = MESSAGE_HEADER_SIZE;
    message_serialize(message, buffer, &buffer_length);

    buffer[0] = 0;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;

    message_t *deserialized_message = message_deserialize(buffer, buffer_length);
    assert_null(deserialized_message);

    message_cleanup(&message);
}

static void test_message_desirialize_too_short(void UNUSED **state)
{
    char buffer[MESSAGE_HEADER_SIZE - 1];
    memset(buffer, 0, MESSAGE_HEADER_SIZE - 1);
    uint32_t buffer_length = MESSAGE_HEADER_SIZE - 1;

    message_t *deserialized_message = message_deserialize(buffer, buffer_length);
    assert_null(deserialized_message);
}

static void test_message_init_return_entities(void UNUSED **state)
{
    message_t *message = message_init_return_entities(0, 0, NULL);
    assert_non_null(message);

    message_cleanup(&message);
    assert_null(message);

    message_t *message2 = message_init_return_entities(0, 1, NULL);
    assert_null(message2);

    entity_t entities_array[2];
    memset(entities_array, 0, sizeof(entities_array));

    entity_t *entity_0 = entity_init(vector_create(1, 2), vector_create(3, 4), 5);
    entity_t *entity_1 = entity_init(vector_create(6, 7), vector_create(8, 9), 10);

    entities_array[0] = *entity_0;
    entities_array[1] = *entity_1;

    message_t *message3 = message_init_return_entities(0, 2, entities_array);
    assert_non_null(message3);

    message_cleanup(&message3);

    entity_cleanup(&entity_0);
    entity_cleanup(&entity_1);
}

static void test_message_return_entities_response_deserialize(void UNUSED **state)
{
    entity_t entities_array[2];
    memset(entities_array, 0, sizeof(entities_array));

    entity_t *entity_0 = entity_init(vector_create(1, 2), vector_create(3, 4), 5);
    entity_t *entity_1 = entity_init(vector_create(6, 7), vector_create(8, 9), 10);

    entities_array[0] = *entity_0;
    entities_array[1] = *entity_1;

    message_t *message = message_init_return_entities(0, 2, entities_array);
    assert_non_null(message);

    char buffer[MESSAGE_MAX_MTU];
    uint32_t buffer_length = MESSAGE_MAX_MTU;
    message_serialize(message, buffer, &buffer_length);

    message_t *deserialized_message = message_deserialize(buffer, buffer_length);
    assert_non_null(deserialized_message);

    assert_int_equal(deserialized_message->protocol_id, MESSAGE_PROTOCOL_ID);
    assert_true(deserialized_message->type == MESSAGE_TYPE_RETURN_ENTITIES);
    assert_int_equal(deserialized_message->length, buffer_length - MESSAGE_HEADER_SIZE);
    assert_memory_equal(deserialized_message->data, buffer + MESSAGE_HEADER_SIZE, deserialized_message->length);

    message_get_entities_response_t* response = message_return_entities_response_deserialize(deserialized_message);
    assert_non_null(response);
    assert_int_equal(response->timestamp, 0);
    assert_int_equal(response->entity_count, 2);
    assert_memory_equal(response->entities, entities_array, sizeof(entities_array));

    message_cleanup(&message);
    message_cleanup(&deserialized_message);
    entity_cleanup(&entity_0);
    entity_cleanup(&entity_1);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_message_init),
        cmocka_unit_test(test_message_cleanup),
        cmocka_unit_test(test_message_serialize),
        cmocka_unit_test(test_message_deserialize),
        cmocka_unit_test(test_message_deserialize_invalid),
        cmocka_unit_test(test_message_deserialize_invalid_protocol_id),
        cmocka_unit_test(test_message_desirialize_too_short),
        cmocka_unit_test(test_message_init_return_entities),
        cmocka_unit_test(test_message_return_entities_response_deserialize),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}