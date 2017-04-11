#ifndef		TDM_SI_INCLUDED_FILE_H
#define		TDM_SI_INCLUDED_FILE_H

#include "tdevice.h"

#include <stdio.h>
#ifdef WIN32 // for windows
#include <winsock2.h>
#else // for linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <sys/types.h>
#include <pthread.h>

//#pragma comment(lib,"ws2_32.lib")

#ifdef __cplusplus
extern "C" {
#endif



#define  CRITICAL_SECTION pthread_mutex_t
//TDVOID InitializeCriticalSection(CRITICAL_SECTION* gSection);
//TDBOOL WHXEnterCriticalSection(CRITICAL_SECTION * gSection);
//TDBOOL WHXLeaveCriticalSection(CRITICAL_SECTION* gSection);
//#define WHXEnterCriticalSection EnterCriticalSection
//#define WHXLeaveCriticalSection LeaveCriticalSection


TDVOID	TDDebugFunction(char * file,int  line,char * message);


#define HELLO  3
void TDDebugW();

//#define TDDebug(msg) TDDebugFunction(__FILE__,__LINE__,(msg))
//#define TDDebug(msg)
/*
 *     TDDebug Plus (can input arguments now)
 *     TonyKong 2012.10.15
*/
#define TDDebug_MAXSIZE 512
char gTDMsg[TDDebug_MAXSIZE];

#define TDDebug(msg, args...)  \
{\
memset(gTDMsg,0,TDDebug_MAXSIZE); \
sprintf(gTDMsg,msg,##args); \
TDDebugFunction(__FILE__,__LINE__,(gTDMsg)); \
}
//end..

#endif		/*TDM_SI_INCLUDED_FILE_H*/


#ifdef __cplusplus
}
#endif
