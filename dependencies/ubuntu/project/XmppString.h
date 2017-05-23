#ifndef __XMPP_STRING_H__
#define __XMPP_STRING_H__

//#include "Jni_call.h"

#ifdef ANDROID
#include<android/log.h>
#define TAG "log_jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型

#else
#define LOGD(...) fprintf(stderr, __VA_ARGS__);// 定义LOGD类型
#endif


#define N 7//固定长度为7

typedef int BOOL;
#define true 1
#define false 0


typedef int status;
#define OK 1
#define ER 0


#define STR_SIZE 10000

#define MAX_SIZE 1000000


status getJidFromSring(char* jid, const char* str);

status getNameFromJid(char* name, const char* jid);

status getJidFromName(char* jid, const char* name);

status str_cat(char* des, const char* res);

BOOL str_equal(const char* des, const char* res);

status create_random_num(char* random);

const char* getSafeString(const char *str);

#endif
