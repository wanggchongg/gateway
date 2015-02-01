#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <netinet/tcp.h> //TCP_NODELAY

#ifndef _LUA_H_
#define _LUA_H_
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#endif

#include "preprocess.h"
#include "global_var.h"
#include "lua_auxi.h"

/**
 * [getCommandArgs 获取命令行参数]
 * @param  arg1 [string, 命令参数名: "GATEWAY_NO", 网关号; "OPTION", 选择本地读还是串口读]
 * @return   	[string, arg1对应的命令参数值]
 */
int getCommandArgs(lua_State *L);

/**
 * [defPacketFormat, 由Lua脚本定义报文的格式]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string/nil, string:标注报文格式的文件路径; nil:标注报文格式的文件为当前文件]
 */
int defPacketFormat(lua_State *L);

/**
 * [recvPacFromSocket, 从socket中接收报文, 并存至预设的IP缓冲区中, 该函数会建立tcp或udp服务器线程]
 * @param  arg1 [string/nil, nil: 服务器端(arm板)内核自动选择IP地址(如果有多个网卡); string: 人工指定服务器端(arm板)进程的IP地址]
 * @param  arg2 [string/nil, nil: 默认服务器端(arm板)的端口号为19911; string: 人工指定服务器端(arm板)进程的端口号]
 * @param  arg3 [number, 0: 使用TCP接收; 非0数字: 使用TCP接收]
 * @param  arg4 [userdata:BUFFET_t, 自定义的含有信号量机制的缓冲区]
 * @return		[boolean, true: 成功; false: 失败]
 */
int recvPacFromSocket(lua_State *L);

/** [recvPacFromIPBuf 从IP缓冲区中接收报文]
 * @param  arg1 [number, 0: 接收可打印的ASCII报文; 非0数字: 接收不可打印的RTU报文]
 * @param  arg2 [userdata:BUFFET_t, 自定义的含有信号量机制的缓冲区]
 * @return		[string, arg1=0,返回可打印的ASCII报文; arg1=非0数字, 返回"GB_packet=RTU"]
 */
int recvPacFromIPBuf(lua_State *L);

/**
 * [setGBPacket 重新设置IP报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 */
int setGBPacket(lua_State *L);

/**
 * [sendCmdToSLCmdBuf 发送串口命令至串口命令缓存区]
 * @param  arg1 [string/nil, string: 串口命令文件所在路径; nil: 本文件]
 * @param  arg2 [string/nil, string: 串口命令的table名; nil: 无串口命令]
 * @param  arg3 [userdata, BUFFER_t类型的缓存区]
 * @return      [boolean, true: 成功; false: 失败]
 */
int sendCmdToSLCmdBuf(lua_State *L);

/**
 * [recvPacFromSerial, 从串口中接收报文, 并存至预设的串口缓冲区中, 该函数会建立串口处理线程]
 * @param  arg1 [userdata/nil, nil: 无串口命令的缓存区; BUFFER_t: 串口命令的缓存区]
 * @param  arg2 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @param  arg3 [table, 串口设置信息:串口路径,波特率,数据位数,奇偶校验,停止位数]
 * @return		[boolean, true: 成功; false: 失败]
 */
int recvPacFromSerial(lua_State *L);

/**
 * [recvPacFromSLBuf 从串口报文缓存区中接收报文存至SL_packet]
 * @param  arg1 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @return      [string, 串口报文]
 */
int recvPacFromSLBuf(lua_State *L);

/**
 * [setSLPacket 重新设置串口报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 */
int setSLPacket(lua_State *L);

/**
 * [recXDPacket, 专为西大项目设置的,从本地lua文件中读取串口报文]
 * @param  arg1 [number/nil, number: 取第number个报文; nil: 取第0个报文]
 * @return  	[string, 返回十六进制字符串的串口报文]
 */
int recXDPacket(lua_State *L);

/**
 * [checkPacket, 校验报文的开始符，校验符和结束符，并将指针定位到除去这些符号的首末位置]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 */
int checkPacket(lua_State *L);

/**
 * [sepPacket, 根据预定义的报文格式，将接收到的报文拆分成各个字段]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 */
int sepPacket(lua_State *L);

/**
 * [resoSLPacket, 根据字段的数据类型，提取并解析该字段值为用户可辨认数据]
 * @param  arg1  [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2  [string, 待解析字段的字段名]
 * @param  arg3  [number，从该字段值的第arg3字节处开始解析]
 * @param  arg4  [number, 需要解析的数据长度：arg4个字节]
 * @return       [string, 根据每个字段的data_type, 解析生成的数据字符串]
 */
int resoSLPacket(lua_State *L);

/**
 * [getFieldValue, 获取指定报文字段的值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 字段键名]
 * @return      [string, 操作后得到的字段值]
 */
int getFieldValue(lua_State *L);

/**
 * [getField, 获取指定的报文字段，其中包括字段名，字段值，名值间隔符，字段间隔符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲获取字段的字段名]
 * @return      [string, 操作后得到的字段, 包括字段名, 字段值, 间隔符，具体根据该字段的havekey决定]
 */
int getField(lua_State *L);

/**
 * [copyFields, 将源报文格式变量的数据复制到目的报文格式变量]
 * @param  arg1 [userdata:PKV_t, 源报文格式变量]
 * @param  arg2 [userdata:PKV_t, 目标报文格式变量]
 * @param  arg3 [number, 0: 不清空目标字段; 非0数字: 清空目标字段]
 * @return      [boolean, true:复制成功; false:复制失败]
 */
int copyFields(lua_State *L);

/**
 * [assemFieldValue, 组装报文指定字段的数据值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 指定的字段名]
 * @param  arg3 [string, 欲组装到字段的数据]
 * @param  arg4 [number, 0: 不清空字段值; 非0数字: 清空字段值]
 * @return      [string, 操作后得到的字段值]
 */
int assemFieldValue(lua_State *L);

/**
 * [setGBPacket 重新设置IP端返回报文内容]
 * @param  arg1 [string, IP端报文十六进制字符串]
 */
int setGBRTPacket(lua_State *L);

/**
 * [connectField, 连接源报文的某个字段并至于目的字符串的首部或尾部]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲连接的字段的字段名]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 连接至首部; 非0数字: 连接至尾部]
 * @return      [string, 操作后得到的字符串]
 */
int connectField(lua_State *L);

/**
 * [connectFields, 将一些指定的字段链接成字符串]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 欲连接的字段名组成的table]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 不清空目标字段; 非0数字: 清空目标字符串]
 * @return      [string, 操作后得到的字符串]
 */
int connectFields(lua_State *L);

/**
 * [countFieldsLen, 计算字符串长度并返回指定数目的string型的长度]
 * @param  arg1 [string, 待操作字符串]
 * @param  arg2 [number, 0: 不指定数目; 非0数字: 若大于字符串长度则多余位补'0',否则不指定数目]
 * @return      [string, 字符串长度]
 */
int countFieldsLen(lua_State *L);

/**
 * [sepField, 根据字段名, 拆分对应的字段值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲拆分字段的字段名]
 * @param  arg3 [number, 返回第arg3个子字段]
 * @return      [string, 子字段名]
 * @return      [string, 子字段值]
 */
int sepField(lua_State *L);

/**
 * [getPassWord, 获取指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [string, 操作后得到的设备密码]
 */
int getPassWord(lua_State *L);

/**
 * [setPassWord, 设置指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @param  arg2 [string, 欲设置的密码]
 * @return      [boolean, true: 设置成功]
 */
int setPassWord(lua_State *L);

/**
 * [checkMonitorNum, 检查系统是否存在现设备, 若有返回true, 若无则存上并返回true, 若存储已满则生成错误]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [boolean, true: 检查成功]
 */
int checkMonitorNum(lua_State *L);

/**
 * [getSystemTime, 获取系统时间]
 * @param  arg1 [string, 选择时间类型: 14/17位日期时间]
 * @return      [string, 操作后得到的日期时间]
 */
int getSystemTime(lua_State *L);

/**
 * [setSystemTime, 设置系统时间]
 * @param  arg1 [string, 标准的14位日期时间]
 * @return      [boolean, true: 设置成功; false: 设置失败]
 */
int setSystemTime(lua_State *L);

/**
 * [assemPacket, 组装报文，根据报文的格式来决定是否连接开始符，校验符，结束符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 * @return      [string, 操作后生成的字符串，此时已是完整的报文]
 */
int assemPacket(lua_State *L);

/**
 * [sendPacToIPBuf, 发送解析后的报文至发送缓冲区]
 * @param  arg1 [number, 0: 传输解析生成的国标报文; 1: 传输ASCII形式的原始串口报文; 2: 传输RTU形式的原始串口报文]
 * @param  arg2 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
int sendPacToIPBuf(lua_State *L);

/**
 * [sendPacToSocket, 发送国标报文至客户端(PC机)]
 * @param  arg1 [string/nil, nil: 由进程自动识别接收的客户端(PC)报文的IP号; string: 人工指定客户端(PC)的IP号]
 * @param  arg2 [string/nil, nil: 默认客户端(PC)进程的端口号为7777; string: 人工指定客户端(PC)的端口号]
 * @param  arg3 [number, 0: 使用TCP传输; 非0数字: 使用UDP传输]
 * @param  arg4 [number, 0: 不使用网络不通时存储本地功能; 非0数字: 使用网络不通时存储本地功能]
 * @param  arg5 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
int sendPacToSocket(lua_State *L);

#endif  /*_GATEWAY_H_*/
