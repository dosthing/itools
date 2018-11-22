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

#include "util.h"


int AscllToNum(char c)
{
	int ret = 0;
	
	if(c >= '0' && c <= '9')
	{
		ret =  (int)(c-'0');
	}
	else if(c >= 'a' && c <= 'f')
	{
		ret =  (int)(c-'a') + 10;
	}
	else if(c >= 'A' && c <= 'F')
	{
		ret =  (int)(c-'A') + 10;
	}
	else
	{
		ret = -1;
		printf("can't match mac char %c\n",c);
	}
	//Printf(" match mac ret %c %d\n",c,ret);
	return ret;
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

 /* base64 */
int Base64Enc(char *dest, const char *src, int count)
{
	const unsigned char Base64_EnCoding[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	int len = 0; 
	unsigned char *pt = (unsigned char *)src;

	/* count0src*/
	if(count < 0)
	{
		while(*pt++)
		{
			count++;
		}
		pt = (unsigned char *)src;
	}

	if(!count)
	{
		return 0;
	}

	while(count > 0)
	{
		*dest++ = Base64_EnCoding[(pt[0] >> 2) & 0x3f];
		if(count > 2)
		{
			*dest++ = Base64_EnCoding[((pt[0] & 3) << 4) | (pt[1] >> 4)];
			*dest++ = Base64_EnCoding[((pt[1] & 0xF) << 2) | (pt[2] >> 6)];
			*dest++ = Base64_EnCoding[ pt[2] & 0x3F];
		}
		else
		{
			switch(count)
			{
				case 1:
					*dest++ = Base64_EnCoding[(pt[0] & 3) << 4];
					*dest++ = '=';
					*dest++ = '=';
					break;
				case 2:
					*dest++ = Base64_EnCoding[((pt[0] & 3) << 4) | (pt[1] >> 4)];
					*dest++ = Base64_EnCoding[((pt[1] & 0x0F) << 2) | (pt[2] >> 6)];
					*dest++ = '=';
			}
		}
		pt += 3;
		count -= 3;
		len += 4;
	}
	*dest = 0;
	return len;
}


/* base64 */
int Base64Dec(char* dest,const char* src, int count)
{
	unsigned char ucs[4];
	unsigned char *pt = (unsigned char *)src;
	int len = 0;
	int equal_flag = 0;
	int nfag = 0;
	int i = 0;

	/*count0src*/
	if(count < 0)
	{
		while(*pt++)
		{
			count++;
		}
		pt = (unsigned char *)src;
	}

	if(!count)
	{
		return 0;
	}

	while(count > 0)
	{
		nfag = 0;
		for(i = 0 ; i < 4 ; i++)
		{
			if (*pt >= 'A' && *pt <= 'Z')
			{
				ucs[i] = *pt - 'A';
			}
			else if (*pt >= 'a' && *pt <= 'z')
			{
				ucs[i] = *pt - 'a' + 26;
			}
			else if (*pt >= '0' && *pt <= '9')
			{
				ucs[i] = *pt - '0' + 52;
			}
			else
			{
				switch (*pt)
				{
					case '+':
						ucs[i] = 62;
						break;
					case '/':
						ucs[i] = 63;
						break;
					case '=': /* base64 padding */ 
						ucs[i] = 0;
						equal_flag++;
						break;
					case '\t':
					case '\r':
					case '\n':
					case ' ':
						nfag++;
						i--;
						break;
					case '\0': /*, buf */ 
						*dest = 0;
						return len;
						break;
					default:  /* , */
						*dest = 0;
						return -1;
				}
			}
			pt++;
		}

		*dest++ = (ucs[0] << 2) | (ucs[1] >> 4);
		*dest++ = (ucs[1] << 4) | (ucs[2] >> 2);
		*dest++ = (ucs[2] << 6) | (ucs[3]);
		count -= nfag + 4;
		len += 3;
	}

	*dest = 0;
	len -= equal_flag;
	return len;
}

