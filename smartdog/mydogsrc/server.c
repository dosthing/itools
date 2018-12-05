#include <sys/types.h>  
#include <sys/stat.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h>
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>  
#include <pthread.h>   
#include <dirent.h> 
#include <time.h>
#include <fcntl.h> 
#include <errno.h>

#include "main.h"
#include "util.h"
#include "mydog.h"
#include "socket.h"
#include "server.h"

void LogServerSendHeader(int fd)
{
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"%s",HEADER);
	if(LogServersend(&fd,sendBuf) == -1) 
	{
		debugpri("serverlog sed error\n");
		//return;
	}
	return;
}

void LogServerSendHeaderstatic(int fd, char * contentType, long length)
{
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"HTTP/1.1 200 OK\r\nContent-type: %s\r\nContent-Length: %ld\r\nPragma: no-cache\r\nCache-Control: no-store\r\nServer: MyDogV1.0\r\n\r\n",contentType,length);
	if(LogServersend(&fd,sendBuf) == -1) 
	{
		debugpri("serverlog sed error\n");
		//return;
	}
	return;
}

void LogServerSendBadRequest(int fd)
{
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"HTTP/1.1 404 Resource not found\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n\r\n");
	if(LogServersend(&fd,sendBuf) == -1) 
	{
		debugpri("serverlog sed error\n");
		return;
	}
	debugpri("serverlog send bad request\n");
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return;
	
}

void LogServerSendUnAuthorized(int fd, char *url)
{
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"HTTP/1.1 401 Unauthorized\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n\r\n");
	if(LogServersend(&fd,sendBuf) == -1) 
	{
		debugpri("serverlog sed error\n");
		return;
	}
	debugpri("serverlog send unauthorized url:%s\n",url);
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return;
}


void LogServerSendOk(int fd)
{
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n\r\n");
	if(LogServersend(&fd,sendBuf) == -1) 
	{
		debugpri("serverlog sed error\n");
		return;
	}
	debugpri("serverlog send Ok \n");
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return;
}

int Mydog_dispatch_get_index(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{
	FILE* pindexfile = NULL;
	struct stat indexinfo;
	char sendBuf[1024]={0};

	memset(sendBuf, 0, 1024);
	LogServerSendHeader(fd);
	if(stat("index.html", &indexinfo) == 0)
	{
		pindexfile=fopen("index.html","r");
		if(pindexfile == NULL)
		{
			debugpri("index file open error!\n"); 					
			pindexfile = NULL;
			shutdown(fd, SHUT_RDWR);
			close(fd);
			return -1;
		}
		memset(sendBuf, 0, 1024);		
		while( fgets(sendBuf, 1024,pindexfile) != NULL)
		{				
			if(LogServersend(&fd,sendBuf) == -1) 
			{
				debugpri("serverlog sed error\n");
				fclose(pindexfile);
				pindexfile = NULL;
				break;
			}
			
			memset(sendBuf, 0, 1024);
		}
	}
	fclose(pindexfile);
	pindexfile = NULL;
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return 0;
	
}

int Mydog_dispatch_get_favicon(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{
	FILE* favicon = NULL;
	int ret = 0;
	int sendedlen = 0;
	struct stat ifaviconinfo;
	char sendBuf[32*1024]={0};
	char contentType[512];

	if(stat("favicon.ico", &ifaviconinfo) == 0)
	{
		GetFileContentType("favicon.ico",contentType);
		favicon=fopen("favicon.ico","r");
		if(favicon == NULL)
		{
			debugpri("favicon file open error!\n");					
			favicon = NULL;
			LogServerSendBadRequest(fd);
			return -1;
		}
		LogServerSendHeaderstatic(fd,contentType,ifaviconinfo.st_size);	
		memset(sendBuf, 0, sizeof(sendBuf));	
		do{
			ret = fread(sendBuf, 1, sizeof(sendBuf), favicon);
			if(ret > 0 && ret <= sizeof(sendBuf))
			{
				ret = send(fd, sendBuf, ret, 0);
				if(ret > 0)
				{
					sendedlen += ret;
				}
				debugpri("static data size = %d ret = %d \n",sendedlen,ret);
			}		
		}while(sendedlen < ifaviconinfo.st_size);

	}
	fclose(favicon);
	favicon = NULL;
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return 0;
}

int Mydog_dispatch_get_log(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{	
	int ret;
	if(pCallBack == NULL)
	{
		debugpri("waring pCallBack is NULL \n");
		LogServerSendBadRequest(fd);
		return -1;
	}	
	ret = pCallBack->GetLogFile(NULL);
	LogServerSendBadRequest(fd);
	return ret;
#if 0
	FILE* plogfile = NULL;
	struct stat logeinfo;

	else if(ret == 2) //RQE_LOG
	{
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n");
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"\r\n<HTML>\n<HEAD>\n<TITLE >My DogServer</TITLE>\n</HEAD>\n<BODY>\n");
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}					

	sizesended = 0;
	logfilesize = 0;
	sizereaded = 0;
	if(stat("/tmp/systemRunInfo", &logeinfo) == 0)//LOG_FILE
	{
	logfilesize = logeinfo.st_size;
	if(logfilesize == 0)
	{	
	getnowtime(nowtime);
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"The system is ok, deosn't exist any error log.\n</br></br>&nbsp&nbspThe last update time %s</BODY>\n</HTML>\n",nowtime);
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}
	debugpri("serverlog send ok\n");
	shutdown(cliSockFd, SHUT_RDWR);
	close(cliSockFd);
	continue;
	}
	}
	else
	{
	debugpri("LOG_FILE deosn't exist!\n");
	checklogfile();						
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"Sorry that the My DogServer can't find log file.\n</br></BODY>\n</HTML>\n");
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}
	debugpri("serverlog send ok\n");
	shutdown(cliSockFd, SHUT_RDWR);
	close(cliSockFd);
	continue;
	}
	plogfile=fopen(LOG_FILE,"r");
	if(plogfile == NULL)
	{
	debugpri("log file open error!\n");						
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"Sorry that the log file open error.\n</BODY>\n</HTML>\n");
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}
	shutdown(cliSockFd, SHUT_RDWR);
	close(cliSockFd);
	continue;
	}
	memset(sendBuf, 0, 1024);
	//while(sizesended < logfilesize)
	while( fgets(sendBuf, 1024,plogfile) != NULL)
	{						
	//sizereaded =  fgets(sendBuf, sizeof(char), 1024,plogfile);
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	fclose(plogfile);
	plogfile = NULL;
	break;
	}
	else
	{
	LogServersend(&cliSockFd," </br> ");
	}
	//sizesended += sizereaded;
	memset(sendBuf, 0, 1024);
	}
	getnowtime(nowtime);
	memset(sendBuf, 0, 1024);
	sprintf(sendBuf,"\n</br>&nbsp&nbsp&nbspThe last update time %s</BODY>\n</HTML>\n",nowtime);
	if(LogServersend(&cliSockFd,sendBuf) == -1) 
	{
	debugpri("serverlog sed error\n");
	continue;
	}
	debugpri("serverlog send ok\n");
	shutdown(cliSockFd, SHUT_RDWR);
	close(cliSockFd);
	continue;
	}	
	#endif
}


int Mydog_dispatch_open_telnet(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{
	int ret =0;	
	char nowtime[64]={0};
	char sendBuf[1024]={0};
	
	memset(sendBuf, 0, 1024);
	if(pCallBack == NULL)
	{
		debugpri("waring pCallBack is NULL \n");
		LogServerSendBadRequest(fd);
		return -1;
	}	
	ret = pCallBack->OpenTelnet();
	if(ret ==0)
	{		
		sprintf(sendBuf,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n");
		if(LogServersend(&fd,sendBuf) == -1) 
		{
			debugpri("serverlog sed error\n");
			return -1;
		}
		getnowtime(nowtime);
		memset(sendBuf, 0, 1024);
		sprintf(sendBuf,"\r\n<HTML>\n<HEAD>\n<TITLE >My DogServer</TITLE>\n</HEAD>\n<BODY>\n&nbsp&nbsp&nbsp&nbspmydog open telnetd sucess\n</br></br>The last update time %s</BODY>\n</HTML>\n",nowtime);
		if(LogServersend(&fd,sendBuf) == -1) 
		{
			debugpri("serverlog sed error\n");
			return -1;
		}
		debugpri("serverlog send ok\n");
		shutdown(fd, SHUT_RDWR);
		close(fd);
		return 0;
	}
	LogServerSendBadRequest(fd);
	return -1;
	
}

int Mydog_dispatch_get_login(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{	
	if((strcmp(arg[0].value,"admin") == 0) && (strcmp(arg[1].value,"12345") == 0))
	{
		//LogServerSendHeader(fd);
		//LogServersend(&fd,"login sucess");
		return Mydog_dispatch_get_static(fd, "static/login/sucess.html");		
	}
	else
	{
		//LogServersend(&fd,"login fail, user name or password error\n");
		return Mydog_dispatch_get_static(fd, "static/login/fail.html");	
	}	
	//shutdown(fd, SHUT_RDWR);
	//close(fd);
	//return 0;
}

int Mydog_dispatch_get_temperature(int fd, DogCallBackObj* pCallBack, int argnum,HTTP_ARGUMENTS_t *arg)
{
	int ret =0;	
	char nowtime[64]={0};
	char sendBuf[1024]={0};
	
	memset(sendBuf, 0, 1024);
	if(pCallBack == NULL)
	{
		debugpri("waring pCallBack is NULL \n");
		LogServerSendBadRequest(fd);
		return -1;
	}	
	ret = pCallBack->OpenTelnet();
}

int GetFileContentType(char *file, char *contentType)
{
	if(strstr(file,"png")!= NULL)
	{
		sprintf(contentType,"%s","image/png");
		return 0;
	}
	else if(strstr(file,"jpg")!= NULL)
	{
		sprintf(contentType,"%s","image/png");
		return 0;
	}
	else if(strstr(file,"ico")!= NULL)
	{
		sprintf(contentType,"%s","image/png");
		return 0;
	}
	else if(strstr(file,"css")!= NULL)
	{
		sprintf(contentType,"%s","text/css");
		return 0;
	}
	else if(strstr(file,"html")!= NULL)
	{
		sprintf(contentType,"%s","text/html");
		return 0;
	}
	else if(strstr(file,"js")!= NULL)
	{
		sprintf(contentType,"%s","application/x-javascript");
		return 0;
	}
	else
	{
		sprintf(contentType,"%s","text/html");
	}

	return 0;
}


int Mydog_dispatch_get_static(int fd, char *file)
{
	int ret;
	int sendedlen = 0;
	FILE* pfile = NULL;
	struct stat indexinfo;
	char contentType[512];
	char sendBuf[256*1024]={0};
	
	if(stat(file, &indexinfo) != 0)
	{
		LogServerSendBadRequest(fd);
		return -1;
	}
	
	GetFileContentType(file,contentType);
	pfile=fopen(file,"r");
	if(pfile == NULL)
	{
		debugpri("index file open error!\n"); 					
		pfile = NULL;
		LogServerSendBadRequest(fd);
		return -1;
	}
	LogServerSendHeaderstatic(fd,contentType,indexinfo.st_size);	
	memset(sendBuf, 0, sizeof(sendBuf));	
	do{
		ret = fread(sendBuf, 1, 256 * 1024, pfile);
		if(ret > 0 && ret <= 256 * 1024)
		{
			ret = send(fd, sendBuf, ret, 0);
			if(ret > 0)
			{
				sendedlen += ret;
			}
			debugpri("static data size = %d ret = %d \n",sendedlen,ret);
		}		
	}while(sendedlen < indexinfo.st_size);
	
/*	while( fgets(sendBuf, 1024,pfile) != NULL)
	{	
		if((send(fd, (char const*) sendBuf, strlen(sendBuf),0)) < 0)	
		//if(LogServersend(&fd,sendBuf) == -1) 
		{
			debugpri("serverlog sed error\n");	
			break;
		}
		debugpri("static data\n");
		memset(sendBuf, 0, 1024);
	}
	*/
	debugpri("static data send ok\n");
	fclose(pfile);
	pfile = NULL;
	shutdown(fd, SHUT_RDWR);
	close(fd);
	return 0;
}



HTTP_URI MydogDispatch [] =
{
	{"/"						,Mydog_dispatch_get_index		,AUTHORITY_NONE       ,0,0,{0},NULL },
	{"/favicon.ico"			,Mydog_dispatch_get_favicon		,AUTHORITY_NONE       ,0,0,{0},NULL },
	{"/loginservlet"			,Mydog_dispatch_get_login		,AUTHORITY_NONE       ,0,0,{0},NULL },
	{"/log"					,Mydog_dispatch_get_log			,AUTHORITY_VIEWER    ,0,0,{0},NULL },
	{"/adminxc12345678"		,Mydog_dispatch_open_telnet		,AUTHORITY_VIEWER    ,0,0,{0},NULL },
	{"/hitemperature"			,Mydog_dispatch_get_temperature	,AUTHORITY_VIEWER    ,0,0,{0},NULL },
};	

int LogServerParseParameters(HTTP_URI *req,char *recvbuf, char *name)
{
	int i,ret = -1;
	char *p = NULL;
	char *stop, *stop2;
	char parameters[2014]={0}; 

	if (!memcmp(recvbuf, "GET ", 4))
	{
		p = strstr(recvbuf,"GET ");	
		if(p != NULL)
		{
			while (*(++p) != ' ');
				stop = p;
			while (*(++p) != '\0' && *p != ' ');
				stop2 = p;
			stop2++;
			memcpy(parameters, stop, stop2 - stop);	
			parameters[stop2 - stop] = '\0';
			printf("parase:  %s\n",parameters);
		}
	}
	else if (!memcmp(recvbuf, "POST ", 5))
	{
		p = strstr(recvbuf,"\r\n\r\n");	
		if(p != NULL)
		{
			p = p+4;		
			stop = p;
			while (*(++p) != '\0' && *p != ' ');
				stop2 = p;
			stop2++;
			memcpy(parameters, stop, stop2 - stop);	
			parameters[stop2 - stop] = '\0';
			printf("parase:  %s\n",parameters);
		}
	}
	else 
	{
		fprintf(stderr, "malformed request: \"%s\"\n",recvbuf);		
		return ret;
	}	

	while(i < strlen(parameters))
	{
		if(parameters[i] == '=') // have parameters, so parse paramerters 
		{
			memset(req->parameters,0,sizeof(req->parameters));
			req->parameterscount = 0;
			p = parameters;
			stop = p;	
			while(*p != '\0')
			{
				while (*(++p) != '=');
				stop2 = p;						
				memcpy(req->parameters[req->parameterscount].name, stop, stop2 - stop);	
				req->parameters[req->parameterscount].name[stop2 - stop] = '\0';			

				stop2++;
				stop = stop2;
				while (*(++p) != '&' && *p != '\0');				
				stop2 = p;	
				memcpy(req->parameters[req->parameterscount].value, stop, stop2 - stop); 
				req->parameters[req->parameterscount].value[stop2 - stop] = '\0';
				stop2++;	
				stop = stop2;
				req->parameterscount++;				
			}
			ret = 0;
			break;
		
		}
		i++;	
	}
	for(i = 0; i < req->parameterscount; i++)
	{
		printf("argname = %s    argvalue = %s\n",req->parameters[i].name,req->parameters[i].value);
	}
	
	return ret;	
}

int LogServerParseDispatch(char *recvbuf, char *name)
{
	char *stop, *stop2;
	char url[2014]={0}; 	
	
	if(recvbuf == NULL)
	{
		debugpri("ServSock is NULL\n");
		return -1;
	}
	if (strlen(recvbuf) < 5) 
	{
		fprintf(stderr, "Request too short: \"%s\"\n",recvbuf);
		//send_r_bad_request(req);
		return -1;
	}
	if (!memcmp(recvbuf, "GET ", 4))
	{
		fprintf(stderr, "GET request: \"%s\"\n",recvbuf);	
	}
	else if (!memcmp(recvbuf, "POST ", 5))
	{
		;
	}
	else 
	{
		fprintf(stderr, "malformed request: \"%s\"\n",recvbuf);		
		return -1;
	}	

	/* Guaranteed to find ' ' since we matched a method above */
	stop = recvbuf;
	stop = recvbuf + 3;
	if (*stop != ' ')
		++stop;
	
	/* scan to start of non-whitespace */
	while (*(++stop) == ' ');
		stop2 = stop;

	/* scan to end of non-whitespace */
	while (*stop2 != '\0' && *stop2 != ' ')
		++stop2;
	
	if (stop2 - stop >1024 ) //MAX_HEADER_LENGTH 1024
	{
		fprintf(stderr, "URI too long %d: \"%s\"\n", 1024,recvbuf); 
		return -1;
	}

	memcpy(url, stop, stop2 - stop);	
	url[stop2 - stop] = '\0';
	memcpy(name,url,strlen(url));	
	debugpri("parse request %d  url = %s\n",stop2 - stop,name);
	return 0;

#if 0
	//sscanf(recvbuf,"%s %s",str1,str2);
	pt = NULL;
	pt = strstr(recvbuf,"/log");
	if(pt != NULL)
	{
		if((strncmp(pt+1,"log",3) == 0) && (strncmp(pt+1+3," ",1) == 0))
		{
			pt = NULL;
			debugpri("find log\n");
			return RQE_LOG;
		}	   
	}
	pt = NULL;
	pt = strstr(recvbuf,"/adminxc12345678");
	if(pt != NULL)
	{
		if((strncmp(pt+1,"adminxc12345678",15) == 0) && (strncmp(pt+1+15," ",1) == 0))
		{
			pt = NULL;
			debugpri("find telnetd\n");
			return RQE_TELNETD;
		}	   
	}
	pt = NULL;
	return RQE_UNKNOW;
#endif	
}

int LogServerParseAuthority(char *recvbuf, char *authority)
{
	char *start,*stop;
	start = NULL;
	stop = NULL;
	start = strstr(recvbuf,"Authorization: Basic ");
	if(start != NULL)
	{
		stop = start;
		/* scan to start of non-whitespace */
		while (*(++stop) == ' ');		
		if (stop - start >128 ) //MAX_HEADER_LENGTH 1024
		{
			fprintf(stderr, "URI too long %d: \"%s\"\n", 128,recvbuf); 
			return -1;
		}
		memcpy(authority, start, stop - start);
		authority[stop - start] = '\0';
		return 0;
	}
	return -1;
}



int LogServerDecodeAuthority(char *recvbuf)
{
	int ret;
	char auth[128];
	char token[128] = {0};

	ret = LogServerParseAuthority(recvbuf,auth);
	if(ret != 0)
	{
		debugpri("ParseAuthority error \n");
		return -1;
	}
	Base64Dec(token,auth,sizeof(auth));
	if(strcmp(token,"admin:12345") == 0)
	{
		return 0;
	}
	return -1;
}

void *LogServerLoop(void *arg)
 {
 	int svrSockFd,cliSockFd;
	int maxFd = 0;
	int i = 0;
	int ret = 0;	
	int recvLen = 0;
	int sendLen = 0;
	int count =0;
	int logfilesize = 0;
	int sizesended = 0;
	int sizereaded = 0;	
	char recvBuf[16*1024];
	char url[1024];
	fd_set sockFds;
	struct timeval timeout;
	
	
	struct linger  socklinger;
	

	DogInitParam *MyDogInitParam = NULL;
	MyDogInitParam = (DogInitParam*)arg; 
	
	svrSockFd = LogServerCreate();
	while(1)
	{
		FD_ZERO(&sockFds);
		FD_SET(svrSockFd, &sockFds);
		maxFd = svrSockFd;
		timeout.tv_sec  = 2;
		timeout.tv_usec = 0;	/* 2 s timeout */
		ret = select(maxFd + 1, &sockFds, NULL, NULL, &timeout);
		if(ret < 0)
		{
			debugpri("select socket error \n");
		}
		else if(ret == 0)
		{
			continue;
		}
		else
		{
			if(FD_ISSET(svrSockFd, &sockFds))
			{			
				cliSockFd =LogServerAccept(svrSockFd);
				if(cliSockFd < 0)
				{
					debugpri("Mydog Server accept error\n");
					continue;
				}
				memset(recvBuf, 0, sizeof(recvBuf));
				recvLen = recv(cliSockFd, recvBuf, sizeof(recvBuf), 0);
				if(recvLen <= 0)
				{
					debugpri("serverlog recv error\n");
					shutdown(cliSockFd, SHUT_RDWR);
					close(cliSockFd);
					continue;
				}		
				printf("recv: %s\n",recvBuf);
				memset(url, 0, sizeof(url));
				ret = LogServerParseDispatch(recvBuf, url);
				if(ret != 0)
				{
					LogServerSendBadRequest(cliSockFd);
					continue;
				}				
				if (!memcmp(url, "/static/", 8))//dispatch static files
				{
					Mydog_dispatch_get_static(cliSockFd, &url[1]);// static/snap.jpg
					continue;
				}
				for(i = 0; i < (sizeof(MydogDispatch)/sizeof(HTTP_URI)); i++)
				{
					
					if(strcmp(MydogDispatch[i].name,url) == 0)
					{						
						if(MydogDispatch[i].authority >= AUTHORITY_OPERATOR)
						{						
							 ret = LogServerDecodeAuthority(recvBuf);
							 if(ret != 0)
							 {
								LogServerSendUnAuthorized(cliSockFd,url);
								break;
							 }
						}
						ret = LogServerParseParameters(&MydogDispatch[i],recvBuf, url);
						if(ret != 0)
						{
							//LogServerSendBadRequest(cliSockFd);
							//break;
						}
						MydogDispatch[i].handler(cliSockFd,MyDogInitParam->pCallBack,MydogDispatch[i].parameterscount,MydogDispatch[i].parameters);						
						break;
					}
				}
				if( i == (sizeof(MydogDispatch)/sizeof(HTTP_URI)))//doesn't match anything 
				{
					LogServerSendBadRequest(cliSockFd);
				}
				
			}
		}
	}
 }

