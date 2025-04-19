#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luastruct.h"

#define DEBUG

#define LUAS_OBJECT_KEY_FORMAT "%.8X_%s"

const char *objects_registry_name = "luastruct_objects";
const char *types_registry_name = "luastruct_types";
const char *struct_metatable_name = "luastruct_struct_metatable";
const char *object_metatable_name = "luastruct_object";

#define LUAS_DEBUG_MSG(...) \
    do { \
        fprintf(stderr, __FILE__ ":" STR(__LINE__) ": " __VA_ARGS__); \
    } while(false)

static void free_struct_fields_recursively(LuastructStructField *field) {
    if(field->next_by_offset) {
        free_struct_fields_recursively(field->next_by_offset);
    }
    free(field);
}

static void insert_struct_field(LuastructStruct *st, const LuastructStructField *field) {
    LuastructStructField *new_field = malloc(sizeof(LuastructStructField));
    memcpy(new_field, field, sizeof(LuastructStructField));
    new_field->next_by_offset = NULL;
    new_field->next_by_name = NULL;
    
    // Insert by offset
    LuastructStructField *prev = NULL;
    LuastructStructField *current = st->fields;
    while(current && current->offset < new_field->offset) {
        prev = current;
        current = current->next_by_offset;
    }
    if(prev) {
        prev->next_by_offset = new_field;
    } 
    else {
        st->fields = new_field;
    }
    new_field->next_by_offset = current;
    
    // Insert by name
    prev = NULL;
    current = st->fields_by_name;
    while(current && strcmp(current->field_name, new_field->field_name) < 0) {
        prev = current;
        current = current->next_by_name;
    }
    if(prev) {
        prev->next_by_name = new_field;
    }
    else {
        st->fields_by_name = new_field;
    }
    new_field->next_by_name = current;
}

int luastruct_get_type(lua_State *state, const char *name) {
    luastruct_get_types_registry(state);
    lua_getfield(state, -1, name);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 2);
        return 0;
    }
    lua_remove(state, -2);
    return 1;
}

static LuastructTypeInfo *get_type_info(lua_State *state, LuastructType type, const char *type_name) {
    switch(type) {
        case LUAS_TYPE_STRUCT:
        case LUAS_TYPE_ENUM:
        case LUAS_TYPE_DYNARRAY:
            luastruct_get_type(state, type_name);
            if(lua_isnil(state, -1)) {
                return NULL;
            }
            return lua_touserdata(state, -1);
        default:
            return NULL;
    }
}

int luastruct_get_types_registry(lua_State *state) {
    lua_getfield(state, LUA_REGISTRYINDEX, types_registry_name);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, types_registry_name);
        lua_getfield(state, LUA_REGISTRYINDEX, types_registry_name);
    }
    return 1;
}

int luastruct_struct__gc(lua_State *state) {
    LuastructStruct *st = luaL_checkudata(state, 1, struct_metatable_name);
    if(!st) {
        return luaL_error(state, "Invalid struct object");
    }
    if(st->fields) {
        free_struct_fields_recursively(st->fields);
    }
    return 0;
}

int luastruct_new_struct(lua_State *state, const char *name, const char *super_name, uint32_t size) {
    if(luastruct_get_type(state, name) != 0) {
        return 1;
    }

    LuastructStruct *super = NULL;
    if(super_name) {
        if(luastruct_get_type(state, super_name) == 0) {
            return luaL_error(state, "Super struct type does not exist: %s", super_name);
        }
        super = luaL_checkudata(state, -1, struct_metatable_name);
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        return luaL_error(state, "Type name too long: %s", name);
    }

    LuastructStruct *st = lua_newuserdata(state, sizeof(LuastructStruct));
    strncpy(st->type_info.name, name, LUASTRUCT_TYPENAME_LENGTH);
    st->type_info.name[strlen(st->type_info.name)] = '\0';
    st->type_info.type = LUAS_TYPE_STRUCT;
    st->super = super;
    st->fields_by_name = NULL;
    st->fields = NULL;
    st->size = size;

    int metatable = luaL_newmetatable(state, struct_metatable_name);
    if(metatable != 0) {
        lua_pushcfunction(state, luastruct_struct__gc);
        lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    luastruct_get_types_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, st->type_info.name);
    lua_pop(state, 1);

    return 1;
}

LuastructStruct *luastruct_check_struct(lua_State *state, int index) {
    return luaL_checkudata(state, index, struct_metatable_name);
}

int luastruct_new_dynamic_array_type(lua_State *state, LuastructType type, const char *type_name, lua_CFunction elements_count_getter, bool elements_are_pointers) {
    char name[LUASTRUCT_TYPENAME_LENGTH];
    snprintf(name, sizeof(name), LUASTRUCT_DYNAMIC_ARRAY_NAME_FORMAT, type_name);
    if(luastruct_get_type(state, name) != 0) {
        return 1;
    }

    LuastructDynamicArray *da = lua_newuserdata(state, sizeof(LuastructDynamicArray));
    strncpy(da->type_info.name, name, LUASTRUCT_TYPENAME_LENGTH);
    da->type_info.name[strlen(da->type_info.name)] = '\0';
    da->type_info.type = LUAS_TYPE_DYNARRAY;
    da->elements_type = type;
    da->elements_type_info = get_type_info(state, type, type_name);
    da->elements_are_pointers = elements_are_pointers;
    da->count_getter = elements_count_getter;

    luastruct_get_types_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, da->type_info.name);
    lua_pop(state, 1);

    return 1;
}

void luastruct_new_struct_field(lua_State *state, const char *name, LuastructType type, const char *type_name, uint32_t offset, uint32_t count, bool pointer, bool readonly) {
    LuastructStruct *st = luastruct_check_struct(state, -1);
    if(!st) {
        luaL_error(state, "Invalid struct object");
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        luaL_error(state, "Field name too long: %s", name);
    }

    LuastructStructField field;
    strncpy(field.field_name, name, LUASTRUCT_TYPENAME_LENGTH);
    field.field_name[strlen(field.field_name)] = '\0';
    field.type = type;
    field.type_info = NULL;
    field.offset = offset;
    field.count = count;
    field.pointer = pointer;
    field.readonly = readonly;

    if(type == LUAS_TYPE_STRUCT || type == LUAS_TYPE_ENUM) {
        if(!type_name) {
            luaL_error(state, "Type name required for struct/enum field");
        }
        luastruct_get_type(state, type_name);
        if(lua_isnil(state, -1)) {
            luaL_error(state, "Type not found: %s", type_name);
        }
        field.type_info = lua_touserdata(state, -1);
        lua_pop(state, 1);
    }

    insert_struct_field(st, &field);
}

void luastruct_new_struct_bit_field(lua_State *state, const char *name, LuastructType type, uint32_t offset, uint32_t bit_offset, bool pointer, bool readonly) {
    LuastructStruct *st = luastruct_check_struct(state, -1);
    if(!st) {
        luaL_error(state, "Invalid struct object");
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        luaL_error(state, "Field name too long: %s", name);
    }

    uint32_t size = 0;
    switch(type) {
        case LUAS_TYPE_INT8:
        case LUAS_TYPE_UINT8:
            size = 1;
            break;
        case LUAS_TYPE_INT16:
        case LUAS_TYPE_UINT16:
            size = 2;
            break;
        case LUAS_TYPE_INT32:
        case LUAS_TYPE_UINT32:
            size = 4;
            break;
        default:
            luaL_error(state, "Invalid bitfield type: %d", type);
    }
    if(bit_offset >= size * 8) {
        luaL_error(state, "Bit offset out of range: %d", bit_offset);
    }

    LuastructStructField field;
    strncpy(field.field_name, name, LUASTRUCT_TYPENAME_LENGTH);
    field.field_name[strlen(field.field_name)] = '\0';
    field.type = LUAS_TYPE_BITFIELD;
    field.type_info = NULL;
    field.offset = offset;
    field.count = 0;
    field.pointer = pointer;
    field.readonly = readonly;
    field.bitfield.size = size;
    field.bitfield.offset = bit_offset;

    insert_struct_field(st, &field);
}

void luastruct_new_struct_dynamic_array_field(lua_State *state, const char *name, LuastructType type, const char *type_name, uint32_t offset, bool pointer, bool readonly, bool elements_are_readonly) {
    LuastructStruct *st = luastruct_check_struct(state, -1);
    if(!st) {
        luaL_error(state, "Invalid struct object");
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        luaL_error(state, "Field name too long: %s", name);
    }

    char da_type_name[LUASTRUCT_TYPENAME_LENGTH];
    snprintf(da_type_name, sizeof(da_type_name), LUASTRUCT_DYNAMIC_ARRAY_NAME_FORMAT, type_name);
    luastruct_get_type(state, da_type_name);
    if(lua_isnil(state, -1)) {
        luaL_error(state, "Dynamic array type not found: %s", da_type_name);
    }
    LuastructTypeInfo *da_type_info = lua_touserdata(state, -1);
    lua_pop(state, 1);

    LuastructStructField field;
    strncpy(field.field_name, name, LUASTRUCT_TYPENAME_LENGTH);
    field.field_name[strlen(field.field_name)] = '\0';
    field.type = LUAS_TYPE_DYNARRAY;
    field.type_info = da_type_info;
    field.offset = offset;
    field.count = 0;
    field.pointer = pointer;
    field.readonly = readonly;
    field.dynamic_array.elements_are_readonly = elements_are_readonly;

    insert_struct_field(st, &field);
}

int luastruct_get_objects_registry(lua_State *state) {
    lua_getfield(state, LUA_REGISTRYINDEX, objects_registry_name);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, objects_registry_name);
        lua_getfield(state, LUA_REGISTRYINDEX, objects_registry_name);

        /**
         * Set the metatable for the objects registry to allow weak references.
         * This allows the Lua garbage collector to collect the objects
         * when there are no strong references to them.
         */
        lua_newtable(state);
        lua_pushstring(state, "v");
        lua_setfield(state, -2, "__mode");
        lua_setmetatable(state, -2);
    }
    return 1;
}

static void get_object_key(char *buffer, void *data, bool readonly) {
    snprintf(buffer, LUASTRUCT_TYPENAME_LENGTH, LUAS_OBJECT_KEY_FORMAT, data, readonly ? "readonly" : "readwrite");
}

int luastruct_get_object(lua_State *state, void *data, bool readonly) {
    luastruct_get_objects_registry(state);
    char key[LUASTRUCT_TYPENAME_LENGTH];
    get_object_key(key, data, readonly);
    lua_getfield(state, -1, key);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 2);
        return 0;
    }
    lua_remove(state, -2);
    return 1;
}

int luastruct_object__gc(lua_State *state) {
    LuastructObject *obj = luaL_checkudata(state, 1, object_metatable_name);
    LuastructTypeInfo *type_info = obj->type;
    LUAS_DEBUG_MSG("Collecting object 0x%.8X of type \"%s\"\n", obj->data, type_info->name);
    if(!obj) {
        return luaL_error(state, "Object is NULL in __gc method");
    }
    if(obj->delete_on_gc) {
        free(obj->data);
    }
    return 0;
}

int luastruct_object__index(lua_State *state) {
    LuastructObject *obj = luaL_checkudata(state, 1, object_metatable_name);
    
    if(!obj || obj->invalid) {
        return luaL_error(state, "Object is invalid in __index method");
    }

    const char *field_name = luaL_checkstring(state, 2);
    LuastructStruct *st = obj->type;
    LUAS_DEBUG_MSG("Accessing field \"%s\" of struct at 0x%.8X (%s) of type \"%s\"\n", field_name, obj->data, obj->readonly ? "ro" : "rw", st->type_info.name);
    
    LuastructStructField *field = st->fields_by_name;
    while(field) {
        if(strcmp(field->field_name, field_name) == 0) {
            void *data = obj->data + field->offset;
            bool readonly = obj->readonly || field->readonly;
            if(field->pointer) {
                data = *(void **)data;
            }
            switch(field->type) {
                case LUAS_TYPE_INT8:
                    lua_pushinteger(state, *(int8_t *)(data));
                    break;
                case LUAS_TYPE_INT16:
                    lua_pushinteger(state, *(int16_t *)(data));
                    break;
                case LUAS_TYPE_INT32:
                    lua_pushinteger(state, *(int32_t *)(data));
                    break;
                case LUAS_TYPE_INT64:
                    lua_pushinteger(state, *(int64_t *)(data));
                    break;
                case LUAS_TYPE_UINT8:
                    lua_pushinteger(state, *(uint8_t *)(data));
                    break;
                case LUAS_TYPE_UINT16:
                    lua_pushinteger(state, *(uint16_t *)(data));
                    break;
                case LUAS_TYPE_UINT32:
                    lua_pushinteger(state, *(uint32_t *)(data));
                    break;
                case LUAS_TYPE_FLOAT:
                    lua_pushnumber(state, *(float *)(data));
                    break;
                case LUAS_TYPE_BOOL:
                    lua_pushboolean(state, *(bool *)(data));
                    break;
                case LUAS_TYPE_STRUCT:
                case LUAS_TYPE_ENUM:
                case LUAS_TYPE_DYNARRAY:
                    luastruct_new_object(state, ((LuastructTypeInfo *)field->type_info)->name, data, readonly);
                    break;
                case LUAS_TYPE_BITFIELD: {
                    switch(field->bitfield.size) {
                        case 1:
                            lua_pushinteger(state, (*(uint8_t *)(data) >> field->bitfield.offset) & 1);
                            break;
                        case 2:
                            lua_pushinteger(state, (*(uint16_t *)(data) >> field->bitfield.offset) & 1);
                            break;
                        case 4:
                            lua_pushinteger(state, (*(uint32_t *)(data) >> field->bitfield.offset) & 1);
                            break;
                        default:
                            return luaL_error(state, "Invalid bitfield size: %d", field->bitfield.size);
                    }
                    break;
                }
                default:
                    return luaL_error(state, "Unknown field type: %d", field->type);
            }
            break;
        }
        field = field->next_by_name;
    }
    return 1;
}

int luastruct_object__newindex(lua_State *state) {
    LuastructObject *obj = luaL_checkudata(state, 1, object_metatable_name);
    
    if(!obj || obj->invalid) {
        return luaL_error(state, "Object is invalid in __newindex method");
    }
    if(obj->readonly) {
        return luaL_error(state, "Object is read-only in __newindex method");
    }

    const char *field_name = luaL_checkstring(state, 2);
    LuastructStruct *st = obj->type;
    LUAS_DEBUG_MSG("Setting field \"%s\" of struct at 0x%.8X (%s) of type \"%s\"\n", field_name, obj->data, obj->readonly ? "ro" : "rw", st->type_info.name);

    LuastructStructField *field = st->fields_by_name;
    while(field) {
        if(strcmp(field->field_name, field_name) == 0) {
            if(field->readonly) {
                return luaL_error(state, "Field is read-only: %s", field_name);
            }
            void *data = obj->data + field->offset;
            if(field->pointer) {
                data = *(void **)data;
            }
            switch(field->type) {
                case LUAS_TYPE_INT8:
                    *(int8_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_INT16:
                    *(int16_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_INT32:
                    *(int32_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_UINT8:
                    *(uint8_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_UINT16:
                    *(uint16_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_UINT32:
                    *(uint32_t *)(data) = luaL_checkinteger(state, 3);
                    break;
                case LUAS_TYPE_FLOAT:
                    *(float *)(data) = luaL_checknumber(state, 3);
                    break;
                case LUAS_TYPE_BOOL:
                    *(bool *)(data) = lua_toboolean(state, 3);
                    break;
                case LUAS_TYPE_STRUCT: {
                    LuastructObject *obj_to_copy = luaL_checkudata(state, 3, struct_metatable_name);
                    LuastructStruct *field_struct = field->type_info;
                    if(!obj_to_copy || obj_to_copy->invalid) {
                        return luaL_error(state, "Object to copy is invalid");
                    }
                    if(obj_to_copy->type != field->type_info) {
                        LuastructTypeInfo *obj_type_info = obj_to_copy->type;
                        LuastructTypeInfo *field_type_info = field->type_info;
                        return luaL_error(state, "Invalid object type to copy: %s != %s", obj_type_info->name, field_type_info->name);
                    }
                    memcpy(data, obj_to_copy->data, field_struct->size);
                    break;
                }
                case LUAS_TYPE_ENUM: {
                    LuastructEnum *enum_type = field->type_info;
                    switch(enum_type->type) {
                        case LUAS_ENUM_INT8:
                            *(int8_t *)(data) = luaL_checkinteger(state, 3);
                            break;
                        case LUAS_ENUM_INT16:
                            *(int16_t *)(data) = luaL_checkinteger(state, 3);
                            break;
                        case LUAS_ENUM_INT32:
                            *(int32_t *)(data) = luaL_checkinteger(state, 3);
                            break;
                        default:
                            return luaL_error(state, "Invalid enum type");
                    }
                    break;
                }
                case LUAS_TYPE_DYNARRAY:
                    *(void **)(data) = lua_touserdata(state, 3);
                    break;
                case LUAS_TYPE_BITFIELD: {
                    switch(field->bitfield.size) {
                        case 1:
                            *(uint8_t *)(data) = (*(uint8_t *)(data) & ~(1 << field->bitfield.offset)) | (lua_toboolean(state, 3) << field->bitfield.offset);
                            break;
                        case 2:
                            *(uint16_t *)(data) = (*(uint16_t *)(data) & ~(1 << field->bitfield.offset)) | (lua_toboolean(state, 3) << field->bitfield.offset);
                            break;
                        case 4:
                            *(uint32_t *)(data) = (*(uint32_t *)(data) & ~(1 << field->bitfield.offset)) | (lua_toboolean(state, 3) << field->bitfield.offset);
                            break;
                        default:
                            return luaL_error(state, "Invalid bitfield size: %d", field->bitfield.size);
                    }
                    break;
                }
                default:
                    return luaL_error(state, "Unknown field type: %d", field->type);
            }
        }
        field = field->next_by_name;
    }
    return 0;
}

static const struct luaL_Reg luastruct_object_metatable_methods[] = {
    {"__gc", luastruct_object__gc},
    {"__index", luastruct_object__index},
    {"__newindex", luastruct_object__newindex},
    {NULL, NULL}
};

int luastruct_new_object(lua_State *state, const char *type_name, void *data, bool readonly) {
    if(luastruct_get_type(state, type_name) == 0) {
        return luaL_error(state, "Type not found: %s", type_name);
    }
    LuastructTypeInfo *type_info = lua_touserdata(state, -1);
    lua_pop(state, 1);

    if(type_info->type != LUAS_TYPE_STRUCT && type_info->type != LUAS_TYPE_ENUM) {
        return luaL_error(state, "Invalid type for object: %s", type_name);
    }

    if(luastruct_get_object(state, data, readonly) != 0) {
        LuastructObject *obj = lua_touserdata(state, -1);
        LuastructTypeInfo *obj_type_info = obj->type;
        if(!obj->invalid) {
            if(obj_type_info->type != type_info->type) {
                return luaL_error(state, "An object of a different type already exists at this address: %s != %s", obj_type_info->name, type_info->name);
            }
            LUAS_DEBUG_MSG("Using existing object of type \"%s\" at 0x%.8X (%s)\n", obj_type_info->name, data, readonly ? "ro" : "rw");
            return 1;
        }
    }

    LUAS_DEBUG_MSG("Creating object of type \"%s\" at 0x%.8X (%s)\n", type_name, data, readonly ? "ro" : "rw");

    LuastructObject *obj = lua_newuserdata(state, sizeof(LuastructObject));
    obj->type = type_info;
    obj->invalid = false;
    obj->readonly = readonly;
    if(data) {
        obj->data = data;
        obj->delete_on_gc = false;
    }
    else {
        obj->delete_on_gc = true;
        switch(type_info->type) {
            case LUAS_TYPE_STRUCT:
                obj->data = malloc(((LuastructStruct *)type_info)->size);
                break;
            case LUAS_TYPE_ENUM: {
                LuastructEnum *enum_type = (LuastructEnum *)type_info;
                switch(enum_type->type) {
                    case LUAS_ENUM_INT8:
                        obj->data = malloc(sizeof(int8_t));
                        break;
                    case LUAS_ENUM_INT16:
                        obj->data = malloc(sizeof(int16_t));
                        break;
                    case LUAS_ENUM_INT32:
                        obj->data = malloc(sizeof(int32_t));
                        break;
                    default:
                        return luaL_error(state, "Invalid enum type");
                }
                break;
            }
            case LUAS_TYPE_DYNARRAY:
                return luaL_error(state, "Dynamic array objects cannot be created without existing data");
            default:
                return luaL_error(state, "Invalid type for object: %s", type_name);
        }
    }

    if(!obj->data) {
        return luaL_error(state, "Failed to allocate memory for object");
    }

    if(luaL_newmetatable(state, object_metatable_name) != 0) {
        luaL_setfuncs(state, luastruct_object_metatable_methods, 0);
    }
    lua_setmetatable(state, -2);
    
    char object_key[LUASTRUCT_TYPENAME_LENGTH];
    get_object_key(object_key, data, readonly);
    luastruct_get_objects_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, object_key);
    lua_pop(state, 1);

    return 1;
}
