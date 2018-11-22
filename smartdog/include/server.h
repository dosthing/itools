#ifndef __SERVER_H__
#define __SERVER_H__


#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif




typedef enum HTTP_METHOD 
{
	M_GET = 1,
	M_HEAD,
	M_PUT, 
	M_POST,
	M_DELETE,
	M_LINK,
	M_UNLINK,
	M_MOVE,
	M_TRACE
}METHOD;


typedef enum {
	AUTHORITY_NONE = 0, 
	AUTHORITY_VIEWER,
	AUTHORITY_OPERATOR,
	AUTHORITY_ADMIN,		
} AUTHORITY;


typedef struct __HTTP_URI
{
	char *name;
	int(*handler)(int fd, DogCallBackObj* pCallBack);
	AUTHORITY authority;
	int uri_flag;

	struct __HTTP_URI *next;

} HTTP_URI;


void *LogServerLoop(void *arg);


#ifdef __cplusplus
	}
#endif

#endif 

