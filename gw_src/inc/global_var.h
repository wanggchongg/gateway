#ifndef _GLOBAL_VAR_H_
#define _GLOBAL_VAR_H_

#ifndef MAXLEN0
#define MAXLEN0 3
#endif

#ifndef MAXLEN1
#define MAXLEN1 10
#endif

#ifndef MAXLEN2
#define MAXLEN2 20
#endif

#ifndef MAXLEN3
#define MAXLEN3 40
#endif

#ifndef MAXLEN4
#define MAXLEN4 80
#endif

#ifndef MAXLEN5
#define MAXLEN5 160
#endif

#ifndef MAXLEN6
#define MAXLEN6 320
#endif

#ifndef MAXLEN7
#define MAXLEN7 640
#endif

#ifndef MAXLEN8
#define MAXLEN8 1000
#endif

#ifndef MAXGBLEN
#define MAXGBLEN 1000
#endif

#ifndef MAXSLLEN
#define MAXSLLEN 640
#endif


/*********************************************************************/
//报文字段格式
typedef struct
{
	int num;
	int len;
	uint8_t key[MAXLEN3];
	uint8_t value[MAXLEN6];
	int data_type;
	int havekey;
}FIELD_KEY_VAL;

typedef struct
{
	int command_type;
	int check_type;
	uint8_t space_mark_fd[MAXLEN1];
	uint8_t space_mark_kv[MAXLEN1];
	uint8_t space_mark_lr[MAXLEN1];
	int field_num;
	FIELD_KEY_VAL fld_kv[MAXLEN3];
}PACKET_KEY_VAL;
/*********************************************************************/

/*********************************************************************/
//设备的序列号和密码
typedef struct
{
	uint8_t MN[MAXLEN2];
	uint8_t PW[MAXLEN2];
}MN_PW;
/*********************************************************************/

/*********************************************************************/
//ip地址和端口号
typedef struct
{
	uint8_t ip[MAXLEN2];
	uint8_t port[MAXLEN1];
}IP_PORT_t;
/*********************************************************************/

/*********************************************************************/
//数据报文缓冲区
typedef struct
{
	uint8_t packet[MAXLEN8];
	int len;
}PACKET_t;

typedef struct
{
	sem_t sem_empty;
	sem_t sem_full;
	sem_t sem_mutex;
	int sig_save;
	int sig_get;
	PACKET_t buffer[MAXLEN0];
}BUFFER_t;
/*********************************************************************/

/*********************************************************************/
//缓冲区和ip端口
typedef struct
{
	BUFFER_t *pBuffer;
	IP_PORT_t *pIpPort;
}BUFFER_IPPORT_t;
/*********************************************************************/

/*********************************************************************/
//缓冲区和ip端口
typedef struct
{
	BUFFER_t *pBuffer;
	IP_PORT_t *pIpPort;
	int *pFlag;
}BUFFER_IPPORT_FLAG_t;
/*********************************************************************/

/*********************************************************************/
//缓冲区和文件描述符
typedef struct
{
	BUFFER_t *pBuffer;
	int *pSockfd;
}BUFFER_FD_t;
/*********************************************************************/

/*********************************************************************/
//两个缓冲区和文件描述符
typedef struct
{
	BUFFER_t *pBuffer1;
	BUFFER_t *pBuffer2;
	int *pFd;
}BUFFER2_FD_t;
/*********************************************************************/

/*********************************************************************/
//IP和文件描述符
typedef struct
{
	IP_PORT_t *pIpPort;
	int *pFd;
	pthread_mutex_t *pMutex;
}IPPORT_FD_MUTEX_t;
/*********************************************************************/

/*********************************************************************/
//IP和文件描述符
typedef struct
{
	IP_PORT_t *pIpPort;
	int *pFd;
	int *pFlag;
	pthread_mutex_t *pMutex;
}IPPORT_FD_FLAG_MUTEX_t;
/*********************************************************************/

/*********************************************************************/
//fp和文件描述符
typedef struct
{
	int *pFd;
	FILE **pFp;
	int *pFlag;
	pthread_mutex_t *pMutex;
}FD_FP_FLAG_MUTEX_t;
/*********************************************************************/

/*********************************************************************/
//地址和文件描述符
typedef struct
{
	struct sockaddr_in *pServaddr;
	int *pFd;
	pthread_mutex_t *pMutex;
}SOCKADDRIN_FD_MUTEX_t;
/*********************************************************************/

#endif
