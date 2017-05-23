#include "XmppSender.h"
#include "XmppReceiver.h"
#include "XmppString.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void(*sendBroadcast)(int name_id, const char* data);

extern char user_name[STR_SIZE];

extern char user_jid[STR_SIZE];

extern char user_status[STR_SIZE];

extern xmpp_conn_t *connection;

extern xmpp_conn_event_t connection_status;

extern Friend_list *friend_list;

void on_get_register_result(int result){

	int send_id = ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID;

	if (result == XMPP_REGISTER_SUCCESSFUL){
		LOGD("注册成功!\n");
		send_id = ON_XMPP_REGISTER_SUCCESSFUL_ID;
	}
	else if (result == XMPP_REGISTER_NOT_SUCCESSFUL){
		LOGD("注册不成功!\n");
		send_id = ON_XMPP_REGISTER_NOT_SUCCESSFUL_ID;
	}
	else if (result == XMPP_REGISTER_ALREADY_EXIST){
		LOGD("用户已存在!\n");
		send_id = ON_XMPP_REGISTER_ALREADY_EXIST_ID;
	}
	else if (result == XMPP_REGISTER_NETWORK_CUT_DOWN){
		LOGD("网络断开!\n");
		send_id = ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID;
	}

	if (sendBroadcast != NULL)
		sendBroadcast(send_id, NULL);
}


int regDevInfoToServer(const char* jid, const char* pass, const char* strServerIP, int serverAltport){

	xmpp_register(jid, pass, strServerIP, serverAltport, on_get_register_result);
	return 0;
}


void xmpp_send_message_by_id(const char* friend_name, const char* id){

	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, friend_name);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_id(message, id);
	xmpp_stanza_set_attribute(message, "from", user_jid);
	xmpp_stanza_set_attribute(message, "to", friend_jid);

	xmpp_send(connection, message);
	xmpp_stanza_release(message);
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

status xmpp_request_game(const char* friend_name) {


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	//char friend_jid[STR_SIZE];
	//get_jid(new_friend, friend_jid);
	//getJidFromName(friend_jid, friend_name);

	xmpp_send_message_by_id(friend_name, ID_REQUEST_GAME);
	return OK;
}




status xmpp_accept_game(const char* friend_name) {


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}


	xmpp_send_message_by_id(friend_name, ID_ACCEPT_GAME);
	return OK;
}




status xmpp_send_game_message(const char* friend_name, const char* content) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	if (content == NULL){
		printf("please enter the content!\n");
		return ER;
	}

	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, friend_name);

	xmpp_stanza_t *text, *command;
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	command = xmpp_stanza_new(ctx);

	xmpp_stanza_set_name(command, "message");
	xmpp_stanza_set_attribute(command, "from", user_jid);
	xmpp_stanza_set_attribute(command, "to", friend_jid);
	xmpp_stanza_set_attribute(command, "xmlns", "jabber:client");

	xmpp_stanza_set_id(command, ID_SEND_GAME_MESSAGE);

	text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, content);
	xmpp_stanza_add_child(command, text);

	xmpp_send(connection, command);

	xmpp_stanza_release(text);
	xmpp_stanza_release(command);

	return OK;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int xmpp_create_groupchat(const char* groupchat_name) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}


	char groupchat_jid[STR_SIZE];
	getJidFromName(groupchat_jid, groupchat_name);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");

	xmpp_stanza_set_attribute(pres, "from", user_jid);
	xmpp_stanza_set_attribute(pres, "to", groupchat_jid);

	xmpp_stanza_t *x = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(x, "x");
	xmpp_stanza_set_attribute(x, "xmlns", "http://jabber.org/protocol/muc");

	xmpp_stanza_add_child(pres, x);

	xmpp_send(connection, pres);

	xmpp_stanza_release(x);
	xmpp_stanza_release(pres);


	/*xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");

	xmpp_stanza_set_id(iq, "create1");
	xmpp_stanza_set_attribute(iq, "from", user_jid);
	xmpp_stanza_set_attribute(iq, "to", groupchat_jid);
	xmpp_stanza_set_attribute(iq, "type", "set");

	xmpp_stanza_t *query= xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_attribute(query, "xmlns", "http://jabber.org/protocol/muc#owner");

	xmpp_stanza_t *x = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(x, "x");
	xmpp_stanza_set_attribute(x, "xmlns", "jabber:x:data");
	xmpp_stanza_set_attribute(x, "type", "submit");
	xmpp_stanza_add_child(query, x);

	xmpp_stanza_add_child(iq, query);

	xmpp_send(connection, iq);

	xmpp_stanza_release(x);
	xmpp_stanza_release(query);
	xmpp_stanza_release(iq);*/


	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


status xmpp_get_friends_list(){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "get");
	xmpp_stanza_set_id(iq, XMPP_GET_FRIEND_LIST_ID);

	xmpp_stanza_t *query = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_attribute(query, "xmlns", "jabber:iq:roster");

	xmpp_stanza_add_child(iq, query);

	xmpp_send(connection, iq);

	xmpp_stanza_release(query);
	xmpp_stanza_release(iq);
	return OK;
}



status xmpp_delete_friends(const char* del_friend_name) {
	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, del_friend_name);

	LOGD("delete:%s\n", friend_jid);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "set");
	xmpp_stanza_set_id(iq, "O34qx-33");

	xmpp_stanza_t *query = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_attribute(query, "xmlns", "jabber:iq:roster");

	xmpp_stanza_t *item = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(item, "item");
	xmpp_stanza_set_attribute(item, "jid", friend_jid);
	xmpp_stanza_set_attribute(item, "subscription", "remove");

	xmpp_stanza_t *group = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(group, "Friends");
	xmpp_stanza_add_child(item, group);

	xmpp_stanza_add_child(query, item);

	xmpp_stanza_add_child(iq, query);

	xmpp_send(connection, iq);


	xmpp_stanza_release(group);
	xmpp_stanza_release(item);
	xmpp_stanza_release(query);
	xmpp_stanza_release(iq);

	return OK;
}



//将对方加入通讯录
status xmpp_add_friends_to_roster(const char *new_friend_name){


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, new_friend_name);
	//get_jid(new_friend, friend_jid);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);

	//////////////////////////////////////////////////////////////////////////////
	//订阅联系人

	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "set");
	xmpp_stanza_set_id(iq, "roster_1");
	xmpp_stanza_t *query = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_attribute(query, "xmlns", "jabber:iq:roster");

	xmpp_stanza_t *item = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(item, "item");
	xmpp_stanza_set_attribute(item, "jid", friend_jid);
	//char friend_name[STR_SIZE];
	//getNameFromJid(friend_name, friend_jid);
	xmpp_stanza_set_attribute(item, "name", new_friend_name);

	xmpp_stanza_t *group = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(group, "group");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "Friends");

	xmpp_stanza_add_child(group, text);
	xmpp_stanza_add_child(item, group);
	xmpp_stanza_add_child(query, item);
	xmpp_stanza_add_child(iq, query);
	xmpp_send(connection, iq);


	xmpp_stanza_release(text);
	xmpp_stanza_release(group);
	xmpp_stanza_release(item);
	xmpp_stanza_release(query);
	xmpp_stanza_release(iq);

	return OK;

}

//订阅对方的出席信息
status xmpp_add_friends(const char *new_friend_name, const char *intent){


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	//将对方加入通讯录
	xmpp_add_friends_to_roster(new_friend_name);


	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, new_friend_name);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);

	//////////////////////////////////////////////////////////////////////////////
	//订阅出席信息

	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");
	xmpp_stanza_set_attribute(pres, "xmlns", "jabber:client");
	xmpp_stanza_set_type(pres, "subscribe");

	xmpp_stanza_set_attribute(pres, "from", user_jid);
	xmpp_stanza_set_attribute(pres, "to", friend_jid);

	xmpp_stanza_set_attribute(pres, "intent", intent);


	xmpp_send(connection, pres);
	xmpp_stanza_release(pres);

	return OK;
}

//是否同意添加好友
int xmpp_add_friends_respond(const char *new_friends, const char *respond){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, new_friends);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");
	xmpp_stanza_set_attribute(pres, "xmlns", "jabber:client");
	xmpp_stanza_set_type(pres, respond);
	xmpp_stanza_set_attribute(pres, "intent", XMPP_ADD_FRIEND_RESPOND);

	xmpp_stanza_set_attribute(pres, "from", user_jid);
	xmpp_stanza_set_attribute(pres, "to", friend_jid);


	xmpp_send(connection, pres);
	xmpp_stanza_release(pres);

	return 1;
}


int xmpp_declare_online(){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");
	xmpp_stanza_set_id(pres, "HOXUk-19");


	xmpp_stanza_t *status = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_t *text1 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text1, "空闲");
	xmpp_stanza_add_child(status, text1);
	xmpp_stanza_add_child(pres, status);


	xmpp_stanza_t *priority = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(priority, "priority");
	xmpp_stanza_t *text2 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text2, "1");
	xmpp_stanza_add_child(priority, text2);
	xmpp_stanza_add_child(pres, priority);

	xmpp_stanza_t *show = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(show, "show");
	xmpp_stanza_t *text3 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text3, "chat");
	xmpp_stanza_add_child(show, text3);
	xmpp_stanza_add_child(pres, show);

	xmpp_send(connection, pres);

	xmpp_stanza_release(text1);
	xmpp_stanza_release(text2);
	xmpp_stanza_release(text3);
	xmpp_stanza_release(show);
	xmpp_stanza_release(priority);
	xmpp_stanza_release(status);
	xmpp_stanza_release(pres);


	strcpy(user_status, "Online");

	return 1;

}


status xmpp_set_status(const char* new_status){
	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");


	xmpp_stanza_t *status = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, new_status);

	xmpp_stanza_add_child(status, text);
	xmpp_stanza_add_child(pres, status);

	xmpp_send(connection, pres);

	xmpp_stanza_release(text);
	xmpp_stanza_release(status);
	xmpp_stanza_release(pres);

	strcpy(user_status, new_status);

	return OK;
}



int xmpp_send_heartbeat(){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "get");
	xmpp_stanza_set_id(iq, "NSken-4");

	xmpp_stanza_t *ping = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(ping, "ping");
	xmpp_stanza_set_attribute(ping, "xmlns", "urn:xmpp:ping");

	xmpp_stanza_add_child(iq, ping);

	xmpp_send(connection, iq);

	xmpp_stanza_release(ping);
	xmpp_stanza_release(iq);

	return 1;
}






status xmpp_send_chat_message(const char* chat_object_name, const char* chat_body, const char* id) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}
	if (chat_body == NULL){
		printf("please enter the chat content!\n");
		return ER;
	}

	char chat_object_jid[STR_SIZE];
	getJidFromName(chat_object_jid, chat_object_name);

	xmpp_stanza_t *body, *command, *thread;
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	command = xmpp_stanza_new(ctx);

	xmpp_stanza_set_name(command, "message");
	xmpp_stanza_set_type(command, "chat");
	xmpp_stanza_set_attribute(command, "from", user_jid);
	xmpp_stanza_set_attribute(command, "to", chat_object_jid);
	xmpp_stanza_set_attribute(command, "xmlns", "jabber:client"); //


	xmpp_stanza_set_id(command, id);

	xmpp_id_handler_add(connection, get_send_chat_feedback_handler, id, NULL);//

	body = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(body, "body");

	xmpp_stanza_t *text1 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text1, chat_body);
	xmpp_stanza_add_child(body, text1);
	xmpp_stanza_add_child(command, body);

	thread = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(thread, "thread");
	xmpp_stanza_t *text2 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text2, "CC8rG0");
	xmpp_stanza_add_child(thread, text2);
	xmpp_stanza_add_child(command, thread);

	xmpp_send(connection, command);

	xmpp_stanza_release(text1);
	xmpp_stanza_release(text2);
	xmpp_stanza_release(thread);
	xmpp_stanza_release(body);
	xmpp_stanza_release(command);


	return OK;
}




status xmpp_send_picture(const char* friend_name, const char* file_name, const char* content, const char* id){


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}


	char friend_jid[STR_SIZE];
	getJidFromName(friend_jid, friend_name);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_type(message, "picture");
	xmpp_stanza_set_attribute(message, "from", user_jid);
	xmpp_stanza_set_attribute(message, "to", friend_jid);
	xmpp_stanza_set_id(message, id);

	xmpp_stanza_set_attribute(message, "file_name", file_name);

	xmpp_stanza_t *body = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(body, "body");

	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, content);
	xmpp_stanza_add_child(body, text);

	xmpp_stanza_add_child(message, body);

	xmpp_send(connection, message);
	xmpp_stanza_release(text);
	xmpp_stanza_release(body);
	xmpp_stanza_release(message);

	return OK;
}



int xmpp_send_chat_feedback_message(
	const char* const to,
	const char* const id) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_stanza_t *text, *command;
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	command = xmpp_stanza_new(ctx);

	xmpp_stanza_set_name(command, "message");
	xmpp_stanza_set_type(command, "check_chat");
	xmpp_stanza_set_attribute(command, "from", user_jid);
	xmpp_stanza_set_attribute(command, "to", to);
	xmpp_stanza_set_attribute(command, "xmlns", "jabber:client");

	xmpp_stanza_set_id(command, id);


	text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "feedback");
	xmpp_stanza_add_child(command, text);

	xmpp_send(connection, command);

	xmpp_stanza_release(text);
	xmpp_stanza_release(command);

	return 1;
}



status xmpp_request_friends_status(const char* friend_name){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}


	char chat_object_jid[STR_SIZE];
	getJidFromName(chat_object_jid, friend_name);

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_id(message, REQUEST_FRIENDS_STATUS_ID);
	xmpp_stanza_set_attribute(message, "from", user_jid);
	xmpp_stanza_set_attribute(message, "to", chat_object_jid);

	xmpp_send(connection, message);
	xmpp_stanza_release(message);

	return OK;
}


void request_friends_status(){

	int i;
	for (i = 0; i < friend_list->friend_sum; i++){
		LOGD("send_status:%s\n", friend_list->list[i]);
		xmpp_request_friends_status(friend_list->list[i]);
	}
}



void test_send_chat_message(char* friend_id, char* body){
	char random_id[N + 1] = { 0 };

	create_random_num(random_id);

	xmpp_send_chat_message(friend_id, body, random_id);
}



status xmpp_get_vCard(const char* str_user_name){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}


	
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "get");
	xmpp_stanza_set_attribute(iq, "from", user_jid);
	xmpp_stanza_set_id(iq, XMPP_GET_VCARD_ID);

	char user_jid[STR_SIZE];
	if (str_user_name == NULL)
		getJidFromName(user_jid, user_name);
	else
		getJidFromName(user_jid, str_user_name);
	xmpp_stanza_set_attribute(iq, "to", user_jid);
	//xmpp_stanza_set_id(iq, XMPP_GET_FRIEND_LIST_ID);

	xmpp_stanza_t *vCard = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(vCard, "vCard");
	xmpp_stanza_set_attribute(vCard, "xmlns", "vcard-temp");

	xmpp_stanza_add_child(iq, vCard);

	xmpp_send(connection, iq);

	xmpp_stanza_release(vCard);
	xmpp_stanza_release(iq);
	return OK;
}



status xmpp_set_vCard(const char* nick_name, const char* photo){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "set");
	xmpp_stanza_set_id(iq, XMPP_SET_VCARD_ID);
	//xmpp_stanza_set_attribute(iq, "from", user_jid);
	//xmpp_stanza_set_attribute(iq, "to", "xu456@tt.com");
	//xmpp_stanza_set_id(iq, XMPP_GET_FRIEND_LIST_ID);


	xmpp_stanza_t *vCard = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(vCard, "vCard");
	xmpp_stanza_set_attribute(vCard, "xmlns", "vcard-temp");


	xmpp_stanza_t *NICKNAME = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(NICKNAME, "NICKNAME");
	xmpp_stanza_t *text_NICKNAME = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text_NICKNAME, nick_name);
	xmpp_stanza_add_child(NICKNAME, text_NICKNAME);
	xmpp_stanza_add_child(vCard, NICKNAME);


	/*xmpp_stanza_t *EMAIL = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(EMAIL, "EMAIL");
	xmpp_stanza_t *text1 = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text1, "abcdeaddsada@dsd");
	xmpp_stanza_add_child(EMAIL, text1);
	xmpp_stanza_add_child(vCard, EMAIL);*/

	xmpp_stanza_t *PHOTO = NULL;
	xmpp_stanza_t *TYPE = NULL;
	xmpp_stanza_t *text_TYPE = NULL;
	xmpp_stanza_t *BINVAL = NULL;
	xmpp_stanza_t *text_BINVAL = NULL;

	if (photo != NULL){

		PHOTO = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(PHOTO, "PHOTO");

		TYPE = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(TYPE, "TYPE");
		text_TYPE = xmpp_stanza_new(ctx);
		xmpp_stanza_set_text(text_TYPE, "image/jpeg");
		xmpp_stanza_add_child(TYPE, text_TYPE);
		xmpp_stanza_add_child(PHOTO, TYPE);

		LOGD("base!\n");
		BINVAL = xmpp_stanza_new(ctx);
		xmpp_stanza_set_name(BINVAL, "BINVAL");
		text_BINVAL = xmpp_stanza_new(ctx);
		xmpp_stanza_set_text(text_BINVAL, photo);// "/home/123.jpg");// base0);
		xmpp_stanza_add_child(BINVAL, text_BINVAL);
		xmpp_stanza_add_child(PHOTO, BINVAL);

		xmpp_stanza_add_child(vCard, PHOTO);

	}

	//char *base0 = "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";
	//char *base = "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";
	

	/*xmpp_stanza_t *vCard = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(vCard, "vCard");
	xmpp_stanza_set_attribute(vCard, "xmlns", "vcard-temp");

	xmpp_stanza_t *stanza_N = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(stanza_N, "N");
	xmpp_stanza_add_child(vCard, stanza_N);

	xmpp_stanza_t *ORG = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(ORG, "ORG");
	xmpp_stanza_add_child(vCard, ORG);

	xmpp_stanza_t *FN = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(FN, "FN");
	xmpp_stanza_add_child(vCard, FN);

	xmpp_stanza_t *URL = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(URL, "URL");
	xmpp_stanza_add_child(vCard, URL);

	xmpp_stanza_t *TITLE = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(TITLE, "TITLE");
	xmpp_stanza_add_child(vCard, TITLE);

	xmpp_stanza_t *NICKNAME = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(NICKNAME, "NICKNAME");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "abcde");
	xmpp_stanza_add_child(NICKNAME, text);
	xmpp_stanza_add_child(vCard, NICKNAME);

	xmpp_stanza_t *EMAIL = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(EMAIL, "EMAIL");
	xmpp_stanza_add_child(vCard, EMAIL);

	xmpp_stanza_t *TEL = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(TEL, "TEL");
	xmpp_stanza_add_child(vCard, TEL);

	xmpp_stanza_t *ADR = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(ADR, "ADR");
	xmpp_stanza_add_child(vCard, ADR);*/

	xmpp_stanza_add_child(iq, vCard);
	xmpp_send(connection, iq);


	if (photo != NULL){
		xmpp_stanza_release(text_BINVAL);
		xmpp_stanza_release(BINVAL);
		xmpp_stanza_release(text_TYPE);
		xmpp_stanza_release(TYPE);
		xmpp_stanza_release(PHOTO);
	}
	xmpp_stanza_release(text_NICKNAME);
	xmpp_stanza_release(NICKNAME);
	xmpp_stanza_release(vCard);
	xmpp_stanza_release(iq);
	return OK;
}
