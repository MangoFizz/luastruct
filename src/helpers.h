#ifndef LUASTRUCT_HELPERS_H
#define LUASTRUCT_HELPERS_H

#include "luastruct.h"

#define LUASTRUCT_STRUCT_FIELD(type, field) (((struct type *)NULL)->field)

#define LUASTRUCT_NEW_STRUCT(state, type) { \
	{ struct type; } \
	luastruct_new_struct(state, #type, NULL, sizeof(struct type)); \
}

#define LUASTRUCT_NEW_STRUCT_EXTENDS(state, type, super_type) { \
	struct type *; \
	struct super_type *; \
	luastruct_new_struct(state, #type, #super_type, sizeof(struct type)); \
}

#define LUASTRUCT_NEW_INT_FIELD(state, type, field, read_only) { \
	{ struct type; } \
	LuastructType type; \
	switch(sizeof(LUASTRUCT_STRUCT_FIELD(type, field))) { \
		case 1: type = LUAS_TYPE_INT8; break; \
		case 2: type = LUAS_TYPE_INT16; break; \
		case 4: type = LUAS_TYPE_INT32; break; \
		case 8: type = LUAS_TYPE_INT64; break; \
		default: type = LUAS_TYPE_INT32; break; \
	} \
	luastruct_new_struct_field(state, #field, type, NULL, offsetof(struct type, field), 0, false, read_only); \
}

#define LUASTRUCT_NEW_FLOAT_FIELD(state, type, field, read_only) { \
	{ struct type; } \
	luastruct_new_struct_field(state, #field, LUAS_TYPE_FLOAT, NULL, offsetof(struct type, field), 0, false, read_only); \
}

#define LUASTRUCT_NEW_BOOL_FIELD(state, type, field, read_only) { \
	{ struct type; } \
	luastruct_new_struct_field(state, #field, LUAS_TYPE_BOOL, NULL, offsetof(struct type, field), 0, false, read_only); \
}

#define LUASTRUCT_NEW_OBJECT(state, type, data, read_only) { \
	{ struct type; } \
	luastruct_new_object(state, #type, (void *)data, read_only); \
}

#endif
