#ifndef _LUA_AUXI_H_
#define _LUA_AUXI_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef _LUA_H_
#define _LUA_H_
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

/**
 * [lerror, lua错误提醒函数,如果有错误则返回错误信息,类似于fprintf函数]
 * @param  L   [lua状态]
 * @param  fmt [打印格式]
 * @return     [0]
 */
int lerror(lua_State *L, uint8_t *fmt, ...);

/**
 * [getField_sn 从table中取出字符串键key对应的整数值值]
 * @param  L      [lua状态]
 * @param  key    [字符串键]
 * @param  result [key对应的整数值]
 * @return        [0]
 */
int getField_sn(lua_State *L, const uint8_t *key, int *result);

/**
 * [getField_ss 从table中取出字符串键key对应的字符串值]
 * @param  L      [lua状态]
 * @param  key    [字符串键]
 * @param  result [key对应的字符串值]
 * @return        [0]
 */
int getField_ss(lua_State *L, const uint8_t *key, uint8_t *result);

/**
 * [getField_s 从table中取出整数键key对应的字符串值]
 * @param  L      [lua状态]
 * @param  key    [整数键]
 * @param  result [key对应的字符串值]
 * @return        [0]
 */
int getField_ns(lua_State *L, int key, uint8_t *result);

/**
 * [getField_nn 从table中取出整数键key对应的整数值]
 * @param  L      [lua状态]
 * @param  key    [整数键]
 * @param  result [key对应的整数值]
 * @return        [0]
 */
int getField_nn(lua_State *L, int key, int *result);

/**
 * [lrawgeti 从Lua栈的table中，取出与key关联的value值]
 * @param L       [Lua状态]
 * @param key     [table的键值]
 * @param nresult [value为int类型时]
 * @param sresult [value为string类型时]
 */
void lrawgeti(lua_State *L, int key, int *nresult, uint8_t *sresult);

#endif
