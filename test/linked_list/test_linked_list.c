#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/utils.h"
#include "common/linked_list.h"

static void test_linked_list(void UNUSED **state)
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
    assert_ptr_equal(link_get_data(list->end), &data2);

    int *pop_data = (int *)linked_list_pop(list);
    assert_ptr_equal(pop_data, &data1);
    assert_int_equal(list->size, 1);

    linked_list_cleanup(&list, NULL);
    assert_null(list);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_linked_list),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}