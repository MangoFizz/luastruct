#ifndef LUASTRUCT_H
#define LUASTRUCT_H

#ifndef __cplusplus
#include <lua.h>
#else
#include <lua.hpp>
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define LUASTRUCT_TYPENAME_LENGTH 64
#define LUASTRUCT_DYNAMIC_ARRAY_NAME_FORMAT "DynamicArray_%s"

#define STR2(s) # s
#define STR(s) STR2(s)

typedef enum LuastructType {
	LUAS_TYPE_INT8,
	LUAS_TYPE_INT16,
	LUAS_TYPE_INT32,
	LUAS_TYPE_INT64,
	LUAS_TYPE_UINT8,
	LUAS_TYPE_UINT16,
	LUAS_TYPE_UINT32,
	LUAS_TYPE_UINT64,
	LUAS_TYPE_FLOAT,
	LUAS_TYPE_BOOL,
	LUAS_TYPE_STRING,
	LUAS_TYPE_STRUCT,
	LUAS_TYPE_ENUM,
	LUAS_TYPE_BITFIELD,
	LUAS_TYPE_DYNARRAY,
	LUAS_TYPE_OBJREF,
	LUAS_TYPE_EXTREF,
	LUAS_TYPE_METHOD
} LuastructType;

typedef struct LuastructTypeInfo {
	LuastructType type;
	char name[LUASTRUCT_TYPENAME_LENGTH];
} LuastructTypeInfo;

typedef struct LuastructDynamicArray {
	LuastructTypeInfo type_info;
	lua_CFunction count_getter;
	LuastructType elements_type;
	void *elements_type_info;
	bool elements_are_pointers;
} LuastructDynamicArray;

typedef struct LuastructStructField {
	char field_name[LUASTRUCT_TYPENAME_LENGTH];
	LuastructType type;
	void *type_info;
	uint32_t offset;
	uint16_t count;
	bool pointer;
	bool readonly;
	struct LuastructStructField *next_by_offset;
	struct LuastructStructField *next_by_name;
	union {
		struct {
			uint8_t size;
			uint8_t offset;
		} bitfield;
		struct {
			bool elements_are_readonly;
		} dynamic_array;
	};
} LuastructStructField;

typedef struct LuastructStruct {
	LuastructTypeInfo type_info;
	struct LuastructStruct *super;
	size_t size;
	LuastructStructField *fields;
	LuastructStructField *fields_by_name;
} LuastructStruct;

typedef enum LuastructEnumValueType {
	LUAS_ENUM_INT8,
	LUAS_ENUM_INT16,
	LUAS_ENUM_INT32
} LuastructEnumValueType;

typedef struct LuastructEnumValue {
	char name[LUASTRUCT_TYPENAME_LENGTH];
	intmax_t value;
	struct LuastructEnumValue *next_by_value;
	struct LuastructEnumValue *next_by_name;
} LuastructEnumValue;

typedef struct LuastructEnum {
	LuastructTypeInfo type_info;
	LuastructEnumValueType type;
	intmax_t max_value;
	LuastructEnumValue *values_by_value;
	LuastructEnumValue *values_by_name;
} LuastructEnum;

typedef struct LuastructObject {
	void *type;
	void *data;
	bool readonly;
	bool invalid;
	bool delete_on_gc;
} LuastructObject;

/**
 * Get the types registry.
 * @param state Lua state.
 */
int luastruct_get_types_registry(lua_State *state);

/**
 * Create a new struct type.
 * @param state Lua state.
 * @param name Name of the struct type.
 * @param super_name Name of the super struct type.
 * @param size Size of the struct.
 * @return the number of values pushed onto the stack.
 */
int luastruct_new_struct(lua_State *state, const char *name, const char *super_name, uint32_t size);

/**
 * Check if the object at the given index is a struct.
 * @param state Lua state.
 * @param index Index of the object.
 * @return The struct object or NULL if the object is not a struct.
 */
LuastructStruct *luastruct_check_struct(lua_State *state, int index);

/**
 * Create a new field in a struct.
 * @param state Lua state.
 * @param name Name of the field.
 * @param type Type of the field.
 * @param type_name Name of the type of the field.
 * @param offset Offset of the field in the struct.
 * @param pointer Whether the field is a pointer.
 * @param readonly Whether the field is read-only.
 */
void luas_new_struct_field(lua_State *state, const char *name, LuasType type, const char *type_name, uint32_t offset, uint32_t count, bool pointer, bool readonly);

/**
 * Create a new dynamic array field in a struct.
 * @param state Lua state.
 * @param name Name of the field.
 * @param type Type of the field.
 * @param type_name Name of the type of the field.
 * @param offset Offset of the field in the struct.
 * @param pointer Whether the field is a pointer.
 * @param readonly Whether the field is read-only.
 * @param elements_are_readonly Whether the elements of the array are read-only.
 */
void luastruct_new_struct_dynamic_array_field(lua_State *state, const char *name, LuastructType type, const char *type_name, uint32_t offset, bool pointer, bool readonly, bool elements_are_readonly);


#ifdef __cplusplus
}
#endif

#endif 
