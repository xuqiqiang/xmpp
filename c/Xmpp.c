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
#include "Xmpp.h"
#include "strophe.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "XmppReceiver.h"
#include "XmppSender.h"





char user_name[STR_SIZE] = { 0 };
char user_jid[STR_SIZE] = { 0 };
char user_password[STR_SIZE] = { 0 };
char server_name[STR_SIZE] = { 0 };
char server_IP[STR_SIZE] = { 0 };
int server_altport;


char user_status[STR_SIZE] = XMPP_STATUS_ONLINE;

Friend_list *friend_list;

pthread_t gServTid;

int start_xmpp;

xmpp_conn_t *connection;

xmpp_conn_event_t connection_status;

void(*sendBroadcast)(int name_id, const char* data) = NULL;

int isConnect(){
	if (connection_status == XMPP_CONN_CONNECT)
		return true;
	else
		return false;
}


status stop_xmpp() {

	start_xmpp = 0;

	if (connection_status != XMPP_CONN_CONNECT){
		LOGD("network disconnect!\n");
		return ER;
	}

	xmpp_disconnect(connection);


	return OK;
}

/*define a handler for connection events*/
void conn_handler(xmpp_conn_t* const conn, const xmpp_conn_event_t status,
	const int error, xmpp_stream_error_t* const stream_error,
	void* const userdata) {
	
	if (error != 0)
	LOGD("error:%d\n", error);
	LOGD("ECONNRESET:%d\n", ECONNRESET);
	if (stream_error != NULL)
	LOGD("stream_error:%s\n", stream_error->text);


	connection = conn;
	connection_status = status;

	////////////////////////////////////////////////////////////

	if (status == XMPP_CONN_CONNECT) {


		LOGD("DEBUG:connected\n");

		xmpp_handler_add(conn, on_receive_message_handler, NULL, "message", NULL, NULL);

		xmpp_handler_add(conn, on_receive_iq_handler, NULL, "iq", NULL, NULL);

		xmpp_handler_add(conn, on_receive_presence_handler, NULL, "presence", NULL, NULL);

		//xmpp_declare_online(conn);
		if (sendBroadcast != NULL)
		sendBroadcast(ON_LOGIN_ID, "");

		char status_tmp[STR_SIZE];
		strcpy(status_tmp, user_status);

		xmpp_set_status(status_tmp);

		//xmpp_get_vCard(NULL);

		//get_friends_list();

		//xmpp_disconnect(conn);
	}
	else {
		LOGD("DEBUG:disconnect\n");

		xmpp_handler_delete(conn, on_receive_message_handler);
		xmpp_handler_delete(conn, on_receive_iq_handler);
		xmpp_handler_delete(conn, on_receive_presence_handler);

		xmpp_ctx_t *ctx = xmpp_conn_get_context(conn);
		xmpp_stop(ctx);

		if (error != 0)
			LOGD("error:%d\n", error);
		LOGD("ECONNRESET:%d\n", ECONNRESET);
		LOGD("ENXIO:%d\n", ENXIO);
		LOGD("ESHUTDOWN:%d\n", ESHUTDOWN);
		LOGD("ETIMEDOUT:%d\n", ETIMEDOUT);
		if (stream_error != NULL)
			LOGD("stream_error:%s\n", stream_error->text);

		if (error == ENETUNREACH){
			start_xmpp = 0;
			LOGD("网络断开!\n");

			if (sendBroadcast != NULL)
				sendBroadcast(ON_NETWORK_DISCONNECT_ID, "");
		}

		if (error == ENXIO){
			start_xmpp = 0;
			LOGD("用户或密码不正确!\n");

			if (sendBroadcast != NULL)
				sendBroadcast(ON_AUTH_ERROR_ID, "");
		}

		//110
		if (error == ESHUTDOWN){
			start_xmpp = 0;
			LOGD("Can't send after socket shutdown!\n");

			if (sendBroadcast != NULL)
				sendBroadcast(ON_NETWORK_DISCONNECT_ID, "");
		}

		if (error == ETIMEDOUT){
			start_xmpp = 0;
			LOGD("Can't send after socket shutdown!\n");

			if (sendBroadcast != NULL)
				sendBroadcast(ON_NETWORK_DISCONNECT_ID, "");
		}
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
			LOGD("strServXmpp:NULL\n");
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

		//Connection reset by peer
		if (get_conn_error(conn) == ECONNRESET){
			LOGD("Connection reset by peer\n");
			sleep(8);
		}
		else
			sleep(1);
	}

	return (void*)0;
}


void xmpp_init(){
	connection = NULL;
	connection_status = XMPP_CONN_DISCONNECT;

	friend_list = (Friend_list*)malloc(sizeof(Friend_list));
	init_friend_list(friend_list);

}

void xmpp_release(){
	if (friend_list!=NULL)
		free(friend_list);
	friend_list = NULL;
}

status connect_xmpp(const char* strUserName, const char* strPassWord, const char* strServerName, const char* strServerIP, int serverAltport){

	strcpy(user_name, strUserName);
	strcpy(user_password, strPassWord);
	strcpy(server_name, strServerName);
	strcpy(server_IP, strServerIP);
	server_altport = serverAltport;

	//获得jid
	strcpy(user_jid, user_name);
	strcat(user_jid, "@");
	strcat(user_jid, server_name);

	start_xmpp = 1;


	pthread_attr_t a; //线程属性
	pthread_attr_init(&a);  //初始化线程属性
	pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);      //设置线程属性
	

	if (pthread_create(&gServTid, &a, (void *)server_thread, NULL) != 0){
		LOGD("err: xmpp_thread_start error! :%s:%d\n", __func__, __LINE__);
		return ER;
	}
	else{
		LOGD("info:xmpp_thread_start success\n");
	}

	return OK;
}










