#include "Friend_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XmppString.h"
#include "json-c/json.h"

void init_friend_list(Friend_list *friend_list){

	friend_list->friend_sum = 0;
	//friend_list->list = (char**)malloc(sizeof(char*));
}

void add_friend(Friend_list *friend_list, const char *friend_name){
	//friend_list->list[friend_list->friend_sum] = (char*)malloc(strlen(friend_jid)*sizeof(char));
	strcpy(friend_list->list[friend_list->friend_sum], friend_name);
	//friend_list->list[friend_list->friend_sum][strlen(friend_jid)] = '\0';

	friend_list->friend_sum++;
}

void clear_friend_list(Friend_list *friend_list){

	if (friend_list->friend_sum > 0){
		friend_list->friend_sum = 0;
	}
}

status getStringFromFriendList(char *str_friend_list, Friend_list *friend_list){
	if (str_friend_list == NULL){
		return ER;
	}


	/*struct json_object *my_object;
	my_object = json_object_new_object();

	json_object_object_add(my_object, "phoneNum", json_object_new_string("1233"));
	json_object_object_add(my_object, "pkgName", json_object_new_string("fesr"));

	char *str = json_object_to_json_string(my_object);
	printf("%s\n", str);


	struct json_object *my_object1 = json_tokener_parse(str);

	json_object * phoneNum = json_object_object_get(my_object1, "phoneNum");

	if (phoneNum == NULL)
	printf("phoneNum=NULL\n");
	char *str_phoneNum = json_object_get_string(phoneNum);
	printf("phoneNum:%s\n", str_phoneNum);


	json_object_put(my_object);
	json_object_put(my_object1);*/


	struct json_object *my_object = json_object_new_object();

	json_object_object_add(my_object, "friend_sum", json_object_new_int(friend_list->friend_sum));
	
	int i;
	for (i = 0; i < friend_list->friend_sum; i++){
		LOGD("%s\n", friend_list->list[i]);
		//strcat(str_friend_list, friend_list->list[i]);
		//strcat(str_friend_list, "|");
		char friend_name[STR_SIZE];
		sprintf(friend_name, "friend_%d", i + 1);
		json_object_object_add(my_object, friend_name, json_object_new_string(friend_list->list[i]));
	}


	const char *str = json_object_to_json_string(my_object);
	strcpy(str_friend_list, str);

	json_object_put(my_object);

	return OK;
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