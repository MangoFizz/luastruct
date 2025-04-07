#ifndef LUASTRUCT_DEBUG_H
#define LUASTRUCT_DEBUG_H

#ifndef __cplusplus
#include <lua.h>
#else
#include <lua.hpp>
extern "C" {
#endif

void luasd_print_registered_types(lua_State *state);
void luasd_print_struct_fields(lua_State *state, const char *name);

#ifdef __cplusplus
}
#endif

#endif 
