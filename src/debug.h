#ifndef LUASTRUCT_DEBUG_H
#define LUASTRUCT_DEBUG_H

#ifndef __cplusplus
#include <lua.h>
#else
#include <lua.hpp>
extern "C" {
#endif

/**
 * Print to stdout the registered types in the Lua state.
 * @param state Lua state.
 */
void luastruct_print_registered_types(lua_State *state);

/**
 * Print to stdout the definition of a struct type.
 * @param state Lua state.
 * @param name Name of the struct type.
 */
void luastruct_print_struct_definition(lua_State *state, const char *name);

#ifdef __cplusplus
}
#endif

#endif 
