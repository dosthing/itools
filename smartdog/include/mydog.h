#ifndef __MYDOG_H__
#define __MYDOG_H__


#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

int MyDogGetLogFile(char *filename);

int OpenTelnet();

int MyDogGetCpuTemperature(int type,float *temperature);



typedef int (*funGetLogFileCb)(char* /*filename*/);
typedef int (*funOpenTelnetCb)(void);
typedef int (*funGetCpuTemperatureCb)(int /*type*/,float * /*temperature*/);


typedef struct _DogCallBackObj {
	funGetLogFileCb         		GetLogFile;
	funOpenTelnetCb			OpenTelnet;
	funGetCpuTemperatureCb     GetCpuTemperature;
} DogCallBackObj;

typedef struct _DogInitParam
{
	char                            bIsOpenPrint;            // 0: close print; 1: open print
	const char*                  strSerialNum;            // device's serial number;
	const char*                  strDevMac;               // device's MAC;
	int                       	nLocalTcpPort;          // local network device listen port
	int                         	nBatch;                     // device's batch when it leave the factory;
	DogCallBackObj*          pCallBack;                  // include all function that sdk need callback;
}DogInitParam;

void InitCallBackFun( DogCallBackObj* pCallBack );


#ifdef __cplusplus
	}
#endif

#endif 

