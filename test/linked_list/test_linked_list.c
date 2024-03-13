#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/utils.h"
#include "common/linked_list.h"

static void test_create_list(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));
    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_clear_list(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    linked_list_clear(list, NULL);
    assert_true(linked_list_is_empty(list));
    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_push_front(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_push_front(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_push_front(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(list->start), &data2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data1);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_append(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_remove(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    link_t *link = list->start;
    assert_true(linked_list_remove(list, &link, NULL));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data2);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_remove_the_other(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    link_t *link = linked_list_get_last(list);
    assert_true(linked_list_remove(list, &link, NULL));
    assert_int_equal(list->size, 1);

    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data1);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_pop(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    int *pop_data = (int *)linked_list_pop(list);
    assert_ptr_equal(pop_data, &data1);
    assert_int_equal(list->size, 1);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_for_each_link(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2;
    assert_true(linked_list_append(list, &data1));
    assert_int_equal(list->size, 1);
    assert_ptr_equal(link_get_data(list->start), &data1);

    assert_true(linked_list_append(list, &data2));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data2);

    int sum = 0;
    for_each_link(link, list)
    {
        sum += *(int *)link_get_data(link);
    }
    assert_int_equal(sum, 3);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_linked_list_remove(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2, data3 = 3;
    assert_true(linked_list_append(list, &data1));
    assert_true(linked_list_append(list, &data2));
    assert_true(linked_list_append(list, &data3));
    assert_int_equal(list->size, 3);

    link_t *link = list->start->next;
    assert_true(linked_list_remove(list, &link, NULL));
    assert_int_equal(list->size, 2);
    assert_ptr_equal(link_get_data(list->start), &data1);
    assert_ptr_equal(link_get_data(linked_list_get_last(list)), &data3);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

static void test_linked_list_get_last(void UNUSED **state)
{
    linked_list_t *list = linked_list_init();
    assert_non_null(list);
    assert_true(linked_list_is_empty(list));

    int data1 = 1, data2 = 2, data3 = 3;
    assert_true(linked_list_append(list, &data1));
    assert_true(linked_list_append(list, &data2));
    assert_true(linked_list_append(list, &data3));
    assert_int_equal(list->size, 3);

    link_t *last = linked_list_get_last(list);
    assert_ptr_equal(link_get_data(last), &data3);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_list),
        cmocka_unit_test(test_clear_list),
        cmocka_unit_test(test_push_front),
        cmocka_unit_test(test_append),
        cmocka_unit_test(test_remove),
        cmocka_unit_test(test_remove_the_other),
        cmocka_unit_test(test_pop),
        cmocka_unit_test(test_for_each_link),
        cmocka_unit_test(test_linked_list_remove),
        cmocka_unit_test(test_linked_list_get_last),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}