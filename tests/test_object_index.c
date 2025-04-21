#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "test_object.h"

static lua_State *state = NULL;
static TestStruct test_struct;

void setup(void) {
    state = luaL_newstate();
    init_test_struct(&test_struct);
    define_test_struct(state);
    LUAS_OBJECT(state, TestStruct, &test_struct, false);
}

void teardown(void) {
    lua_gc(state, LUA_GCCOLLECT, 0);
    lua_close(state);
    state = NULL;
}

#define TEST_INDEX_INT(type, min_val, max_val)                               \
    START_TEST(test_index_##type##_min) {                                    \
        test_struct.type = min_val;                                          \
        lua_getfield(state, -1, #type);                                      \
        type##_t value = luaL_checkinteger(state, -1);                       \
        ck_assert_int_eq(value, min_val);                                    \
        lua_pop(state, 1);                                                   \
    }                                                                        \
    END_TEST                                                                 \
                                                                             \
    START_TEST(test_index_##type##_max) {                                    \
        test_struct.type = max_val;                                          \
        lua_getfield(state, -1, #type);                                      \
        type##_t value = luaL_checkinteger(state, -1);                       \
        ck_assert_int_eq(value, max_val);                                    \
        lua_pop(state, 1);                                                   \
    }                                                                        \
    END_TEST

TEST_INDEX_INT(int32, INT32_MIN, INT32_MAX)
TEST_INDEX_INT(int16, INT16_MIN, INT16_MAX)
TEST_INDEX_INT(int8, INT8_MIN, INT8_MAX)
TEST_INDEX_INT(uint32, 0, UINT32_MAX)
TEST_INDEX_INT(uint16, 0, UINT16_MAX)
TEST_INDEX_INT(uint8, 0, UINT8_MAX)

START_TEST(test_index_float_precision) {
    float highPrecisionFloat = 3.14159265358979323846f;
    test_struct.number = highPrecisionFloat;
    lua_getfield(state, -1, "number");
    float value = luaL_checknumber(state, -1);
    ck_assert_float_eq(value, highPrecisionFloat);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_boolean) {
    lua_getfield(state, -1, "boolean");
    int value = lua_toboolean(state, -1);
    ck_assert_int_eq(value, test_struct.boolean);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_unknown_field) {
    lua_getfield(state, -1, "some_random_unexisting_field");
    ck_assert_msg(lua_isnil(state, -1), "Expected nil for unknown field");
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_object) {
    lua_getfield(state, -1, "sub_struct");
    LuastructStructObject *obj = lua_touserdata(state, -1);
    ck_assert_ptr_eq(obj->data, &test_struct.sub_struct);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_object_field) {
    test_struct.sub_struct.a = 42;
    lua_getfield(state, -1, "sub_struct");
    lua_getfield(state, -1, "a");
    int32_t value = luaL_checkinteger(state, -1);
    ck_assert_int_eq(value, 42);
    lua_pop(state, 2);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("object_index_metamethod");
    
    TCase *primitives = tcase_create("primitives");
    tcase_add_checked_fixture(primitives, setup, teardown);
    tcase_add_test(primitives, test_index_int32_max);
    tcase_add_test(primitives, test_index_int32_min);
    tcase_add_test(primitives, test_index_int16_max);
    tcase_add_test(primitives, test_index_int16_min);
    tcase_add_test(primitives, test_index_int8_max);
    tcase_add_test(primitives, test_index_int8_min);
    tcase_add_test(primitives, test_index_uint32_max);
    tcase_add_test(primitives, test_index_uint32_min);
    tcase_add_test(primitives, test_index_uint16_max);
    tcase_add_test(primitives, test_index_uint16_min);
    tcase_add_test(primitives, test_index_uint8_max);
    tcase_add_test(primitives, test_index_uint8_min);
    tcase_add_test(primitives, test_index_float_precision);
    tcase_add_test(primitives, test_index_boolean);
    suite_add_tcase(s, primitives);

    TCase *unknown_field = tcase_create("unknown_field");
    tcase_add_checked_fixture(unknown_field, setup, teardown);
    tcase_add_test(unknown_field, test_index_unknown_field);
    suite_add_tcase(s, unknown_field);

    TCase *objects = tcase_create("objects");
    tcase_add_checked_fixture(objects, setup, teardown);
    tcase_add_test(objects, test_index_object);
    tcase_add_test(objects, test_index_object_field);
    suite_add_tcase(s, objects);

    return s;
}

int main(int argc, char *argv[]) {
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);

    if(argc > 1) {
        srunner_run(sr, NULL, argv[1], CK_NORMAL);
    }
    else {
        srunner_run_all(sr, CK_NORMAL);
    }

    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
