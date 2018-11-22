/************************************************************************************************
*****Describe: This program is writen to detect the runing infomation of demo and main_gui  *****
*****Authorization: longsee                                                                 *****
*****Author: shuang liang li																*****
*****Date: 2018-04-18																		*****
*************************************************************************************************/


#ifndef __DAEMON_H__
#define __DAEMON_H__

char getPidByName(pid_t *pid, char *task_name);
void getNameByPid(pid_t pid, char *task_name);
int my_system(const char * cmdstring);
void checklogfile();
void writeloginfo(char * logs);
int LogServerParse(char *recvbuf);
int LogServersend(int *ServSock,char *sendbuf); 
int LogServerCreate();
  

#endif