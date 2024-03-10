#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "common/utils.h"
#include "common/vector.h"

static void test_vector_create(void UNUSED **state)
{
    vector_t vector = vector_create(fixed_from_float(5.0), fixed_from_float(10.0));
    assert_float_equal(fixed_to_float(vector.x), 5.0, 0.0001);
    assert_float_equal(fixed_to_float(vector.y), 10.0, 0.0001);
}

static void test_vector_add(void UNUSED **state)
{
    vector_t v1 = vector_create(fixed_from_float(5.0), fixed_from_float(10.0));
    vector_t v2 = vector_create(fixed_from_float(2.0), fixed_from_float(3.0));
    vector_t result = vector_add(v1, v2);
    assert_float_equal(fixed_to_float(result.x), 7.0, 0.0001);
    assert_float_equal(fixed_to_float(result.y), 13.0, 0.0001);
}

static void test_vector_subtract(void UNUSED **state)
{
    vector_t v1 = vector_create(fixed_from_float(5.0), fixed_from_float(10.0));
    vector_t v2 = vector_create(fixed_from_float(2.0), fixed_from_float(3.0));
    vector_t result = vector_subtract(v1, v2);
    assert_float_equal(fixed_to_float(result.x), 3.0, 0.0001);
    assert_float_equal(fixed_to_float(result.y), 7.0, 0.0001);
}

static void test_vector_multiply(void UNUSED **state)
{
    vector_t v = vector_create(fixed_from_float(5.0), fixed_from_float(10.0));
    vector_t result = vector_multiply(v, fixed_from_float(2.0));
    assert_float_equal(fixed_to_float(result.x), 10.0, 0.0001);
    assert_float_equal(fixed_to_float(result.y), 20.0, 0.0001);
}

static void test_vector_dot_product(void UNUSED **state)
{
    vector_t v1 = vector_create(fixed_from_float(5.0), fixed_from_float(10.0));
    vector_t v2 = vector_create(fixed_from_float(2.0), fixed_from_float(3.0));
    fixed_t result = vector_dot_product(v1, v2);
    assert_float_equal(fixed_to_float(result), 40.0, 0.0001);
}

static void test_vector_magnitude(void UNUSED **state)
{
    vector_t v = vector_create(fixed_from_float(3.0), fixed_from_float(4.0));
    fixed_t result = vector_magnitude(v);
    assert_float_equal(fixed_to_float(result), 5.0, 0.0001);
}

static void test_vector_normalize(void UNUSED **state)
{
    vector_t v = vector_create(fixed_from_float(3.0), fixed_from_float(4.0));
    vector_t result = vector_normalize(v);
    assert_float_equal(fixed_to_float(result.x), 0.6, 0.0001);
    assert_float_equal(fixed_to_float(result.y), 0.8, 0.0001);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_vector_create),
        cmocka_unit_test(test_vector_add),
        cmocka_unit_test(test_vector_subtract),
        cmocka_unit_test(test_vector_multiply),
        cmocka_unit_test(test_vector_dot_product),
        cmocka_unit_test(test_vector_magnitude),
        cmocka_unit_test(test_vector_normalize),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}