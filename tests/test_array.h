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
    int32_t static_int32[5];
    int16_t static_int16[5];
    int8_t static_int8[5];
    uint32_t static_uint32[5];
    uint16_t static_uint16[5];
    uint8_t static_uint8[5];
    float static_number[5];
    bool static_boolean[5];
    SubStruct static_sub_struct[5];
    int32_t *dynamic_int32;
    int16_t *dynamic_int16;
    int8_t *dynamic_int8;
    uint32_t *dynamic_uint32;
    uint16_t *dynamic_uint16;
    uint8_t *dynamic_uint8;
    float *dynamic_number;
    bool *dynamic_boolean;
    SubStruct *dynamic_sub_struct;
} TestStruct;

static inline int get_dynamic_array_size(lua_State *state) {
    lua_pushinteger(state, 5);
    return 1;
}

static inline void init_test_struct(TestStruct *test_struct) {
    #define INIT_ARRAY(arr, type) \
        for(int i = 0; i < 5; i++) { \
            test_struct->static_##arr[i] = 0; \
        } \
        test_struct->dynamic_##arr = malloc(sizeof(type) * 5); \
        for(int i = 0; i < 5; i++) { \
            test_struct->dynamic_##arr[i] = 0; \
        }
    
    INIT_ARRAY(int32, int32_t);
    INIT_ARRAY(int16, int16_t);
    INIT_ARRAY(int8, int8_t);
    INIT_ARRAY(uint32, uint32_t);
    INIT_ARRAY(uint16, uint16_t);
    INIT_ARRAY(uint8, uint8_t);
    INIT_ARRAY(number, float);
    INIT_ARRAY(boolean, bool);

    for(int i = 0; i < 5; i++) {
        test_struct->static_sub_struct[i].a = 0;
        test_struct->static_sub_struct[i].b = 0;
        test_struct->static_sub_struct[i].c = 0;
        for(int j = 0; j < 5; j++) {
            test_struct->static_sub_struct[i].d[j] = 0;
        }
    }
    test_struct->dynamic_sub_struct = malloc(sizeof(SubStruct) * 5);
    for(int i = 0; i < 5; i++) {
        test_struct->dynamic_sub_struct[i].a = 0;
        test_struct->dynamic_sub_struct[i].b = 0;
        test_struct->dynamic_sub_struct[i].c = 0;
        for(int j = 0; j < 5; j++) {
            test_struct->dynamic_sub_struct[i].d[j] = 0;
        }
    }
    #undef INIT_ARRAY
}

static inline void define_test_struct(lua_State *state) {
    LUAS_STRUCT(state, SubStruct);
    LUAS_PRIMITIVE_FIELD(state, SubStruct, a, LUAST_INT32, 0);
    lua_pop(state, 1);

    LUAS_STRUCT(state, TestStruct);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_int32, LUAST_INT32, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_int16, LUAST_INT16, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_int8, LUAST_INT8, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_uint32, LUAST_UINT32, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_uint16, LUAST_UINT16, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_uint8, LUAST_UINT8, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_number, LUAST_FLOAT, 0);
    LUAS_PRIMITIVE_ARRAY_FIELD(state, TestStruct, static_boolean, LUAST_BOOL, 0);
    LUAS_OBJREF_ARRAY_FIELD(state, TestStruct, static_sub_struct, SubStruct, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_int32, get_dynamic_array_size, LUAST_INT32, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_int16, get_dynamic_array_size, LUAST_INT16, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_int8, get_dynamic_array_size, LUAST_INT8, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_uint32, get_dynamic_array_size, LUAST_UINT32, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_uint16, get_dynamic_array_size, LUAST_UINT16, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_uint8, get_dynamic_array_size, LUAST_UINT8, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_number, get_dynamic_array_size, LUAST_FLOAT, 0);
    LUAS_PRIMITIVE_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_boolean, get_dynamic_array_size, LUAST_BOOL, 0);
    LUAS_OBJREF_DYNAMIC_ARRAY_FIELD(state, TestStruct, dynamic_sub_struct, get_dynamic_array_size, SubStruct, 0);
    lua_pop(state, 1);
}

#endif 
