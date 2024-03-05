#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/utils.h"

static void test_get_time(void UNUSED **state)
{
    int64_t time = get_time();
    assert_true(time > 0);
}

static void test_generate_unique_id(void UNUSED **state)
{
    int32_t id_1 = generate_unique_id();
    int32_t id_2 = generate_unique_id();

    assert_false(id_1 == id_2);
}

static void test_compare_int64(void UNUSED **state)
{
    int64_t a = 1, b = 2;
    int result = compare_int64(&a, &b);
    assert_true(result < 0);

    a = 2;
    b = 2;
    result = compare_int64(&a, &b);
    assert_true(result == 0);

    a = 2;
    b = 1;
    result = compare_int64(&a, &b);
    assert_true(result > 0);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_get_time),
        cmocka_unit_test(test_generate_unique_id),
        cmocka_unit_test(test_compare_int64),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}