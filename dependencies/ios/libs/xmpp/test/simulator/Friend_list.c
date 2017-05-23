#include "Friend_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*#include<android/log.h>
#define TAG "log_jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
*/
void init_friend_list(Friend_list *friend_list){

	friend_list->friend_sum = 0;
	//friend_list->list = (char**)malloc(sizeof(char*));
}

void add_friend(Friend_list *friend_list, const char *friend_jid){
	//friend_list->list[friend_list->friend_sum] = (char*)malloc(strlen(friend_jid)*sizeof(char));
	strcpy(friend_list->list[friend_list->friend_sum], friend_jid);
	//friend_list->list[friend_list->friend_sum][strlen(friend_jid)] = '\0';

	friend_list->friend_sum++;
}

void clear_friend_list(Friend_list *friend_list){

	if (friend_list->friend_sum > 0){
		friend_list->friend_sum = 0;
	}
}
/*void clear_friend_list(Friend_list *friend_list){

	LOGD("123");
	if (friend_list->friend_sum > 0){
		LOGD("234");
		int i;
		for (i = 0; i < friend_list->friend_sum; i++){
			free(friend_list->list[i]);
		}
		friend_list->friend_sum = 0;
	}
}


void free_friend_list(Friend_list *friend_list){

	clear_friend_list(friend_list);
	if (friend_list->list != NULL){
		free(friend_list->list);
	}
}*/