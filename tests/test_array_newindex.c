#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "test_array.h"
#include "debug.h"

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

#define TEST_ARRAY_NEWINDEX(type, min_val, max_val)                                                         \
    START_TEST(test_newindex_static_##type##_min) {                                                         \
        lua_getfield(state, -1, "static_" #type);                                                           \
        for(int i = 0; i < 5; i++) {                                                                        \
            test_struct.static_##type[i] = 0;                                                               \
            type##_t value = min_val;                                                                       \
            lua_pushinteger(state, i + 1);                                                                  \
            lua_pushinteger(state, value);                                                                  \
            lua_settable(state, -3);                                                                        \
            ck_assert_int_eq(value, test_struct.static_##type[i]);                                          \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_static_##type##_max) {                                                         \
        lua_getfield(state, -1, "static_" #type);                                                           \
        for(int i = 0; i < 5; i++) {                                                                        \
            test_struct.static_##type[i] = 0;                                                               \
            type##_t value = max_val;                                                                       \
            lua_pushinteger(state, i + 1);                                                                  \
            lua_pushinteger(state, value);                                                                  \
            lua_settable(state, -3);                                                                        \
            ck_assert_int_eq(value, test_struct.static_##type[i]);                                          \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_static_##type##_overflow_min) {                                                \
        lua_getfield(state, -1, "static_" #type);                                                           \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = %d - 1 end", i + 1, min_val);                     \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_static_##type##_overflow_max) {                                                \
        lua_getfield(state, -1, "static_" #type);                                                           \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = %ld + 1 end", i + 1, max_val);                    \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_static_##type##_nil) {                                                         \
        lua_getfield(state, -1, "static_" #type);                                                           \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = nil end", i + 1);                                 \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_dynamic_##type##_min) {                                                        \
        lua_getfield(state, -1, "dynamic_" #type);                                                          \
        for(int i = 0; i < 5; i++) {                                                                        \
            test_struct.dynamic_##type[i] = 0;                                                              \
            type##_t value = min_val;                                                                       \
            lua_pushinteger(state, i + 1);                                                                  \
            lua_pushinteger(state, value);                                                                  \
            lua_settable(state, -3);                                                                        \
            ck_assert_int_eq(value, test_struct.dynamic_##type[i]);                                         \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_dynamic_##type##_max) {                                                        \
        lua_getfield(state, -1, "dynamic_" #type);                                                          \
        for(int i = 0; i < 5; i++) {                                                                        \
            test_struct.dynamic_##type[i] = 0;                                                              \
            type##_t value = max_val;                                                                       \
            lua_pushinteger(state, i + 1);                                                                  \
            lua_pushinteger(state, value);                                                                  \
            lua_settable(state, -3);                                                                        \
            ck_assert_int_eq(value, test_struct.dynamic_##type[i]);                                         \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_dynamic_##type##_overflow_min) {                                               \
        lua_getfield(state, -1, "dynamic_" #type);                                                          \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = %d - 1 end", i + 1, min_val);                     \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_dynamic_##type##_overflow_max) {                                               \
        lua_getfield(state, -1, "dynamic_" #type);                                                          \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = %ld + 1 end", i + 1, max_val);                    \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST                                                                                                \
                                                                                                            \
    START_TEST(test_newindex_dynamic_##type##_nil) {                                                        \
        lua_getfield(state, -1, "dynamic_" #type);                                                          \
        for(int i = 0; i < 5; i++) {                                                                        \
            char script[256];                                                                               \
            sprintf(script, "function test(arr) arr[%d] = nil end", i + 1);                                 \
            int res = luaL_dostring(state, script);                                                         \
            lua_getglobal(state, "test");                                                                   \
            lua_pushvalue(state, -2);                                                                       \
            ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);                                            \
        }                                                                                                   \
        lua_pop(state, 1);                                                                                  \
    }                                                                                                       \
    END_TEST


TEST_ARRAY_NEWINDEX(int32, INT32_MIN, INT32_MAX)
TEST_ARRAY_NEWINDEX(int16, INT16_MIN, INT16_MAX)
TEST_ARRAY_NEWINDEX(int8, INT8_MIN, INT8_MAX)
TEST_ARRAY_NEWINDEX(uint32, 0, UINT32_MAX)
TEST_ARRAY_NEWINDEX(uint16, 0, UINT16_MAX)
TEST_ARRAY_NEWINDEX(uint8, 0, UINT8_MAX)

START_TEST(test_newindex_static_float_precision) {
    float highPrecisionFloat = 3.14159265358979323846f;
    for(int i = 0; i < 5; i++) {
        test_struct.static_number[i] = highPrecisionFloat;
    }
    lua_getfield(state, -1, "static_number");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        float value = luaL_checknumber(state, -1);
        ck_assert_float_eq(value, highPrecisionFloat);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_newindex_static_boolean) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_boolean[i] = (i % 2 == 0);
    }
    lua_getfield(state, -1, "static_boolean");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        bool value = lua_toboolean(state, -1);
        ck_assert_int_eq(value, (i % 2 == 0));
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_newindex_dynamic_float_precision) {
    float highPrecisionFloat = 3.14159265358979323846f;
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_number[i] = highPrecisionFloat;
    }
    lua_getfield(state, -1, "dynamic_number");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        float value = luaL_checknumber(state, -1);
        ck_assert_float_eq(value, highPrecisionFloat);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_newindex_dynamic_boolean) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_boolean[i] = (i % 2 == 0);
    }
    lua_getfield(state, -1, "dynamic_boolean");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        bool value = lua_toboolean(state, -1);
        ck_assert_int_eq(value, (i % 2 == 0));
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_newindex_out_of_bounds) {
    int res = luaL_dostring(state, "function test(obj) obj.static_int32[6] = 0 end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

START_TEST(test_newindex_static_object) {
    lua_getfield(state, -1, "static_sub_struct");
    ck_assert_msg(!lua_isnil(state, -1), "static_sub_struct is nil");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        LuastructStructObject *obj = lua_touserdata(state, -1);
        ck_assert_ptr_eq(obj->data, &test_struct.static_sub_struct[i]);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_newindex_dynamic_object) {
    lua_getfield(state, -1, "dynamic_sub_struct");
    ck_assert_msg(!lua_isnil(state, -1), "dynamic_sub_struct is nil");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        LuastructStructObject *obj = lua_touserdata(state, -1);
        ck_assert_ptr_eq(obj->data, &test_struct.dynamic_sub_struct[i]);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("array_newindex_metamethod");
    
    TCase *primitives = tcase_create("primitives");
    tcase_add_checked_fixture(primitives, setup, teardown);
    tcase_add_test(primitives, test_newindex_static_int32_min);
    tcase_add_test(primitives, test_newindex_static_int32_max);
    tcase_add_test(primitives, test_newindex_static_int32_overflow_min);
    tcase_add_test(primitives, test_newindex_static_int32_overflow_max);
    tcase_add_test(primitives, test_newindex_static_int16_min);
    tcase_add_test(primitives, test_newindex_static_int16_max);
    tcase_add_test(primitives, test_newindex_static_int16_overflow_min);
    tcase_add_test(primitives, test_newindex_static_int16_overflow_max);
    tcase_add_test(primitives, test_newindex_static_int8_min);
    tcase_add_test(primitives, test_newindex_static_int8_max);
    tcase_add_test(primitives, test_newindex_static_int8_overflow_min);
    tcase_add_test(primitives, test_newindex_static_int8_overflow_max);
    tcase_add_test(primitives, test_newindex_static_uint32_min);
    tcase_add_test(primitives, test_newindex_static_uint32_max);
    tcase_add_test(primitives, test_newindex_static_uint32_overflow_min);
    tcase_add_test(primitives, test_newindex_static_uint32_overflow_max);
    tcase_add_test(primitives, test_newindex_static_uint16_min);
    tcase_add_test(primitives, test_newindex_static_uint16_max);
    tcase_add_test(primitives, test_newindex_static_uint16_overflow_min);
    tcase_add_test(primitives, test_newindex_static_uint16_overflow_max);
    tcase_add_test(primitives, test_newindex_static_uint8_min);
    tcase_add_test(primitives, test_newindex_static_uint8_max);
    tcase_add_test(primitives, test_newindex_static_uint8_overflow_min);
    tcase_add_test(primitives, test_newindex_static_uint8_overflow_max);
    tcase_add_test(primitives, test_newindex_static_float_precision);
    tcase_add_test(primitives, test_newindex_static_boolean);
    tcase_add_test(primitives, test_newindex_dynamic_int32_min);
    tcase_add_test(primitives, test_newindex_dynamic_int32_max);
    tcase_add_test(primitives, test_newindex_dynamic_int32_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_int32_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_int16_min);
    tcase_add_test(primitives, test_newindex_dynamic_int16_max);
    tcase_add_test(primitives, test_newindex_dynamic_int16_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_int16_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_int8_min);
    tcase_add_test(primitives, test_newindex_dynamic_int8_max);
    tcase_add_test(primitives, test_newindex_dynamic_int8_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_int8_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint32_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint32_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint32_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint32_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint16_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint16_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint16_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint16_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint8_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint8_max);
    tcase_add_test(primitives, test_newindex_dynamic_uint8_overflow_min);
    tcase_add_test(primitives, test_newindex_dynamic_uint8_overflow_max);
    tcase_add_test(primitives, test_newindex_dynamic_float_precision);
    tcase_add_test(primitives, test_newindex_dynamic_boolean);
    suite_add_tcase(s, primitives);

    TCase *out_of_bounds = tcase_create("out_of_bounds");
    tcase_add_checked_fixture(out_of_bounds, setup, teardown);
    tcase_add_test(out_of_bounds, test_newindex_out_of_bounds);
    suite_add_tcase(s, out_of_bounds);

    TCase *objects = tcase_create("objects");
    tcase_add_checked_fixture(objects, setup, teardown);
    tcase_add_test(objects, test_newindex_static_object);
    tcase_add_test(objects, test_newindex_dynamic_object);
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
