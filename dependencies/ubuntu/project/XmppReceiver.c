#include "XmppReceiver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strophe.h"
#include "XmppSender.h"
#include "XmppString.h"
#include "json-c/json.h"

extern char user_name[STR_SIZE];
extern char user_status[STR_SIZE];

extern void(*sendBroadcast)(int name_id, const char* data);

extern Friend_list *friend_list;



void on_receive_chat_message(const char* const from, const char* const content){

	LOGD("%s:%s\n", from, content);

	char friend_message[STR_SIZE];
	getNameFromJid(friend_message, from);
	//get_name(from, friend_message);

	strcat(friend_message, ":");
	str_cat(friend_message, content);

	//LOGD("friend_message:%s\n", friend_message);

	if (sendBroadcast != NULL)
		sendBroadcast(ON_RECEIVE_MESSAGE_ID, friend_message);
}



void on_receive_picture(const char* const from, const char *id, const char* const file_name, const char* const content){

	LOGD("%s:%s\n", from, content);


	if (sendBroadcast != NULL){

		char friend_name[STR_SIZE];
		getNameFromJid(friend_name, from);

		LOGD("%s:%d\n", __func__, __LINE__);

		LOGD("file_name:%s,%s:%d\n", file_name, __func__, __LINE__);

		LOGD("len:%d,%s:%d\n", strlen(content), __func__, __LINE__);

		struct json_object *json_data = json_object_new_object();
		json_object_object_add(json_data, "sender_name", json_object_new_string(getSafeString(friend_name)));
		json_object_object_add(json_data, "id", json_object_new_string(getSafeString(id)));
		json_object_object_add(json_data, "file_name", json_object_new_string(getSafeString(file_name)));


		LOGD("%s:%d\n", __func__, __LINE__);

		

		json_object_object_add(json_data, "content", json_object_new_string(getSafeString(content)));

		LOGD("%s:%d\n", __func__, __LINE__);

		char send_data[MAX_SIZE];
		const char *str = json_object_to_json_string(json_data);
		strcpy(send_data, str);

		LOGD("%s:%d\n", __func__, __LINE__);

		sendBroadcast(ON_RECEIVE_PICTURE_ID, send_data);// friend_message);

		json_object_put(json_data);
	}
}


status handle_friends_list(xmpp_stanza_t * const stanza){
	xmpp_stanza_t *query = xmpp_stanza_get_children(stanza);
	xmpp_stanza_t *item = xmpp_stanza_get_children(query);

	LOGD("%s:%d\n", __func__, __LINE__);

	clear_friend_list(friend_list);

	LOGD("%s:%d\n", __func__, __LINE__);
	char str_friend_list[STR_SIZE] = "";


	if (item == NULL){
		LOGD("no friends!\n");
		//return 1;
	}
	else{
		while (item != NULL){
			char friend_name[STR_SIZE];
			getNameFromJid(friend_name, xmpp_stanza_get_attribute(item, "jid"));
			add_friend(friend_list, friend_name);
			item = xmpp_stanza_get_next(item);
		}

		LOGD("%s:%d\n", __func__, __LINE__);

		request_friends_status();

		LOGD("好友列表:\n");

		getStringFromFriendList(str_friend_list, friend_list);

	}

	LOGD("%s:%d\n", __func__, __LINE__);


	if (sendBroadcast != NULL)
		sendBroadcast(ON_GET_FRIENDS_LIST_ID, str_friend_list);


	return OK;
}


status handle_vCard(xmpp_stanza_t * const stanza){

	char *from = xmpp_stanza_get_attribute(stanza, "from");

	char friend_name[STR_SIZE];
	if (from != NULL)
		getNameFromJid(friend_name, from);
	else
		strcpy(friend_name, user_name);

	if (str_equal(friend_name, user_name)){//strcmp(friend_name, user_name) == 0){		//自己
		LOGD("myself!\n");
	}

	xmpp_stanza_t *vCard = xmpp_stanza_get_children(stanza);

	xmpp_stanza_t *NICKNAME = NULL;

	if (vCard != NULL)
		NICKNAME = xmpp_stanza_get_child_by_name(vCard, "NICKNAME");

	//if (str_equal(friend_name, user_name) && (vCard == NULL || NICKNAME == NULL)){
	//	if (sendBroadcast != NULL)
	//		sendBroadcast(ON_NEED_SET_VCARD_ID, NULL);
	//	else{
	//		xmpp_set_vCard(user_name, NULL);
	//	}

	//	return OK;
	//}

	if (vCard == NULL || NICKNAME == NULL){
		if (str_equal(friend_name, user_name)){
			if (sendBroadcast != NULL)
				sendBroadcast(ON_NEED_SET_VCARD_ID, NULL);
			else{
				xmpp_set_vCard(user_name, NULL);
			}

		}

		return OK;
	}
	
	//else{


		
		

		char *nick_name = xmpp_stanza_get_text(NICKNAME);
		LOGD("nickname is %s!\n", nick_name);

		

		if (sendBroadcast != NULL){

			struct json_object *json_vcard = json_object_new_object();
			json_object_object_add(json_vcard, "user_name", json_object_new_string(getSafeString(friend_name)));
			json_object_object_add(json_vcard, "nick_name", json_object_new_string(getSafeString(nick_name)));
			//char send_data[STR_SIZE];
			//strcpy(send_data, nick_name);

			LOGD("%s:%d\n", __func__, __LINE__);

			xmpp_stanza_t *PHOTO = xmpp_stanza_get_child_by_name(vCard, "PHOTO");
			if (PHOTO != NULL){
				xmpp_stanza_t *BINVAL = xmpp_stanza_get_child_by_name(PHOTO, "BINVAL");
				char *photo_data = xmpp_stanza_get_text(BINVAL);
				json_object_object_add(json_vcard, "photo_data", json_object_new_string(getSafeString(photo_data)));
				free(photo_data);
			}
			LOGD("%s:%d\n", __func__, __LINE__);
			char send_data[MAX_SIZE];
			const char *str = json_object_to_json_string(json_vcard);
			strcpy(send_data, str);
			LOGD("%s:%d\n", __func__, __LINE__);

			sendBroadcast(ON_GET_VCARD_ID, send_data);
			json_object_put(json_vcard);

		}

		free(nick_name);


	//}

	return OK;
}


status handle_message(xmpp_stanza_t * const stanza){
	char *type = xmpp_stanza_get_attribute(stanza, "type");
	if (type != NULL && strcmp(type, "error") == 0)
		return ER;

	char *id = xmpp_stanza_get_id(stanza);

	if (id != NULL && !strcmp(id, REQUEST_FRIENDS_STATUS_ID)){
		char status_tmp[STR_SIZE];
		strcpy(status_tmp, user_status);
		xmpp_set_status(status_tmp);

		return OK;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	if (id != NULL && !strcmp(id, ID_REQUEST_GAME)){
		char *from = xmpp_stanza_get_attribute(stanza, "from");

		char friend_name[STR_SIZE];
		getNameFromJid(friend_name, from);

		LOGD("%s请求与您游戏", friend_name);

		if (sendBroadcast != NULL)
			sendBroadcast(ON_REQUEST_GAME_ID, friend_name);

		return OK;
	}


	if (id != NULL && !strcmp(id, ID_ACCEPT_GAME)){
		char *from = xmpp_stanza_get_attribute(stanza, "from");

		char friend_name[STR_SIZE];
		getNameFromJid(friend_name, from);

		LOGD("开始与%s游戏", friend_name);

		if (sendBroadcast != NULL)
			sendBroadcast(ON_ACCEPT_GAME_ID, friend_name);

		return OK;
	}

	//接收到游戏信息
	if (id != NULL && !strcmp(id, ID_SEND_GAME_MESSAGE)){
		char *message = xmpp_stanza_get_text(stanza);
		if (message != NULL){
			LOGD("%s\n", message);
			if (sendBroadcast != NULL)
				sendBroadcast(ON_RECEIVE_GAME_MESSAGE_ID, message);

			free(message);
		}

		return OK;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	if (type == NULL || !(strcmp(type, "chat") == 0 || strcmp(type, "picture") == 0))
		return OK;

	char *from = xmpp_stanza_get_attribute(stanza, "from");
	char *content = xmpp_stanza_get_text(xmpp_stanza_get_children(stanza));


	if (type != NULL && !strcmp(type, "chat"))
		on_receive_chat_message(from, content);
	else if (type != NULL && strcmp(type, "picture") == 0){
		char *file_name = xmpp_stanza_get_attribute(stanza, "file_name");
		on_receive_picture(from, id, file_name, content);
	}

	free(content);

	//char *id = xmpp_stanza_get_attribute(stanza, "id");
	if (id != NULL){
		xmpp_send_chat_feedback_message(
			xmpp_stanza_get_attribute(stanza, "from"),
			xmpp_stanza_get_id(stanza));
	}

	return OK;
}


status handle_iq(xmpp_stanza_t * const stanza){
	char *type = xmpp_stanza_get_attribute(stanza, "type");
	if (type != NULL && strcmp(type, "error") == 0)
		return ER;

	char *id = xmpp_stanza_get_id(stanza);

	LOGD("%s:%d\n", __func__, __LINE__);

	if (str_equal(id, XMPP_GET_FRIEND_LIST_ID)){//id != NULL && !strcmp(id, XMPP_GET_FRIEND_LIST_ID)){

		LOGD("%s:%d\n", __func__, __LINE__);

		handle_friends_list(stanza);
		

	}
	else if (str_equal(id, XMPP_GET_VCARD_ID)){

		handle_vCard(stanza);
		
	}
	else if (str_equal(id, XMPP_SET_VCARD_ID)){
		if (sendBroadcast != NULL)
			sendBroadcast(ON_SET_VCARD_SUCCESSFUL_ID, NULL);
	}

	return OK;
}



status handle_friends_status(xmpp_stanza_t * const stanza){

	char *type = xmpp_stanza_get_attribute(stanza, "type");

	//接收到好友状态
	char *from = xmpp_stanza_get_attribute(stanza, "from");

	LOGD("%s:%d\n", __func__, __LINE__);

	char friend_name[STR_SIZE];
	getNameFromJid(friend_name, from);
	if (str_equal(friend_name, user_name)){//strcmp(friend_name, user_name) == 0){		//自己
		return OK;
	}

	LOGD("%s:%d\n", __func__, __LINE__);


	char friend_status[STR_SIZE];
	strcpy(friend_status, "");

	if (str_equal(type, "unavailable")){
		//type != NULL && strcmp(type, "unavailable") == 0){

		LOGD("好友%s下线\n", friend_name);
		strcpy(friend_status, "离线");
	}
	else{

		xmpp_stanza_t *status = xmpp_stanza_get_children(stanza);
		if (status != NULL){
			char *text = xmpp_stanza_get_text(status);
			LOGD("获得好友状态%s:%s\n", friend_name, text);

			strcpy(friend_status, text);

			free(text);

		}
	}

	LOGD("%s:%d\n", __func__, __LINE__);

	if (strlen(friend_status) != 0){
		//char friend_jid[STR_SIZE];
		//getJidFromSring(friend_jid, from);
		//get_jid(from, friend_jid);

		strcat(friend_name, "|");
		strcat(friend_name, friend_status);

		//LOGD("friend_jid:%s", friend_jid);

		if (sendBroadcast != NULL)
			sendBroadcast(ON_GET_FRIENDS_STATUS_ID, friend_name);
	}

	LOGD("%s:%d\n", __func__, __LINE__);
	return OK;
}


status handle_presence(xmpp_stanza_t * const stanza){
	char *type = xmpp_stanza_get_attribute(stanza, "type");

	if (type != NULL && strcmp(type, "error") == 0)
		return ER;


	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if (str_equal(type, XMPP_ADD_FRIEND_REQUEST)){


		char *from = xmpp_stanza_get_attribute(stanza, "from");
		LOGD("add_friend_jid:%s\n", from);

		//char add_friend_jid[STR_SIZE];
		//getJidFromSring(add_friend_jid, from);
		//get_jid(from, add_friend_jid);


		char add_friend_name[STR_SIZE];
		getNameFromJid(add_friend_name, from);

		if (sendBroadcast != NULL){

			char *intent = xmpp_stanza_get_attribute(stanza, "intent");

			if (!str_equal(intent, XMPP_ADD_FRIEND_INTENT_AGREE)){
				sendBroadcast(ON_ADD_FRIEND_ID, add_friend_name);
			}
			else{
				xmpp_add_friends_respond(add_friend_name, XMPP_ADD_FRIEND_RESPOND_AGREE);
			}


		}
		else{

			//xmpp_add_friends_respond(add_friend_jid, XMPP_ADD_FRIEND_RESPOND_DISAGREE);


			xmpp_add_friends_respond(add_friend_name, XMPP_ADD_FRIEND_RESPOND_AGREE);

			char *intent = xmpp_stanza_get_attribute(stanza, "intent");


			if (!str_equal(intent, XMPP_ADD_FRIEND_INTENT_AGREE)){//intent != NULL && strcmp(intent, XMPP_ADD_FRIEND_INTENT_AGREE) != 0){

				xmpp_add_friends(add_friend_name, XMPP_ADD_FRIEND_INTENT_AGREE);
			}

		}

		return OK;
	}

	if (str_equal(type, XMPP_ADD_FRIEND_RESPOND_AGREE)){//type != NULL && !strcmp(type, XMPP_ADD_FRIEND_RESPOND_AGREE)){

		char *from = xmpp_stanza_get_attribute(stanza, "from");
		char add_friend_name[STR_SIZE];
		getNameFromJid(add_friend_name, from);
		LOGD("%s同意了\n", add_friend_name);

		if (sendBroadcast != NULL)
			sendBroadcast(ON_ADD_FRIEND_RESPOND_AGREE_ID, add_friend_name);

		return OK;
	}


	if (str_equal(type, XMPP_ADD_FRIEND_RESPOND_DISAGREE)){
		//type != NULL && !strcmp(type, XMPP_ADD_FRIEND_RESPOND_DISAGREE)){

		char *from = xmpp_stanza_get_attribute(stanza, "from");
		char add_friend_name[STR_SIZE];
		getNameFromJid(add_friend_name, from);

		char *intent = xmpp_stanza_get_attribute(stanza, "intent");


		if (str_equal(intent, XMPP_ADD_FRIEND_RESPOND)){
			//intent != NULL && !strcmp(intent, XMPP_ADD_FRIEND_RESPOND)){

			LOGD("%s不同意\n", add_friend_name);

			if (sendBroadcast != NULL)
				sendBroadcast(ON_ADD_FRIEND_RESPOND_DISAGREE_ID, add_friend_name);
		}




		//char add_friend_jid[STR_SIZE];
		//getJidFromSring(add_friend_jid, from);
		//get_jid(from, add_friend_jid);

		xmpp_delete_friends(add_friend_name);

		return OK;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	handle_friends_status(stanza);


	return OK;
}






int on_receive_message_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char *data;
	int size;
	xmpp_stanza_to_text(stanza, &data, &size);

	LOGD("%s\n", data);
	LOGD("%d\n", size);
	free(data);

	handle_message(stanza);
	return 1;
}

int on_receive_iq_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char *data;
	int size;
	xmpp_stanza_to_text(stanza, &data, &size);

	LOGD("%s\n", data);
	LOGD("%d\n", size);
	free(data);

	handle_iq(stanza);
	return 1;

}

int on_receive_presence_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char *data;
	int size;
	xmpp_stanza_to_text(stanza, &data, &size);

	LOGD("%s\n", data);
	LOGD("%d\n", size);
	free(data);


	handle_presence(stanza);

	return 1;
}


int get_send_chat_feedback_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char *type = xmpp_stanza_get_attribute(stanza, "type");
	if (type != NULL && strcmp(type, "error") == 0){
		LOGD("发送不成功，请重试!\n");
		return 0;
	}

	//char *name = xmpp_stanza_get_name(stanza);
	LOGD("%s 已收到!\n", xmpp_stanza_get_attribute(stanza, "from"));


	if (sendBroadcast != NULL)
		sendBroadcast(ON_RECEIVE_FEEDBACK_ID, xmpp_stanza_get_id(stanza));

	return 0;
}
