#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>  
#include <pthread.h>  
#include <errno.h>
#include <signal.h> 


#include "daemon.h"
#include "mydog.h"
#include "main.h"
#include "server.h"

void MyDog_sigsegv(int dummy)
{
	fprintf(stderr, "MyDog Caught SIGSEGV, Abort!\n");
	abort();
}

void MyDog_sigterm(int dummy)
{
	fprintf(stderr, "MyDog Caught SIGTERM, Abort!\n");
	fclose(stderr);
	exit(0);
}

void MyDog_init_signals(void)
{
	struct sigaction sa;
	sa.sa_flags = 0;
	
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGSEGV);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGPIPE);
	
	sa.sa_handler = MyDog_sigsegv;
	sigaction(SIGSEGV, &sa, NULL);

	sa.sa_handler = MyDog_sigterm;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}


int main (int argc, char *argv[])
{
	static pthread_t LogLoopThreadId;
	static pthread_t ServerLoopThreadId;
	
	DogCallBackObj MyDogCallback;
	DogInitParam MyDogInitParam, *p=NULL;
	
	MyDog_init_signals();

	InitCallBackFun(&MyDogCallback);
	MyDogInitParam.bIsOpenPrint = 0;
	MyDogInitParam.strSerialNum= "12345678";
	MyDogInitParam.httpport = SERVERPORT;
	

	MyDogInitParam.pCallBack = &MyDogCallback;

/*	if(pthread_create(&LogLoopThreadId, NULL, dectectloop, NULL) < 0)
	{
		debugpri("LogLoopThreadId  thread error\r\n");
		//exit(0);
	}
*/	MyDogInitParam.pCallBack->OpenTelnet();
	if(pthread_create(&ServerLoopThreadId, NULL, LogServerLoop, (void *)&MyDogInitParam) < 0)//(void *)&MyDogInitParam
	{
		debugpri("ServerLoopThreadId  thread error\r\n");
		//exit(0);
	} 
	
	while(1)
	{
		sleep(15);
	}
	

}
