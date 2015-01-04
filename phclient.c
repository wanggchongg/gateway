#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define SERV_PORT 19910 //PC's port

#define ALIN_ADDR "10.103.240.246" //arm board's IP
#define ALIN_PORT 19911 //arm board's port

#define LISTENQ 10
#define MAXLINE 512

typedef void Sigfunc(int);

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0)
	{
		if((nread = read(fd, ptr, nleft)) < 0)
		{
			if(errno = EINTR)
				nread = 0;
			else
				return -1;
		}
		else if (nread == 0)
		{
			break;
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n-nleft);
}

static ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwriten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwriten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwriten < 0 && errno == EINTR)
				nwriten = 0;		/* and call write() again */
			else
				return -1;		/* error */
		}

		nleft -= nwriten;
		ptr   += nwriten;
	}
	return n;
}

int str_do(int sockfd)
{
	char buf[MAXLINE] = {'\0'};
	FILE *fp = NULL;

	if(readn(sockfd, buf, MAXLINE) < 0)
	{
		perror("str_do: recvn error");
		return -1;
	}
	fputs(buf, stdout);
	fp = fopen("./GB_log", "ab+");
	fputs(buf, fp);
	fclose(fp);
	return 0;
}

int main(int argc, char **argv)
{
	int 				i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set 				rset, allset;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	int 				alinfd;
	struct sockaddr_in	alinaddr;
	socklen_t			alinlen;

	memset(&alinaddr, '\0', sizeof(alinaddr));
	alinaddr.sin_family      = AF_INET;
	inet_pton(AF_INET, ALIN_ADDR, &alinaddr.sin_addr);
	alinaddr.sin_port        = htons(ALIN_PORT);


	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return -1;
	}

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	if(bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("bind error");
		return -1;
	}

	if(listen(listenfd, LISTENQ) < 0)
	{
		perror("listen error");
		return -1;
	}

	int filefd = fileno(stdin);
	maxfd = listenfd > filefd ? listenfd : filefd;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; i++)
		client[i] = -1;
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	FD_SET(filefd, &allset);

	printf("---------------choose the command number: ---------------\n");
	printf("---------------[1]: get system time ---------------------\n");
	printf("---------------[2]: set system time ---------------------\n");
	printf("---------------[3]: get real time data ------------------\n");
	printf("---------------[4]: get ACK -----------------------------\n");
	while(1)
	{
		rset = allset;
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		printf("select: %d\n", nready);
		if(nready == -1)
		{
			fprintf(stderr, "errno: %d,%s\n", errno, strerror(errno));
			return -1;
		}

		if(FD_ISSET(listenfd, &rset))
		{
			printf("listenfd is ready.\n");
			clilen = sizeof(cliaddr);
			if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen)) < 0)
			{
				perror("accept error");
				return -1;
			}
			char buff[MAXLINE] = {'\0'};
			printf("connection from %s, port %d\n",
				inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)),
				ntohs(cliaddr.sin_port));

			for(i=0; i<FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				perror("too many clients");
				return -1;
			}
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i;
			if(--nready <= 0)
				continue;
		}

		for(i=0; i<=maxi; i++)
		{
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset))
			{
				printf("connfd is ready.\n");
				str_do(sockfd);	/* process the request */
				close(sockfd);
				FD_CLR(sockfd, &allset);
				client[i] = -1;
			}
			if(--nready <= 0)
				break;
		}

		if(FD_ISSET(filefd, &rset))
		{
			printf("stdin is ready.\n");
			int choose;
			scanf("%d", &choose);

			if((alinfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				perror("socket error");
				return -1;
			}

			if(connect(alinfd, (struct sockaddr *)&alinaddr, sizeof(alinaddr)) < 0)
			{
				perror("connect error");
				return -1;
			}

			char str1[] = "##0079;QN=20140617095521000;ST=32;CN=1011;PW=123456;MN=77777770000001;Flag=1;CP=&&&&;F001\r\n";
			char str2[] = "##0111;QN=20140617095521000;ST=32;CN=1012;PW=123456;MN=77777770000001;Flag=1;CP=&&SystemTime=20140619000001;&&;A781\r\n";
			char str3[] = "##0079;QN=20140617095521000;ST=32;CN=2011;PW=123456;MN=77777770000001;Flag=1;CP=&&&&;F3C1\r\n";
			char str4[] = "##0079;QN=20000229092200000;ST=91;CN=9021;PW=123456;MN=77777770000001;Flag=1;CP=&&&&;2EC0\r\n";
			char *ptr = NULL;
			switch(choose)
			{
				case 1: ptr = str1; break;
				case 2: ptr = str2; break;
				case 3: ptr = str3; break;
				default:
				case 4: ptr = str4; break;
			}
			int len = strlen(ptr);

			if(writen(alinfd, ptr, len) != len)
			{
				perror("writen error");
				return -1;
			}
			close(alinfd);
		}
	}
	return 0;
}
