#include <stdio.h>
#include <string.h>
#include "lauxlib.h"
#include "luastruct.h"
#include "debug.h"

int count_getter(lua_State *state) {
    lua_pushinteger(state, 10);
    return 1;
}

int main(void) {
    void *state = luaL_newstate();
    printf("State: %p\n", state);

    luas_new_struct(state, "TestStruct", NULL, 20);
    luas_new_struct_field(state, "super_field_1", LUAS_TYPE_INT32, NULL, 0, 8, false, false);
    luas_new_struct_field(state, "super_field_4", LUAS_TYPE_FLOAT, NULL, 12, 1, true, true);
    luas_new_struct_field(state, "super_field_2", LUAS_TYPE_BOOL, NULL, 4, 0, true, false);
    luas_new_struct_field(state, "super_field_3", LUAS_TYPE_INT8, NULL, 8, 2, false, true);
    luas_new_struct_dynamic_array_field(state, "dynamic_array_field", LUAS_TYPE_UINT16, NULL, 16, false, false, false);

    luas_new_struct(state, "TestStruct2", NULL, 30);
    luas_new_struct(state, "TestStruct3", "TestStruct", 25);

    luasd_print_registered_types(state);
    luasd_print_struct_fields(state, "TestStruct");

    lua_close(state);

    return 0;
}
