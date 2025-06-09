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
    int res = luaL_dostring(state, "function test(obj1, obj2) return obj1 == obj2 end");
    LUAS_OBJECT(state, TestStruct, &test_struct, false);
}

void teardown(void) {
    lua_gc(state, LUA_GCCOLLECT, 0);
    lua_close(state);
    state = NULL;
}

START_TEST(test_eq_success) {
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    LUAS_OBJECT(state, TestStruct, &test_struct, false);
    ck_assert_int_eq(lua_pcall(state, 2, 1, 0), LUA_OK);
    ck_assert_int_eq(lua_toboolean(state, -1), 1);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_eq_failure) {
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    TestStruct other_struct;
    LUAS_OBJECT(state, TestStruct, &other_struct, false);
    ck_assert_int_eq(lua_pcall(state, 2, 1, 0), LUA_OK);
    ck_assert_int_eq(lua_toboolean(state, -1), 0);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_eq_nil) {
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    lua_pushnil(state);
    ck_assert_int_eq(lua_pcall(state, 2, 1, 0), LUA_OK);
    ck_assert_int_eq(lua_toboolean(state, -1), 0);
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_eq_different_type) {
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    SubStruct sub_struct;
    LUAS_OBJECT(state, SubStruct, &sub_struct, false);
    ck_assert_int_eq(lua_pcall(state, 2, 1, 0), LUA_OK);
    ck_assert_int_eq(lua_toboolean(state, -1), 0);
    lua_pop(state, 1);
}
END_TEST

Suite *create_suite(void) {
    Suite *s = suite_create("object_eq_metamethod");
    
    TCase *equals = tcase_create("equals");
    tcase_add_checked_fixture(equals, setup, teardown);
    tcase_add_test(equals, test_eq_success);
    tcase_add_test(equals, test_eq_failure);
    tcase_add_test(equals, test_eq_nil);
    tcase_add_test(equals, test_eq_different_type);
    suite_add_tcase(s, equals);

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

