#ifndef __SOCKET_H__
#define __SOCKET_H__


#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

int LogServersend(int *ServSock,char *sendbuf);

int LogServerAccept(int serverfd);

int LogServerCreate(int port);



#ifdef __cplusplus
	}
#endif

#endif 

