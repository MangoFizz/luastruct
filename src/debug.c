// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include "debug.h"
#include "luastruct.h"

#define MAX_CLASS_NAME 64 
#define COLUMN_WIDTH 64  
#define TABLE_BORDER "+----------------------------------------------------------------+"

const char *luastruct_name_for_type(LuastructType type) {
    switch(type) {
        case LUAS_TYPE_INT8:
            return "int8";
        case LUAS_TYPE_INT16:
            return "int16";
        case LUAS_TYPE_INT32:
            return "int32";
        case LUAS_TYPE_INT64:
            return "int64";
        case LUAS_TYPE_UINT8:
            return "uint8";
        case LUAS_TYPE_UINT16:
            return "uint16";
        case LUAS_TYPE_UINT32:
            return "uint32";
        case LUAS_TYPE_UINT64:
            return "uint64";
        case LUAS_TYPE_FLOAT:
            return "float";
        case LUAS_TYPE_BOOL:
            return "bool";
        case LUAS_TYPE_STRING:
            return "string";
        case LUAS_TYPE_STRUCT:
            return "struct";
        case LUAS_TYPE_ENUM:
            return "enum";
        case LUAS_TYPE_BITFIELD:
            return "bitfield";
        case LUAS_TYPE_DYNARRAY:
            return "dynarray";
        case LUAS_TYPE_OBJREF:
            return "objref";
        case LUAS_TYPE_EXTREF:
            return "extref";
        case LUAS_TYPE_METHOD:
            return "method";
        default:
            return "unknown";
    }
}

const char *luastruct_type_for_bitfield(size_t size) {
    switch(size) {
        case 1:
            return "uint8";
        case 2:
            return "uint16";
        case 4:
            return "uint32";
        default:
            return "unknown";
    }
}

void luastruct_print_registered_types(lua_State *state) {
    printf("%s\n", TABLE_BORDER);
    printf("| %-*s |\n", COLUMN_WIDTH - 2, "Registered Luastructtruct types");
    printf("%s\n", TABLE_BORDER);

    luastruct_get_types_registry(state);
    lua_pushnil(state);
    while(lua_next(state, -2) != 0) {
        // key is at index -2 and value is at index -1
        const char *key = lua_tostring(state, -2);
        LuastructStruct *value = lua_touserdata(state, -1);
        
        char row[128];
        const char *type_string = luastruct_name_for_type(value->type_info.type);
        if(value->super) {
            snprintf(row, sizeof(row), "%s %s extends %s", type_string, key, value->super->type_info.name);
        }
        else {
            snprintf(row, sizeof(row), "%s %s", type_string, key);
        }
        printf("| %-*s |\n", COLUMN_WIDTH - 2, row);
        lua_pop(state, 1);
    }
    lua_pop(state, 1);

    printf("%s\n\n", TABLE_BORDER);
}

void luastruct_print_struct_definition(lua_State *state, const char *name) {
    luastruct_get_types_registry(state);
    lua_getfield(state, -1, name);
    if(lua_isnil(state, -1)) {
        printf("Struct type not found: %s\n", name);
        lua_pop(state, 2);
        return;
    }

    LuastructStruct *st = lua_touserdata(state, -1);
    lua_pop(state, 2);

    printf("%s\n", TABLE_BORDER);
    printf("| %-*s |\n", COLUMN_WIDTH - 2, name);
    printf("%s\n", TABLE_BORDER);

    LuastructStructField *field = st->fields;
    while(field) {
        const char *type = luastruct_name_for_type(field->type);
        if(field->type == LUAS_TYPE_BITFIELD) {
            type = luastruct_type_for_bitfield(field->bitfield.size);
        }
        else if(field->type == LUAS_TYPE_DYNARRAY) {
            LuastructDynamicArray *da = field->type_info;
            type = luastruct_type_for_bitfield(da->elements_type);
        }

        char buffer[32];
        char row[128];
        row[0] = '\0';
        sprintf(buffer, "[0x%.4X]", field->offset);
        strcat(row, buffer);
        if(field->readonly) {
            strcat(row, " const");
        }
        sprintf(buffer, " %s ", type);
        strcat(row, buffer);
        if(field->pointer) {
            strcat(row, "*");
        }
        strcat(row, field->field_name);
        if(field->count > 0) {
            sprintf(buffer, "[%d]", field->count);
            strcat(row, buffer);
        }
        if(field->type == LUAS_TYPE_DYNARRAY) {
            strcat(row, "[]");
        }
        if(field->type == LUAS_TYPE_BITFIELD) {
            sprintf(buffer, " : 1");
            strcat(row, buffer);
        }
        printf("| %-*s |\n", COLUMN_WIDTH - 2, row);

        field = field->next_by_offset;
    }

    printf("%s\n\n", TABLE_BORDER);
}

void luastruct_load_lua_script(lua_State *state, const char *filename) {
    if(luaL_loadfile(state, filename) != LUA_OK || lua_pcall(state, 0, 0, 0)  != LUA_OK) {
        fprintf(stderr, "Error: %s\n", lua_tostring(state, -1));
        lua_pop(state, 1); // remove error message from stack
    }
}

void luastruct_call_lua_function(lua_State *state, const char *function_name, int n_args) {
    lua_getglobal(state, function_name);
    if(lua_isfunction(state, -1)) {
        for(int i = 0; i < n_args; i++) {
            lua_pushvalue(state, -2 - i);
        }
        int res = lua_pcall(state, n_args, 0, 0);
        if(res != LUA_OK) {
            fprintf(stderr, "Error: %s\n", lua_tostring(state, -1));
            lua_pop(state, 1); 
        }
    } 
    else {
        fprintf(stderr, "Error: %s is not a function\n", function_name);
    }
    lua_pop(state, 1);
}
