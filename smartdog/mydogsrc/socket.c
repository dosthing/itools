#include <sys/types.h>  
#include <sys/stat.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>  
#include <fcntl.h> 
#include <errno.h>

#include "main.h"
#include "socket.h"


int LogServersend(int *ServSock,char *sendbuf)
{
   int sendLen;
   if(ServSock == NULL)
   {
	   debugpri("ServSock is NULL\n");
	   return -1;
   }
   sendLen = send(*ServSock, (char const*) sendbuf, strlen(sendbuf), 0);			   
   if(sendLen <= 0)
   {
	   debugpri("serverlog send error\n");
	   shutdown(*ServSock, SHUT_RDWR);
	   close(*ServSock);
	   //exit(0);
	   return -1;
   }   
   return sendLen;
}

int LogServerAccept(int serverfd)
{
	int cliSockFd;
	int sockOptVal = 1;
	int recvSize = 0;
	struct timeval timeout;
	struct sockaddr_in cliAddr;
	socklen_t addrLen;
	
	addrLen = sizeof(cliAddr);
	cliSockFd = accept(serverfd, (struct sockaddr *)&cliAddr,&addrLen);
	if(cliSockFd > 0)
	{
		debugpri("new socket accept  %d \n",cliSockFd);
	}
	if(cliSockFd <= 0)
	{
		debugpri("new socket accept   error %d \n",cliSockFd);
		usleep(500*1000);
		return -1;
	}
	timeout.tv_sec  = 3;
	timeout.tv_usec = 0;
	if(setsockopt(cliSockFd, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(timeout)) == -1)
	{
		debugpri("setsockopt cliSockFd = %d error\n",cliSockFd);
		if(cliSockFd > 0)
		{
			shutdown(cliSockFd, SHUT_RDWR);
			close(cliSockFd);
			cliSockFd = -1;
		}
		return -1;
	}
	if(setsockopt(cliSockFd, SOL_SOCKET, SO_SNDTIMEO,(char*)&timeout, sizeof(timeout)) == -1)
	{
		debugpri("setsockopt cliSockFd = %d error\n",cliSockFd);
		if(cliSockFd > 0)
		{
			shutdown(cliSockFd, SHUT_RDWR);
			close(cliSockFd);
			cliSockFd = -1;
		}
		return -1;
	}
	recvSize = 1024;
	if(setsockopt(cliSockFd, SOL_SOCKET, SO_SNDBUF, (char *)&recvSize, sizeof(recvSize)) == -1)
	{
		debugpri("setsockopt cliSockFd = %d error\n",cliSockFd);
		if(cliSockFd > 0)
		{
			shutdown(cliSockFd, SHUT_RDWR);
			close(cliSockFd);
			cliSockFd = -1;
		}
		return -1;
	}
	if(setsockopt(cliSockFd ,IPPROTO_TCP, TCP_NODELAY,&sockOptVal, sizeof(int)) == -1)
	{
		fprintf(stderr, "[%s:%s:%d]setsockopt error, sockfd = %d\n", __FILE__, __FUNCTION__, __LINE__, cliSockFd);
		if(cliSockFd > 0)
		{
			close(cliSockFd);
			cliSockFd = -1;
		}
		return -1;
	}
	
	return cliSockFd;
}

int LogServerCreate()
{
	int server_s;
	int sockOptVal = 1;
	int flags = 1;
	struct sockaddr_in svrAddr;
	
	server_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_s == -1)
	{
	   debugpri("unable to create socket");
	}
	flags = fcntl(server_s, F_GETFL, 0);
	if(fcntl(server_s, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		debugpri("set nonblock");
	}
	if (fcntl(server_s, F_SETFD, 1) == -1)
	{
		debugpri("setfd");
	}
	if(setsockopt(server_s, SOL_SOCKET, SO_REUSEADDR, &sockOptVal, sizeof(int)) == -1)
	{
	   debugpri("setsockopt error \n");
	   //shutdown(sock_fd, SHUT_RDWR);
	   //close(sock_fd);
	   //return -1;
	}
	
	memset(&svrAddr, 0, sizeof(svrAddr));
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(SERVERPORT);
	if(bind(server_s, (struct sockaddr *)&svrAddr,sizeof(struct sockaddr)) == -1)
	{
	   debugpri("bind socket error \n");
	   //shutdown(sock_fd, SHUT_RDWR);
	   //close(sock_fd);
	   return -1;
	}
	if(listen(server_s,SO_MAXCONN) == -1)
	{
	   debugpri("listen socket error \n");
	   //shutdown(sock_fd, SHUT_RDWR);
	   //close(sock_fd);
	   return -1;
	}
   return server_s;
}

