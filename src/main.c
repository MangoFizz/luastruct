#include <stdio.h>
#include <string.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luastruct.h"
#include "helpers.h"
#include "debug.h"

typedef struct TestStruct {
    int a;
    float b;
    bool c;
    const int8_t d;
} TestStruct;

int main(void) {
    void *state = luaL_newstate();
    luaL_openlibs(state);
    printf("State: %p\n", state);

    LUASTRUCT_NEW_STRUCT(state, TestStruct);
    LUASTRUCT_NEW_INT_FIELD(state, TestStruct, a, false);
    LUASTRUCT_NEW_FLOAT_FIELD(state, TestStruct, b, false);
    LUASTRUCT_NEW_BOOL_FIELD(state, TestStruct, c, false);
    LUASTRUCT_NEW_INT_FIELD(state, TestStruct, d, true);

    luastruct_print_registered_types(state);
    luastruct_print_struct_definition(state, "TestStruct");

    TestStruct asd;
    asd.a = 77;
    
    luastruct_load_lua_script(state, "test.lua");
    LUASTRUCT_NEW_OBJECT(state, TestStruct, &asd, false);
    luastruct_call_lua_function(state, "test", 1);
    lua_pop(state, 1);
    LUASTRUCT_NEW_OBJECT(state, TestStruct, &asd, false);
    luastruct_call_lua_function(state, "test", 1);
    lua_gc(state, LUA_GCCOLLECT, 0);

    return 0;
}
