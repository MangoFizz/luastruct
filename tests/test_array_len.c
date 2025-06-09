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

START_TEST(test_len_static_array) {
    lua_getfield(state, -1, "static_int32");
    int res = luaL_dostring(state, "function test(array) return #array end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_eq(lua_pcall(state, 1, 1, 0), LUA_OK);
    int len = luaL_checkinteger(state, -1);
    ck_assert_int_eq(len, 5);
    lua_pop(state, 2);
}
END_TEST

START_TEST(test_len_dynamic_array) {
    lua_getfield(state, -1, "dynamic_int32");
    int res = luaL_dostring(state, "function test(array) return #array end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_eq(lua_pcall(state, 1, 1, 0), LUA_OK);
    int len = luaL_checkinteger(state, -1);
    ck_assert_int_eq(len, 5);
    lua_pop(state, 2);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("array_len_metamethod");
    
    TCase *len = tcase_create("len");
    tcase_add_checked_fixture(len, setup, teardown);
    tcase_add_test(len, test_len_static_array);
    tcase_add_test(len, test_len_dynamic_array);
    suite_add_tcase(s, len);

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

