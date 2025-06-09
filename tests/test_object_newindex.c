#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "debug.h"
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

#define TEST_NEWINDEX_INT(type, min_val, max_val)                                                       \
    START_TEST(test_newindex_##type##_min) {                                                            \
        test_struct.type = 0;                                                                           \
        lua_pushinteger(state, min_val);                                                                \
        lua_setfield(state, -2, #type);                                                                 \
        ck_assert_int_eq(test_struct.type, min_val);                                                    \
    }                                                                                                   \
    END_TEST                                                                                            \
                                                                                                        \
    START_TEST(test_newindex_##type##_max) {                                                            \
        test_struct.type = 0;                                                                           \
        lua_pushinteger(state, max_val);                                                                \
        lua_setfield(state, -2, #type);                                                                 \
        ck_assert_int_eq(test_struct.type, max_val);                                                    \
    }                                                                                                   \
    END_TEST                                                                                            \
                                                                                                        \
    START_TEST(test_newindex_##type##_overflow_max) {                                                   \
        test_struct.type = 0;                                                                           \
        int res = luaL_dostring(state, "function test(obj) obj." #type " = " STR(max_val) " + 1 end");  \
        lua_getglobal(state, "test");                                                                   \
        lua_pushvalue(state, -2);                                                                       \
        ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
    }                                                                                                   \
    END_TEST                                                                                            \
                                                                                                        \
    START_TEST(test_newindex_##type##_overflow_min) {                                                   \
        test_struct.type = 0;                                                                           \
        int res = luaL_dostring(state, "function test(obj) obj." #type " = " STR(min_val) " - 1 end");  \
        lua_getglobal(state, "test");                                                                   \
        lua_pushvalue(state, -2);                                                                       \
        ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
    }                                                                                                   \
    END_TEST                                                                                            \
                                                                                                        \
    START_TEST(test_newindex_##type##_nil) {                                                            \
        int res = luaL_dostring(state, "function test(obj) obj." #type " = nil end");                   \
        lua_getglobal(state, "test");                                                                   \
        lua_pushvalue(state, -2);                                                                       \
        ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
    }                                                                                                   \
    END_TEST

TEST_NEWINDEX_INT(int32, INT32_MIN, INT32_MAX)
TEST_NEWINDEX_INT(int16, INT16_MIN, INT16_MAX)
TEST_NEWINDEX_INT(int8, INT8_MIN, INT8_MAX)
TEST_NEWINDEX_INT(uint32, 0, UINT32_MAX)
TEST_NEWINDEX_INT(uint16, 0, UINT16_MAX)
TEST_NEWINDEX_INT(uint8, 0, UINT8_MAX)

START_TEST(test_newindex_float_precision) {
    float highPrecisionFloat = 3.14159265358979323846f;
    test_struct.number = 0.0f;
    lua_pushnumber(state, highPrecisionFloat);
    lua_setfield(state, -2, "number");
    ck_assert_float_eq(test_struct.number, highPrecisionFloat);
}
END_TEST

START_TEST(test_newindex_float_nil) {
    test_struct.number = 0.0f;
    int res = luaL_dostring(state, "function test(obj) obj.number = nil end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

START_TEST(test_newindex_boolean) {
    test_struct.boolean = false;
    lua_pushboolean(state, true);
    lua_setfield(state, -2, "boolean");
    ck_assert_int_eq(test_struct.boolean, true);
}
END_TEST

START_TEST(test_newindex_boolean_nil) { 
    test_struct.boolean = false;
    int res = luaL_dostring(state, "function test(obj) obj.boolean = nil end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_eq(lua_pcall(state, 1, 0, 0), LUA_OK); // this should be perfectly fine
}
END_TEST

START_TEST(test_newindex_unknown_field) {
    int res = luaL_dostring(state, "function test(obj) obj.some_random_unexisting_field = 42 end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

START_TEST(test_newindex_object_success) {
    SubStruct sub_struct_2;
    sub_struct_2.a = 42;
    sub_struct_2.b = 93;
    sub_struct_2.c = 7;
    for(int i = 0; i < 5; i++) {
        sub_struct_2.d[i] = i + 1;
    }
    LUAS_OBJECT(state, SubStruct, &sub_struct_2, false);
    lua_setfield(state, -2, "sub_struct");
    ck_assert_mem_eq(&test_struct.sub_struct, &sub_struct_2, sizeof(SubStruct));
}
END_TEST

START_TEST(test_newindex_object_type_mismatch) {
    TestStruct test_struct_2;
    int res = luaL_dostring(state, "function test(ts1, ts2) ts1.sub_struct = ts2 end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    LUAS_OBJECT(state, TestStruct, &test_struct_2, false);
    ck_assert_int_ne(lua_pcall(state, 2, 0, 0), LUA_OK);
}
END_TEST

START_TEST(test_newindex_object_nil) {
    int res = luaL_dostring(state, "function test(obj) obj.sub_struct = nil end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("object_newindex_metamethod");
    
    TCase *primitives = tcase_create("primitives");
    tcase_add_checked_fixture(primitives, setup, teardown);
    tcase_add_test(primitives, test_newindex_int32_min);
    tcase_add_test(primitives, test_newindex_int32_max);
    tcase_add_test(primitives, test_newindex_int32_overflow_max);
    tcase_add_test(primitives, test_newindex_int32_overflow_min);
    tcase_add_test(primitives, test_newindex_int32_nil);
    tcase_add_test(primitives, test_newindex_int16_min);
    tcase_add_test(primitives, test_newindex_int16_max);
    tcase_add_test(primitives, test_newindex_int16_overflow_max);
    tcase_add_test(primitives, test_newindex_int16_overflow_min);
    tcase_add_test(primitives, test_newindex_int16_nil);
    tcase_add_test(primitives, test_newindex_int8_min);
    tcase_add_test(primitives, test_newindex_int8_max);
    tcase_add_test(primitives, test_newindex_int8_overflow_max);
    tcase_add_test(primitives, test_newindex_int8_overflow_min);
    tcase_add_test(primitives, test_newindex_int8_nil);
    tcase_add_test(primitives, test_newindex_uint32_min);
    tcase_add_test(primitives, test_newindex_uint32_max);
    tcase_add_test(primitives, test_newindex_uint32_overflow_max);
    tcase_add_test(primitives, test_newindex_uint32_overflow_min);
    tcase_add_test(primitives, test_newindex_uint32_nil);
    tcase_add_test(primitives, test_newindex_uint16_min);
    tcase_add_test(primitives, test_newindex_uint16_max);
    tcase_add_test(primitives, test_newindex_uint16_overflow_max);
    tcase_add_test(primitives, test_newindex_uint16_overflow_min);
    tcase_add_test(primitives, test_newindex_uint16_nil);
    tcase_add_test(primitives, test_newindex_uint8_min);
    tcase_add_test(primitives, test_newindex_uint8_max);
    tcase_add_test(primitives, test_newindex_uint8_overflow_max);
    tcase_add_test(primitives, test_newindex_uint8_overflow_min);
    tcase_add_test(primitives, test_newindex_uint8_nil);
    tcase_add_test(primitives, test_newindex_float_precision);
    tcase_add_test(primitives, test_newindex_float_nil);
    tcase_add_test(primitives, test_newindex_boolean);
    tcase_add_test(primitives, test_newindex_boolean_nil);
    suite_add_tcase(s, primitives);

    TCase *unknown_field = tcase_create("unknown_field");
    tcase_add_checked_fixture(unknown_field, setup, teardown);
    tcase_add_test(unknown_field, test_newindex_unknown_field);
    suite_add_tcase(s, unknown_field);

    TCase *objects = tcase_create("objects");
    tcase_add_checked_fixture(objects, setup, teardown);
    tcase_add_test(objects, test_newindex_object_success);
    tcase_add_test(objects, test_newindex_object_type_mismatch);
    tcase_add_test(objects, test_newindex_object_nil);
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
