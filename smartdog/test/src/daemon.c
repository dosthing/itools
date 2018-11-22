/************************************************************************************************
*****Describe: This program is writen to detect the runing information of demo and main_gui *****
*****Authorization: longsee                                                                 *****
*****Author: shuang liang li																*****
*****Date: 2018-04-18																		*****
*************************************************************************************************/
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
#include"daemon.h"

#ifdef debugprintf
	#define debugpri(mesg, args...) fprintf(stderr, "[Dog print:%s:%d:] " mesg "\n", __FILE__, __LINE__, ##args) 
#else
	#define debugpri(mesg, args...)
#endif

#define NOBLOCK O_NONBLOCK      /* Linux */
#define SO_MAXCONN 16	
#define SERVERPORT 10080
#define LOG_FILE "/tmp/systemRunInfo"   
#define BUF_SIZE 1024  
#define MAX_LOGFILE_SIZE 100*1024 
#define TELNET_TIME (60*60*2)
#define RQE_UNKNOW 0
#define RQE_TELNETD 1
#define RQE_LOG 2

static int telnetStartFlag = 0;

char getPidByName(pid_t *pid, char *task_name)  
 {  
     DIR *dir;  
     struct dirent *ptr;  
     FILE *fp;  
     char filepath[50];  
     char cur_task_name[50];  
     char buf[BUF_SIZE];
	 char ret = 0;	 
   
     dir = opendir("/proc");   
     if (NULL != dir)  
     {  
         while ((ptr = readdir(dir)) != NULL)  
         {  
             //jump . ..
             if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))  
                 continue;  
             if (DT_DIR != ptr->d_type)  
                 continue;  
              
             sprintf(filepath, "/proc/%s/status", ptr->d_name); 
             fp = fopen(filepath, "r");  
             if (NULL != fp)  
             {  
                 if( fgets(buf, BUF_SIZE-1, fp)== NULL ){  
                     fclose(fp);  
                     continue;  
                 }  
                 sscanf(buf, "%*s %s", cur_task_name);  
           
                 //print pid  
                 if (!strcmp(task_name, cur_task_name)){  
                     sscanf(ptr->d_name, "%d", pid);  
					 ret = 1;
                 }  
                 fclose(fp);  
             }  
         }  
         closedir(dir);  
     } 
	 return ret;
 }  
   
 void getNameByPid(pid_t pid, char *task_name) 
 {  
     char proc_pid_path[BUF_SIZE];  
     char buf[BUF_SIZE];  
   
     sprintf(proc_pid_path, "/proc/%d/status", pid);  
     FILE* fp = fopen(proc_pid_path, "r");  
     if(NULL != fp){  
         if( fgets(buf, BUF_SIZE-1, fp)== NULL ){  
             fclose(fp);  
         }  
         fclose(fp);  
         sscanf(buf, "%*s %s", task_name);  
     }  
 }  
 
 int my_system(const char * cmdstring) 
 { 
	pid_t pid; 
	int status; 
	if(cmdstring == NULL) 
	{ 
		return (1); 
	} 
	if((pid = vfork())<0) 
	{ 
	 	status = -1; 
	} 
	else if(pid == 0) 
	{ 
		execl("/bin/sh", "sh", "-c", cmdstring, (char *)0); 
		_exit(127); 
	} 
	else 
	{ 
		while(waitpid(pid, &status, 0) < 0) 
		{ 
			if(errno != EINTR) 
			{ 
				status = -1;
				break; 
			} 
		} 
	} 
	return status;  
 } 

 void checklogfile()
 {
	int ret = 0;
	FILE* pFile = NULL;
	struct stat fileinfo;
	
	ret = stat(LOG_FILE, &fileinfo);
	if(ret != 0)
	{
		debugpri("file %s doesn't exist and will be creat one now \n",LOG_FILE);
		/*
		my_system("touch /tmp/systemRunInfo");
		*/
		pFile = fopen(LOG_FILE, "wb");
		if(pFile != NULL)
		{
			debugpri("%s %d: File %s create sucess \n", __func__, __LINE__,LOG_FILE);
			fclose(pFile);
			pFile = NULL;
		}
		else
		{
			debugpri("%s %d: File %s create fail \n", __func__, __LINE__,LOG_FILE);
		}
	}	
 } 

 void writeloginfo(char * logs)
 {
	FILE* pFile = NULL;
	struct stat fileinfo;
	struct tm *ptm;
	char sys_time[64];
	char syscmd[64];
	int ret;
	long tnow = time(NULL);	
		
		
	ptm = localtime(&tnow);
	memset(sys_time, 0 ,sizeof(sys_time));
	sprintf(sys_time, "%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec);
		
	ret = stat(LOG_FILE, &fileinfo);
	if(ret != 0)
	{
		debugpri("Get %s stat error \n",LOG_FILE);
		return;
	}
	if(fileinfo.st_size > MAX_LOGFILE_SIZE)
	{
		debugpri("the file of SystemRunInfo is too large and will be delect\n");
		memset(syscmd,0,sizeof(syscmd));
		sprintf(syscmd,"rm %s",LOG_FILE);
		my_system(syscmd);
		sleep(2);
		//create  new logfile 
		memset(syscmd,0,sizeof(syscmd));
		sprintf(syscmd,"touch %s",LOG_FILE);
		my_system(syscmd);
		sleep(1);
	}
		
	pFile = fopen(LOG_FILE, "ab");
	if(pFile != NULL)
	{
		debugpri("[Time %04d-%02d-%02d %02d:%02d:%02d]  %s\n",ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec,logs);
		fprintf(pFile,"[Time %04d-%02d-%02d %02d:%02d:%02d]  %s\n",ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec,logs);
		//fprintf(pFile, "\n");
		fclose(pFile);
		pFile = NULL;
	}
	else
	{
		debugpri("%s %d: pFile == NULL\n", __func__, __LINE__);
	}
 } 
 
 void *dectectloop(void *arg)
 {
	 pid_t demopid;
	 pid_t guipid;
	 char demostatus = 0;
	 char demostatustemp = 0;
	 char miniguistatus = 0;	 
	 char miniguistatustemp = 0;
	 int count = 0;
	while(1)
	{
		if(telnetStartFlag == 1)
		{
			count ++;
			if(count > TELNET_TIME)
			{
				telnetStartFlag = 0;
				count = 0;
				my_system("kill -s 9 `ps | grep -v grep | grep telnetd | awk '{print $1}'`");
			}
		}
		else
			count = 0;
		
		demostatustemp = getPidByName(&demopid, "demo");
		miniguistatustemp = getPidByName(&guipid, "main_gui");
		//debugpri("demo pid = %d gui pid = %d\n",demopid,guipid);
	 
		if(demostatus > demostatustemp)
		{
			debugpri("demo process dump or exit, demostatus = %d demostatustemp = %d\n",demostatus,demostatustemp);
			writeloginfo("demo process dump or exit");
			demostatus = 0;
			miniguistatus = 0;	
		}
		if(((demostatus == demostatustemp) && (demostatustemp == 1)) && ((miniguistatus > miniguistatustemp)))
		{
			debugpri("minigui process dump or exit, miniguistatus = %d miniguistatustemp = %d\n",miniguistatus,miniguistatustemp);
			writeloginfo("minigui process dump or exit");
			demostatus = 0;
			miniguistatus = 0;	
		}

		if(demostatus != demostatustemp)
		{
			debugpri("demo process status change, oldstauts = %d newstatus = %d\n",demostatus,demostatustemp);
			demostatus = demostatustemp;		 
		}
		if(miniguistatus != miniguistatustemp)
		{
			debugpri("minigui process status change, oldstauts = %d newstatus = %d\n",miniguistatus,miniguistatustemp);
			miniguistatus = miniguistatustemp;		 
		}
		sleep(1);
	} 

 }
 int LogServerParse(char *recvbuf)
 {
 	 char *pt;
	 if(recvbuf == NULL)
 	{
 		debugpri("ServSock is NULL\n");
		return -1;
	}
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
 }
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

void getnowtime(char *systime)
{
	struct stat fileinfo;
	struct tm *ptm;
	char now_time[64];
	long tnow = time(NULL);
	
	ptm = localtime(&tnow);
	memset(now_time, 0 ,sizeof(now_time));
	sprintf(now_time, "%04d-%02d-%02d %02d:%02d:%02d",ptm->tm_year+1900 ,ptm->tm_mon+1 ,ptm->tm_mday ,ptm->tm_hour ,ptm->tm_min ,ptm->tm_sec);
	memcpy(systime,now_time,strlen(now_time));
}
 
void *LogServerLoop(void *arg)
 {
 	int svrSockFd, cliSockFd;
	int maxFd = 0;
	int ret = 0;
	int recvSize = 0;
	int recvLen = 0;
	int sendLen = 0;
	int count =0;
	int logfilesize = 0;
	int sizesended = 0;
	int sizereaded = 0;
	char nowtime[64]={0};
	char recvBuf[1024];
	char sendBuf[1024];
	FILE* plogfile = NULL;	
	fd_set sockFds;
	socklen_t addrLen;
	struct sockaddr_in cliAddr;
	struct linger  socklinger;
	struct timeval timeout;
	struct stat logeinfo;

	svrSockFd = LogServerCreate();
	while(1)
	{
		usleep(100*1000);
		if(telnetStartFlag == 1)
		{
			count ++;
			if(count >600)
			{
				count= 600;
				//dectectloop();
			}
		}
		
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
				addrLen = sizeof(cliAddr);
				cliSockFd = accept(svrSockFd, (struct sockaddr *)&cliAddr,&addrLen);
				if(cliSockFd > 0)
				{
					debugpri("new socket accept  %d \n",cliSockFd);
				}
				if(cliSockFd <= 0)
				{
					debugpri("new socket accept   error %d \n",cliSockFd);
					usleep(500*1000);
					continue;
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
					continue;
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
					continue;
				}
				recvSize = 1024;
				ret = setsockopt(cliSockFd, SOL_SOCKET, SO_SNDBUF, (char *)&recvSize, sizeof(recvSize));
				   
				memset(recvBuf, 0, 1024);
				recvLen = recv(cliSockFd, recvBuf, 2024, 0);
				if(recvLen <= 0)
				{
					debugpri("serverlog recv error\n");
					shutdown(cliSockFd, SHUT_RDWR);
					close(cliSockFd);
					continue;
				}
				printf("recv: %s\n",recvBuf);				
				//ret = LogServerParse(recvBuf);
				ret = 0;
				if(ret == RQE_TELNETD)
				{
					if(telnetStartFlag == 0 || count > 300) //limit it open telnetd too much in  a short time
					{
						my_system("telnetd &");
						telnetStartFlag = 1;
						count = 0;
					}
					memset(sendBuf, 0, 1024);
					sprintf(sendBuf,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n");
					if(LogServersend(&cliSockFd,sendBuf) == -1) 
					{
						debugpri("serverlog sed error\n");
						continue;
					}
					getnowtime(nowtime);
					memset(sendBuf, 0, 1024);
					sprintf(sendBuf,"\r\n<HTML>\n<HEAD>\n<TITLE >My DogServer</TITLE>\n</HEAD>\n<BODY>\n&nbsp&nbsp&nbsp&nbspmydog open telnetd sucess\n</br></br>The last update time %s</BODY>\n</HTML>\n",nowtime);
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
				else if(ret == RQE_LOG)
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
					if(stat(LOG_FILE, &logeinfo) == 0)
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
				else
				{
					memset(sendBuf, 0, 1024);
					sprintf(sendBuf,"HTTP/1.1 404 Resource not found\r\nContent-type: text/html\r\nServer: MyDogV1.0\r\n");
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
		}
	}
 }
 
 void main(int argc, char** argv)  
 {  
 	static pthread_t LogLoopThreadId;
	static pthread_t ServerLoopThreadId;
     char task_name[50];  
     pid_t pid = getpid();  
   
     //debugpri("pid of this process:%d\n", pid);  
     //getNameByPid(pid, task_name);  
       
	int i;
    debugpri("argc = %d\n", argc);
   // for(i = 0; i < argc; debugpri("argv[%d]=%s\n", i, argv[i]), i ++);

/*	checklogfile();
	if(pthread_create(&LogLoopThreadId, NULL, dectectloop, NULL) < 0)
	{
		debugpri("LogLoopThreadId  thread error\r\n");
		//exit(0);
	}
 */	
	if(pthread_create(&ServerLoopThreadId, NULL, LogServerLoop, NULL) < 0)
	{
		debugpri("ServerLoopThreadId  thread error\r\n");
		//exit(0);
	} 
	while(1)
	{
		sleep(15);
	}
	
	//dectectloop();
	//LogServerLoop();
/*	
     //strcpy(task_name, argv[0]+2); 
	 strcpy(task_name, *(argv+1));	 
     debugpri("task name is %s\n", task_name);  
     //getPidByName(task_name);  
       
     debugpri("getNameByPid:%s\n", task_name);  
     getPidByName(&pid, task_name);  
     debugpri("getPidByName:%d\n", pid);       
*/
 }  