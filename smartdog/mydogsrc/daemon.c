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

#include "daemon.h"
#include "main.h"
#include "util.h"

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
		checklogfile();
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

	 checklogfile();
	while(1)
	{
#if 0	
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
#endif		
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


#if 0
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

	checklogfile();
	if(pthread_create(&LogLoopThreadId, NULL, dectectloop, NULL) < 0)
	{
		debugpri("LogLoopThreadId  thread error\r\n");
		//exit(0);
	} 	
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
#endif