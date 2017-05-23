/**********************************************
 * 版权所有：长虹技术中心(2014)
 * 文件名：xmpp.c
 * 版本号：V0.1
 * 作者：许启强
 * 创建日期：2014-10-15
 * 概述：TODO
 * 函数：
 * ****** : *********
 * 修改历史: 修改时间 作者 修改部分(哪个函数体等) 原因 
 * 1)       年月日   xxxx    xxxxxxxx          xxxx
 **********************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strophe.h>
#include "xmpp.h"
#include "strophe.h"
#include <pthread.h>
#include <unistd.h>

#define LOGD(...) fprintf(stderr, __VA_ARGS__);// 定义LOGD类型
//#include "Jni_call.h"




char user_jid[STR_SIZE] = { 0 };
char user_password[STR_SIZE] = { 0 };
char server_IP[STR_SIZE] = { 0 };
int server_altport;

char server_name[STR_SIZE] = { 0 };

char user_name[STR_SIZE] = { 0 };

char user_status[STR_SIZE] = "Online";


char request_friends_status_id[STR_SIZE] = "O15qx-15";	//请求获得好友状态



char ID_REQUEST_GAME[STR_SIZE] = "O17qx-17";	//请求游戏

char ID_ACCEPT_GAME[STR_SIZE] = "O18qx-18";	//接受游戏

char ID_SEND_GAME_MESSAGE[STR_SIZE] = "O20qx-20";	//发送游戏信息


Friend_list *friend_list;

char wait_add_friend[STR_SIZE] = { 0 };	//已经请求添加的好友jid


pthread_t gServTid;

int start_xmpp;


long rand_seed;


xmpp_conn_t *connection;

xmpp_conn_event_t connection_status;



void(*sendBroadcast)(int name_id, const char* data) = NULL;

/*void set_sendBroadcast(void *mySendBroadcast){
	sendBroadcast = mySendBroadcast;
}*/

int isConnect(){
	if (connection_status == XMPP_CONN_CONNECT)
		return true;
	else
		return false;
}



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

int get_jid(const char* from, char* jid)
{
	if (from == NULL){
		return 0;
	}
		

	int i;

	for (i = 0; from[i] != '\0'; i++){
		if (from[i] == '/')
			break;
	}

	strncpy(jid, from, i);
	jid[i] = '\0';

	return 1;
}


int get_name(const char* from, char* name)
{
	if (from == NULL){
		//jid = NULL;
		return 0;
	}


	int i;

	for (i = 0; from[i] != '\0'; i++){
		if (from[i] == '@')
			break;
	}

	strncpy(name, from, i);
	name[i] = '\0';

	return 1;
}


int str_cat(char* des, const char* res)
{
	if (strlen(res) > STR_SIZE - 1 - strlen(des))
		LOGD("Exceeds the maximum length!\n");
	strncat(des, res, STR_SIZE - 1 - strlen(des));
	return 1;
}


void xmpp_send_message_by_id(const char* friend_jid, const char* id){

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

int xmpp_request_game(const char* new_friend) {


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	char friend_jid[STR_SIZE];
	get_jid(new_friend, friend_jid);

	xmpp_send_message_by_id(friend_jid, ID_REQUEST_GAME);
	return 1;
}



int on_request_game_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	char *type = xmpp_stanza_get_attribute(stanza, "type");

	if (type != NULL && strcmp(type, "error") == 0)
		return 1;

	char *from = xmpp_stanza_get_attribute(stanza, "from");

	LOGD("%s请求与您游戏", from);


	if (sendBroadcast != NULL)
	sendBroadcast(ON_REQUEST_GAME_ID, from);
	
	return 1;

}


int xmpp_accept_game(const char* new_friend) {

	/*sem_wait(&thread_user_wait);

	pthread_mutex_lock(&thread_mutex);

	msg->type = ACCEPT_GAME;

	char friend_jid[STR_SIZE];
	get_jid(new_friend, friend_jid);

	strcpy(msg->to, friend_jid);

	pthread_mutex_unlock(&thread_mutex);

	sem_post(&thread_sem);
	*/


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	char friend_jid[STR_SIZE];
	get_jid(new_friend, friend_jid);

	xmpp_send_message_by_id(friend_jid, ID_ACCEPT_GAME);
	return 1;
}

int on_accept_game_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	char *type = xmpp_stanza_get_attribute(stanza, "type");

	if (type != NULL && strcmp(type, "error") == 0)
		return 1;

	char *from = xmpp_stanza_get_attribute(stanza, "from");

	LOGD("开始与%s游戏", from);

	if (sendBroadcast != NULL)
	sendBroadcast(ON_ACCEPT_GAME_ID, from);

	return 1;

}



int xmpp_send_game_message(const char* new_friend, const char* content) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	if (content == NULL){
		printf("please enter the content!\n");
		return 0;
	}

	char friend_jid[STR_SIZE];
	get_jid(new_friend, friend_jid);

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

	return 1;
}


//接收到游戏信息
int on_receive_game_message_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	

	char *message = xmpp_stanza_get_text(stanza);
	if (message != NULL){
		LOGD("%s\n", message);
		if (sendBroadcast != NULL)
		sendBroadcast(ON_RECEIVE_GAME_MESSAGE_ID, message);

		free(message);
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int xmpp_create_groupchat(const char* groupchat_jid) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}
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

int stop_xmpp() {

	start_xmpp = 0;

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_disconnect(connection);


	return 1;
}


/*int add_friends(const char* new_friends) {

	sem_wait(&thread_user_wait);

	pthread_mutex_lock(&thread_mutex);

	msg->type = ADD_FRIENDS;

	char friend_jid[STR_SIZE];
	get_jid(new_friends, friend_jid);

	strcpy(msg->to, friend_jid);
	//msg->to = new_friends;

	strcpy(wait_add_friend, friend_jid);	//设置请求添加的好友jid
	
	pthread_mutex_unlock(&thread_mutex);

	sem_post(&thread_sem);
	return 0;
}*/

int xmpp_add_friends(const char* new_friend){


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}


	char friend_jid[STR_SIZE];
	get_jid(new_friend, friend_jid);

	strcpy(wait_add_friend, friend_jid);	//设置请求添加的好友jid

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	//////////////////////////////////////////////////////////////////////////////

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
	char friend_name[STR_SIZE];
	get_name(friend_jid, friend_name);
	xmpp_stanza_set_attribute(item, "name", friend_name);

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
	//////////////////////////////////////////////////////////////////////////////

	
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	//xmpp_handler_add(conn,presence_handler, NULL, "presence", NULL, ctx);
	xmpp_stanza_set_name(pres, "presence");
	xmpp_stanza_set_attribute(pres, "xmlns", "jabber:client");
	xmpp_stanza_set_type(pres, "subscribe");

	xmpp_stanza_set_attribute(pres, "from", user_jid);
	xmpp_stanza_set_attribute(pres, "to", friend_jid);


	xmpp_send(connection, pres);
	xmpp_stanza_release(pres);

	return 1;
}


int xmpp_agree_add_friends(xmpp_conn_t* conn, const char* new_friends){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");
	xmpp_stanza_set_attribute(pres, "xmlns", "jabber:client");
	xmpp_stanza_set_type(pres, "subscribed");

	xmpp_stanza_set_attribute(pres, "from", user_jid);
	xmpp_stanza_set_attribute(pres, "to", new_friends);


	xmpp_send(conn, pres);
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


int xmpp_set_status(const char* new_status){
	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");


	xmpp_stanza_t *status = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, new_status);//"Online");

	xmpp_stanza_add_child(status, text);
	xmpp_stanza_add_child(pres, status);

	xmpp_send(connection, pres);

	xmpp_stanza_release(text);
	xmpp_stanza_release(status);
	xmpp_stanza_release(pres);

	strcpy(user_status, new_status);

	return 1;
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

int xmpp_get_friends_list(){

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *iq = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(iq, "iq");
	xmpp_stanza_set_type(iq, "get");
	xmpp_stanza_set_id(iq, "s1dWX-4");

	xmpp_stanza_t *query = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(query, "query");
	xmpp_stanza_set_attribute(query, "xmlns", "jabber:iq:roster");

	xmpp_stanza_add_child(iq, query);

	xmpp_send(connection, iq);

	xmpp_stanza_release(query);
	xmpp_stanza_release(iq);
	return 1;
}



int xmpp_delete_friends(const char* gDelFriend_id) {
	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

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
	xmpp_stanza_set_attribute(item, "jid", gDelFriend_id);
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

	return 1;
}



char* create_random_num(char* ch)
{
	int flag;
	int j;

	long seed = rand_seed;

	for (j = 0; j < N; j++)
	{
		ch[j] = 'a'+ seed % 10;
		seed /= 10;
	}

	rand_seed++;
	ch[j] = '\0';

	return ch;
}

int get_send_chat_feedback_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char *type = xmpp_stanza_get_attribute(stanza, "type");
	if (type != NULL && strcmp(type, "error") == 0){
		LOGD("发送不成功，请重试!\n");
		return 0;
	}

	char *name = xmpp_stanza_get_name(stanza);
	LOGD("%s 已收到!\n", xmpp_stanza_get_attribute(stanza, "from"));

	//LOGD("id:%s\n", xmpp_stanza_get_id(stanza));

	if (sendBroadcast != NULL)
	sendBroadcast(ON_RECEIVE_FEEDBACK_ID, xmpp_stanza_get_id(stanza));
	//LOGD("%s\n",xmpp_stanza_get_text(xmpp_stanza_get_children(stanza)));

	return 0;
}

int xmpp_send_chat_message(const char* chat_object, const char* chat_body, const char* id) {

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}
	if (chat_body == NULL){
		printf("please enter the chat content!\n");
		return 0;
	}

	xmpp_stanza_t *body, *command, *thread;
	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	command = xmpp_stanza_new(ctx);

	xmpp_stanza_set_name(command, "message");
	xmpp_stanza_set_type(command, "chat");
	xmpp_stanza_set_attribute(command, "from", user_jid);
	xmpp_stanza_set_attribute(command, "to", chat_object);
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


	return 1;
}




int xmpp_send_picture(const char* chat_object, const char* name, const char* content, const char* id){


	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return 0;
	}

	xmpp_ctx_t *ctx = xmpp_conn_get_context(connection);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_type(message, "picture");
	xmpp_stanza_set_attribute(message, "from", user_jid);
	xmpp_stanza_set_attribute(message, "to", chat_object);
	xmpp_stanza_set_id(message, id);

	xmpp_stanza_set_attribute(message, "path", name);
	//xmpp_stanza_set_attribute(message, "destin", "/");

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

}

/*void* thread_user(void* param){
	LOGD("thread_user run!\n");

	xmpp_conn_t* conn = (xmpp_conn_t*)param;

	while (thread_user_start){

		sem_post(&thread_user_wait);

		sem_wait(&thread_sem);



		pthread_mutex_lock(&thread_mutex);

		switch (msg->type){
		case SET_STATUS:
			xmpp_set_status(conn);
			break;
		case SEND_CHAT_MSG:
			xmpp_send_chat_message(conn, msg->from, msg->to, msg->msg, msg->id);
			break;
		case GET_FRIEND_LIST:
			xmpp_get_friends_list(conn, msg->from);
			break;
		case DELETE_FRIENDS:
			xmpp_delete_friends(conn, msg->msg);
			break;
		case SEND_HEARTBEAT:
			xmpp_send_heartbeat(conn);
			break;
		case ADD_FRIENDS:
			xmpp_add_friends(conn, msg->to);
			break;
		case DECLARE_ONLINE:
			xmpp_declare_online(conn);
			break;
		case STOP:
			

			free(msg);


			xmpp_disconnect(conn);
			LOGD("3");
			break;

		case REQUEST_GAME:
			xmpp_send_message_by_id(conn, ID_REQUEST_GAME);
			break;
		case ACCEPT_GAME:
			xmpp_send_message_by_id(conn, ID_ACCEPT_GAME);
			break;
		case SEND_GAME_MESSAGE:
			xmpp_send_game_message(conn);
			break;
		}

		pthread_mutex_unlock(&thread_mutex);

	}

	LOGD("thread_user end!\n");
}*/



void xmpp_send_chat_feedback_message(xmpp_conn_t* conn, const char* const from,
	const char* const to,
	const char* const id) {
	xmpp_stanza_t *text, *command;//, *thread;
	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	command = xmpp_stanza_new(ctx);

	xmpp_stanza_set_name(command, "message");
	xmpp_stanza_set_type(command, "check_chat");
	xmpp_stanza_set_attribute(command, "from", from);
	xmpp_stanza_set_attribute(command, "to", to);
	xmpp_stanza_set_attribute(command, "xmlns", "jabber:client"); //

	xmpp_stanza_set_id(command, id);


	//body = xmpp_stanza_new(ctx);
	//xmpp_stanza_set_name(body, "body");

	text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, "feedback");
	//xmpp_stanza_add_child(body, text);
	xmpp_stanza_add_child(command, text);

	xmpp_send(conn, command);

	xmpp_stanza_release(text);
	xmpp_stanza_release(command);
}


void on_receive_chat_message(const char* const from, const char* const content){
	
	LOGD("%s:%s\n", from, content);

	char friend_message[STR_SIZE];
	get_name(from, friend_message);

	strcat(friend_message, ":");
	str_cat(friend_message, content);

	//LOGD("friend_message:%s\n", friend_message);

	if (sendBroadcast != NULL)
		sendBroadcast(ON_RECEIVE_MESSAGE_ID, friend_message);
}



void on_receive_picture(const char* const from, const char* const name, const char* const content){

	LOGD("%s:%s\n", from, content);

	LOGD("%s:%d\n", __func__, __LINE__);
	/*char friend_message[MAX_SIZE];
	get_name(from, friend_message);

	strcat(friend_message, ":");
	str_cat(friend_message, content);*/

	//LOGD("friend_message:%s\n", friend_message);

	if (sendBroadcast != NULL)
		sendBroadcast(ON_RECEIVE_PICTURE_ID, content);// friend_message);
}

int on_receive_message_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	//char *name = xmpp_stanza_get_name(stanza);

	char *type = xmpp_stanza_get_attribute(stanza, "type");

	if (type == NULL || !(strcmp(type, "chat") == 0 || strcmp(type, "picture") == 0))
		return 1;

	char *from = xmpp_stanza_get_attribute(stanza, "from");
	char *content = xmpp_stanza_get_text(xmpp_stanza_get_children(stanza));
	

	if (strcmp(type, "chat") == 0)
		on_receive_chat_message(from, content);
	else if (strcmp(type, "picture") == 0){
		LOGD("%s:%d\n", __func__, __LINE__);
		char *name = xmpp_stanza_get_attribute(stanza, "name");
		LOGD("%s:%d\n", __func__, __LINE__);
		on_receive_picture(from, name, content);
	}
	
	free(content);

	xmpp_send_chat_feedback_message(conn,
		xmpp_stanza_get_attribute(stanza, "to"),
		xmpp_stanza_get_attribute(stanza, "from"),
		xmpp_stanza_get_id(stanza));
	return 1;
}



int on_add_friends_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	
	char *from = xmpp_stanza_get_attribute(stanza, "from");
	LOGD("add_friend_jid:%s\n", from);

	char add_friend_jid[STR_SIZE];
	get_jid(from, add_friend_jid);

	xmpp_agree_add_friends(conn, add_friend_jid);


	if (strcmp(add_friend_jid, wait_add_friend) != 0){
		
		xmpp_add_friends(add_friend_jid);
	}


	
	
	return 1;
}


/*void send_status_presence(xmpp_conn_t * const conn, const char* friend_jid){

	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	xmpp_stanza_t *pres = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(pres, "presence");
	//xmpp_stanza_set_id(pres, id);
	xmpp_stanza_set_attribute(pres, "from", msg->from);
	xmpp_stanza_set_attribute(pres, "to", friend_jid);

	//xmpp_stanza_set_ns(pres, "jabber:client");


	xmpp_stanza_t *status = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, user_status);

	xmpp_stanza_add_child(status, text);
	xmpp_stanza_add_child(pres, status);

	xmpp_send(conn, pres);

	xmpp_stanza_release(text);
	xmpp_stanza_release(status);
	xmpp_stanza_release(pres);
}

void send_status_message(xmpp_conn_t * const conn, const char* friend_jid, const char* id){

	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_id(message, id);
	xmpp_stanza_set_attribute(message, "from", msg->from);
	xmpp_stanza_set_attribute(message, "to", friend_jid);

	//LOGD("%s", friend_jid);

	xmpp_stanza_t *status = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(status, "status");
	xmpp_stanza_t *text = xmpp_stanza_new(ctx);
	xmpp_stanza_set_text(text, user_status);

	xmpp_stanza_add_child(status, text);
	xmpp_stanza_add_child(message, status);

	xmpp_send(conn, message);

	xmpp_stanza_release(text);
	xmpp_stanza_release(status);
	xmpp_stanza_release(message);
}*/

void xmpp_request_friends_status(xmpp_conn_t * const conn, const char* friend_jid){

	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	xmpp_stanza_t *message = xmpp_stanza_new(ctx);
	xmpp_stanza_set_name(message, "message");
	xmpp_stanza_set_id(message, request_friends_status_id);
	xmpp_stanza_set_attribute(message, "from", user_jid);
	xmpp_stanza_set_attribute(message, "to", friend_jid);

	xmpp_send(conn, message);
	xmpp_stanza_release(message);
}



void request_friends_status(xmpp_conn_t * const conn){

	int i;
	for (i = 0; i < friend_list->friend_sum; i++){
		LOGD("send_status:%s\n", friend_list->list[i]);
		//char friend_jid[50];
		//strcpy(friend_jid, friend_list->list[i]);
		//send_status_presence(conn, friend_list->list[i]);//xu456@tt.com
		//send_status_message(conn, friend_list->list[i], send_status_id);
		xmpp_request_friends_status(conn, friend_list->list[i]);
	}
}


int on_get_friends_list_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	xmpp_stanza_t *query = xmpp_stanza_get_children(stanza);
	xmpp_stanza_t *item = xmpp_stanza_get_children(query);

	LOGD("%s:%d\n", __func__, __LINE__);

	clear_friend_list(friend_list);

	LOGD("%s:%d\n", __func__, __LINE__);
	char str_friend_list[100];
	strcpy(str_friend_list, "");

	if (item == NULL){
		LOGD("no friends!\n");
		//return 1;
	}
	else{
		while (item != NULL){
			add_friend(friend_list, xmpp_stanza_get_attribute(item, "jid"));
			item = xmpp_stanza_get_next(item);
		}

		LOGD("%s:%d\n", __func__, __LINE__);

		request_friends_status(conn);

		LOGD("好友列表:\n");
		int i;
		for (i = 0; i < friend_list->friend_sum; i++){
			LOGD("%s\n", friend_list->list[i]);
			strcat(str_friend_list, friend_list->list[i]);
			strcat(str_friend_list, "|");
		}
		
	}

	LOGD("%s:%d\n", __func__, __LINE__);


	if (sendBroadcast != NULL)
	sendBroadcast(ON_GET_FRIENDS_LIST_ID, str_friend_list);
	return 1;

}


int on_xmpp_request_friends_status_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){

	char status_tmp[STR_SIZE];
	strcpy(status_tmp, user_status);

	xmpp_set_status(status_tmp);

	return 1;
}

//接收到好友状态
int on_get_friend_presence_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata){
	char *type = xmpp_stanza_get_attribute(stanza, "type");

	if (type != NULL && strcmp(type, "error") == 0)
		return 1;

	char *from = xmpp_stanza_get_attribute(stanza, "from");

	char friend_name[STR_SIZE];
	get_name(from, friend_name);
	if (strcmp(friend_name, user_name) == 0){		//自己
		return 1;
	}


	

	char friend_status[STR_SIZE];
	strcpy(friend_status, "");

	if (type != NULL && strcmp(type, "unavailable") == 0){

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

	if (strlen(friend_status) != 0){
		char friend_jid[STR_SIZE];
		get_jid(from, friend_jid);

		strcat(friend_jid, "|");
		strcat(friend_jid, friend_status);

		//LOGD("friend_jid:%s", friend_jid);

		if (sendBroadcast != NULL)
		sendBroadcast(ON_GET_FRIENDS_STATUS_ID, friend_jid);
	}

	return 1;
}




/*define a handler for connection events*/
void conn_handler(xmpp_conn_t* const conn, const xmpp_conn_event_t status,
	const int error, xmpp_stream_error_t* const stream_error,
	void* const userdata) {
	xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
	//struct msgdata* msg = (struct msgdata*) userdata;

	connection = conn;
	connection_status = status;

	////////////////////////////////////////////////////////////

	if (status == XMPP_CONN_CONNECT) {

		xmpp_stanza_t* pres;

		LOGD("DEBUG:connected\n");

		//xmpp_handler_add(conn, on_receive_message_handler, NULL, "message", "chat", NULL);///
		xmpp_handler_add(conn, on_receive_message_handler, NULL, "message", NULL, NULL);

		xmpp_id_handler_add(conn, on_get_friends_list_handler, "s1dWX-4", NULL);

		xmpp_id_handler_add(conn, on_xmpp_request_friends_status_handler, request_friends_status_id, NULL);//


		xmpp_handler_add(conn, on_add_friends_handler, NULL, "presence", "subscribe", NULL);

		xmpp_handler_add(conn, on_get_friend_presence_handler, NULL, "presence", NULL, NULL);//


		xmpp_id_handler_add(conn, on_request_game_handler, ID_REQUEST_GAME, NULL);
		xmpp_id_handler_add(conn, on_accept_game_handler, ID_ACCEPT_GAME, NULL);

		xmpp_id_handler_add(conn, on_receive_game_message_handler, ID_SEND_GAME_MESSAGE, NULL);



		//xmpp_declare_online(conn);
		if (sendBroadcast != NULL)
		sendBroadcast(ON_LOGIN_ID, "");

		char status_tmp[STR_SIZE];
		strcpy(status_tmp, user_status);

		xmpp_set_status(status_tmp);

		//get_friends_list();

		//xmpp_disconnect(conn);
	}
	else {
		LOGD("DEBUG:disconnect\n");

		//msg->type = DO_NONE;
		//thread_user_start = 0;
		//sem_post(&thread_sem);

		xmpp_handler_delete(conn, on_receive_message_handler);
		xmpp_id_handler_delete(conn, on_get_friends_list_handler, "s1dWX-4");
		xmpp_id_handler_delete(conn, on_xmpp_request_friends_status_handler, request_friends_status_id);

		xmpp_handler_delete(conn, on_add_friends_handler);
		xmpp_handler_delete(conn, on_get_friend_presence_handler);


		xmpp_id_handler_delete(conn, on_request_game_handler, ID_REQUEST_GAME);
		xmpp_id_handler_delete(conn, on_accept_game_handler, ID_ACCEPT_GAME);
		xmpp_id_handler_delete(conn, on_receive_game_message_handler, ID_SEND_GAME_MESSAGE);

		xmpp_stop(ctx);
	}
}


void* server_thread(void* param){
	while (start_xmpp){
		LOGD("start_xmpp!\n");
		xmpp_ctx_t *ctx;
		xmpp_conn_t *conn;
		xmpp_log_t *log;


		LOGD("xmpp_initialize\n");



		/*initalize library*/
		xmpp_initialize();

		/*create a context*/
		log = xmpp_get_default_logger(XMPP_LEVEL_DEBUG);
		ctx = xmpp_ctx_new(NULL, log);

		/* create a connection*/
		conn = xmpp_conn_new(ctx);

		LOGD("gUserName:%s", user_jid);
		LOGD("user_password:%s", user_password);
		/* setup authentication information*/
		xmpp_conn_set_jid(conn, user_jid);
		xmpp_conn_set_pass(conn, user_password);

		LOGD("xmpp_connect_client\n");
		/* initialize connection*/
		char *strServerIP;
		if (strlen(server_IP) == 0){//strcmp(strServXmpp, "NULL") == 0){//strlen(c_ServXmpp) == 0)
			LOGD("strServXmpp:NULL");
			strServerIP = NULL;
		}
		else
			strServerIP = server_IP;

		//xmpp_connect_client(conn, strServXmpp, 0, conn_handler, msg);
		xmpp_connect_client(conn, strServerIP, server_altport, conn_handler, NULL);

		/* enter the event loop - our connect handler will trigger an exit*/
		xmpp_run(ctx);
		/*release our connection and contex*/
		xmpp_conn_release(conn);
		xmpp_ctx_free(ctx);

		//xmpp_log_free(log);

		/*final shutdown of the libraty*/
		xmpp_shutdown();

		sleep(1);
	}
}


void xmpp_init(){
	connection = NULL;
	connection_status = XMPP_CONN_DISCONNECT;

	

	friend_list = (Friend_list*)malloc(sizeof(Friend_list));
	init_friend_list(friend_list);

	rand_seed = 0;
}

void xmpp_release(){
	if (friend_list!=NULL)
		free(friend_list);
	friend_list = NULL;
}

int connect_xmpp(const char* strUserName, const char* strPassWord, const char* strServerIP, int serverAltport){

	get_jid(strUserName, user_jid);
	//strcpy(user_jid, strUserName);
	strcpy(user_password, strPassWord);
	strcpy(server_IP, strServerIP);
	server_altport = serverAltport;

	//server_name

	get_name(user_jid, user_name);	//获得用户名

	start_xmpp = 1;


	pthread_attr_t a; //线程属性
	pthread_attr_init(&a);  //初始化线程属性
	pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);      //设置线程属性
	

	if (pthread_create(&gServTid, &a, (void *)server_thread, NULL) != 0){
		LOGD("err: xmpp_thread_start error! :%s:%d\n", __func__, __LINE__);
	}
	else{
		LOGD("info:xmpp_thread_start success\n");
	}

	return 0;
}


void test_send_chat_message(char* friend_id, char* body){
	char ch[N + 1] = { 0 };

	const char *random_id = create_random_num(ch);

	xmpp_send_chat_message(friend_id, body, random_id);
}


/*int main(void) {


	xmpp_init();

	//regDevInfoToServer("xu1230@tt.com", "123", "202.98.157.54", 5223);

	connect_xmpp("xu123@tt.com", "123", "202.98.157.54", 5223);
	//connect_xmpp("xu2@shinelon-pc", "123", "192.168.1.12", 5222);


	char input[50] = { 0 };


	while (gets(input)){//strlen(gets(input)) != 0) {
		if (strcmp(input, "0") == 0) {	//输入0：修改当前状态
			fprintf(stderr, "输入状态:");
			gets(input);
			xmpp_set_status(input);
		}
		else if (strcmp(input, "1") == 0) {	//输入1：向好友发送聊天数据
			fprintf(stderr, "输入聊天数据:");
			gets(input);
			test_send_chat_message("xu456@tt.com", input);
			//test_send_chat_message("xu1@shinelon-pc", input);
			//fprintf(stderr, "send:%s\n", input);
			
			//while(1){
			//	test_send_chat_message("xu456@tt.com", input);
			//}
		}
		else if (strcmp(input, "2") == 0) {	//输入2：获取好友列表
			xmpp_get_friends_list();
		}
		else if (strcmp(input, "3") == 0) {	//输入3：添加好友
			fprintf(stderr, "好友jid:");
			gets(input);
			xmpp_add_friends(input);
		}
		else if (strcmp(input, "4") == 0) {	//输入4：删除好友
			fprintf(stderr, "好友jid:");
			gets(input);
			xmpp_delete_friends(input);
		}
		else if (strcmp(input, "5") == 0) {	//输入5：发送心跳
			xmpp_send_heartbeat();
		}
		else if (strcmp(input, "6") == 0) {	//输入6：declare_online
			xmpp_declare_online();
		}
		else if (strcmp(input, "7") == 0) {	//输入7：断开连接
			stop_xmpp();
			break;
		}
		else if (strcmp(input, "8") == 0) {	//输入8：
			xmpp_create_groupchat("mygroupchat@conference.shinelon-pc");
		}
		else{
			fprintf(stderr, "输入0：修改当前状态\n"
				"输入1：向好友发送聊天数据\n"
				"输入2：获取好友列表\n"
				"输入3：添加好友\n"
				"输入4：删除好友\n"
				"输入5：发送心跳\n"
				"输入6：declare_online\n"
				"输入7：断开连接\n");


		}


	}

	xmpp_release();

	LOGD("释放内存中...\n");

	sleep(2);

	return 0;

}*/





