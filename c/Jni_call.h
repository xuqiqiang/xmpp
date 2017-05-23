#ifndef __JNI_CALL_H__
#define __JNI_CALL_H__

#ifndef ANDROID
#define ANDROID
#endif

#include <jni.h>

#define XMPP_JAVA_PATH "com/xuqiqiang/xmpp/Xmpp"

void initEnv(JNIEnv* env);


#endif