#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#ifdef WIN32 // for windows
#include <winsock2.h>
#else // for linux
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#include "config.h"
#include "error.h"
#include "log.h"
#include "Hachiko.h"
#include "bms.h"

extern void * thread_hachiko_init(void *) ___THREAD_ENTRY___;
extern void * thread_bms_write_service(void *) ___THREAD_ENTRY___;
extern void * thread_bms_read_service(void *) ___THREAD_ENTRY___;

int thread_done[2] = {ENTER_THREAD};

int bms_canbus()
{
    const char *user_cfg = NULL;
    pthread_t tid = 0;
    pthread_attr_t attr;
    int s;
    //int thread_done[ 2 ] = {EXIT_THREAD};
    char buff[32];
    int errcode = 0, ret;

    thread_done[0] = ENTER_THREAD;
    thread_done[1] = ENTER_THREAD;
    thread_done[2] = ENTER_THREAD;

    printf(
            "+-+-+-+-+-+-+-+-+-+-+-+-+\n"
            "|S|e|r|i|a|l|S|y|s|t|e|m|\n"
            "+-+-+-+-+-+-+-+-+-+-+-+-+\n");

    printf(
            "                  _       _               _ \n"
            "                 (_)     | |             | | \n"
            "    ___  ___ _ __ _  __ _| |___ _   _ ___| |_ ___ _ __ ___ \n"
            "   / __|/ _ \\ '__| |/ _` | / __| | | / __| __/ _ \\ '_ ` _ \\ \n"
            "   \\__ \\  __/ |  | | (_| | \\__ \\ |_| \\__ \\ ||  __/ | | | | |\n"
            "   |___/\\___|_|  |_|\\__,_|_|___/\\__, |___/\\__\\___|_| |_| |_|\n"
            "                                 __/ |\n"
            "                                |___/ \n"
          );

    printf( "           \nCopyright © Andy zhao for SerialSystem\n");
    printf( "                            %s %s\n\n", __DATE__, __TIME__);
    printf("系统启动中.....\n\n\n\n");

    // 读取配置文件的顺序必须是
    // 1. 系统配置文件
    // 2. 用户参数配置文件
    // 需要注意的是，用户配置数据和用户配置数据可以有相同的配置项
    // 但优先级最高的是用户配置数据，如果某个配置项同时出现在系统配置
    // 和用户配置中，那么系统最终采用的值将是用户配置数据中的值
    // 因此这里需要注意的是：
    // * 有两个配置文件是一种冗余设计
    // * 非必要的情况下，分别将系统配置和用户配置分开存储到两个文件中
    config_initlize("ontom.cfg");
    user_cfg = config_read("user_config_file");
    if ( user_cfg == NULL ) {
        log_printf(WRN, "not gave user config file,"
                   "use 'user.cfg' by default.");
        user_cfg = "user.cfg";
    }
    config_initlize(user_cfg);

    s = pthread_attr_init(&attr);
    if ( 0 != s ) {
        log_printf(WRN, "could not set thread stack size, use default.");
    }

    if ( 0 == s ) {
        int stack_KB;
        const char *stack_size = config_read("thread_stack_size");
        if ( stack_size == NULL ) {
            log_printf(INF, "thread_stack_size not set, use 1024K");
            stack_size = "1024";
        }
        stack_KB = atoi(stack_size);
        if ( stack_KB <= 10 || stack_KB >= 20480 ) {
            log_printf(WRN, "stack size not statisfied(%d KB)."
                       "use default 1024 K instead.", stack_KB);
            stack_KB = 1024;
        }
        if ( 0 == pthread_attr_setstacksize(&attr, stack_KB * 1024) ) {
            log_printf(INF, "set all thread stack size to: %d KB", stack_KB);
        } else {
            log_printf(ERR, "set all thread stack_size to %d KB faile, "
                       "use system default size.", stack_KB);
        }
    }
    //pthread_attr_setdetachstat(&attr, PTHREAD_CREATE_DETACHED);

    // 启动定时器
   //Hachiko_init();
   ret = pthread_create( & tid, &attr, thread_hachiko_init,
                         &thread_done[0]);
   if ( 0 != ret ) {
       errcode  = 0x1001;
       log_printf(ERR,
                  "CAN-BUS reader start up.                       FAILE!!!!");
       goto die;
   }
   log_printf(INF, "CAN-BUS reader start up.                           DONE.");

#if 1
    // BMS 数据包写线程，从队列中取出要写的数据包并通过CAN总线发送出去
    ret = pthread_create( & tid, &attr, thread_bms_write_service,
                          &thread_done[1]);
    if ( 0 != ret ) {
        errcode  = 0x1001;
        log_printf(ERR,
                   "CAN-BUS reader start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS reader start up.                           DONE.");
//#else
    // BMS读书举报线程，从CAN总线读取数据包后将数据存入读入数据队列等待处理。
    ret = pthread_create( & tid, &attr, thread_bms_read_service,
                          &thread_done[2]);
    if ( 0 != ret ) {
        errcode  = 0x1002;
        log_printf(ERR,
                   "CAN-BUS writer start up.                       FAILE!!!!");
        goto die;
    }
    log_printf(INF, "CAN-BUS writer start up.                           DONE.");
#endif
    if ( s == 0 ) {
        pthread_attr_destroy(&attr);
    }

#if CONFIG_DEBUG_CONFIG >= 1
    config_print();
#endif
    // 主循环中放置看门狗代码
//    for ( ;; ) {
//        sleep(1);
//    }
//去掉这个循环
    return 0;
die:
    log_printf(ERR, "going to die. system aborted!");
    return errcode;
}

int bms_canstop()
{
    thread_done[0] = EXIT_THREAD;
    thread_done[1] = EXIT_THREAD;
    thread_done[2] = EXIT_THREAD;
    return 0;
}
