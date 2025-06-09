// SPDX-License-Identifier: GPL-3.0-only

#ifndef LUASTRUCT_TEST_STRUCT_H
#define LUASTRUCT_TEST_STRUCT_H

#include <stdint.h>
#include <stdbool.h>
#include "helpers.h"

typedef struct SubStruct {
    int32_t a;
    int16_t b;
    int8_t c;
    int32_t d[5];
} SubStruct;

typedef struct TestStruct {
    int32_t int32;
    int16_t int16;
    int8_t int8;
    uint32_t uint32;
    uint16_t uint16;
    uint8_t uint8;
    float number;
    bool boolean;
    int8_t static_array[5];
    int8_t *dynamic_array;
    SubStruct sub_struct;
} TestStruct;

static inline int get_dynamic_array_size(lua_State *state) {
    lua_pushinteger(state, 5);
    return 1;
}

static inline void init_test_struct(TestStruct *test_struct) {
    test_struct->int32 = 0;
    test_struct->int16 = 0;
    test_struct->int8 = 0;
    test_struct->uint32 = 0;
    test_struct->uint16 = 0;
    test_struct->uint8 = 0;
    test_struct->number = 0.0f;
    test_struct->boolean = false;
    for(int i = 0; i < 5; i++) {
        test_struct->static_array[i] = 0;
    }
    test_struct->dynamic_array = malloc(sizeof(int8_t) * 5);
    for(int i = 0; i < 5; i++) {
        test_struct->dynamic_array[i] = 0;
    }
    test_struct->sub_struct.a = 0;
    test_struct->sub_struct.b = 0;
    test_struct->sub_struct.c = 0;
    for(int i = 0; i < 5; i++) {
        test_struct->sub_struct.d[i] = 0;
    }
}

static inline void define_test_struct(lua_State *state) {
    LUAS_STRUCT(state, SubStruct);
    LUAS_PRIMITIVE_FIELD(state, SubStruct, a, LUAST_INT32, 0);
    lua_pop(state, 1);

    LUAS_STRUCT(state, TestStruct);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, int32, LUAST_INT32, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, int16, LUAST_INT16, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, int8, LUAST_INT8, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, uint32, LUAST_UINT32, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, uint16, LUAST_UINT16, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, uint8, LUAST_UINT8, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, number, LUAST_FLOAT, 0);
    LUAS_PRIMITIVE_FIELD(state, TestStruct, boolean, LUAST_BOOL, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_array, LUAST_INT8, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_array, get_dynamic_array_size, LUAST_INT8, 0);
    LUAS_OBJREF_FIELD(state, TestStruct, sub_struct, SubStruct, 0);
    lua_pop(state, 1);
}

#endif 
