#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#include "log.h"
#include "config.h"
#include "my_debug/tdevice/file.h"

int log_printf(unsigned int level, const char *fmt, ...)
{
    return 1;
    char buff[512]={0};
	va_list ap;
    //char *lvlstr[] = {"INF", "WRN", "ERR", "DBG"};
    const char *cfg;
    time_t timep;
    struct tm *p;

	va_start(ap, fmt);

    switch (level) {
    case INF:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[32m[INF@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        struct timeval    tv;
        struct timezone tz;
        //gettimeofday(&tv, &tz);
        gettimeofday(&tv, NULL);
        //printf("tv_sec:%ld tv_usec:%ld tv_msec:%ld\n",tv.tv_sec,tv.tv_usec,tv.tv_usec/1000);
        //printf("tz_minuteswest:%d\n",tz.tz_minuteswest);
        //printf("tz_dsttime:%d\n",tz.tz_dsttime);
        printf("ms===%d\n",p->tm_sec*1000 + tv.tv_usec/1000);
        break;
    case WRN:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[33m[WRN@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case ERR:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[31m[ERR@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        printf("\033[34m[DBG@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
                buff);
        break;
    case DBG_LV0:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvA");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[35m[DBG_LV0@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV1:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvB");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[35m[DBG_LV1@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV2:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvC");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[36m[DBG_LV2@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
               (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    case DBG_LV3:
        time(&timep);
        p =localtime(&timep);
        vsnprintf(buff, sizeof(buff), fmt, ap);
        cfg = config_read("debug_lvD");
        if ( cfg == NULL ) return 1;
        if ( strcmp(cfg, "false") == 0 ||
             strcmp(cfg, "FALSE") == 0 ) {
            return 1;
        }
        printf("\033[36m[DBG_LV3@%u %04d-%02d-%02d %02d:%02d:%02d]\033[0m %s\n",
              (unsigned int)pthread_self(),
               p->tm_year + 1900,
               p->tm_mon + 1,
               p->tm_mday,
               p->tm_hour,
               p->tm_min,
               p->tm_sec,
               buff);
        break;
    default:
        //return printf("%s\n", buff);
        break;
    }
    //TDDebug(buff);
    va_end(ap);
    return 0;
}
