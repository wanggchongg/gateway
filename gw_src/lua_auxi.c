#include "lua_auxi.h"

/**
 * [lerror, lua错误提醒函数,如果有错误则返回错误信息,类似于fprintf函数]
 * @param  L   [lua状态]
 * @param  fmt [打印格式]
 * @return     [0]
 */
int lerror(lua_State *L, uint8_t *fmt, ...)
{
	va_list argp;   //argp是一个字符型指针
	va_start(argp, fmt);  //此时，argp指向第一个可变参数
	vfprintf(stderr, fmt, argp);
	fprintf(stderr, "\n");
	va_end(argp);
	
	return 0;
}

/**
 * [getField_sn 从table中取出字符串键key对应的整数值]
 * @param  L      [lua状态]
 * @param  key    [字符串键]
 * @param  result [key对应的整数值]
 * @return        [0]
 */
int getField_sn(lua_State *L, const uint8_t *key, int *result)
{
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if(!lua_isnumber(L, -1))
		luaL_error(L, "getField_sn error: result isn't a number");
	*result = lua_tointeger(L, -1);
	lua_pop(L, 1);

	return 0;
}

/**
 * [getField_ss 从table中取出字符串键key对应的字符串值]
 * @param  L      [lua状态]
 * @param  key    [字符串键]
 * @param  result [key对应的字符串值]
 * @return        [0]
 */
int getField_ss(lua_State *L, const uint8_t *key, uint8_t *result)
{
	lua_pushstring(L, key);
	lua_gettable(L, -2);
	if(!lua_isstring(L, -1))
		luaL_error(L, "getField_ss error: result isn't a string");
	strcpy(result, lua_tostring(L, -1));
	lua_pop(L, 1);

	return 0;
}

/**
 * [getField_s 从table中取出整数键key对应的字符串值]
 * @param  L      [lua状态]
 * @param  key    [整数键]
 * @param  result [key对应的字符串值]
 * @return        [0]
 */
int getField_ns(lua_State *L, int key, uint8_t *result)
{
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isstring(L, -1))
		lerror(L, "not a string");
	strcpy(result, lua_tostring(L, -1));
	lua_pop(L, 1);

	return 0;
}

/**
 * [getField_nn 从table中取出整数键key对应的整数值]
 * @param  L      [lua状态]
 * @param  key    [整数键]
 * @param  result [key对应的整数值]
 * @return        [0]
 */
int getField_nn(lua_State *L, int key, int *result)
{
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if(!lua_isnumber(L, -1))
		luaL_error(L, "getField_nn error: result isn't a number");
	*result = lua_tointeger(L, -1);
	lua_pop(L, 1);
	
	return 0;
}

/**
 * [lrawgeti 从Lua栈的table中，取出与key关联的value值;此方法是原始模式,不会激发任何元方法,有可能有些值不能取出]
 * @param L       [lua状态]
 * @param key     [整数键]
 * @param nresult [key对应的整数值]
 * @param sresult [key对应的字符串值]
 */
void lrawgeti(lua_State *L, int key, int *nresult, uint8_t *sresult) // The access is raw; that is, it does not invoke metamethods.
{
	lua_rawgeti(L, -1, key);
	if(lua_isnumber(L, -1))
	{
		*nresult = lua_tointeger(L, -1);
		// printf("nresult: %d\n", *nresult);
	}
	else if(lua_isstring(L, -1))
	{
		strcpy(sresult, lua_tostring(L, -1));
		// printf("sresult: %s\n", sresult);
	}
	else
	{
		luaL_error(L, "lrawgeti error: datatype error");
	}
	lua_pop(L, 1);
}
