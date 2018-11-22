
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


void main()
{
	int fd,i=0;
	char buf[64]={0};
	fd = open("/tmp/systemRunInfo",O_RDWR|O_APPEND,0666);
	
	while(1)
	{
		sprintf(buf,"######log in demo %d\n",i);
		write(fd,buf,strlen(buf));
		sleep(1); 
		i++;
	}	
	
}