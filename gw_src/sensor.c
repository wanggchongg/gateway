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
int exOldTemperature(lua_State *L)//老传感器温度转换
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
int exOldHumidity(lua_State *L)//老传感器湿度转换公式
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

/**
 * [exHumidityInSalt description]
 * @param  L [description]
 * @return   [description]
 */
int exHumidityInSalt(lua_State *L)
{
	int nHum = 0;
	double fHum = 0;
	nHum = luaL_checkint(L, 1);

	fHum = 1.9-nHum/((pow(2, 12)-1)*5.2)*2.25;
	fHum = fHum/100;
	if(fHum<0)
		fHum = 0;
	if(fHum>1)
		fHum = 1;

	lua_pushnumber(L, fHum);
	return 0;
}

/**
 * [exSalt_v description]
 * @param  L [description]
 * @return   [description]
 */
int exSalt_v(lua_State *L) //解析垂直盐分
{
    int v1n = 0;
    double tem = 0;
    double hum = 0;
    double verticalSalt = 0;
    double v1 = 0, ECa1 = 0, ECa10 = 0, fun1 = 0, ECw10 = 0;

    v1n = luaL_checkint(L, 1); //垂直盐分测量值
    tem = luaL_checknumber(L, 2); //实际温度值
    hum = luaL_checknumber(L, 3); //实际湿度值

    v1=1000*(0.44381-v1n/((pow(2,12)-1)*5.2)*2.25);
    ECa1=25000/(2*3.14*6.5*v1);
    ECa10=ECa1*(0.4470+1.4034*exp((-1)*tem/26.815));

    fun1=(-2.2556)*hum*hum+1.9135*hum+0.0791;
    if(fabs(fun1-0)<=1e-8)
    {
        ECw10=0;
    }
    else
    {
        ECw10=ECa10/fun1;
    }
    verticalSalt=ECw10;

    lua_pushnumber(L, verticalSalt);
	return 0;
}

/**
 * [exSalt_s description]
 * @param  L [description]
 * @return   [description]
 */
int exSalt_s(lua_State *L) //解析表面盐分
{
	int v1n = 0;
	double tem = 0;
	double hum = 0;
	double surfaceSalt = 0;
	double v2 = 0, ECa2 = 0, ECa20 = 0, fun2 = 0, ECw20 = 0;

 	v1n = luaL_checkint(L, 1); //表面盐分测量值
 	tem = luaL_checknumber(L, 2); //实际温度值
 	hum = luaL_checknumber(L, 3); //实际湿度值

    v2=1000*(0.44381-v1n/((pow(2,12)-1)*5.2)*2.25);
    ECa2=25000/(2*3.14*6.5*v2);//之前这里是v1
    ECa20=ECa2*(0.4470+1.4034*exp((-1)*tem/26.815));

    fun2=(-1.4731)*hum*hum+1.4124*hum-0.0189;
    if(fabs(fun2-0)<=1e-8)
    {
        ECw20=0;
    }
    else
    {
        ECw20=ECa20/fun2;
    }
    surfaceSalt=ECw20;

    lua_pushnumber(L, surfaceSalt);
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


