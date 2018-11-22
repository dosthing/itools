#ifndef __MAIN_H__
#define __MAIN_H__


#ifdef __cplusplus
extern "C" {
#endif

#ifdef debugprintf
	#define debugpri(mesg, args...) fprintf(stderr, "[Dog print:%s:%d:] " mesg "\n", __FILE__, __LINE__, ##args) 
#else
	#define debugpri(mesg, args...)
#endif 


#define NOBLOCK O_NONBLOCK      /* Linux */
#define SO_MAXCONN 16	
#define SERVERPORT 10080
#define LOG_FILE "/tmp/systemRunInfo"   
#define BUF_SIZE 1024  
#define MAX_LOGFILE_SIZE 100*1024 
#define TELNET_TIME (60*60*2)
#define RQE_UNKNOW 0
#define RQE_TELNETD 1
#define RQE_LOG 2


typedef enum _SDK_MAIN_MSG_ID{
      SDK_MAIN_MSG_VERSION = 0x001    //    	
    , SDK_MAIN_MSG_STATUS             //		
    , SDK_MAIN_MSG_PARAM_DEFAULT      //	
    , SDK_MAIN_MSG_UPGRAD             //	
	,SDK_MAIN_MSG_BUTT
}SDK_MAIN_MSG_ID_E;



#ifdef __cplusplus
	}
#endif

#endif 
