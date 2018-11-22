/************************************************************************************************
*****Describe: This program is writen to detect the runing infomation of demo and main_gui  *****
*****Authorization: longsee                                                                 *****
*****Author: shuang liang li																*****
*****Date: 2018-04-18																		*****
*************************************************************************************************/


#ifndef __DAEMON_H__
#define __DAEMON_H__

void *dectectloop(void *arg);
char getPidByName(pid_t *pid, char *task_name);
void getNameByPid(pid_t pid, char *task_name);
void checklogfile();

  

#endif