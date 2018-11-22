
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>   
#include <time.h>

#include "daemon.h"
#include "main.h"
#include "mydog.h"


int MyDogGetLogFile(char *filename)
{
	printf("MyDogGetLogFile\n");
	return 0;
}

int OpenTelnet()
{
	printf("OpenTelnet\n");
	//my_system("telnetd &");
	return 0;
}

int MyDogGetCpuTemperature(int type,float *temperature)
{
	printf("MyDogGetCpuTemperature\n");
	return 0;
}



void InitCallBackFun( DogCallBackObj* pCallBack )
{
	pCallBack->GetLogFile = MyDogGetLogFile;
	pCallBack->OpenTelnet = OpenTelnet;
	pCallBack->GetCpuTemperature = MyDogGetCpuTemperature;
}




