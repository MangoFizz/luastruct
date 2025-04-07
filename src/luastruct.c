#include <stdlib.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "luastruct.h"

#define LUAS_OBJECT_KEY_FORMAT "%.8X_%s"

const char *objects_registry_name = "luastruct_objects";
const char *types_registry_name = "luastruct_types";
const char *struct_metatable_name = "luas_struct_metatable";

#define LUAS_DEBUG(...) \
    do { \
        luaL_error(state, __FILE__ ":" STR(__LINE__) ": " __VA_ARGS__); \
    } while(false)

#define LUAS_ASSERT(expr) \
    do { \
        if(!(expr)) { \
            LUAS_DEBUG("Assertion failed: %s", #expr); \
            return false; \
        } \
    } while(false)

static void free_struct_fields_recursively(LuasStructField *field) {
    if(field->next_by_offset) {
        free_struct_fields_recursively(field->next_by_offset);
    }
    free(field);
}

static void insert_struct_field(LuasStruct *st, const LuasStructField *field) {
    LuasStructField *new_field = malloc(sizeof(LuasStructField));
    memcpy(new_field, field, sizeof(LuasStructField));
    new_field->next_by_offset = NULL;
    new_field->next_by_name = NULL;
    
    // Insert by offset
    LuasStructField *prev = NULL;
    LuasStructField *current = st->fields;
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

int luas_get_type(lua_State *state, const char *name) {
    luas_get_types_registry(state);
    lua_getfield(state, -1, name);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 2);
        return 0;
    }
    lua_remove(state, -2);
    return 1;
}

static LuasTypeInfo *get_type_info(lua_State *state, LuasType type, const char *type_name) {
    switch(type) {
        case LUAS_TYPE_STRUCT:
        case LUAS_TYPE_ENUM:
        case LUAS_TYPE_DYNARRAY:
            luas_get_type(state, type_name);
            if(lua_isnil(state, -1)) {
                return NULL;
            }
            return lua_touserdata(state, -1);
        default:
            return NULL;
    }
}

int luas_get_types_registry(lua_State *state) {
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

int luas_struct__gc(lua_State *state) {
    LuasStruct *st = luaL_checkudata(state, 1, struct_metatable_name);
    if(!st) {
        LUAS_DEBUG("Invalid struct object");
        return 0;
    }
    if(st->fields) {
        free_struct_fields_recursively(st->fields);
    }
    return 0;
}

int luas_new_struct(lua_State *state, const char *name, const char *super_name, uint32_t size) {
    if(luas_get_type(state, name) != 0) {
        return 1;
    }

    LuasStruct *super = NULL;
    if(super_name) {
        if(luas_get_type(state, super_name) == 0) {
            LUAS_DEBUG("Super struct type does not exist: %s", super_name);
            return 0;
        }
        super = luaL_checkudata(state, -1, struct_metatable_name);
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        LUAS_DEBUG("Type name too long: %s", name);
        return 0;
    }

    LuasStruct *st = lua_newuserdata(state, sizeof(LuasStruct));
    strncpy(st->type_info.name, name, LUASTRUCT_TYPENAME_LENGTH);
    st->type_info.name[strlen(st->type_info.name)] = '\0';
    st->type_info.type = LUAS_TYPE_STRUCT;
    st->super = super;
    st->fields_by_name = NULL;
    st->fields = NULL;
    st->size = size;

    int metatable = luaL_newmetatable(state, struct_metatable_name);
    if(metatable != 0) {
        lua_pushcfunction(state, luas_struct__gc);
        lua_setfield(state, -2, "__gc");
    }
    lua_setmetatable(state, -2);

    luas_get_types_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, st->type_info.name);
    lua_pop(state, 1);

    return 1;
}

LuasStruct *luas_check_struct(lua_State *state, int index) {
    return luaL_checkudata(state, index, struct_metatable_name);
}

int luas_new_dynamic_array_type(lua_State *state, LuasType type, const char *type_name, lua_CFunction elements_count_getter, bool elements_are_pointers) {
    char name[LUASTRUCT_TYPENAME_LENGTH];
    snprintf(name, sizeof(name), LUASTRUCT_DYNAMIC_ARRAY_NAME_FORMAT, type_name);
    if(luas_get_type(state, name) != 0) {
        return 1;
    }

    LuasDynamicArray *da = lua_newuserdata(state, sizeof(LuasDynamicArray));
    strncpy(da->type_info.name, name, LUASTRUCT_TYPENAME_LENGTH);
    da->type_info.name[strlen(da->type_info.name)] = '\0';
    da->type_info.type = LUAS_TYPE_DYNARRAY;
    da->elements_type = type;
    da->elements_type_info = get_type_info(state, type, type_name);
    da->elements_are_pointers = elements_are_pointers;
    da->count_getter = elements_count_getter;

    luas_get_types_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, da->type_info.name);
    lua_pop(state, 1);

    return 1;
}

void luas_new_struct_field(lua_State *state, const char *name, LuasType type, const char *type_name, uint32_t offset, uint32_t count, bool pointer, bool readonly) {
    LuasStruct *st = luas_check_struct(state, -1);
    if(!st) {
        LUAS_DEBUG("Invalid struct object");
        return;
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        LUAS_DEBUG("Field name too long: %s", name);
        return;
    }

    LuasStructField field;
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
            LUAS_DEBUG("Type name required for struct/enum field");
            return;
        }
        luas_get_type(state, type_name);
        if(lua_isnil(state, -1)) {
            LUAS_DEBUG("Type not found: %s", type_name);
            return;
        }
        field.type_info = lua_touserdata(state, -1);
        lua_pop(state, 1);
    }

    insert_struct_field(st, &field);
}

void luas_new_struct_dynamic_array_field(lua_State *state, const char *name, LuasType type, const char *type_name, uint32_t offset, bool pointer, bool readonly, bool elements_are_readonly) {
    LuasStruct *st = luas_check_struct(state, -1);
    if(!st) {
        LUAS_DEBUG("Invalid struct object");
        return;
    }

    if(strlen(name) >= LUASTRUCT_TYPENAME_LENGTH) {
        LUAS_DEBUG("Field name too long: %s", name);
        return;
    }

    char da_type_name[LUASTRUCT_TYPENAME_LENGTH];
    snprintf(da_type_name, sizeof(da_type_name), LUASTRUCT_DYNAMIC_ARRAY_NAME_FORMAT, type_name);
    luas_get_type(state, da_type_name);
    if(lua_isnil(state, -1)) {
        LUAS_DEBUG("Dynamic array type not found: %s", da_type_name);
        return;
    }
    LuasTypeInfo *da_type_info = lua_touserdata(state, -1);
    lua_pop(state, 1);

    LuasStructField field;
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

int luas_get_objects_registry(lua_State *state) {
    lua_getfield(state, LUA_REGISTRYINDEX, objects_registry_name);
    if(lua_isnil(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
        lua_pushvalue(state, -1);
        lua_setfield(state, LUA_REGISTRYINDEX, objects_registry_name);
        lua_getfield(state, LUA_REGISTRYINDEX, objects_registry_name);
    }
    return 1;
}

static void get_object_key(char *buffer, void *data, bool readonly) {
    snprintf(buffer, LUASTRUCT_TYPENAME_LENGTH, LUAS_OBJECT_KEY_FORMAT, data, readonly ? "readonly" : "readwrite");
}

int luas_get_object(lua_State *state, void *data, bool readonly) {
    luas_get_objects_registry(state);
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

int luas_new_object(lua_State *state, const char *type_name, void *data, bool readonly) {
    if(luas_get_type(state, type_name) == 0) {
        LUAS_DEBUG("Type not found: %s", type_name);
        return 0;
    }
    LuasTypeInfo *type_info = lua_touserdata(state, -1);
    lua_pop(state, 1);

    if(luas_get_object(state, data, readonly) != 0) {
        LuasObject *obj = lua_touserdata(state, -1);
        LuasTypeInfo *obj_type_info = obj->type;
        if(obj_type_info->type != type_info->type) {
            LUAS_DEBUG("An object of a different type already exists at this address");
            return 0;
        }
        obj->invalid = false;
        return 1;
    }

    LuasObject *obj = lua_newuserdata(state, sizeof(LuasObject));
    obj->type = type_info;
    obj->invalid = false;
    obj->readonly = readonly;
    if(data) {
        obj->data = data;
        obj->delete_on_gc = false;
    }
    else {
        switch(type_info->type) {
            case LUAS_TYPE_STRUCT:
                obj->data = malloc(((LuasStruct *)type_info)->size);
                obj->delete_on_gc = true;
                break;
            case LUAS_TYPE_ENUM: {
                switch(((LuasEnum *)type_info)->type) {
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
                        LUAS_DEBUG("Invalid enum type");
                        return 0;
                }
                break;
            }
            case LUAS_TYPE_DYNARRAY:
                LUAS_DEBUG("Dynamic array objects cannot be created without existing data");
                return 0;
            default:
                LUAS_DEBUG("Invalid object type");
                return 0;
        }
    }
    
    char object_key[LUASTRUCT_TYPENAME_LENGTH];
    get_object_key(object_key, data, readonly);
    luas_get_objects_registry(state);
    lua_pushvalue(state, -2);
    lua_setfield(state, -2, object_key);
    lua_pop(state, 1);

    return 1;
}

int luas_object__gc(lua_State *state) {
    LuasObject *obj = luaL_checkudata(state, 1, struct_metatable_name);
    if(!obj) {
        LUAS_DEBUG("Invalid object");
        return 0;
    }
    if(obj->delete_on_gc) {
        free(obj->data);
    }
    return 0;
}

int luas_object_struct__index(lua_State *state) {
    LuasObject *obj = luaL_checkudata(state, 1, struct_metatable_name);
    if(!obj || obj->invalid) {
        LUAS_DEBUG("Invalid object");
        return 0;
    }

    const char *field_name = luaL_checkstring(state, 2);
    LuasStruct *st = obj->type;
    LuasStructField *field = st->fields_by_name;
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
                    luas_new_object(state, ((LuasTypeInfo *)field->type_info)->name, data, readonly);
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
                            LUAS_DEBUG("Invalid bitfield size: %d", field->bitfield.size);
                            break;
                    }
                    break;
                }
                default:
                    LUAS_DEBUG("Unknown field type: %d", field->type);
                    break;
            }
            break;
        }
        field = field->next_by_name;
    }
    return 1;
}

int luas_object_struct__newindex(lua_State *state) {
    LuasObject *obj = luaL_checkudata(state, 1, struct_metatable_name);
    if(!obj || obj->invalid) {
        LUAS_DEBUG("Invalid object");
        return 0;
    }
    if(obj->readonly) {
        LUAS_DEBUG("Object is read-only");
        return 0;
    }

    const char *field_name = luaL_checkstring(state, 2);
    LuasStruct *st = obj->type;
    LuasStructField *field = st->fields_by_name;
    while(field) {
        if(strcmp(field->field_name, field_name) == 0) {
            if(field->readonly) {
                LUAS_DEBUG("Field is read-only");
                return 0;
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
                    LuasObject *obj_to_copy = luaL_checkudata(state, 3, struct_metatable_name);
                    LuasStruct *field_struct = field->type_info;
                    if(!obj_to_copy || obj_to_copy->invalid) {
                        LUAS_DEBUG("Invalid object to copy");
                        break;
                    }
                    if(obj_to_copy->type != field->type_info) {
                        LUAS_DEBUG("Invalid object type to copy");
                        break;
                    }
                    memcpy(data, obj_to_copy->data, field_struct->size);
                    break;
                }
                case LUAS_TYPE_ENUM: {
                    LuasEnum *enum_type = field->type_info;
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
                            LUAS_DEBUG("Invalid enum type");
                            break;
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
                            LUAS_DEBUG("Invalid bitfield size: %d", field->bitfield.size);
                            break;
                    }
                    break;
                }
                default:
                    LUAS_DEBUG("Unknown field type: %d", field->type);
                    break;
            }
        }
        field = field->next_by_name;
    }
    return 0;
}
