LOCAL_PATH := $(call my-dir)



include $(CLEAR_VARS)

LOCAL_MODULE := expat
LOCAL_SRC_FILES := libexpat.a

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)




include $(CLEAR_VARS)

LOCAL_MODULE := json
LOCAL_SRC_FILES := libjson-c.a

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_MODULE := strophe
LOCAL_SRC_FILES := libstrophe.a

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES += expat

include $(PREBUILT_STATIC_LIBRARY)



## to call the static libs
include $(CLEAR_VARS)

LOCAL_LDLIBS := -llog -ldl

LOCAL_MODULE:= xmpp
LOCAL_SRC_FILES :=  Friend_list.c Xmpp.c Jni_call.c XmppString.c XmppReceiver.c XmppSender.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_STATIC_LIBRARIES += strophe json

include $(BUILD_SHARED_LIBRARY)


