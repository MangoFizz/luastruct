#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "test_object.h"
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

START_TEST(test_pairs) {
    int res = luaL_dostring(state, "function test(obj) for k, v in pairs(obj) do end end"); // test against values
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_eq(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("object_pairs_metamethod");
    
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

