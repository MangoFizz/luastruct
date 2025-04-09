#include <stdio.h>
#include <string.h>
#include "lauxlib.h"
#include "luastruct.h"
#include "debug.h"

struct TestStruct {
    int a;
    float b;
    bool c;
    int8_t d;
};

int count_getter(lua_State *state) {
    lua_pushinteger(state, 10);
    return 1;
}

int main(void) {
    void *state = luaL_newstate();
    printf("State: %p\n", state);

    LUASTRUCT_NEW_STRUCT(state, TestStruct);

    luasd_print_registered_types(state);
    // luasd_print_struct_fields(state, "TestStruct");

    lua_close(state);

    return 0;
}
