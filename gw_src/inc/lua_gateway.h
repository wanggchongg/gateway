#ifndef _LUA_GATEWAY_H_
#define _LUA_GATEWAY_H_
#include "gateway.h"
#include "sensor.h"

/**
 * [lregister 向Lua虚拟机中注册C库函数]
 * @param L [Lua状态]
 */
extern void lregister(lua_State *L);

#endif
