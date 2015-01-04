#ifndef _LUA_H_
#define _LUA_H_
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

int exTemperature(lua_State *L);
int exOldTemperature(lua_State *L);//温度转换
int exHumidity(lua_State *L);
int exOldHumidity(lua_State *L);//湿度转换公式
int exVoltage(lua_State *L);
int exLight(lua_State *L);
int exSalt(lua_State *L);
int exShock(lua_State *L);
