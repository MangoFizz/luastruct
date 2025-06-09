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
    luaL_openlibs(state);
    init_test_struct(&test_struct);
    define_test_struct(state);
    LUAS_OBJECT(state, TestStruct, &test_struct, false);
}

void teardown(void) {
    lua_gc(state, LUA_GCCOLLECT, 0);
    lua_close(state);
    state = NULL;
}

static int lua_check_pair(lua_State *state) {
    int key = luaL_checkinteger(state, 1);
    int value = luaL_checkinteger(state, 2);
    ck_assert_int_eq(test_struct.static_int32[key - 1], value);
    return 0;
}

START_TEST(test_pairs) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int32[i] = (i + 1) * 11;
    }
    lua_pushcfunction(state, lua_check_pair);
    lua_setglobal(state, "check_pair");
    int res = luaL_dostring(state, "function test(array) for k, v in pairs(array) do check_pair(k, v) end end");
    lua_getglobal(state, "test");
    lua_getfield(state, -2, "static_int32");
    ck_assert_int_eq(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("array_pairs_metamethod");
    
    TCase *len = tcase_create("pairs");
    tcase_add_checked_fixture(len, setup, teardown);
    tcase_add_test(len, test_pairs);
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

