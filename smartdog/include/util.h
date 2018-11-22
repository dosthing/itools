#ifndef __UTIL_H__
#define __UTIL_H__


#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif


#define HEX_TO_DECIMAL(char1, char2)	\
	(((char1 >= 'A') ? (((char1 & 0xdf) - 'A') + 10) : (char1 - '0')) * 16) + \
(((char2 >= 'A') ? (((char2 & 0xdf) - 'A') + 10) : (char2 - '0')))


int AscllToNum(char c);
void getnowtime(char *systime);
int my_system(const char * cmdstring); 
int Base64Enc(char *dest, const char *src, int count);
int Base64Dec(char* dest,const char* src, int count);





#ifdef __cplusplus
	}
#endif

#endif 

