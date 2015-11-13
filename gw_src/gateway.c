#include "gateway.h"

/*************************************************************************************/
// define statical variable
static uint8_t _CLIENTADDR_[20] = {'\0'};        //客户端(发送请求)IP号
static uint8_t GB_packet[MAXGBLEN] = {'\0'};     //socket接收的IP端报文
static int     GB_packet_len = 0;                //socket接收的IP端报文长度
static uint8_t SL_buffer[MAXSLLEN] = {'\0'};     //存储从串口接收的二进制比特串
static int 	   SL_buflen = 0;				     //串口接收的二进制比特串的长度
static uint8_t SL_packet[MAXSLLEN] = {'\0'};     //存储串口十六进制比特串转化的ASCII字符串
static uint8_t GB_packet_ret[MAXGBLEN] = {'\0'}; //IP端的返回报文
/*************************************************************************************/


/**
 * [ASCIIToRTU 将十六进制的字符串转换为二进制数组数据]
 * @param  arg1 [string, 十六进制字符串]
 * @return      [buffer, 转换后的二进制数组数据]
 * @return      [number, 二进制数组数据的长度]
 */
int ASCIIToRTU(lua_State *L) {
	const uint8_t *arg1 = NULL;
	int len = 0;
	arg1 = luaL_checkstring(L, 1);
	len = strlen(arg1);

	uint8_t *rtn1 = (uint8_t *)malloc(len);
	int rtn2 = HCStrToBU_CAry(arg1, rtn1, len);

	lua_pushlstring(L, rtn1, rtn2);
	lua_pushinteger(L, rtn2);
	free(rtn1);
	return 0;
}



/**
 * [setGBPacket 重新设置IP报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 */
int setGBPacket(lua_State *L)
{
	const uint8_t *pcArg1 = NULL;
	pcArg1 = luaL_checkstring(L, 1);
	memset(GB_packet, '\0', sizeof(GB_packet));
	strcpy(GB_packet, pcArg1);

	return 0;
}



/**
 * [setSLBuffer 重新设置二进制形式的串口报文]
 * @param  arg1 [buffer, 二进制比特串]
 * @param  arg2 [number, 二进制比特串长度]
 */
int setSLBuffer(lua_State *L) {
	const uint8_t *arg1 = NULL;
	int arg2 = 0;
	arg1 = luaL_checkstring(L, 1);
	arg2 = luaL_checkint(L, 2);

	if (arg2 > 0 && arg2 < sizeof(SL_buffer)) {
		memcpy(SL_buffer, arg1, arg2);
		SL_buflen = arg2;
	}
	return 0;
}



/**
 * [setSLPacket 重新设置串口报文内容]
 * @param  arg1 [string, 串口报文十六进制字符串]
 */
int setSLPacket(lua_State *L) {
	const uint8_t *arg1 = NULL;
	arg1 = luaL_checkstring(L, 1);
	memset(SL_packet, 0, sizeof(SL_packet));
	if (strlen(arg1) < sizeof(SL_packet)) {
		strcpy(SL_packet, arg1);
	}
	return 0;
}



/**
 * [setGBPacket 重新设置IP端返回报文内容]
 * @param  arg1 [string, IP端报文十六进制字符串]
 */
int setGBRTPacket(lua_State *L)
{
	const uint8_t *arg1 = NULL;
	arg1 = luaL_checkstring(L, 1);
	memset(GB_packet_ret, 0, sizeof(GB_packet_ret));
	if (strlen(arg1) < sizeof(GB_packet_ret)) {
		strcpy(GB_packet_ret, arg1);
	}

	return 0;
}



/**
 * [getCommandArgs 获取命令行参数]
 * @param  arg1 [string, 命令参数名: "GATEWAY_NO", 网关号; "OPTION", 选择本地读还是串口读]
 * @return   	[string, arg1对应的命令参数值]
 */
int getCommandArgs(lua_State *L)
{
	extern char GATEWAY_NO[];
	extern char OPTION[];
	const  char *pcArg1 = NULL;
	const  char *pcRtn1 = NULL;
	pcArg1 = luaL_checkstring(L, 1);

	if(!strcmp("GATEWAY_NO", pcArg1)) //GATEWAY_NO: 网关号
	{
		if(!strlen(GATEWAY_NO))
			strcpy(GATEWAY_NO, "0"); //默认网关号: 0
		pcRtn1 = GATEWAY_NO;
	}

	if(!strcmp("OPTION", pcArg1)) //OPTION: 设置本地读还是串口读串口报文
	{
		if(!strlen(OPTION))
			strcpy(OPTION, "0"); //默认选择串口数据来自本地: 0
		pcRtn1 = OPTION;
	}

	lua_pushstring(L, pcRtn1);
	return 0;
}



/**
 * [defFieldFormat 由Lua脚本定义各个字段的格式]
 */
static int defFieldFormat(lua_State *L, PACKET_KEY_VAL *pPKVPac, char *name)
{
	int i;
	int num;
	int len;
	uint8_t key[30];
	uint8_t value[30];
	int data_type;
	int havekey;
	lua_getglobal(L, name);
	int field_num = lua_objlen(L, -1);
	// printf("%s field_num: %d\n", lua_typename(L, lua_type(L, -1)), field_num);
	for(i=0; i<field_num; i++)
	{
		lua_rawgeti(L, -1, i+1);

		len = 0;
		lrawgeti(L, 2, &len, NULL); //get the field length
		// printf("len: %d\n", len);

		if(len)
		{
			memset(value, '\0', sizeof(value));
			getField_ns(L, 3, value);
			//lrawgeti(L, 3, NULL, value);  //get the field value
			// printf("value: %s\n", value);

			data_type = 0;
			lrawgeti(L, 4, &data_type, NULL); //get the field data_type
			// printf("data_type: %d\n", data_type);

			pPKVPac->field_num++;
			if(-1 == data_type)
			{
				pPKVPac->field_num--;
				defFieldFormat(L, pPKVPac, value);
			}
			else
			{
				memset(key, '\0', sizeof(key));
				lrawgeti(L, 1, NULL, key); //get the field key

				havekey = 0;
				lrawgeti(L, 5, &havekey, NULL); //get the field have_key

				num = pPKVPac->field_num;
				pPKVPac->fld_kv[num-1].num = num;
				pPKVPac->fld_kv[num-1].len = len;
				strcpy(pPKVPac->fld_kv[num-1].key, key);
				strcpy(pPKVPac->fld_kv[num-1].value, value);
				pPKVPac->fld_kv[num-1].data_type = data_type;
				pPKVPac->fld_kv[num-1].havekey = havekey;

				// printf("num:%d,key:%s,value:%s\n",pPKVPac->fld_kv[num-1].num,
				// 	pPKVPac->fld_kv[num-1].key,pPKVPac->fld_kv[num-1].value);
			}
		}
		lua_pop(L, 1); //pop lua_rawgeti(L, -1, i+1);
	}
	lua_pop(L, 1); //pop the lua_getglobal(L, name);
	return 0;
}

/**
 * [defPacketFormat, 由Lua脚本定义报文的格式]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string/nil, string:标注报文格式的文件路径; nil:标注报文格式的文件为当前文件]
 */
int defPacketFormat(lua_State *L)
{
	PACKET_KEY_VAL *pPKVPac = NULL;
	const uint8_t *format_addr = NULL;

	pPKVPac = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	format_addr = luaL_optstring(L, 2, "_SELFFILE_");
	if(strcmp(format_addr, "_SELFFILE_"))
	{
		if(luaL_dofile(L, format_addr)) //load the file and compile it, while the function:luaL_loadfile just load file.
		{
			fprintf(stderr, "\tdofile format file: ***.lua error\n");
			return -2;
		}
		lua_pop(L, 1);
	}

	memset(pPKVPac, '\0', sizeof(PACKET_KEY_VAL));

	lua_getglobal(L, "command_type");
	pPKVPac->command_type = lua_tointeger(L, -1);
	lua_pop(L, 1);
	// printf("command_type: %d\n", pPKVPac->command_type);

	lua_getglobal(L, "space_mark_fd");
	if(!lua_isnil(L, -1))
		strcpy(pPKVPac->space_mark_fd, lua_tostring(L, -1));
	lua_pop(L, 1);
	// printf("space_mark_fd: %s\n", pPKVPac->space_mark_fd);

	lua_getglobal(L, "space_mark_kv");
	if(!lua_isnil(L, -1))
		strcpy(pPKVPac->space_mark_kv, lua_tostring(L, -1));
	lua_pop(L, 1);
	// printf("space_mark_kv: %s\n", pPKVPac->space_mark_kv);

	lua_getglobal(L, "space_mark_lr");
	if(!lua_isnil(L, -1))
		strcpy(pPKVPac->space_mark_lr, lua_tostring(L, -1));
	lua_pop(L, 1);
	// printf("space_mark_lr: %s\n", pPKVPac->space_mark_lr);

	lua_getglobal(L, "check_type");
	pPKVPac->check_type = lua_tointeger(L, -1);
	lua_pop(L, 1);
	// printf("check_type: %d\n", pPKVPac->check_type);

	pPKVPac->field_num = 0;
	defFieldFormat(L, pPKVPac, "packet");
	return 0;
}




/**
 * [recGBPacket, 初步从lua文件中接收GB报文，以后会直接在socket中接收]
 */
// int recGBPacket(lua_State *L)
// {
// 	memset(GB_packet, '\0', sizeof(GB_packet));
// 	if(luaL_dofile(L, "./GB_packet.lua"))
// 		luaL_error(L, "dofile GB_packet.lua error");

// 	lua_getglobal(L, "GB_start_command");
// 	strcpy(GB_packet, lua_tostring(L, -1));
// 	lua_pop(L, 1);

// 	printf("GB_packet: %s\n", GB_packet);
// 	return 0;
// }



/**
 * [recvn 防止接收的数据由于缓冲区满而比要求的少而写的recv函数增强版]
 */
static ssize_t recvn(int sockfd, void *vptr, size_t n, int flags)
{
	size_t nleft;
	ssize_t nrecvn;
	uint8_t *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nrecvn = recv(sockfd, ptr, nleft, flags)) < 0)
		{
			if(errno = EINTR)
				nrecvn = 0;
			else
				return -1;
		}
		else if (nrecvn == 0)
		{
			break;
		}
		nleft -= nrecvn;
		ptr += nrecvn;
	}
	return (n-nleft);
}



/**
 * [tcpDo_thread 对每个tcp连接进行处理的线程]
 * @param  arg [缓存区和文件描述符]
 */
static void *tcpDo_thread(void *arg)
{
	BUFFER_FD_t pBuffer_fd = *(BUFFER_FD_t *)arg;
	BUFFER_t *pBuffer = pBuffer_fd.pBuffer;
	int connfd = *(pBuffer_fd.pSockfd);

	pthread_detach(pthread_self()); //使线程处理分离状态

	uint8_t *message = NULL;
	ssize_t  mesglen = 0;
	struct timeval timeout;
	time_t time_old, time_new;
	timeout.tv_sec = 250;
	timeout.tv_usec = 0;

	if(setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)  //设置套接字选项
    {
        perror("\tthread tcpDo: tcp socket setsockopt(SO_RCVTIMEO) failed");
        close(connfd);
		pthread_exit((void *)-1); //结束此线程
    }

    message = (uint8_t *)malloc(MAXLEN8 * sizeof(uint8_t));
	while(1)
	{
		memset(message, 0, MAXLEN8);
		time_old = time(NULL);
		mesglen = recv(connfd, message, MAXLEN8, 0);
		time_new = time(NULL);
		if((time_new - time_old) > 240)
		{
			printf("\tthread tcpDo: time out of 4 minute\n");
			close(connfd);
			free(message);
			pthread_exit((void *)-1); //结束此线程
		}

		if(mesglen == 0) //表示连接已经断开
		{
			fprintf(stderr, "\tthread tcpDo: connection disconnected\n");
			close(connfd);
			free(message);
			pthread_exit((void *)-1); //结束此线程
		}
		else if(mesglen < 0 && (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)) //recv函数出现错误，期待下次继续recv
		{
			perror("\tthread tcpDo: TCP recvn error");
			continue;
		}
		else if(mesglen < 0 && errno != EINTR && errno != EWOULDBLOCK && errno != EAGAIN)  //recv函数出现错误
		{
			perror("\tthread tcpDo: TCP recvn error, close socket");
			close(connfd);
			free(message);
			pthread_exit((void *)-1); //结束此线程
		}

		sem_wait(&pBuffer->sem_empty);
		sem_wait(&pBuffer->sem_mutex);

		memset(pBuffer->buffer[pBuffer->sig_save].packet, 0, MAXLEN8);
		pBuffer->buffer[pBuffer->sig_save].len = 0;
		memcpy(pBuffer->buffer[pBuffer->sig_save].packet, message, mesglen);
		pBuffer->buffer[pBuffer->sig_save].len = mesglen;
		printf("tcpRecv_buffer num: %d, len: %d\n",pBuffer->sig_save, pBuffer->buffer[pBuffer->sig_save].len);
		pBuffer->sig_save = (pBuffer->sig_save + 1) % MAXLEN0;

		sem_post(&pBuffer->sem_mutex);
		sem_post(&pBuffer->sem_full);
	}

	close(connfd);
	free(message);
	pthread_exit((void *)0);
}



/**
 * [tcpRecv_thread tcp监听线程,监听对端的连接]
 * @param  arg [缓存区和ip地址端口号]
 */
static void *tcpRecv_thread(void *arg)
{
	BUFFER_IPPORT_t pBuf_ipport = *(BUFFER_IPPORT_t *)arg;

	pthread_detach(pthread_self());
	int err;
	pthread_t tid;
	int 				listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(pBuf_ipport.pIpPort->port));
	if(strcmp(pBuf_ipport.pIpPort->ip, "INADDR_ANY"))
	{
		if(inet_pton(AF_INET, pBuf_ipport.pIpPort->ip, &servaddr.sin_addr) <= 0)
		{
			fprintf(stderr, "\ttcpRecv_thread: inet_pton error for %s\n", pBuf_ipport.pIpPort->ip);
			pthread_exit((void *)-1);
		}
	}
	else
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("\ttcpRecv_thread: TCP socket error");
		pthread_exit((void *)-1);
	}

	int on=1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)  //设置套接字选项避免地址使用错误:address already in use
    {
        perror("\ttcpRecv_thread: TCP socket setsockopt failed");
        close(listenfd);
        pthread_exit((void *)-1);
    }

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("\ttcpRecv_thread: TCP bind error");
		close(listenfd);
		pthread_exit((void *)-1);
	}

	if(listen(listenfd, 10) < 0)
	{
		perror("\ttcpRecv_thread: TCP listen error");
		close(listenfd);
		pthread_exit((void *)-1);
	}

	while(1)
	{
		printf("waiting for the TCP connection...\n");
		clilen = sizeof(cliaddr);
		memset(&cliaddr, '\0', clilen);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0)
		{
			perror("\ttcpRecv_thread: TCP accept error");
			close(listenfd);
			pthread_exit((void *)-1);
		}

		memset(_CLIENTADDR_, '\0', sizeof(_CLIENTADDR_));
		inet_ntop(AF_INET, &cliaddr.sin_addr, _CLIENTADDR_, 20);
		printf("TCP connection from: %s, port %d\n", _CLIENTADDR_,  ntohs(cliaddr.sin_port));

		BUFFER_FD_t buffer_fd;
		buffer_fd.pBuffer = pBuf_ipport.pBuffer;
		buffer_fd.pSockfd = &connfd;
		err = pthread_create(&tid, NULL, tcpDo_thread, &buffer_fd);
		if(err)
		{
			fprintf(stderr, "\ttcpRecv_thread: can't create tcpDo thread: %s\n", strerror(err));
			pthread_exit((void *)-1);
		}
	}

	pthread_exit((void *)0);
}



/**
 * [udpRecv_thread udp接收处理线程]
 * @param  arg [缓存区和ip地址端口号]
 */
static void *udpRecv_thread(void *arg)
{
	BUFFER_IPPORT_t pBuf_ipport = *(BUFFER_IPPORT_t *)arg;
	BUFFER_t *pBuffer = pBuf_ipport.pBuffer;

	pthread_detach(pthread_self()); //使线程处理分离状态

	int err;
	pthread_t tid;
	int 				listenfd, connfd;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	uint8_t *message = NULL;
	ssize_t  mesglen = 0;

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(pBuf_ipport.pIpPort->port));
	if(strcmp(pBuf_ipport.pIpPort->ip, "INADDR_ANY"))
	{
		if(inet_pton(AF_INET, pBuf_ipport.pIpPort->ip, &servaddr.sin_addr) <= 0)
		{
			fprintf(stderr, "\tudpRecv_thread: inet_pton error for %s\n", pBuf_ipport.pIpPort->ip);
			pthread_exit((void *)-1);
		}
	}
	else
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("\tudpRecv_thread: UDP socket error");
		pthread_exit((void *)-1);
	}

	int on=1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)  //设置套接字选项避免地址使用错误:address already in use
    {
        perror("\tudpRecv_thread: UDP socket setsockopt failed");
        close(listenfd);
        pthread_exit((void *)-1);
    }

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("\tudpRecv_thread: UDP bind error");
		close(listenfd);
		pthread_exit((void *)-1);
	}

	message = (uint8_t *)malloc(MAXLEN8 * sizeof(uint8_t));

///////////////////////////////////////////////////////////////////////////////////
//调试用,将程序重启时间写入文件                                                 ///
	FILE *heartFp = fopen("./scalar_heart.txt", "a+");    						///
	char *heartLog  = (char *)malloc(50);										///
	char *timeStamp = (char *)malloc(30);										///
																				///
	memset(heartLog, 0, 50);													///
	memset(timeStamp, 0, 30);													///
	gettimestamp(timeStamp);													///
	snprintf(heartLog, 50, "\r\n程序重启时间：%s\r\n", timeStamp);				///
	fputs(heartLog, heartFp);													///
	fflush(heartFp);															///
///////////////////////////////////////////////////////////////////////////////////

	while(1)
	{
		printf("waiting for the UDP packet...\n");
		memset(message, 0, MAXLEN8);
		clilen = sizeof(cliaddr);
		memset(&cliaddr, '\0', clilen);

		if((mesglen = recvfrom(listenfd, message, MAXLEN8, 0, (struct sockaddr*)&cliaddr, &clilen)) < 0)
		{
			fprintf(stderr, "\tudpRecv_thread: UDP recvfrom error\n");
			continue;
		}
		else if(mesglen >=2 && mesglen <= 4)
		{
			sendto(listenfd, "###", 3, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

///////////////////////////////////////////////////////////////////////////////////
//调试用,将收到的心跳写入文件                                                   ///
			memset(heartLog, 0, 50);											///
			memset(timeStamp, 0, 30);											///
			gettimestamp(timeStamp);											///
			snprintf(heartLog, 50, "%s--%s\r\n", message, timeStamp);			///
			fputs(heartLog, heartFp);											///
			fflush(heartFp);													///
			if(ftell(heartFp) > 52428800) //50MB								///
			{																	///
				ftruncate(fileno(heartFp), 0);									///
			}																	///
///////////////////////////////////////////////////////////////////////////////////

		}
		else if(mesglen == 0)
		{
			continue;
		}

		memset(_CLIENTADDR_, '\0', sizeof(_CLIENTADDR_));
		inet_ntop(AF_INET, &cliaddr.sin_addr, _CLIENTADDR_, 20);
		printf("\nUDP packet from %s, port %d\n", _CLIENTADDR_, ntohs(cliaddr.sin_port));

		sem_wait(&pBuffer->sem_empty);
		sem_wait(&pBuffer->sem_mutex);

		memset(pBuffer->buffer[pBuffer->sig_save].packet, 0, MAXLEN8);
		pBuffer->buffer[pBuffer->sig_save].len = 0;
		memcpy(pBuffer->buffer[pBuffer->sig_save].packet, message, mesglen);
		pBuffer->buffer[pBuffer->sig_save].len = mesglen;
		printf("udpRecv_buffer num: %d, len: %d\n",pBuffer->sig_save, pBuffer->buffer[pBuffer->sig_save].len);
		pBuffer->sig_save = (pBuffer->sig_save + 1) % MAXLEN0;

		sem_post(&pBuffer->sem_mutex);
		sem_post(&pBuffer->sem_full);
	}

	free(message);
	pthread_exit((void *)0);
}



/**
 * [recvPacFromSocket, 从socket中接收报文, 并存至预设的IP缓冲区中, 该函数会建立tcp或udp服务器线程]
 * @param  arg1 [string/nil, nil: 服务器端(arm板)内核自动选择IP地址(如果有多个网卡); string: 人工指定服务器端(arm板)进程的IP地址]
 * @param  arg2 [string/nil, nil: 默认服务器端(arm板)的端口号为7777; string: 人工指定服务器端(arm板)进程的端口号]
 * @param  arg3 [number, 0: 使用TCP接收; 非0数字: 使用UDP接收]
 * @param  arg4 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 * @return		[boolean, true: 成功; false: 失败]
 */
int recvPacFromSocket(lua_State *L)
{
	const uint8_t *LOCA_ADDR = NULL;
	const uint8_t *LOCA_PORT = NULL;
	int 		   MODE_OPT = 0;
	BUFFER_t      *pBuffer = NULL;
	IP_PORT_t 	   ip_port = {0};

	LOCA_ADDR = luaL_optstring(L, 1, "INADDR_ANY");
	LOCA_PORT = luaL_optstring(L, 2, "7777");
	MODE_OPT  = luaL_checkint(L, 3);
	pBuffer   = luaL_checkudata(L, 4, "BUFFER_t");

	strcpy(ip_port.ip, LOCA_ADDR);
	strcpy(ip_port.port, LOCA_PORT);

	if(MODE_OPT) //使用UDP
	{
		int err;
		pthread_t tid;
		BUFFER_IPPORT_t pBuf_ipport;

		pBuf_ipport.pIpPort = &ip_port;
		pBuf_ipport.pBuffer = pBuffer;

		err = pthread_create(&tid, NULL, udpRecv_thread, &pBuf_ipport);
		if(err)
		{
			fprintf(stderr, "\tcan't create tcpRecv thread: %s\n", strerror(err));
			return -2;
		}
	}

	else //使用TCP
	{
		int err;
		pthread_t tid;
		BUFFER_IPPORT_t pBuf_ipport;

		pBuf_ipport.pIpPort = &ip_port;
		pBuf_ipport.pBuffer = pBuffer;

		err = pthread_create(&tid, NULL, tcpRecv_thread, &pBuf_ipport);
		if(err)
		{
			fprintf(stderr, "\tcan't create tcpRecv thread: %s\n", strerror(err));
			return -2;
		}
	}
	usleep(1000*200);
	return 0;
}



/** [recvPacFromIPBuf 从IP缓冲区中接收报文]
 * @param  arg1 [number, 0: 接收可打印的ASCII报文; 非0数字: 接收不可打印的RTU报文]
 * @param  arg2 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 * @return		[string, arg1=0,返回可打印的ASCII报文; arg1=非0数字, 返回"GB_packet=RTU"]
 */
int recvPacFromIPBuf(lua_State *L)
{
	int MESG_OPT = 0;
	BUFFER_t *pBuffer = NULL;

	MESG_OPT = luaL_checkint(L, 1);
	pBuffer  = luaL_checkudata(L, 2, "BUFFER_t");

	memset(GB_packet, 0, sizeof(GB_packet));

	sem_wait(&pBuffer->sem_full);
	sem_wait(&pBuffer->sem_mutex);

	memcpy(GB_packet, pBuffer->buffer[pBuffer->sig_get].packet, pBuffer->buffer[pBuffer->sig_get].len);
	GB_packet_len = pBuffer->buffer[pBuffer->sig_get].len;
	pBuffer->sig_get = (pBuffer->sig_get + 1) % MAXLEN0;

	sem_post(&pBuffer->sem_mutex);
	sem_post(&pBuffer->sem_empty);

	if(MESG_OPT) //RTU
	{
		printf("Length: %d, received GB_packet: ", GB_packet_len);
		int i;
		for(i=0; i<GB_packet_len; i++)
		{
			printf("0x%X ", GB_packet[i]);
		}
		putchar('\n');
		lua_pushstring(L, "GB_packet=RTU");
	}

	else //ASCII
	{
		printf("Length: %d, received GB_packet: %s\n", GB_packet_len, GB_packet);
		lua_pushstring(L, GB_packet);
	}
	return 0;
}



/**
 * [readn 防止接收的数据由于缓冲区满而比要求的少而写的read函数增强版]
 */
static ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nreaded;
	uint8_t *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nreaded = read(fd, ptr, nleft)) < 0)
		{
			if(errno = EINTR)
			{
				fprintf(stderr, "errno = EINTR\n");
				nreaded = 0;
			}
			else
				return -1;
		}
		else if(nreaded == 0)
		{
			break;
		}
		nleft -= nreaded;
		ptr += nreaded;
	}
	return (n-nleft);
}



/**
 * [sendCmdToSLCmdBuf 发送串口命令至串口命令缓存区]
 * @param  arg1 [string/nil, string: 串口命令文件所在路径; nil: 本文件]
 * @param  arg2 [string/nil, string: 串口命令的table名; nil: 无串口命令]
 * @param  arg3 [userdata, BUFFER_t类型的缓存区]
 * @return      [boolean, true: 成功; false: 失败]
 */
int sendCmdToSLCmdBuf(lua_State *L)
{
	const uint8_t *FILEPATH = NULL;
	const uint8_t *CMDTABLE = NULL;
	BUFFER_t *pBuffer = NULL;

	FILEPATH = luaL_optstring(L, 1, "_SELFFILE_");
	CMDTABLE = luaL_optstring(L, 2, "_NOUSEFULL_");
	pBuffer  = luaL_checkudata(L, 3, "BUFFER_t");

	if(strcmp(FILEPATH, "_SELFFILE_"))
	{
		if(luaL_dofile(L, FILEPATH))
		{
			fprintf(stderr, "\tcan't dofile the command file\n");
			return -2;
		}
	}

	/*************************************************/
	//接收modbus命令并存至SL_Cmd_Buffer
	if(strcmp(CMDTABLE, "_NOUSEFULL_"))
	{
		lua_getglobal(L, CMDTABLE);
		int byte_num = lua_objlen(L, -1);
		int i;
		uint8_t SL_command[byte_num];
		memset(SL_command, '\0', byte_num);

		for(i=0; i<byte_num; i++)
		{
			lua_rawgeti(L, -1, i+1);
			SL_command[i] = (uint8_t)lua_tointeger(L, -1);
			lua_pop(L, 1);
		}
		putchar('\n');

		sem_wait(&pBuffer->sem_empty);
		sem_wait(&pBuffer->sem_mutex);

		memset(pBuffer->buffer[pBuffer->sig_save].packet, '\0', MAXLEN8);
		memcpy(pBuffer->buffer[pBuffer->sig_save].packet, SL_command, byte_num);
		pBuffer->buffer[pBuffer->sig_save].len = byte_num;
		pBuffer->sig_save = (pBuffer->sig_save + 1) % MAXLEN0;

		sem_post(&pBuffer->sem_mutex);
		sem_post(&pBuffer->sem_full);
	}
	/************************************************/
	return 0;
}



/**
 * [setSerialConf, 设置串口信息]
 * @param  serial_fd [串口文件描述符]
 * @param  baud_rate [波特率]
 * @param  data_bits [数据位数]
 * @param  parity    [奇偶校验]
 * @param  stop_bits [停止位数]
 */
static int setSerialConf(int serial_fd, int baud_rate, int data_bits, int parity, int stop_bits)  //设置串口信息
{
	struct termios new_cfg, old_cfg;
	int  speed;
	if(tcgetattr(serial_fd, &old_cfg) != 0)  //获取termios属性
	{
		perror("\ttcgetattr error");
		return -1;
	}
	printf("old_cfg:\nc_cflag:%X\n",old_cfg.c_cflag);
	printf("c_iflag:%X\n",old_cfg.c_iflag);
	printf("c_oflag:%X\n",old_cfg.c_oflag);
	printf("c_lflag:%X\n",old_cfg.c_lflag);

	memcpy(&new_cfg, &old_cfg, sizeof(old_cfg));
	cfmakeraw(&new_cfg);   //以非规范的原始模式运行
	new_cfg.c_cflag &= ~CSIZE;  //对CSIZE标志位段清0, CSIZE:=0000...01110...00,CSIZE标志指明发送和接收的每个字节位数

  	switch (baud_rate)  //波特率
  	{
  		case 2400:	 speed = B2400;	 break;
  		case 4800:	 speed = B4800;	 break;
  		case 9600:	 speed = B9600;	 break;
  		case 19200:	 speed = B19200; break;
  		case 38400:	 speed = B38400; break;
  		case 57600:  speed = B57600; break;
		default:
		case 115200: speed = B115200;break;
  	}
	cfsetispeed(&new_cfg, speed);  //设置输入速度
	cfsetospeed(&new_cfg, speed);  //设置输出速度

	switch (data_bits)  //字节的数据位数,CS字段就是CSIZE字段的取值
	{
		case 7:
		{
			new_cfg.c_cflag |= CS7;  //CS5:=0000...011...000
		}
		break;

		default:
		case 8:
		{
			new_cfg.c_cflag |= CS8;  //CS5:=0000...100...000
		}
		break;
  	}

  	switch (parity)   //奇偶校验位
  	{
		default:
		case -1:
		{
			new_cfg.c_cflag &= ~PARENB;   //PARENB：对输出字符产生奇偶位，对输入字符执行奇偶性校验，此处对该标志位清零
			new_cfg.c_iflag &= ~INPCK;    //INPCK：使输入字符奇偶校验起作用，当字符的奇偶为错时，决定是否检查IGNPAR位段，此处清零
		}
		break;

		case 1: //奇校验
		{
			new_cfg.c_cflag |= (PARODD | PARENB);  //PARODD:设置为奇校验，否则为偶校验
			new_cfg.c_iflag |= INPCK;
		}
		break;

		case 2: //偶校验
		{
			new_cfg.c_cflag |= PARENB;   //打开奇偶校验
			new_cfg.c_cflag &= ~PARODD;   //奇校验位清零，即设置为偶校验
			new_cfg.c_iflag |= INPCK;
		}
		break;

		case 0:  /*as no parity*/
		{
			new_cfg.c_cflag &= ~PARENB;  //无奇偶校验
			new_cfg.c_cflag &= ~CSTOPB;  //使用一位作停止位
		}
		break;
	}

	switch (stop_bits)   //停止位
	{
		default:
		case 1:
		{
			new_cfg.c_cflag &= ~CSTOPB;  //使用一位作停止位
		}
		break;

		case 2:
		{
			new_cfg.c_cflag |= CSTOPB;  //使用两位作停止位
		}
	}

	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN]  = 1;
	tcflush(serial_fd, TCIOFLUSH);  //刷清输入，输出队列
	if((tcsetattr(serial_fd, TCSANOW, &new_cfg)) != 0) //指定新的终端属性立即生效
	{
		perror("\ttcsetattr error");
		return -1;
	}

	if(tcgetattr(serial_fd, &new_cfg)  !=  0)  //获取termios属性
	{
		perror("\ttcgetattr error");
		return -1;
	}
	printf("tcsetattr:\nc_cflag:%X\n",new_cfg.c_cflag);
	printf("c_iflag:%X\n",new_cfg.c_iflag);
	printf("c_oflag:%X\n",new_cfg.c_oflag);
	printf("c_lflag:%X\n",new_cfg.c_lflag);

	return 0;
}



/**
 * [serialDo_thread 串口处理线程]
 * @param  arg [两个缓存区和一个文件描述符]
 */
static void *serialDo_thread(void *arg)
{
	BUFFER2_FD_t buffer2_fd = *(BUFFER2_FD_t *)arg;

	pthread_detach(pthread_self());

	BUFFER_t *pCmdBuffer = buffer2_fd.pBuffer1;
	BUFFER_t *pSLBuffer  = buffer2_fd.pBuffer2;
	int fd = *buffer2_fd.pFd;

	uint8_t *SL_buf = NULL;
	ssize_t  SL_len = 0;
	fd_set readfd;
	struct timeval timeout;
	int i;

	ssize_t  nreaded = 0;
	uint8_t *temp_buf = NULL;

	SL_buf   = (uint8_t *)malloc(MAXSLLEN * sizeof(uint8_t));
	temp_buf = (uint8_t *)malloc(MAXSLLEN * sizeof(uint8_t));
	while(1)
	{
		if(pCmdBuffer)
		{
			sem_wait(&pCmdBuffer->sem_full);
			sem_wait(&pCmdBuffer->sem_mutex);
			//发送modbus命令至串口
			if(write(fd, pCmdBuffer->buffer[pCmdBuffer->sig_get].packet, pCmdBuffer->buffer[pCmdBuffer->sig_get].len)
			 != pCmdBuffer->buffer[pCmdBuffer->sig_get].len);
			{
				perror("\tcan't send modbus command to serial port");
			}
			pCmdBuffer->sig_get = (pCmdBuffer->sig_get + 1) % MAXLEN0;
			sem_post(&pCmdBuffer->sem_mutex);
			sem_post(&pCmdBuffer->sem_empty);
		}

		memset(SL_buf, 0, MAXSLLEN);
		SL_len = 0;
		FD_ZERO(&readfd);
		FD_SET(fd, &readfd);
		while(1)
		{
			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;
			int retn = select(fd+1, &readfd, NULL, NULL, &timeout);  //如果设置超时时间，select相当于一个半阻塞

			if(0 == retn)
			{
				break;
			}

			else if(-1 == retn)
			{
				perror("\tselect error");
				break;
			}

			else
			{
				nreaded = 0;
				memset(temp_buf, 0, MAXSLLEN);
				if(SL_len > 500)
				{
					break;
				}
				if((nreaded = read(fd, temp_buf, MAXSLLEN)) > 0)
				{
					printf("The Linux has received %d datas.\n", nreaded);
					for(i=0; i<nreaded; i++, SL_len++)
			        {
			            //printf("%X ", temp_buf[i]);
			            SL_buf[SL_len] = temp_buf[i];
			        }
				}

				else if(nreaded <= 0)
				{
					break;
				}
			}
	    }

	    if(SL_len)
	    {
		    sem_wait(&pSLBuffer->sem_empty);
		    sem_wait(&pSLBuffer->sem_mutex);
		    memset(pSLBuffer->buffer[pSLBuffer->sig_save].packet, 0, MAXLEN8);
		    memcpy(pSLBuffer->buffer[pSLBuffer->sig_save].packet, SL_buf, SL_len);
		    pSLBuffer->buffer[pSLBuffer->sig_save].len = SL_len;
		    pSLBuffer->sig_save = (pSLBuffer->sig_get + 1) % MAXLEN0;
		    sem_post(&pSLBuffer->sem_mutex);
		    sem_post(&pSLBuffer->sem_full);
		}
	}

	close(fd);
	free(SL_buf);
	free(temp_buf);
	pthread_exit((void *)0);
}



/**
 * [recvPacFromSerial, 从串口中接收报文, 并存至预设的串口缓冲区中, 该函数会建立串口处理线程]
 * @param  arg1 [userdata/nil, nil: 无串口命令的缓存区; BUFFER_t: 串口命令的缓存区]
 * @param  arg2 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @param  arg3 [table, 串口设置信息:串口路径,波特率,数据位数,奇偶校验,停止位数]
 * @return		[boolean, true: 成功; false: 失败]
 */
int recvPacFromSerial(lua_State *L)
{
	BUFFER_t *pCmdBuffer = NULL; //串口命令的缓存区
	BUFFER_t *pSLBuffer = NULL; //接收串口报文的缓存区

	uint8_t serial_path[MAXLEN3] = {0}; //串口路径
	int baud_rate = -1; //波特率
	int data_bits = -1; //数据位数
	int parity    = -1; //奇偶校验
	int stop_bits = -1; //停止位数
	int serial_fd = -1; //串口文件描述符

	BUFFER2_FD_t pBuffer2_fd;

	if(lua_type(L, -3) == LUA_TUSERDATA)
		pCmdBuffer = luaL_checkudata(L, 1, "BUFFER_t");
	pSLBuffer = luaL_checkudata(L, 2, "BUFFER_t");

	luaL_checktype(L, 3, LUA_TTABLE);
	if(lua_objlen(L, -1) != 5)
	{
		fprintf(stderr, "LUA_TTABLE error: lua_objlen != 5\n");
		return -2;
	}
	getField_ns(L, 1, serial_path); //串口路径
	getField_nn(L, 2, &baud_rate);  //波特率
	getField_nn(L, 3, &data_bits);  //数据位数
	getField_nn(L, 4, &parity);     //奇偶校验
	getField_nn(L, 5, &stop_bits);  //停止位数

	printf("start...\n");
	if((serial_fd = open(serial_path, O_RDWR|O_NOCTTY|O_NDELAY)) < 0) //O_NOCTTY：不将终端设备作为进程的控制终端
    {
		perror("\topen serial_PORT error");
		close(serial_fd);
		return -2;
    }

    //此处不能加isatty(STDIN_FILENO)函数,因为会导致在shell脚本不能启动程序

    if(setSerialConf(serial_fd, baud_rate, data_bits, parity, stop_bits) < 0)
    {
    	fprintf(stderr, "\tcan't set serial_com: \"%s\"\n", serial_path);
    	close(serial_fd);
    	return -2;
    }

    pBuffer2_fd.pBuffer1 = pCmdBuffer;
    pBuffer2_fd.pBuffer2 = pSLBuffer;
    pBuffer2_fd.pFd = &serial_fd;

    int err;
    pthread_t tid;
    err = pthread_create(&tid, NULL, serialDo_thread, &pBuffer2_fd);
	if(err)
	{
		fprintf(stderr, "\tcan't create serialDo thread: %s\n", strerror(err));
		close(serial_fd);
		return -2;
	}

	usleep(1000*200);
	return 0;
}



/**
 * [recvPacFromSLBuf 从串口报文缓存区中接收报文存至SL_packet]
 * @param  arg1 [userdata, BUFFER_t: 接收串口报文的缓存区]
 * @return      [string, 串口报文]
 * @return      [number, SL_buflen]
 */
int recvPacFromSLBuf(lua_State *L)
{
	BUFFER_t *pBuffer = NULL;
	pBuffer = luaL_checkudata(L, 1, "BUFFER_t");

	memset(SL_buffer, '\0', MAXSLLEN);

	sem_wait(&pBuffer->sem_full);
	sem_wait(&pBuffer->sem_mutex);

	memcpy(SL_buffer, pBuffer->buffer[pBuffer->sig_get].packet, pBuffer->buffer[pBuffer->sig_get].len);
	SL_buflen = pBuffer->buffer[pBuffer->sig_get].len;
	pBuffer->sig_get = (pBuffer->sig_get + 1) % MAXLEN0;
	sem_post(&pBuffer->sem_mutex);
	sem_post(&pBuffer->sem_empty);

	memset(SL_packet, '\0', sizeof(SL_packet));
	BU_CAryToHCStr(SL_buffer, SL_packet, SL_buflen); //将二进制01比特流转换为十六进制字符串
	printf("Length: %d, received SL_packet: %s\n", SL_buflen*2, SL_packet);

	lua_pushstring(L, SL_packet);
	lua_pushinteger(L, SL_buflen);
	return 0;
}



/**
 * [recXDPacket, 专为西大项目设置的,从本地lua文件中读取串口报文]
 * @param  arg1 [number/nil, number: 取第number个报文; nil: 取第0个报文]
 * @return  	[string, 返回十六进制字符串的串口报文]
 */
int recXDPacket(lua_State *L)
{
	int  OPTION = 0;
	OPTION = luaL_optint(L, 1, 0);

	int i;
	memset(SL_buffer, '\0', sizeof(SL_buffer));
	if(luaL_dofile(L, "./XD_packet.lua"))
	{
		fprintf(stderr, "\tcan't dofile XD_packet.lua\n");
		return -2;
	}

	uint8_t packetName[20] = {0};
	sprintf(packetName, "XD_packet%d", OPTION);
	lua_getglobal(L, packetName);
	SL_buflen = 0;
	SL_buflen = lua_objlen(L, -1);
	for(i=0; i<SL_buflen; i++)
	{
		lua_rawgeti(L, -1, i+1);
		SL_buffer[i] = (uint8_t)lua_tointeger(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	// printf("SL_buffer: ");
	// for(i=0; i<SL_buflen; i++)
	// {
	// 	printf("0X%X ", SL_buffer[i]);
	// }
	// putchar('\n');

	memset(SL_packet, '\0', sizeof(SL_packet));
	BU_CAryToHCStr(SL_buffer, SL_packet, SL_buflen);
	printf("Length: %d, received SL_packet: %s\n", SL_buflen*2, SL_packet);

	lua_pushstring(L, SL_packet);
	return 0;
}



/*****************************************************/
// define statical variable
static uint8_t *pac_point1 = NULL; //指向IP端报文的头指针
static uint8_t *pac_point2 = NULL; //指向IP端报文的尾指针
static int 		fld_index1 = 0; //与头指针对应的IP端报文的数组标号
static int 		fld_index2 = 0; //与尾指针对应的IP端报文的数组标号
/*****************************************************/
/**
 * [checkPacket, 校验报文的开始符，校验符和结束符，并将指针定位到除去这些符号的首末位置]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 */
int checkPacket(lua_State *L)
{
	int i;
	uint8_t *pac_point = NULL;
	uint8_t  check_code[10] = {0};
	uint8_t  check_str[10] = {0};
	int      check_num;
	int      check_data_type;
	uint8_t  elemt[3][30] = {0};
	PACKET_KEY_VAL *pPKVPac = NULL;

	pPKVPac = luaL_checkudata(L, 1, "PKV_t");
	luaL_checktype(L, 2, LUA_TTABLE);
	////printf("top: %d,table:%d\n", lua_gettop(L), lua_objlen(L, -1));
	if(lua_objlen(L, -1) != 3)
	{
		fprintf(stderr, "LUA_TTABLE: lua_objlen error\n");
		return -2;
	}
	for(i=0; i<3; i++)
	{
		lrawgeti(L, i+1, NULL, elemt[i]);
		////printf("element: %s\n", elemt[i]);
	}
	lua_pop(L, 1);

	if(1 == pPKVPac->command_type)
		pac_point = SL_packet;
	else //the default is GB_packet
		pac_point = GB_packet;
	pac_point1 = pac_point;
	pac_point2 = pac_point + strlen(pac_point); //指向字符串的最后一个字符的下一个字符

	fld_index1 = 0;
	fld_index2 = pPKVPac->field_num - 1;

	if(pPKVPac->check_type) //the case with check code
	{
		if(!strcmp(pPKVPac->fld_kv[0].key, elemt[0])) //if have start code, check the start code
		{
			pac_point1 = strstr(pac_point, pPKVPac->fld_kv[0].value); //search the location of start code
			if(!pac_point1)
			{
				fprintf(stderr, "\tlack of start code\n");
				return -1;
			}
			pac_point1 += pPKVPac->fld_kv[0].len; //locate on the data code
			////printf("pac_point: %s;len: %d\n", pac_point1, strlen(pac_point1));
			fld_index1 = 1;
		}

		if(!strcmp(pPKVPac->fld_kv[pPKVPac->field_num-1].key, elemt[2])) //if have end code, check the end code
		{
			pac_point2 = strstr(pac_point, pPKVPac->fld_kv[pPKVPac->field_num-1].value); //search the location of end code
			if(!pac_point2)
			{
				fprintf(stderr, "\tlack of end code\n");
				return -1;
			}
			if(!strcmp(pPKVPac->fld_kv[pPKVPac->field_num-2].key, elemt[1]))
			{
				check_num = pPKVPac->fld_kv[pPKVPac->field_num-2].len;
				pac_point2 -= check_num; //locate on the check code
				check_data_type = pPKVPac->fld_kv[pPKVPac->field_num-2].data_type;
				fld_index2 = pPKVPac->field_num-2;
			}
			else
			{
				fprintf(stderr, "\tlack of check code\n");
				return -1;
			}
		}

		else if(!strcmp(pPKVPac->fld_kv[pPKVPac->field_num-1].key, elemt[1])) // the case without end code
		{
			check_num = pPKVPac->fld_kv[pPKVPac->field_num-1].len;
			pac_point2 -= check_num; //指向第一个校验符
			check_data_type = pPKVPac->fld_kv[pPKVPac->field_num-1].data_type;
			fld_index2 = pPKVPac->field_num-1;
		}
		else
		{
			fprintf(stderr, "\tlack of check code\n");
			return -1;
		}

		memcpy(check_code, pac_point2, check_num);
		switch(check_data_type) //convert the check_code with any type to a string
		{
			case 0: break;
			default:
			case 1: strcpy(check_str, check_code); break;
			case 2: BU_CAryToHCStr(check_code, check_str, check_num); break;
			case 3:	break;
			case 4: break;
		}

		uint16_t CRC_ush;
		if(1 == pPKVPac->command_type)
		{
			int mid_code_num = pac_point2-pac_point1;
			uint8_t mid_code[mid_code_num];
			memset(mid_code, '\0', mid_code_num);
			HCStrToBU_CAry(pac_point1, mid_code, mid_code_num);
			switch(pPKVPac->check_type) //check the code with a defined way
			{
				case 0: break;
				default:
				case 1: CRC_ush = CRC16(mid_code, mid_code_num/2); break;
				case 2: CRC_ush = hj212CRC16(mid_code, mid_code_num/2); break;
			}
		}
		else
		{
			switch(pPKVPac->check_type) //check the code with a defined way
			{
				case 0: break;
				default:
				case 1: CRC_ush = CRC16(pac_point1, pac_point2-pac_point1); break;
				case 2: CRC_ush = hj212CRC16(pac_point1, pac_point2-pac_point1); break;
			}
		}
		uint8_t CRC_str[10] = {0};
		HU_CNumToHCStr(CRC_ush, CRC_str); //convert a uint16_t to a string
		printf("CRC_str: %s, check_str: %s, check_num: %d, check_data_type: %d\n", CRC_str, check_str,check_num,check_data_type);

		if(strcmp(CRC_str, check_str))
		{
			fprintf(stderr, "\tcheck error\n");
			return -1;
		}
	}

	else //the case without check code
	{
		if(!strcmp(pPKVPac->fld_kv[0].key, elemt[0])) //if have start code, check the start code
		{
			pac_point1 = strstr(pac_point, pPKVPac->fld_kv[0].value); //search the location of start code
			if(!pac_point1)
			{
				fprintf(stderr, "\tlack of start code\n");
				return -1;
			}
			pac_point1 += pPKVPac->fld_kv[0].len; //locate on the data code
			////printf("pac_point: %s;len: %d\n", pac_point1, strlen(pac_point1));
			fld_index1 = 1;
		}
		if(!strcmp(pPKVPac->fld_kv[pPKVPac->field_num-1].key, elemt[2])) //if have end code, check the end code
		{
			pac_point2 = strrstr(pac_point, pPKVPac->fld_kv[pPKVPac->field_num-1].value); //search the location of end code
			if(!pac_point2)
			{
				fprintf(stderr, "\tlack of end code\n");
				return -1;
			}
			// printf("len: %d\n", pPKVPac->fld_kv[pPKVPac->field_num-1].len);
			fld_index2 = pPKVPac->field_num-1;
		}
	}
	//printf("check success\n");
	// printf("pac_point1: %s\npac_point2: %s\n",pac_point1, pac_point2);
	return 0;
}



/**
 * [schMatchMark, 寻找与给定符号相匹配的符号开始位置并返回]
 * @param  t_str [源字符串]
 * @param  mark1 [头符号]
 * @param  mark2 [与头符号适配的尾符号]
 * @return       [适配尾符号的开始位置]
 */
static uint8_t *schMatchMark(uint8_t *t_str, const uint8_t *mark1, const uint8_t *mark2)
{
	int match_num = 1;
	uint8_t *t_point = t_str;
	int mark1_len = strlen(mark1);
	int mark2_len = strlen(mark2);
	while(*t_point)
	{
		if(!memcmp(mark1, t_point, mark1_len))
		{
			match_num++;
			t_point += mark1_len;
		}
		if(!memcmp(mark2, t_point, mark2_len))
		{
			match_num--;
			if(!match_num)
			{
				return t_point;
			}
			t_point += mark2_len;
		}
		t_point++;
	}
	return t_point;
}



/**
 * [sepPacket, 根据预定义的报文格式，将接收到的报文拆分成各个字段]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 */
int sepPacket(lua_State *L)
{
	int i;
	PACKET_KEY_VAL *pPKVPac = NULL;

	pPKVPac = luaL_checkudata(L, 1, "PKV_t");

	if(2 == pPKVPac->command_type)
	{
		int fd_len = strlen(pPKVPac->space_mark_fd);
		int kv_len = strlen(pPKVPac->space_mark_kv);
		int lr_len = strlen(pPKVPac->space_mark_lr);
		uint8_t *fd_point = NULL;
		uint8_t *kv_point = NULL;
		uint8_t *lr_point = NULL;
		for(i=fld_index1; i<fld_index2; i++)
		{
			if(pPKVPac->fld_kv[i].havekey)
			{
				kv_point = strstr(pac_point1, pPKVPac->space_mark_kv);
				if(memcmp(pPKVPac->fld_kv[i].key, pac_point1, kv_point-pac_point1))
				{
					fprintf(stderr, "\tbad field order\n");
					return -2;
				}
				pac_point1 = kv_point;
				pac_point1 += kv_len;
			}
			if(!memcmp(pPKVPac->space_mark_lr, pac_point1, lr_len))
			{
				uint8_t mark1[8] = {'\0'};
				uint8_t mark2[8] = {'\0'};
				snprintf(mark1, sizeof(mark1), "%s%s", pPKVPac->space_mark_kv, pPKVPac->space_mark_lr);
				snprintf(mark2, sizeof(mark2), "%s%s", pPKVPac->space_mark_lr, pPKVPac->space_mark_fd);
				pac_point1 += lr_len;
				lr_point = schMatchMark(pac_point1, mark1, mark2);
				memset(pPKVPac->fld_kv[i].value, '\0', sizeof(pPKVPac->fld_kv[i].value));
				memcpy(pPKVPac->fld_kv[i].value, pac_point1, lr_point-pac_point1);
				pac_point1 = lr_point;
				pac_point1 += strlen(mark2);
			}
			else
			{
				fd_point = strstr(pac_point1, pPKVPac->space_mark_fd);
				memset(pPKVPac->fld_kv[i].value, '\0', sizeof(pPKVPac->fld_kv[i].value));
				memcpy(pPKVPac->fld_kv[i].value, pac_point1, fd_point-pac_point1);
				pac_point1 = fd_point;
				pac_point1 += fd_len;
			}
			// printf("num:%d,key:%s,value:%s\n",pPKVPac->fld_kv[i].num,
			// 		pPKVPac->fld_kv[i].key,pPKVPac->fld_kv[i].value);
			if(pac_point1 >= pac_point2)
				break;
		}
	}
	else
	{
		for(i=fld_index1; i<fld_index2; i++)
		{
			if(-1 == pPKVPac->fld_kv[i].len)
			{
				int len = 0;
				pac_point1 -= pPKVPac->fld_kv[i-1].len;
				switch(pPKVPac->fld_kv[i-1].data_type)
				{
					default:
					case 1: len = DIStrToDINum(pac_point1, pPKVPac->fld_kv[i-1].len)*2; break;
					case 2: len = BU_CAryToDINum(pac_point1, pPKVPac->fld_kv[i-1].len)*2; break;
					case 3: len = HCStrToDINum(pac_point1, pPKVPac->fld_kv[i-1].len)*2; break;
				}
				pac_point1 += pPKVPac->fld_kv[i-1].len;
				memset(pPKVPac->fld_kv[i].value, '\0', sizeof(pPKVPac->fld_kv[i].value));
				memcpy(pPKVPac->fld_kv[i].value, pac_point1, len);
				pPKVPac->fld_kv[i].len = len;
				pac_point1 += len;
			}
			else
			{
				memset(pPKVPac->fld_kv[i].value, '\0', sizeof(pPKVPac->fld_kv[i].value));
				memcpy(pPKVPac->fld_kv[i].value, pac_point1, pPKVPac->fld_kv[i].len);
				pac_point1 += pPKVPac->fld_kv[i].len;
			}

			// printf("num:%d,key:%s,value:%s\n",pPKVPac->fld_kv[i].num,
			// 		pPKVPac->fld_kv[i].key,pPKVPac->fld_kv[i].value);

			if(pac_point1 >= pac_point2)
				break;
		}
	}

	return 0;
}



/**
 * [resoSLPacket, 根据字段的数据类型，提取并解析该字段值为用户可辨认数据]
 * @param  arg1  [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2  [string, 待解析字段的字段名]
 * @param  arg3  [number，从该字段值的第arg3字节处开始解析]
 * @param  arg4  [number, 需要解析的数据长度：arg4个字节]
 * @return       [string/number, 根据每个字段的data_type, 解析生成的数据字符串或数字]
 */
int resoSLPacket(lua_State *L)
{
	int i;
	PACKET_KEY_VAL *pPKVPac = NULL;
	uint8_t fld_key[MAXSLLEN] = {'\0'};
	uint8_t fld_dat_S[MAXSLLEN] = {'\0'};
	int fld_dat_I = 0;
	uint8_t *fld_poi = NULL;
	int fld_fir = 0;
	int fld_len = 0;

	pPKVPac = luaL_checkudata(L, 1, "PKV_t");
	strcpy(fld_key, luaL_checkstring(L, 2)); //the field key
	fld_fir = luaL_checkint(L, 3); //the location of first byte
	fld_len = luaL_checkint(L, 4); //the length of resolved data
	////printf("%s,%d,%d\n", fld_key, fld_fir, fld_len);

	for(i=0; i<pPKVPac->field_num; i++)
	{
		if(!strcmp(fld_key, pPKVPac->fld_kv[i].key))
		{
			fld_poi = pPKVPac->fld_kv[i].value + fld_fir -1;
			////printf("fld_poi: %s\n", fld_poi);
			////printf("pPKVPac->fld_kv[i].data_type: %d\n", pPKVPac->fld_kv[i].data_type);
			switch(pPKVPac->fld_kv[i].data_type)
			{
				default:
				case 1: break;
				case 2: break;
				case 3: break;
				case 4: break;
				case 5: fld_dat_I = DIStrToDINum(fld_poi, fld_len); lua_pushinteger(L, fld_dat_I); break;
				case 6: fld_dat_I = HCStrToDINum(fld_poi, fld_len); lua_pushinteger(L, fld_dat_I); break;
				case 7: HCStrToDFStr(fld_poi, fld_dat_S, fld_len); lua_pushstring(L, fld_dat_S); break;
				case 8: fld_dat_I = HCStrToDU_INum(fld_poi, fld_len); lua_pushinteger(L, fld_dat_I); break;
			}
			break;
		}
	}
	////printf("fld_dat_S: %s\n", fld_dat_S);
	return 0;
}



/**
 * [getFieldValue_ , get the value of assigned packet's field]
 * @param  arg1    [自定义的报文格式变量]
 * @param  arg2    [字段键名]
 * @param  result1 [取出的字段值]
 * @return         [0,成功;-1,失败]
 */
static int getFieldValue_(const PACKET_KEY_VAL *pArg1, const uint8_t *pcArg2, uint8_t *cResult1)
{
	int i;

	for(i=0; i<pArg1->field_num; i++)
	{
		if(!strcmp(pcArg2, pArg1->fld_kv[i].key))
		{
			strcat(cResult1, pArg1->fld_kv[i].value);
			return 0;
		}
	}
	return -1;
}

/**
 * [getFieldValue, 获取指定报文字段的值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 字段键名]
 * @return      [string, 操作后得到的字段值]
 */
int getFieldValue(lua_State *L)
{
	const PACKET_KEY_VAL *pArg1 = NULL;
	const uint8_t *pcArg2 = NULL;
	uint8_t cResult1[MAXLEN6] = {'\0'};

	pArg1 = luaL_checkudata(L, 1, "PKV_t");
	pcArg2 = luaL_checkstring(L, 2);

	if(getFieldValue_(pArg1, pcArg2, cResult1) < 0)
	{
		fprintf(stderr, "\tcan't find the target field\n");
		return -2;
	}

	lua_pushstring(L, cResult1);
	return 0;
}



/**
 * [getField_ , get assigned field included key,value, space_mark_kv and space_mark_fd]
 * @param  pArg1    [自定义的报文格式变量]
 * @param  pcArg2   [欲获取字段的字段名]
 * @param  cResult1 [取出的字段]
 * @return          [0,成功;-1,失败]
 */
static int getField_(const PACKET_KEY_VAL *pArg1, const uint8_t *pcArg2, uint8_t *cResult1)
{
	int i;

	for(i=0; i<pArg1->field_num; i++)
	{
		if(!strcmp(pcArg2, pArg1->fld_kv[i].key))
		{
			if(pArg1->fld_kv[i].havekey)
			{
				strcat(cResult1, pArg1->fld_kv[i].key);
				strcat(cResult1, pArg1->space_mark_kv);
			}
			strcat(cResult1, pArg1->fld_kv[i].value);
			strcat(cResult1, pArg1->space_mark_fd);
			return 0;
		}
	}
	return -1;
}

/**
 * [getField, 获取指定的报文字段，其中包括字段名，字段值，名值间隔符，字段间隔符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲获取字段的字段名]
 * @return      [string, 操作后得到的字段, 包括字段名, 字段值, 间隔符，具体根据该字段的havekey决定]
 */
int getField(lua_State *L)
{
	const PACKET_KEY_VAL *pArg1 = NULL;
	const uint8_t *pcArg2 = NULL; //arg2, the original field's key
	uint8_t cResult1[MAXLEN6] = {'\0'};

	pArg1 = luaL_checkudata(L, 1, "PKV_t");
	pcArg2 = luaL_checkstring(L, 2);

	if(getField_(pArg1, pcArg2, cResult1) < 0)
	{
		fprintf(stderr, "the function getField error: can't find the target field\n");
		return -2;
	}

	lua_pushstring(L, cResult1);
	return 0;
}



/**
 * [copyFields, 将源报文格式变量的数据复制到目的报文格式变量]
 * @param  arg1 [userdata:PKV_t, 源报文格式变量]
 * @param  arg2 [userdata:PKV_t, 目标报文格式变量]
 * @param  arg3 [number, 0: 不清空目标字段; 非0数字: 清空目标字段]
 * @return      [boolean, true:复制成功; false:复制失败]
 */
int copyFields(lua_State *L)
{
	const PACKET_KEY_VAL *pArg1 = NULL;
	PACKET_KEY_VAL *pArg2 = NULL;
	int nArg3 = 0;

	pArg1 = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	pArg2 = (PACKET_KEY_VAL *)luaL_checkudata(L, 2, "PKV_t");
	nArg3 = luaL_checkint(L, 3);

	if(nArg3)
	{
		memset(pArg2, 0, sizeof(*pArg2));
	}
	if(!memcpy(pArg2, pArg1, sizeof(*pArg1)))
	{
		fprintf(stderr, "\tcopy fail\n");
		return -1;
	}

	return 0;
}



/**
 * [assemFieldValue, 组装报文指定字段的数据值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 指定的字段名]
 * @param  arg3 [string, 欲组装到字段的数据]
 * @param  arg4 [number, 0: 不清空字段值; 非0数字: 清空字段值]
 * @return      [string, 操作后得到的字段值]
 */
int assemFieldValue(lua_State *L)
{
	int i;
	PACKET_KEY_VAL *pArg1 = NULL;
	const uint8_t *pcArg2 = NULL;
	const uint8_t *pcArg3 = NULL;
	int nArg4 = 0;

	pArg1  = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	pcArg2 = luaL_checkstring(L, 2); //arg2, the targeted field's key
	pcArg3 = luaL_checkstring(L, 3); //arg3, the original string to joint into targeted field's value
	nArg4  = luaL_checkint(L, 4); //arg4, choose clear value or not

	////printf("pcArg2:%s\npcArg3:%s\nnArg4:%d\n",pcArg2, pcArg3, nArg4);
	for(i=0; i<pArg1->field_num; i++)
	{
		if(!strcmp(pcArg2, pArg1->fld_kv[i].key))
		{
			if(nArg4)
				memset(pArg1->fld_kv[i].value, '\0', sizeof(pArg1->fld_kv[i].value));
			strcat(pArg1->fld_kv[i].value, pcArg3);
			lua_pushstring(L, pArg1->fld_kv[i].value);
			return 0;
		}
	}

	fprintf(stderr, "\tcan't find assigned field\n");
	return -2;
}



/**
 * [connectField, 连接源报文的某个字段并至于目的字符串的首部或尾部]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲连接的字段的字段名]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 连接至首部; 非0数字: 连接至尾部]
 * @return      [string, 操作后得到的字符串]
 */
int connectField(lua_State *L)
{
	int i;
	const PACKET_KEY_VAL *pArg1 = NULL;
	const uint8_t *pcArg2 = NULL;
	int nArg3 = 0;
	int nArg4 = 0;
	uint8_t acMid_code[MAXLEN7] = {'\0'};

	pArg1  = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	pcArg2 = luaL_checkstring(L, 2);
	nArg3  = luaL_checkint(L, 3);
	nArg4  = luaL_checkint(L, 4);

	getField_(pArg1, pcArg2, acMid_code);
	if(nArg3)
	{
		int fd_len = strlen(pArg1->space_mark_fd);
		int code_len = strlen(acMid_code);
		for(i=0; i<fd_len; i++)
			acMid_code[code_len-1-i] = '\0';
	}
	if(nArg4)
	{
		strcat(GB_packet_ret, acMid_code);
	}
	else
	{
		strcat(acMid_code, GB_packet_ret);
		memset(GB_packet_ret, '\0', sizeof(GB_packet_ret));
		strcpy(GB_packet_ret, acMid_code);
	}

	lua_pushstring(L, GB_packet_ret);
	return 0;
}



/**
 * [connectFields, 将一些指定的字段链接成字符串]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 欲连接的字段名组成的table]
 * @param  arg3 [number, 0: 不清除字段间隔符; 非0数字: 清除字段间隔符]
 * @param  arg4 [number, 0: 不清空目标字段; 非0数字: 清空目标字符串]
 * @return      [string, 操作后得到的字符串]
 */
int connectFields(lua_State *L)
{
	int i;
	const PACKET_KEY_VAL *pArg1 = NULL;
	uint8_t acArg2[20] = {'\0'};
	int nArg3 = 0;
	int nArg4 = 0;
	uint8_t cResult1[MAXLEN7] = {'\0'};
	int nCount = 0;

	pArg1 = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	luaL_checktype(L, 2, LUA_TTABLE);
	nArg3 = luaL_checkint(L, 3);
	nArg4 = luaL_checkint(L, 4);

	lua_pop(L, 2); //释放nArg3,nArg4

	if(nArg4)
	{
		memset(GB_packet_ret, '\0', sizeof(GB_packet_ret));
	}

	nCount = lua_objlen(L, -1);
	for(i=0; i<nCount; i++)
	{
		memset(acArg2, '\0', sizeof(acArg2));
		lrawgeti(L, i+1, NULL, acArg2);
		memset(cResult1, '\0', sizeof(cResult1));
		getField_(pArg1, acArg2, cResult1);
		if(nArg3)
		{
			int fd_len = strlen(pArg1->space_mark_fd);
			int code_len = strlen(cResult1);
			int j;
			for(j=0; j<fd_len; j++)
				cResult1[code_len-1-j] = '\0';
		}
		strcat(GB_packet_ret, cResult1);
	}
	lua_pop(L, 1);

	lua_pushstring(L, GB_packet_ret);
	return 0;
}



/**
 * [countFieldsLen, 计算字符串长度并返回指定数目的string型的长度]
 * @param  arg1 [string, 待操作字符串]
 * @param  arg2 [number, 0: 不指定数目; 非0数字: 若大于字符串长度则多余位补'0',否则不指定数目]
 * @return      [string, 字符串长度]
 */
int countFieldsLen(lua_State *L)
{
	const uint8_t *arg1 = NULL;
	int arg2 = 0;
	uint8_t result1[MAXLEN1] = {'\0'};
	arg1 = luaL_checkstring(L, 1);
	arg2 = luaL_checkint(L, 2);

	int len1 = strlen(arg1);
	DINumToDIStr(len1, result1, arg2);

	lua_pushstring(L, result1);
	return 0;
}



/**
 * [sepField, 根据字段名, 拆分对应的字段值]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [string, 欲拆分字段的字段名]
 * @param  arg3 [number, 返回第arg3个子字段]
 * @return      [string, 子字段名]
 * @return      [string, 子字段值]
 */
int sepField(lua_State *L)
{
	int i;
	const PACKET_KEY_VAL *pArg1 = NULL;
	const uint8_t *pcArg2 = NULL;
	int nArg3 = 0;
	uint8_t fld_val[MAXLEN7] = {'\0'};
	FIELD_KEY_VAL nxt_layer[MAXLEN2] = {'\0'};

	pArg1 = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	pcArg2 = luaL_checkstring(L, 2);
	nArg3 = luaL_checkint(L, 3);
	////printf("pcArg2:%s nArg3:%d\n",pcArg2,nArg3);

	getFieldValue_(pArg1, pcArg2, fld_val);
	int fld_val_len = strlen(fld_val);
	uint8_t *fld_point1 = fld_val;
	uint8_t *fld_point2 = fld_val + fld_val_len -1;

	if(2 == pArg1->command_type)
	{
		int fd_len = strlen(pArg1->space_mark_fd);
		int kv_len = strlen(pArg1->space_mark_kv);
		int lr_len = strlen(pArg1->space_mark_lr);
		uint8_t *fd_point = NULL;
		uint8_t *kv_point = NULL;

		for(i=0; i<MAXLEN2; i++)
		{
			kv_point = strstr(fld_point1, pArg1->space_mark_kv);
			memcpy(nxt_layer[i].key, fld_point1, kv_point-fld_point1);
			fld_point1 = kv_point;
			fld_point1 += kv_len;

			fd_point = strstr(fld_point1, pArg1->space_mark_fd);
			memcpy(nxt_layer[i].value, fld_point1, fd_point-fld_point1);
			fld_point1 = fd_point;
			fld_point1 += fd_len;

			if(fld_point1 >= fld_point2)
			{
				break;
			}
		}
	}

	lua_pushstring(L, nxt_layer[nArg3-1].key);
	lua_pushstring(L, nxt_layer[nArg3-1].value);
	return 0;
}



/************************************************************/
// define global variable
static MN_PW mn_pw[MAXLEN1] = {'\0'}; //存储设备的序列号和密码
/************************************************************/
/**
 * [getPassWord, 获取指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [string, 操作后得到的设备密码]
 */
int getPassWord(lua_State *L)
{
	int i;
	const uint8_t *arg1 = NULL;
	arg1 = luaL_checkstring(L, 1);
	for(i=0; i<MAXLEN1; i++)
	{
		if((!strcmp(arg1, mn_pw[i].MN)) && strlen(mn_pw[i].PW))
		{
			lua_pushstring(L, mn_pw[i].PW);
			break;
		}
	}
	if(i >= MAXLEN1)
	{
		return -2;
	}

	return 0;
}



/**
 * [setPassWord, 设置指定设备(MN)的密码]
 * @param  arg1 [string, 设备的序列号MN]
 * @param  arg2 [string, 欲设置的密码]
 * @return      [boolean, true: 设置成功]
 */
int setPassWord(lua_State *L)
{
	int i;
	const uint8_t *arg1 = NULL;
	const uint8_t *arg2 = NULL;
	arg1 = luaL_checkstring(L, 1);
	arg2 = luaL_checkstring(L, 2);
	if(strlen(arg1) >= sizeof(mn_pw[0].MN) || strlen(arg2) >= sizeof(mn_pw[0].PW))
	{
		fprintf(stderr, "can't set this model's password, writing off the end of the array\n");
		return -2;
	}

	for(i=0; i<MAXLEN1; i++)
	{
		if(strlen(mn_pw[i].MN) && !strcmp(arg1, mn_pw[i].MN))
		{
			memset(mn_pw[i].PW, '\0', sizeof(mn_pw[i].PW));
			strcpy(mn_pw[i].PW, arg2);
			return 0;
		}
	}
	if(i >= MAXLEN1)
	{
		return -2;
	}

	return 0;
}



/**
 * [checkMonitorNum, 检查系统是否存在现设备, 若有返回true, 若无则存上并返回true, 若存储已满则生成错误]
 * @param  arg1 [string, 设备的序列号MN]
 * @return      [boolean, true: 检查成功]
 */
int checkMonitorNum(lua_State *L)
{
	int i;
	const uint8_t *arg1 = NULL;
	arg1 = luaL_checkstring(L, 1);
	if(strlen(arg1) >= sizeof(mn_pw[0].MN))
	{
		fprintf(stderr, "can't set monitor number, writing off the end of the array\n");
		return -2;
	}

	for(i=0; i<MAXLEN1; i++)
	{
		if(!strcmp(arg1, mn_pw[i].MN))
		{
			lua_pushboolean(L, 1);
			break;
		}
		else if(!strlen(mn_pw[i].MN))
		{
			strcpy(mn_pw[i].MN, arg1);
			return 0;
		}
	}
	if(i >= MAXLEN1)
	{
		return -2;
	}

	return 0;
}



/**
 * [getSystemTime, 获取系统时间]
 * @param  arg1 [string, 选择时间类型: 14/17位日期时间]
 * @return      [string, 操作后得到的日期时间]
 */
int getSystemTime(lua_State *L)
{
	time_t tt_sec;
	struct tm *tptr;
	struct timeb mt;
	uint8_t system_time[MAXLEN3] = {'\0'};
	int arg1 = luaL_checkint(L, 1);

	time(&tt_sec);
	tptr = localtime(&tt_sec);
	ftime(&mt);

	switch(arg1)
	{
		default:
		case 14: //year,month,day,hour,minute,second
			snprintf(system_time, sizeof(system_time), "%.4d%.2d%.2d%.2d%.2d%.2d",
				tptr->tm_year+1900,tptr->tm_mon+1,tptr->tm_mday,
				tptr->tm_hour,tptr->tm_min,tptr->tm_sec);
			break;
		case 17: //year,month,day,hour,minute,second,millisecond
			snprintf(system_time, sizeof(system_time), "%.4d%.2d%.2d%.2d%.2d%.2d%.3d",
				tptr->tm_year+1900,tptr->tm_mon+1,tptr->tm_mday,
				tptr->tm_hour,tptr->tm_min,tptr->tm_sec,mt.millitm);
			break;
		case 18: //year/month/day hour:minute:second
			snprintf(system_time, sizeof(system_time), "%.4d/%.2d/%.2d %.2d:%.2d:%.2d",
				tptr->tm_year+1900,tptr->tm_mon+1,tptr->tm_mday,
				tptr->tm_hour,tptr->tm_min,tptr->tm_sec);
			break;
	}
	lua_pushstring(L, system_time);
	return 0;
}



/**
 * [setSystemTime, 设置系统时间]
 * @param  arg1 [string, 标准的14位日期时间]
 * @return      [boolean, true: 设置成功; false: 设置失败]
 */
int setSystemTime(lua_State *L)
{
	uint8_t arg1[MAXLEN3] = {'\0'};
	strcpy(arg1, luaL_checkstring(L, 1));
	uint8_t *ptm = arg1;

	time_t tt_sec;
	struct tm tptr;
	struct timeval tv;

	tptr.tm_year = DIStrToDINum(ptm, 4) - 1900;
	ptm += 4;
	tptr.tm_mon = DIStrToDINum(ptm, 2) - 1;
	ptm += 2;
	tptr.tm_mday = DIStrToDINum(ptm, 2);
	ptm += 2;
	tptr.tm_hour = DIStrToDINum(ptm, 2);
	ptm += 2;
	tptr.tm_min = DIStrToDINum(ptm, 2);
	ptm += 2;
	tptr.tm_sec = DIStrToDINum(ptm, 2);
	tv.tv_sec = mktime(&tptr);
	tv.tv_usec = 0;
	if(settimeofday(&tv, NULL) != 0)
		return -1;

	return 0;
}



/**
 * [assemPacket, 组装报文，根据报文的格式来决定是否连接开始符，校验符，结束符]
 * @param  arg1 [userdata:PKV_t, 自定义的报文格式变量]
 * @param  arg2 [table, 存有开始符，校验符，结束符名称的table]
 * @return      [string, 操作后生成的字符串，此时已是完整的报文]
 */
int assemPacket(lua_State *L)
{
	int i;
	const PACKET_KEY_VAL *pArg1 = NULL;
	uint8_t acArg2[3][20] = {0};

	pArg1 = (PACKET_KEY_VAL *)luaL_checkudata(L, 1, "PKV_t");
	luaL_checktype(L, 2, LUA_TTABLE);
	//printf("top: %d,table:%d\n", lua_gettop(L), lua_objlen(L, -1));
	for(i=0; i<3; i++)
	{
		lrawgeti(L, i+1, NULL, acArg2[i]);
		//printf("element: %s\n", acArg2[i]);
	}
	lua_pop(L, 1);

	uint8_t *pac_point = GB_packet_ret;
	int pac_point_len = strlen(pac_point);

	if(pArg1->check_type) //the case with check code
	{
		uint16_t CRC_ush;
		if(1 == pArg1->command_type)
		{
			uint8_t mid_code[pac_point_len];
			memset(mid_code, '\0', pac_point_len);
			int mid_code_len = HCStrToBU_CAry(pac_point, mid_code, pac_point_len);
			switch(pArg1->check_type) //check the code with a defined way
			{
				case 0: break;
				default:
				case 1: CRC_ush = CRC16(mid_code, mid_code_len); break;
				case 2: CRC_ush = hj212CRC16(mid_code, mid_code_len); break;
			}
		}
		else
		{
			switch(pArg1->check_type) //check the code with a defined way
			{
				case 0: break;
				default:
				case 1: CRC_ush = CRC16(pac_point, pac_point_len); break;
				case 2: CRC_ush = hj212CRC16(pac_point, pac_point_len); break;
			}
		}
		uint8_t CRC_str[10] = {0};
		HU_CNumToHCStr(CRC_ush, CRC_str); //convert a uint16_t to a string
		strcat(pac_point, CRC_str); //add the check code
		// printf("CRC_str: %s\n", CRC_str);
	}

	if(!strcmp(pArg1->fld_kv[pArg1->field_num-1].key, acArg2[2])) //the case with end code
	{
		strcat(pac_point, pArg1->fld_kv[pArg1->field_num-1].value); //add the end code
	}

	if(!strcmp(pArg1->fld_kv[0].key, acArg2[0])) //the case with start code
	{
		uint8_t mid_code1[MAXLEN7] = {'\0'};
		snprintf(mid_code1, sizeof(mid_code1), "%s%s", pArg1->fld_kv[0].value, pac_point); //add the start code
		memset(pac_point, '\0', sizeof(pac_point));
		strcpy(pac_point, mid_code1);
	}

	lua_pushstring(L, pac_point);
	return 0;
}



/**
 * [sendn 防止发送的数据由于缓冲区满而比要求的少而写的send函数增强版]
 */
static ssize_t sendn(int sockfd, const void *vptr, size_t n, int flags)
{
	size_t		   nleft;
	ssize_t		   nsended;
	const uint8_t *ptr;

	ptr   = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nsended = send(sockfd, ptr, nleft, flags)) <= 0)
		{
			if (nsended < 0 && errno == EINTR)
				nsended = 0;		/* and call write() again */
			else
				return -1;			/* error */
		}

		nleft -= nsended;
		ptr   += nsended;
	}
	return n;
}



/**
 * [sendPacToIPBuf, 发送解析后的报文至发送缓冲区]
 * @param  arg1 [number, 0: 传输解析生成的国标报文; 1: 传输ASCII形式的原始串口报文; 2: 传输RTU形式的原始串口报文]
 * @param  arg2 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
int sendPacToIPBuf(lua_State *L)
{
	int 	  MESG_OPT = 0;
	BUFFER_t *pBuffer  = NULL;

	uint8_t  *message  = NULL;
	ssize_t   mesglen  = 0;

	MESG_OPT = luaL_checkint(L, 1);
	pBuffer = luaL_checkudata(L, 2, "BUFFER_t");

	if(MESG_OPT == 1)
	{
		message = SL_packet;
		mesglen = strlen(SL_packet);
	}
	else if(MESG_OPT == 2)
	{
		message = SL_buffer;
		mesglen = SL_buflen;
	}
	else if(MESG_OPT == 0)
	{
		message = GB_packet_ret;
		mesglen = strlen(GB_packet_ret);
	}
	else
	{
		fprintf(stderr, "\targ1 input error, only 0, 1, 2\n");
		return -2;
	}

	sem_wait(&pBuffer->sem_empty);
	sem_wait(&pBuffer->sem_mutex);

	memset(pBuffer->buffer[pBuffer->sig_save].packet, 0, MAXLEN8);
	pBuffer->buffer[pBuffer->sig_save].len = 0;
	memcpy(pBuffer->buffer[pBuffer->sig_save].packet, message, mesglen);
	pBuffer->buffer[pBuffer->sig_save].len = mesglen;
	printf("send_buffer num: %d, len: %d\n",pBuffer->sig_save, pBuffer->buffer[pBuffer->sig_save].len);
	pBuffer->sig_save = (pBuffer->sig_save + 1) % MAXLEN0;

	sem_post(&pBuffer->sem_mutex);
	sem_post(&pBuffer->sem_full);

	return 0;
}



static int saveFile(FILE **pFp, void *pac, ssize_t len)
{
	FILE *tempfp = NULL;
	char tempName[MAXLEN5] = {0};
    uint8_t message[MAXLEN8] = {0};
    ssize_t mesglen;
    char procName[MAXLEN3] = {0};
    char fileName[MAXLEN5] = {0};

    sprintf(procName, "/proc/%d/fd/%d", getpid(), fileno(*pFp));
    if(readlink(procName, fileName, MAXLEN5) < 0)
    {
    	fprintf(stderr, "\tFunction saveFile: readlink error\n");
    }
    sprintf(tempName, "%s~", fileName);

	tempfp = fopen(tempName, "wb+"); //打开临时文件

    fwrite(&len, 1, sizeof(len), tempfp);
	fwrite(pac, 1, len, tempfp);
    while(1)
    {
    	mesglen = 0;
		fread(&mesglen, 1, sizeof(mesglen), *pFp);
		if(feof(*pFp))
			break;
		memset(message, 0, MAXLEN8);
    	fread(message, 1, mesglen, *pFp);
    	fwrite(&mesglen, 1, sizeof(mesglen), tempfp);
		fwrite(message, 1, mesglen, tempfp);
    }

    fclose(tempfp);
    fclose(*pFp);

	rename(tempName, fileName); //用临时文件替换原文件。
    *pFp = fopen(fileName, "ab+");

    return 0;
}




static void *sendLocalPac_thread(void *arg)
{
	FD_FP_FLAG_MUTEX_t *pFd_fp_flag_mutex;
	pFd_fp_flag_mutex = (FD_FP_FLAG_MUTEX_t *)arg;

	pthread_detach(pthread_self());

	uint8_t *message = (uint8_t *)malloc(MAXLEN8);
	ssize_t mesglen;
	int nsend;

	while(1)
	{
		if(*pFd_fp_flag_mutex->pFlag == 1)
		{
			pthread_mutex_lock(pFd_fp_flag_mutex->pMutex);

			fseek(*pFd_fp_flag_mutex->pFp, 0, SEEK_SET);
			while(1)
			{
				mesglen = 0;
				fread(&mesglen, 1, sizeof(mesglen), *pFd_fp_flag_mutex->pFp);
				if(!feof(*pFd_fp_flag_mutex->pFp))
				{
					memset(message, 0, MAXLEN8);
					mesglen = fread(message, 1, mesglen, *pFd_fp_flag_mutex->pFp);

					if(*pFd_fp_flag_mutex->pFlag == 1)
					{
						nsend = send(*pFd_fp_flag_mutex->pFd, message, mesglen, 0);
						printf("\t---local sended: %d---\n", nsend);
					}
					else
					{
						saveFile(pFd_fp_flag_mutex->pFp, message, mesglen);
						break;
					}
					sleep(1);
				}
				else
				{
					ftruncate(fileno(*pFd_fp_flag_mutex->pFp), 0);
					break;
				}
			}
			pthread_mutex_unlock(pFd_fp_flag_mutex->pMutex);
		}

		sleep(30);
	}
}


/**
 * [connect_nonb TCP非阻塞连接]
 * @param  nsec   [0:阻塞; 非0正整数:阻塞的秒数]
 */
static int connect_nonb(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec)
{
	int flags, n, error;
	fd_set rset, wset;
	struct timeval tval;

	if((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
	{
		perror("\tF_GETFL error");
		return -1;
	}
	if(fcntl(sockfd, F_SETFL, flags|O_NONBLOCK) < 0)
	{
		perror("\tF_SETFL |= O_NONBLOCK error");
		return -1;
	}

	error = 0;
    if((n = connect(sockfd, saptr, salen)) == 0)
    {
        goto done;
    }
    else if(n < 0 && errno != EINPROGRESS)
    {
        return -1;
    }

    /* Do whatever we want while the connect is taking place */
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
    tval.tv_sec = nsec;
    tval.tv_usec = 0;

    if((n = select(sockfd+1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0)
    {
        errno = ETIMEDOUT;  /*timeout*/
        return -1;
    }
    else if(n < 0)
    {
    	perror("\tselect error");
    	return -1;
    }

    if(FD_ISSET(sockfd, &rset), FD_ISSET(sockfd, &wset))
    {
        socklen_t len = sizeof(error);
        n = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
        /* 如果发生错误，Solaris实现的getsockopt返回-1，把pending error设置给errno. Berkeley实现的
         * getsockopt返回0, pending error返回给error.我们需要处理这两种情况 */
        if(n < 0 || error)
        {
            if(error)
            	errno = error;
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "\tselect error: sockfd not set\n");
        return -1;
    }

done:
    if(fcntl(sockfd, F_SETFL, flags) < 0) /*restore file status flags*/
	{
		perror("\tF_SETFL error");
		return -1;
	}
    return 0;
}



/**
 * [udpConnect_thread udp连接与心跳包发送线程]
 * @param  arg [地址与文件描述符和互斥信号量]
 */
static void *udpConnect_thread(void *arg)
{
	IPPORT_FD_FLAG_MUTEX_t *pIpport_fd_flag_mutex = (IPPORT_FD_FLAG_MUTEX_t *)arg;

	struct sockaddr_in servaddr;
	extern char GATEWAY_NO[];
	char heart[MAXLEN1] = {0};
	char ack[MAXLEN1] = {0};
	struct timeval timeout;

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(pIpport_fd_flag_mutex->pIpPort->port));
	if(inet_pton(AF_INET, pIpport_fd_flag_mutex->pIpPort->ip, &servaddr.sin_addr) <= 0)
	{
		fprintf(stderr, "\tudpConnect_thread: inet_pton error for %s\n", pIpport_fd_flag_mutex->pIpPort->ip);
		pthread_exit((void *)-1);
	}

	sprintf(heart, "#%s#", GATEWAY_NO);
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

udpConnect:
	pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
	if((*pIpport_fd_flag_mutex->pFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("\tudpConnect_thread: udp socket error");
		pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		goto udpConnect;
	}

    if(setsockopt(*pIpport_fd_flag_mutex->pFd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)  //设置套接字选项
    {
        perror("\tudpConnect_thread: udp socket setsockopt failed");
        close(*pIpport_fd_flag_mutex->pFd);
        pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		goto udpConnect;
    }

	if(connect(*pIpport_fd_flag_mutex->pFd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("\tudpConnect_thread: udp connect_nonb error");
		close(*pIpport_fd_flag_mutex->pFd);
		pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		sleep(30);
		goto udpConnect;
	}
	pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);

	while(1)
	{
		pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
		send(*pIpport_fd_flag_mutex->pFd, heart, strlen(heart), 0);
		pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);

		memset(ack, 0, MAXLEN1);
		recv(*pIpport_fd_flag_mutex->pFd, ack, MAXLEN1, 0);

		if(strcmp(ack, "###"))
		{
			perror("\tudpConnect_thread: disconnected");
			pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
			*pIpport_fd_flag_mutex->pFlag = 0;
			pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		}
		else
		{
			pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
			*pIpport_fd_flag_mutex->pFlag = 1;
			pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		}
		sleep(60); //60秒发送一次心跳
	}
}



/**
 * [udpSend_thread udp发送线程]
 * @param  arg [缓冲区和ip地址]
 */
static void *udpSend_thread(void *arg)
{
	BUFFER_IPPORT_FLAG_t buf_ipport_flag = *(BUFFER_IPPORT_FLAG_t *)arg;

	int err;
	pthread_t tid;
	int sockfd = -1;
	IP_PORT_t ip_port = {0};
	int connectFlag = 0;
	int localOPT = 0;
	pthread_mutex_t sockfd_mutex = PTHREAD_MUTEX_INITIALIZER; //用于保护sockfd
	IPPORT_FD_FLAG_MUTEX_t ipport_fd_flag_mutex;
	BUFFER_t *pBuffer = buf_ipport_flag.pBuffer;
	uint8_t *message = NULL;
	ssize_t  mesglen = 0;

	memcpy(&ip_port, buf_ipport_flag.pIpPort, sizeof(ip_port));
	ipport_fd_flag_mutex.pIpPort = &ip_port;
	ipport_fd_flag_mutex.pFd = &sockfd;
	ipport_fd_flag_mutex.pFlag = &connectFlag;
	ipport_fd_flag_mutex.pMutex = &sockfd_mutex;

	localOPT = *buf_ipport_flag.pFlag;

	err = pthread_create(&tid, NULL, udpConnect_thread, &ipport_fd_flag_mutex);
	if(err)
	{
		fprintf(stderr, "\tudpSend_thread can't create udpConnect thread: %s\n", strerror(err));
		pthread_exit((void *)-1);
	}
	usleep(1000*200);

	message = (uint8_t *)malloc(MAXLEN8 * sizeof(uint8_t));

	FILE *fp = NULL;
	pthread_mutex_t fp_mutex = PTHREAD_MUTEX_INITIALIZER; //用于保护fp

	if(localOPT)
	{
		fp = fopen("./udp.pac", "ab+");

		FD_FP_FLAG_MUTEX_t fd_fp_flag_mutex;
		fd_fp_flag_mutex.pFd = &sockfd;
		fd_fp_flag_mutex.pFp = &fp;
		fd_fp_flag_mutex.pFlag = &connectFlag;
		fd_fp_flag_mutex.pMutex = &fp_mutex;

		err = pthread_create(&tid, NULL, sendLocalPac_thread, &fd_fp_flag_mutex);
		if(err)
		{
			fprintf(stderr, "\tudpSend_thread can't create sendLocalPac_thread: %s\n", strerror(err));
			pthread_exit((void *)-1);
		}
		usleep(200*1000);
	}

	while(1)
	{
		memset(message, 0, MAXLEN8);

		sem_wait(&pBuffer->sem_full);
		sem_wait(&pBuffer->sem_mutex);

		memcpy(message, pBuffer->buffer[pBuffer->sig_get].packet, pBuffer->buffer[pBuffer->sig_get].len);
		mesglen = pBuffer->buffer[pBuffer->sig_get].len;
		pBuffer->sig_get = (pBuffer->sig_get + 1) % MAXLEN0;

		sem_post(&pBuffer->sem_mutex);
		sem_post(&pBuffer->sem_empty);

		pthread_mutex_lock(&sockfd_mutex);
		send(sockfd, message, mesglen, 0);
		pthread_mutex_unlock(&sockfd_mutex);

		if(localOPT && (connectFlag == 0))
		{
			pthread_mutex_lock(&fp_mutex);
			fseek(fp, 0, SEEK_END);
			if(ftell(fp) > 2000000)
			{
				pthread_mutex_unlock(&fp_mutex);
				continue;
			}
			fwrite(&mesglen, 1, sizeof(mesglen), fp);
			fwrite(message, 1, mesglen, fp);
			fflush(fp);
			pthread_mutex_unlock(&fp_mutex);
		}
	}

	free(message);
	pthread_exit((void *)0);
}



/**
 * [tcpConnect_thread tcp连接与心跳包发送线程]
 * @param  arg [地址与文件描述符和互斥信号量]
 */
static void *tcpConnect_thread(void *arg)
{
	IPPORT_FD_FLAG_MUTEX_t *pIpport_fd_flag_mutex = (IPPORT_FD_FLAG_MUTEX_t *)arg;

	extern char GATEWAY_NO[];
	struct sockaddr_in servaddr;
	char heart[MAXLEN1] = {0};
	int len;

	sprintf(heart, "#%s#", GATEWAY_NO);
	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(pIpport_fd_flag_mutex->pIpPort->port));
	if(inet_pton(AF_INET, pIpport_fd_flag_mutex->pIpPort->ip, &servaddr.sin_addr) <= 0)
	{
		fprintf(stderr, "\ttcpConnect_thread: inet_pton error for %s\n", pIpport_fd_flag_mutex->pIpPort->ip);
		pthread_exit((void *)-1);
	}

tcpConnect:
	pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
	if((*pIpport_fd_flag_mutex->pFd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("\ttcpConnect_thread: TCP socket error");
	}

	int on=1;
    if(setsockopt(*pIpport_fd_flag_mutex->pFd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0)  //设置套接字选项，禁用了Nagle算法，可以小块发送
    {
        perror("\ttcpConnect_thread: TCP socket setsockopt failed");
        close(*pIpport_fd_flag_mutex->pFd);
    }

	signal(SIGPIPE, SIG_IGN);/*忽略SIGPIPE,防止进程在接收到对端发来的RST分节时再次send导致SIGPIPE信号出现而使进程崩溃*/

	if(connect_nonb(*pIpport_fd_flag_mutex->pFd, (struct sockaddr *)&servaddr, sizeof(servaddr), 2) < 0)
	{
		perror("\ttcpConnect_thread: TCP connect_nonb error");
		close(*pIpport_fd_flag_mutex->pFd);
	}
	pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);

	while(1)
	{
		pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
		len = send(*pIpport_fd_flag_mutex->pFd, heart, strlen(heart), 0);
		pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);

		if(len != strlen(heart))
		{
			perror("\ttcpConnect_thread: disconnected");
			pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
			*pIpport_fd_flag_mutex->pFlag = 0;
			close(*pIpport_fd_flag_mutex->pFd);
			pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
			sleep(30); //连接不通时，30秒后再次连接
			goto tcpConnect;
		}
		else
		{
			pthread_mutex_lock(pIpport_fd_flag_mutex->pMutex);
			*pIpport_fd_flag_mutex->pFlag = 1;
			pthread_mutex_unlock(pIpport_fd_flag_mutex->pMutex);
		}
		sleep(60); //60秒发送一次心跳
	}
}



/**
 * [tcpSend_thread, tcp发送线程]
 * @param  arg [缓冲区和ip地址]
 */
static void *tcpSend_thread(void *arg)
{
	BUFFER_IPPORT_FLAG_t buf_ipport_flag = *(BUFFER_IPPORT_FLAG_t *)arg;

	pthread_detach(pthread_self());
	int err;
	pthread_t tid;
	int sockfd = -1;
	IP_PORT_t ip_port = {0};
	int localOPT = 0;
	int connectFlag = 0;
	pthread_mutex_t sockfd_mutex = PTHREAD_MUTEX_INITIALIZER; //用于保护sockfd
	IPPORT_FD_FLAG_MUTEX_t ipport_fd_flag_mutex;
	BUFFER_t *pBuffer = buf_ipport_flag.pBuffer;
	uint8_t *message = NULL;
	ssize_t  mesglen = 0;

	memcpy(&ip_port, buf_ipport_flag.pIpPort, sizeof(ip_port));
	ipport_fd_flag_mutex.pIpPort = &ip_port;
	ipport_fd_flag_mutex.pFd = &sockfd;
	ipport_fd_flag_mutex.pFlag = &connectFlag;
	ipport_fd_flag_mutex.pMutex = &sockfd_mutex;

	localOPT = *buf_ipport_flag.pFlag;

	err = pthread_create(&tid, NULL, tcpConnect_thread, &ipport_fd_flag_mutex);
	if(err)
	{
		fprintf(stderr, "\ttcpSend_thread can't create tcpConnect thread: %s\n", strerror(err));
		pthread_exit((void *)-1);
	}
	usleep(200*1000);

	message = (uint8_t *)malloc(MAXLEN8 * sizeof(uint8_t));

	pthread_mutex_t fp_mutex = PTHREAD_MUTEX_INITIALIZER; //用于保护fp
	FILE *fp = NULL;
	if(localOPT)
	{
		fp = fopen("./tcp.pac", "ab+");

		FD_FP_FLAG_MUTEX_t fd_fp_flag_mutex;
		fd_fp_flag_mutex.pFd = &sockfd;
		fd_fp_flag_mutex.pFp = &fp;
		fd_fp_flag_mutex.pFlag = &connectFlag;
		fd_fp_flag_mutex.pMutex = &fp_mutex;

		err = pthread_create(&tid, NULL, sendLocalPac_thread, &fd_fp_flag_mutex);
		if(err)
		{
			fprintf(stderr, "\ttcpSend_thread can't create sendLocalPac_thread: %s\n", strerror(err));
			pthread_exit((void *)-1);
		}
		usleep(200*1000);
	}

	while(1)
	{
		memset(message, 0, MAXLEN8);

		sem_wait(&pBuffer->sem_full);
		sem_wait(&pBuffer->sem_mutex);

		memcpy(message, pBuffer->buffer[pBuffer->sig_get].packet, pBuffer->buffer[pBuffer->sig_get].len);
		mesglen = pBuffer->buffer[pBuffer->sig_get].len;
		pBuffer->sig_get = (pBuffer->sig_get + 1) % MAXLEN0;

		sem_post(&pBuffer->sem_mutex);
		sem_post(&pBuffer->sem_empty);

		pthread_mutex_lock(&sockfd_mutex);
		send(sockfd, message, mesglen, 0);
		pthread_mutex_unlock(&sockfd_mutex);

		if(localOPT && (connectFlag == 0))
		{
			pthread_mutex_lock(&fp_mutex);
			fseek(fp, 0, SEEK_END);
			if(ftell(fp) > 2000000)
			{
				pthread_mutex_unlock(&fp_mutex);
				continue;
			}
			fwrite(&mesglen, 1, sizeof(mesglen), fp);
			fwrite(message, 1, mesglen, fp);
			fflush(fp);
			pthread_mutex_unlock(&fp_mutex);
		}
	}

	free(message);
	pthread_exit((void *)0);
}



/**
 * [sendPacToSocket, 发送国标报文至客户端(PC机)]
 * @param  arg1 [string/nil, nil: 由进程自动识别接收的客户端(PC)报文的IP号; string: 人工指定客户端(PC)的IP号]
 * @param  arg2 [string/nil, nil: 默认客户端(PC)进程的端口号为7777; string: 人工指定客户端(PC)的端口号]
 * @param  arg3 [number, 0: 使用TCP传输; 非0数字: 使用UDP传输]
 * @param  arg4 [number, 0: 不使用网络不通时存储本地功能; 非0数字: 使用网络不通时存储本地功能]
 * @param  arg5 [userdata:BUFFER_t, 自定义的含有信号量机制的缓冲区]
 */
int sendPacToSocket(lua_State *L)
{
	const uint8_t *SERV_ADDR = NULL;
	const uint8_t *SERV_PORT = NULL;
	int 		   MODE_OPT = 0;
	int 		   LOCA_OPT = 0;
	BUFFER_t      *pBuffer = NULL;

	IP_PORT_t ip_port; //远程服务器的地址,端口
	int err;
	pthread_t tid;
	BUFFER_IPPORT_FLAG_t buffer_ipport_flag;

	SERV_ADDR = luaL_optstring(L, 1, _CLIENTADDR_); //默认由进程自动识别接收的客户端(PC)报文的IP号
	SERV_PORT = luaL_optstring(L, 2, "7777"); //默认客户端(PC)进程的端口号为7777
	MODE_OPT  = luaL_checkint(L, 3); //默认使用TCP传输
	LOCA_OPT  = luaL_checkint(L, 4); //默认不使用网络不通时存储本地功能
	pBuffer   = (BUFFER_t *)luaL_checkudata(L, 5, "BUFFER_t");

	memset(&ip_port, 0, sizeof(ip_port));
	memcpy(ip_port.ip, SERV_ADDR, strlen(SERV_ADDR));
	memcpy(ip_port.port, SERV_PORT, strlen(SERV_PORT));

	buffer_ipport_flag.pBuffer = pBuffer;
	buffer_ipport_flag.pIpPort = &ip_port;
	buffer_ipport_flag.pFlag   = &LOCA_OPT;

	if(MODE_OPT) //使用UDP传输
	{
		err = pthread_create(&tid, NULL, udpSend_thread, &buffer_ipport_flag);
		if(err)
		{
			fprintf(stderr, "\tcan't create udpSend_thread: %s\n", strerror(err));
			return -2;
		}
	}
	else //使用TCP传输
	{
		err = pthread_create(&tid, NULL, tcpSend_thread, &buffer_ipport_flag);
		if(err)
		{
			fprintf(stderr, "\tcan't create tcpSend_thread: %s\n", strerror(err));
			return -2;
		}
	}

	usleep(1000*200);
	return 0;
}
