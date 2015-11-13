#include "lua_gateway.h"

/**
 * [lASCIIToRTU 将十六进制的字符串转换为二进制数组数据]
 * @param  arg1 [string, 十六进制字符串]
 * @return      [buffer, 转换后的二进制数组数据]
 * @return      [number, 二进制数组数据的长度]
 */
static int lASCIIToRTU(lua_State *L)
{
	switch(ASCIIToRTU(L))
	{
		case -1:
			lerror(L, "\tfunction ASCIIToRTU error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction ASCIIToRTU error");
			break;
		default:
			break;
	}
	return 2;
}

/**
 * [lsetSLBuffer 重新设置二进制形式的串口报文]
 * @param  arg1 [buffer, 二进制比特串]
 * @param  arg2 [number, 二进制比特串长度]
 * @return      [boolean, 成功, true]
 */
static int lsetSLBuffer(lua_State *L)
{
	switch(setSLBuffer(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setSLBuffer error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setSLBuffer error:");
			break;
	}
	return 1;
}


/**
 * [lsetSLPacket 重新设置串口报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 * @return 		[boolean, true: success; false: failure]
 */
static int lsetSLPacket(lua_State *L)
{
	switch(setSLPacket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setSLPacket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setSLPacket error:");
			break;
	}
	return 1;
}


/**
 * [lsetGBRTPacket 重新设置IP端返回报文内容]
 * @param  arg1 [string, IP端报文十六进制字符串]
 * @return		[boolean, true: success; false: failure]
 */
static int lsetGBRTPacket(lua_State *L)
{
	switch(setGBRTPacket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setGBRTPacket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setGBRTPacket error:");
			break;
	}
	return 1;
}

/**
 * [lsetGBPacket 重新设置IP报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 * @return 		[boolean, true: success; false: failure]
 */
static int lsetGBPacket(lua_State *L)
{
	switch(setGBPacket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setGBPacket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setGBPacket error:");
			break;
	}
	return 1;
}

/**
 * [lgetCommandArgs 获取命令行参数]
 * @param  arg1 [string, 命令参数名: "GATEWAY_NO", 网关号; "OPTION", 选择本地读还是串口读]
 * @return   	[string, arg1对应的命令参数值]
 */
static int lgetCommandArgs(lua_State *L)
{
	switch(getCommandArgs(L))
	{
		case -1:
			lerror(L, "\tfunction getCommandArgs error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction getCommandArgs error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [ldefPacketFormat, 由Lua脚本定义报文的格式]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string/nil, string:标注报文格式的文件路径; nil:标注报文格式的文件为当前文件]
 * @return		[boolean, true: success; false: failure]
 */
static int ldefPacketFormat(lua_State *L)
{
	switch(defPacketFormat(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction defPacketFormat error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction defPacketFormat error:");
			break;
	}
	return 1;
}

/**
 * [lrecvPacFromSocket, 从socket中接收报文, 并存至预设的IP缓冲区中, 该函数会建立tcp或udp服务器线程]
 * @param  arg1 [string/nil, nil: 服务器端(arm板)内核自动选择IP地址(如果有多个网卡); string: 人工指定服务器端(arm板)进程的IP地址]
 * @param  arg2 [string/nil, nil: 默认服务器端(arm板)的端口号为7777; string: 人工指定服务器端(arm板)进程的端口号]
 * @param  arg3 [number, 0: 使用TCP接收; 非0数字: 使用UDP接收]
 * @param  arg4 [userdata:BUFFET_t, 自定义的含有信号量机制的缓冲区类型]
 * @return		[boolean, true: 成功; false: 失败]
 */
static int lrecvPacFromSocket(lua_State *L)
{
	switch(recvPacFromSocket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction recvPacFromSocket error");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction recvPacFromSocket error");
			break;
	}
	return 1;
}

/** [lrecvPacFromIPBuf 从IP缓冲区中接收报文]
 * @param  arg1 [number, 0: 接收可打印的ASCII报文; 非0数字: 接收不可打印的RTU报文]
 * @param  arg2 [userdata:BUFFET_t, 自定义的含有信号量机制的缓冲区]
 * @return		[string, arg1=0,返回可打印的ASCII报文; arg1=非0数字, 返回"GB_packet=RTU"]
 */
static int lrecvPacFromIPBuf(lua_State *L)
{
	switch(recvPacFromIPBuf(L))
	{
		case -1:
			lerror(L, "\tfunction recvPacFromIPBuf error");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction recvPacFromIPBuf error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lsendCmdToSLCmdBuf 发送串口命令至串口命令缓存区]
 * @param  arg1 [string/nil, string: 串口命令文件所在路径; nil: 本文件]
 * @param  arg2 [string/nil, string: 串口命令的table名; nil: 无串口命令]
 * @param  arg3 [userdata, BUFFER_t类型的缓存区]
 * @return      [boolean, true: 成功; false: 失败]
 */
static int lsendCmdToSLCmdBuf(lua_State *L)
{
	switch(sendCmdToSLCmdBuf(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction sendCmdToSLCmdBuf error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction sendCmdToSLCmdBuf error:");
			break;
	}
	return 1;
}

/**
 * [lrecvPacFromSerial, 从串口中接收报文, 并存至预设的串口缓冲区中, 该函数会建立串口处理线程]
 * @param  arg1 [userdata/nil, nil: 无串口命令的缓存区; BUFFER_t: 串口命令的缓存区]
 * @param  arg2 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @param  arg3 [table, 串口设置信息:串口路径,波特率,数据位数,奇偶校验,停止位数]
 * @return		[boolean, true: 成功; false: 失败]
 */
static int lrecvPacFromSerial(lua_State *L)
{
	switch(recvPacFromSerial(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction recvPacFromSerial error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction recvPacFromSerial error:");
			break;
	}
	return 1;
}

/**
 * [lrecvPacFromSLBuf 从串口报文缓存区中接收报文存至SL_packet]
 * @param  arg1 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @return      [string, 串口报文]
 * @return      [number, SL_buflen]
 */
static int lrecvPacFromSLBuf(lua_State *L)
{
	switch(recvPacFromSLBuf(L))
	{
		case -1:
			lerror(L, "\tfunction recvPacFromSLBuf error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction recvPacFromSLBuf error");
			break;
		default:
			break;
	}
	return 2;
}

/**
 * [lrecXDPacket, 专为西大项目设置的,从本地lua文件中读取串口报文]
 * @param  arg1 [number/nil, number: 取第number个报文; nil: 取第0个报文]
 * @return  	[string, 返回十六进制字符串的串口报文]
 */
static int lrecXDPacket(lua_State *L)
{
	switch(recXDPacket(L))
	{
		case -1:
			lerror(L, "\tfunction recXDPacket error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction recXDPacket error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lcheckPacket, 校验报文的开始符，校验符和结束符，并将指针定位到除去这些符号的首末位置]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 * @return 		[boolean, true: success; false: failure]
 */
static int lcheckPacket(lua_State *L)
{
	switch(checkPacket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction checkPacket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction checkPacket error:");
			break;
	}
	return 1;
}

/**
 * [lsepPacket, 根据预定义的报文格式，将接收到的报文拆分成各个字段]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @return 		[boolean, true: success; false: failure]
 */
static int lsepPacket(lua_State *L)
{
	switch(sepPacket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction sepPacket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction sepPacket error:");
			break;
	}
	return 1;
}

/**
 * [lresoSLPacket, 根据字段的数据类型，提取并解析该字段值为用户可辨认数据]
 * @param  arg1  [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2  [string, 待解析字段的字段名]
 * @param  arg3  [number，从该字段值的第arg3字节处开始解析]
 * @param  arg4  [number, 需要解析的数据长度：arg4个字节]
 * @return       [string/number, 根据每个字段的data_type, 解析生成的数据字符串或数字]
 */
static int lresoSLPacket(lua_State *L)
{
	switch(resoSLPacket(L))
	{
		case -1:
			lerror(L, "\tfunction resoSLPacket error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction resoSLPacket error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lgetFieldValue, 获取指定报文字段的值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 字段键名]
 * @return      [string, 操作后得到的字段值]
 */
static int lgetFieldValue(lua_State *L)
{
	switch(getFieldValue(L))
	{
		case -1:
			lerror(L, "\tfunction getFieldValue error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction getFieldValue error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lassemFieldValue, 组装报文指定字段的数据值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 指定的字段名]
 * @param  arg3 [string, 欲组装到字段的数据]
 * @param  arg4 [number, 0: 不清空字段值; 非0数字: 清空字段值]
 * @return      [string, 操作后得到的字段值]
 */
static int lassemFieldValue(lua_State *L)
{
	switch(assemFieldValue(L))
	{
		case -1:
			lerror(L, "\tfunction assemFieldValue error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction assemFieldValue error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lgetField, 获取指定的报文字段，其中包括字段名，字段值，名值间隔符，字段间隔符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲获取字段的字段名]
 * @return      [string, 操作后得到的字段, 包括字段名, 字段值, 间隔符，具体根据该字段的havekey决定]
 */
static int lgetField(lua_State *L)
{
	switch(getField(L))
	{
		case -1:
			lerror(L, "\tfunction getField error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction getField error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lcopyFields, 将源报文格式变量的数据复制到目的报文格式变量]
 * @param  arg1 [userdata:PKV_t, 源报文格式变量]
 * @param  arg2 [userdata:PKV_t, 目标报文格式变量]
 * @param  arg3 [number, 0: 不清空目标字段; 非0数字: 清空目标字段]
 * @return      [boolean, true:复制成功; false:复制失败]
 */
static int lcopyFields(lua_State *L)
{
	switch(copyFields(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction copyFields error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction copyFields error:");
			break;
	}
	return 1;
}

/**
 * [lconnectField, 连接源报文的某个字段并至于目的字符串的首部或尾部]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲连接的字段的字段名]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 连接至首部; 非0数字: 连接至尾部]
 * @return      [string, 操作后得到的字符串]
 */
static int lconnectField(lua_State *L)
{
	switch(connectField(L))
	{
		case -1:
			lerror(L, "\tfunction connectField error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction connectField error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lconnectFields, 将一些指定的字段链接成字符串]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 欲连接的字段名组成的table]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 不清空目标字段; 非0数字: 清空目标字符串]
 * @return      [string, 操作后得到的字符串]
 */
static int lconnectFields(lua_State *L)
{
	switch(connectFields(L))
	{
		case -1:
			lerror(L, "\tfunction connectFields error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction connectFields error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lgetPassWord, 获取指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [string, 操作后得到的设备密码]
 */
static int lgetPassWord(lua_State *L)
{
	switch(getPassWord(L))
	{
		case -1:
			lerror(L, "\tfunction getPassWord error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction getPassWord error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lsetPassWord, 设置指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @param  arg2 [string, 欲设置的密码]
 * @return      [boolean, true: 设置成功]
 */
static int lsetPassWord(lua_State *L)
{
	switch(setPassWord(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setPassWord error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setPassWord error:");
			break;
	}
	return 1;
}

/**
 * [lcheckMonitorNum, 检查系统是否存在现设备, 若有返回true, 若无则存上并返回true, 若存储已满则生成错误]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [boolean, true: 检查成功]
 */
static int lcheckMonitorNum(lua_State *L)
{
	switch(checkMonitorNum(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction checkMonitorNum error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction checkMonitorNum error:");
			break;
	}
	return 1;
}

/**
 * [lsetSystemTime, 设置系统时间]
 * @param  arg1 [string, 标准的14位日期时间]
 * @return      [boolean, true: 设置成功; false: 设置失败]
 */
static int lsetSystemTime(lua_State *L)
{
	switch(setSystemTime(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction setSystemTime error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction setSystemTime error:");
			break;
	}
	return 1;
}

/**
 * [lgetSystemTime, 获取系统时间]
 * @param  arg1 [string, 选择时间类型: 14/17位日期时间]
 * @return      [string, 操作后得到的日期时间]
 */
static int lgetSystemTime(lua_State *L)
{
	switch(getSystemTime(L))
	{
		case -1:
			lerror(L, "\tfunction getSystemTime error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction getSystemTime error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lcountFieldsLen, 计算字符串长度并返回指定数目的string型的长度]
 * @param  arg1 [string, 待操作字符串]
 * @param  arg2 [number, 0: 不指定数目; 非0数字: 若大于字符串长度则多余位补'0',否则不指定数目]
 * @return      [string, 字符串长度]
 */
static int lcountFieldsLen(lua_State *L)
{
	switch(countFieldsLen(L))
	{
		case -1:
			lerror(L, "\tfunction countFieldsLen error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction countFieldsLen error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lsepField, 根据字段名, 拆分对应的字段值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲拆分字段的字段名]
 * @param  arg3 [number, 返回第arg3个子字段]
 * @return      [string, 子字段名]
 * @return      [string, 子字段值]
 */
static int lsepField(lua_State *L)
{
	switch(sepField(L))
	{
		case -1:
			lerror(L, "\tfunction sepField error:");
			lua_pushstring(L, NULL);
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction sepField error");
			break;
		default:
			break;
	}
	return 2;
}

/**
 * [lassemPacket, 组装报文，根据报文的格式来决定是否连接开始符，校验符，结束符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 * @return      [string, 操作后生成的字符串，此时已是完整的报文]
 */
static int lassemPacket(lua_State *L)
{
	switch(assemPacket(L))
	{
		case -1:
			lerror(L, "\tfunction assemPacket error:");
			lua_pushstring(L, NULL);
			break;
		case -2:
			luaL_error(L, "\tfunction assemPacket error");
			break;
		default:
			break;
	}
	return 1;
}

/**
 * [lnewPKV, 定义名为PKV_t的数据类型]
 * @return   [userdata, 名为PKV_t,大小为10464byte的自定义数据类型]
 */
static int lnewPKV(lua_State *L)
{
	int nbytes = 0;

	nbytes = sizeof(PACKET_KEY_VAL);
	lua_newuserdata(L, nbytes);
	luaL_getmetatable(L, "PKV_t");
	lua_setmetatable(L, -2);

	return 1;
}

/**
 * [lnewBuffer, 定义名为BUFFER_t的数据类型]
 * @return   [userdata, 名为BUFFER_t的自定义数据类型]
 */
static int lnewBuffer(lua_State *L)
{
	int nbytes = 0;
	nbytes = sizeof(BUFFER_t);

	BUFFER_t *pTemp = NULL;
	pTemp = (BUFFER_t *)lua_newuserdata(L, nbytes);
	sem_init(&pTemp->sem_empty, 0, MAXLEN0);
	sem_init(&pTemp->sem_full, 0, 0);
	sem_init(&pTemp->sem_mutex, 0, 1);
	pTemp->sig_save = 0;
	pTemp->sig_get = 0;
	memset(pTemp->buffer, 0, sizeof(pTemp->buffer));

	luaL_getmetatable(L, "BUFFER_t");
	lua_setmetatable(L, -2);
	return 1;
}

/**
 * [lsleep, sleep的外包函数：使进程停止多少秒]
 * @param  arg1 [number, 进程停止的秒数]
 */
static int lsleep(lua_State *L)
{
	int nSec = 0;

	nSec = luaL_checkint(L, 1);
	sleep(nSec);

	return 0;
}

/**
 * [lsendPacToIPBuf, 发送解析后的报文至发送缓冲区]
 * @param  arg1 [number, 0: 传输解析生成的国标报文; 1: 传输ASCII形式的原始串口报文; 2: 传输RTU形式的原始串口报文]
 * @param  arg2 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
static int lsendPacToIPBuf(lua_State *L)
{
	switch(sendPacToIPBuf(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction sendPacToIPBuf error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction sendPacToIPBuf error:");
			break;
	}
	return 1;
}

/**
 * [lsendPacToSocket, 发送国标报文至客户端(PC机)]
 * @param  arg1 [string/nil, nil: 由进程自动识别接收的客户端(PC)报文的IP号; string: 人工指定客户端(PC)的IP号]
 * @param  arg2 [string/nil, nil: 默认客户端(PC)进程的端口号为7777; string: 人工指定客户端(PC)的端口号]
 * @param  arg3 [number, 0: 使用TCP传输; 非0数字: 使用UDP传输]
 * @param  arg4 [number, 0: 不使用网络不通时存储本地功能; 非0数字: 使用网络不通时存储本地功能]
 * @param  arg5 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
static int lsendPacToSocket(lua_State *L)
{
	switch(sendPacToSocket(L))
	{
		default:
		case 0:
			lua_pushboolean(L, 1);
			break;
		case -1:
			lerror(L, "\tfunction sendPacToSocket error:");
			lua_pushboolean(L, 0);
			break;
		case -2:
			luaL_error(L, "\tfunction sendPacToSocket error:");
			break;
	}
	return 1;
}

static const struct luaL_Reg lgateway[] =
{
	{"lASCIIToRTU", lASCIIToRTU},
	{"lsetSLBuffer", lsetSLBuffer},
	{"lsetSLPacket", lsetSLPacket},
	{"lsetGBRTPacket", lsetGBRTPacket},
	{"lsetGBPacket", lsetGBPacket},
	{"lgetCommandArgs", lgetCommandArgs},
	{"ldefPacketFormat", ldefPacketFormat},
	{"lrecvPacFromSocket", lrecvPacFromSocket},
	{"lrecvPacFromIPBuf", lrecvPacFromIPBuf},
	{"lsendCmdToSLCmdBuf", lsendCmdToSLCmdBuf},
	{"lrecvPacFromSerial", lrecvPacFromSerial},
	{"lrecvPacFromSLBuf", lrecvPacFromSLBuf},
	{"lcheckPacket", lcheckPacket},
	{"lsepPacket", lsepPacket},
	{"lresoSLPacket", lresoSLPacket},
	{"lgetFieldValue", lgetFieldValue},
	{"lassemFieldValue", lassemFieldValue},
	{"lgetField", lgetField},
	{"lcopyFields", lcopyFields},
	{"lconnectField", lconnectField},
	{"lconnectFields", lconnectFields},
	{"lgetPassWord", lgetPassWord},
	{"lsetPassWord", lsetPassWord},
	{"lcheckMonitorNum", lcheckMonitorNum},
	{"lsetSystemTime", lsetSystemTime},
	{"lgetSystemTime", lgetSystemTime},
	{"lcountFieldsLen", lcountFieldsLen},
	{"lsepField", lsepField},
	{"lassemPacket", lassemPacket},
	{"lnewPKV", lnewPKV},
	{"lnewBuffer", lnewBuffer},
	{"lsleep", lsleep},
	{"lrecXDPacket", lrecXDPacket},
	{"lsendPacToIPBuf", lsendPacToIPBuf},
	{"lsendPacToSocket", lsendPacToSocket},
	{NULL, NULL}
};

/**
 * [lexTemperature description]
 * @param  L [description]
 * @return   [description]
 */
static int lexTemperature(lua_State *L)
{
	if(exTemperature(L) < 0)
	{
		luaL_error(L, "\tfunction exTemperature error");
	}
	return 1;
}

/**
 * [lexHumidity description]
 * @param  L [description]
 * @return   [description]
 */
static int lexHumidity(lua_State *L)
{
	if(exHumidity(L) < 0)
	{
		luaL_error(L, "\tfunction exHumidity error");
	}
	return 1;
}

/**
 * [lexVoltage description]
 * @param  L [description]
 * @return   [description]
 */
static int lexVoltage(lua_State *L)
{
	if(exVoltage(L) < 0)
	{
		luaL_error(L, "\tfunction exVoltage error");
	}
	return 1;
}

/**
 * [lexLight description]
 * @param  L [description]
 * @return   [description]
 */
static int lexLight(lua_State *L)
{
	if(exLight(L) < 0)
	{
		luaL_error(L, "\tfunction exLight error");
	}
	return 1;
}

/**
 * [lexTemperature1 description]
 * @param  L [description]
 * @return   [description]
 */
static int lexOldTemperature(lua_State *L)
{
	if(exOldTemperature(L) < 0)
	{
		luaL_error(L, "\tfunction exOldTemperature error");
	}
	return 1;
}

/**
 * [lexHumidity1 description]
 * @param  L [description]
 * @return   [description]
 */
static int lexOldHumidity(lua_State *L)
{
	if(exOldHumidity(L) < 0)
	{
		luaL_error(L, "\tfunction exOldHumidity error");
	}
	return 1;
}

/**
 * [lexHumidityInSalt description]
 * @param  L [description]
 * @return   [description]
 */
static int lexHumidityInSalt(lua_State *L)
{
	if(exHumidityInSalt(L) < 0)
	{
		luaL_error(L, "\tfunction exHumidityInSalt error");
	}
	return 1;
}

/**
 * [lexSalt_v description]
 * @param  L [description]
 * @return   [description]
 */
static int lexSalt_v(lua_State *L)
{
	if(exSalt_v(L) < 0)
	{
		luaL_error(L, "\tfunction exSalt_v error");
	}
	return 1;
}

/**
 * [lexSalt_s description]
 * @param  L [description]
 * @return   [description]
 */
static int lexSalt_s(lua_State *L)
{
	if(exSalt_s(L) < 0)
	{
		luaL_error(L, "\tfunction exSalt_s error");
	}
	return 1;
}

static int lexShock(lua_State *L)
{
	if(exShock(L) < 0)
	{
		luaL_error(L, "\tfunction exShock error");
	}
	return 1;
}

static const struct luaL_Reg lsensor[] =
{
	{"lexTemperature", lexTemperature},
	{"lexHumidity", lexHumidity},
	{"lexVoltage", lexVoltage},
	{"lexLight", lexLight},
	{"lexOldTemperature", lexOldTemperature},
	{"lexOldHumidity", lexOldHumidity},
	{"lexHumidityInSalt", lexHumidityInSalt},
	{"lexSalt_v", lexSalt_v},
	{"lexSalt_s", lexSalt_s},
	{"lexShock", lexShock},
	{NULL, NULL}
};

/**
 * [lregister 向Lua虚拟机中注册C库函数]
 * @param L [Lua状态]
 */
void lregister(lua_State *L)
{
	luaL_newmetatable(L, "PKV_t");
	luaL_newmetatable(L, "BUFFER_t");
	luaL_register(L, "lgateway", lgateway);
	luaL_register(L, "lsensor", lsensor);
}
