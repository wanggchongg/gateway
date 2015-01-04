#include "sensor.h"
#include <stdio.h>
/**
 * [exTemperature description]
 * @param  nTem [description]
 * @return      [description]
 */
int exTemperature(lua_State *L)
{
	int nTem = 0;
	double fTem = 0;
	double temp = 0;

	nTem = luaL_checkint(L, 1);

	if(nTem == 0)
		fTem = 0;
	else
	{
	    temp = (double)nTem;
	    fTem = temp/10;
	}

    lua_pushnumber(L, fTem);
    return 0;
}

/**
 * [exTemperature1 description]
 * @param  L [description]
 * @return   [description]
 */
int exOldTemperature(lua_State *L)//温度转换
{
	int nTem = 0;
	double fTem = 0;

	nTem = luaL_checkint(L, 1);

    fTem = (double)nTem * 0.01 - 39.66;

    lua_pushnumber(L, fTem);
    return 0;
}

/**
 * [exHumidity description]
 * @param  nHum [description]
 * @return      [description]
 */
int exHumidity(lua_State *L)
{
	int nHum = 0;
	double A = 0;
	double Z = 0;
	double fHum= 0;

	nHum = luaL_checkint(L, 1);
	A = luaL_checknumber(L, 2);
	Z = luaL_checknumber(L, 3);

	fHum = (double)A * (nHum - Z);

	lua_pushnumber(L, fHum);
	return 0;
}

/**
 * [exHumidity1 description]
 * @param  L [description]
 * @return   [description]
 */
int exOldHumidity(lua_State *L)//湿度转换公式
{
	int nHum = 0;
	double fTem = 0;
	double fHum = 0;
	double C1 = -4.0, C2 = 0.0405, C3 = -0.0000028, T1 = 0.01, T2 = 0.00008;
	double rh_lin = 0;

	nHum = luaL_checkint(L, 1);
	fTem = luaL_checknumber(L, 2);
    
    rh_lin = C3 * nHum * nHum + C2 * nHum + C1;
    fHum = (fTem - 25) * (T1 + T2 * nHum) + rh_lin;
    if(fHum>100) fHum = 100;
    if(fHum<0.1) fHum = 0.1;

    lua_pushnumber(L, fHum);
    return 0;
}

/**
 * [exVoltage description]
 * @param  L [description]
 * @return   [description]
 */
int exVoltage(lua_State *L)//电压转换
{
	int nVol = 0;
	double fVol = 0;

	nVol = luaL_checkint(L, 1);

	if(nVol == 0)
		fVol = 0;
	else
    	fVol = 1252.352/nVol;

    lua_pushnumber(L, fVol);
    return 0;
}

/**
 * [exLight description]
 * @param  L [description]
 * @return   [description]
 */
int exLight(lua_State *L)//参数：转换前的电压，光照强度
{
	int nVol = 0;
	int nLig = 0;
	double fLig = 0;

	nVol = luaL_checkint(L, 1);
	nLig = luaL_checkint(L, 2);

	if(nVol == 0)
		fLig = 0;
	else
		fLig = nLig * (1252352/nVol)/1023;

	lua_pushnumber(L, fLig);
    return 0;
}

int exSalt(lua_State *L)
{
	return 0;
}

int exShock(lua_State *L)
{
	int nShock = 0;
	double fShock = 0;
	double temp = 0;

	nShock = luaL_checkint(L, 1);

	if(nShock == 0)
		fShock = 0;
	else
	{
		temp = nShock * 2.56 / 1024;
		fShock = 1.42 * temp + 0.004378;
		if(fShock - 2.56 > 1e-8)
			fShock = 2.56;
	}
	lua_pushnumber(L, fShock);
	return 0;
}


