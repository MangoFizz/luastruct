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

START_TEST(test_index_static_int32) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int32[i] = INT32_MIN + i;
    }
    lua_getfield(state, -1, "static_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int32_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int32[i] = INT32_MIN;
    }
    lua_getfield(state, -1, "static_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int32_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int32[i] = INT32_MAX;
    }
    lua_getfield(state, -1, "static_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int16) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int16[i] = INT16_MIN + i;
    }
    lua_getfield(state, -1, "static_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int16_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int16[i] = INT16_MIN;
    }
    lua_getfield(state, -1, "static_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int16_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int16[i] = INT16_MAX;
    }
    lua_getfield(state, -1, "static_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int8) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int8[i] = INT8_MIN + i;
    }
    lua_getfield(state, -1, "static_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int8_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int8[i] = INT8_MIN;
    }
    lua_getfield(state, -1, "static_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_int8_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_int8[i] = INT8_MAX;
    }
    lua_getfield(state, -1, "static_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint32) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint32[i] = UINT32_MAX - i;
    }
    lua_getfield(state, -1, "static_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT32_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint32_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint32[i] = 0;
    }
    lua_getfield(state, -1, "static_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint32_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint32[i] = UINT32_MAX;
    }
    lua_getfield(state, -1, "static_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT32_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint16) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint16[i] = UINT16_MAX - i;
    }
    lua_getfield(state, -1, "static_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT16_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint16_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint16[i] = 0;
    }
    lua_getfield(state, -1, "static_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint16_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint16[i] = UINT16_MAX;
    }
    lua_getfield(state, -1, "static_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT16_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint8) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint8[i] = UINT8_MAX - i;
    }
    lua_getfield(state, -1, "static_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT8_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint8_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint8[i] = 0;
    }
    lua_getfield(state, -1, "static_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_uint8_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.static_uint8[i] = UINT8_MAX;
    }
    lua_getfield(state, -1, "static_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT8_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_static_float_precision) {
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

START_TEST(test_index_static_boolean) {
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

START_TEST(test_index_dynamic_int32) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int32[i] = INT32_MIN + i;
    }
    lua_getfield(state, -1, "dynamic_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
} 
END_TEST

START_TEST(test_index_dynamic_int32_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int32[i] = INT32_MIN;
    }
    lua_getfield(state, -1, "dynamic_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int32_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int32[i] = INT32_MAX;
    }
    lua_getfield(state, -1, "dynamic_int32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT32_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int16) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int16[i] = INT16_MIN + i;
    }
    lua_getfield(state, -1, "dynamic_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int16_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int16[i] = INT16_MIN;
    }
    lua_getfield(state, -1, "dynamic_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int16_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int16[i] = INT16_MAX;
    }
    lua_getfield(state, -1, "dynamic_int16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT16_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int8) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int8[i] = INT8_MIN + i;
    }
    lua_getfield(state, -1, "dynamic_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MIN + i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int8_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int8[i] = INT8_MIN;
    }
    lua_getfield(state, -1, "dynamic_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MIN);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_int8_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_int8[i] = INT8_MAX;
    }
    lua_getfield(state, -1, "dynamic_int8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        int8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, INT8_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint32) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint32[i] = UINT32_MAX - i;
    }
    lua_getfield(state, -1, "dynamic_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT32_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint32_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint32[i] = 0;
    }
    lua_getfield(state, -1, "dynamic_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint32_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint32[i] = UINT32_MAX;
    }
    lua_getfield(state, -1, "dynamic_uint32");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint32_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT32_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint16) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint16[i] = UINT16_MAX - i;
    }
    lua_getfield(state, -1, "dynamic_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT16_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint16_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint16[i] = 0;
    }
    lua_getfield(state, -1, "dynamic_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint16_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint16[i] = UINT16_MAX;
    }
    lua_getfield(state, -1, "dynamic_uint16");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint16_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT16_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint8) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint8[i] = UINT8_MAX - i;
    }
    lua_getfield(state, -1, "dynamic_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT8_MAX - i);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint8_min) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint8[i] = 0;
    }
    lua_getfield(state, -1, "dynamic_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, 0);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_uint8_max) {
    for(int i = 0; i < 5; i++) {
        test_struct.dynamic_uint8[i] = UINT8_MAX;
    }
    lua_getfield(state, -1, "dynamic_uint8");
    for(int i = 0; i < 5; i++) {
        lua_pushinteger(state, i + 1);
        lua_gettable(state, -2);
        uint8_t value = luaL_checkinteger(state, -1);
        ck_assert_int_eq(value, UINT8_MAX);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);
}
END_TEST

START_TEST(test_index_dynamic_float_precision) {
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

START_TEST(test_index_dynamic_boolean) {
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

START_TEST(test_index_out_of_bounds) {
    int res = luaL_dostring(state, "function test(obj) obj.static_int32[6] = 0 end");
    lua_getglobal(state, "test");
    lua_pushvalue(state, -2);
    ck_assert_int_ne(lua_pcall(state, 1, 0, 0), LUA_OK);
}
END_TEST

START_TEST(test_index_static_object) {
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

START_TEST(test_index_dynamic_object) {
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
    Suite *s = suite_create("object_index_metamethod");
    
    TCase *primitives = tcase_create("primitives");
    tcase_add_checked_fixture(primitives, setup, teardown);
    tcase_add_test(primitives, test_index_static_int32);
    tcase_add_test(primitives, test_index_static_int32_min);
    tcase_add_test(primitives, test_index_static_int32_max);
    tcase_add_test(primitives, test_index_static_int16);
    tcase_add_test(primitives, test_index_static_int16_min);
    tcase_add_test(primitives, test_index_static_int16_max);
    tcase_add_test(primitives, test_index_static_int8);
    tcase_add_test(primitives, test_index_static_int8_min);
    tcase_add_test(primitives, test_index_static_int8_max);
    tcase_add_test(primitives, test_index_static_uint32);
    tcase_add_test(primitives, test_index_static_uint32_min);
    tcase_add_test(primitives, test_index_static_uint32_max);
    tcase_add_test(primitives, test_index_static_uint16);
    tcase_add_test(primitives, test_index_static_uint16_min);
    tcase_add_test(primitives, test_index_static_uint16_max);
    tcase_add_test(primitives, test_index_static_uint8);
    tcase_add_test(primitives, test_index_static_uint8_min);
    tcase_add_test(primitives, test_index_static_uint8_max);
    tcase_add_test(primitives, test_index_static_float_precision);
    tcase_add_test(primitives, test_index_static_boolean);
    tcase_add_test(primitives, test_index_dynamic_int32);
    tcase_add_test(primitives, test_index_dynamic_int32_min);
    tcase_add_test(primitives, test_index_dynamic_int32_max);
    tcase_add_test(primitives, test_index_dynamic_int16);
    tcase_add_test(primitives, test_index_dynamic_int16_min);
    tcase_add_test(primitives, test_index_dynamic_int16_max);
    tcase_add_test(primitives, test_index_dynamic_int8);
    tcase_add_test(primitives, test_index_dynamic_int8_min);
    tcase_add_test(primitives, test_index_dynamic_int8_max);
    tcase_add_test(primitives, test_index_dynamic_uint32);
    tcase_add_test(primitives, test_index_dynamic_uint32_min);
    tcase_add_test(primitives, test_index_dynamic_uint32_max);
    tcase_add_test(primitives, test_index_dynamic_uint16);
    tcase_add_test(primitives, test_index_dynamic_uint16_min);
    tcase_add_test(primitives, test_index_dynamic_uint16_max);
    tcase_add_test(primitives, test_index_dynamic_uint8);
    tcase_add_test(primitives, test_index_dynamic_uint8_min);
    tcase_add_test(primitives, test_index_dynamic_uint8_max);
    tcase_add_test(primitives, test_index_dynamic_float_precision);
    tcase_add_test(primitives, test_index_dynamic_boolean);
    suite_add_tcase(s, primitives);

    TCase *out_of_bounds = tcase_create("out_of_bounds");
    tcase_add_checked_fixture(out_of_bounds, setup, teardown);
    tcase_add_test(out_of_bounds, test_index_out_of_bounds);
    suite_add_tcase(s, out_of_bounds);

    TCase *objects = tcase_create("objects");
    tcase_add_checked_fixture(objects, setup, teardown);
    tcase_add_test(objects, test_index_static_object);
    tcase_add_test(objects, test_index_dynamic_object);
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
