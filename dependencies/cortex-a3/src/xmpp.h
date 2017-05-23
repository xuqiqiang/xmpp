#ifndef XMPP_H_
#define XMPP_H_

#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "datatype.h"



typedef struct {

	unsigned char to[256];

	unsigned char jid[256];
	unsigned char name[512];


}friendslistbuffer,*pfriendslistbuffer;

typedef struct {

	unsigned char from[256];

	unsigned char to[256];
	unsigned char body[1024];


}chatbuffer,*pchatbuffer;

typedef struct {
	unsigned char friend_item[64];
    unsigned char friend_status[64];
}friendstatus,*pfriendstatus;


void show_log(const char* message, const char* func, int line);


//登陆服务器
//strServXmpp 服务器ip和端口
//strUserName jid
//strPassWord 密码
void connect_xmpp(char* strServXmpp,char* strUserName,char* strPassWord);



int stop_xmpp();

//处理来自服务器的数据
//xmpp xml数据
//xmpplen 数据长度
int  handlexmpp(char* xmpp,int xmpplen);

//获得好友列表
int get_friends_list();

//添加好友
int add_friends(char* newFriends);

int on_confirm_add_friends();

int send_on_confirm_add_friends();

int send_on_confirm_add_friends_third_time();

int declare_online();

//发送聊天数据
//chat_object 发送对象的jid
//body 聊天数据
int send_chat_message(char* chat_object,char* body);

//删除好友
int send_on_delete_friends();

//发送心跳
int send_heartbeat();

//接收到好友列表
//data 好友列表指针
//len 好友列表长度
int recv_friends_list_message(pfriendslistbuffer data,int len);

//接收到聊天数据
//chat_buffer 聊天数据
int recv_chat_message(pchatbuffer chat_buffer);

/*
 * status:
 * 1 means connect complete
 * 0 means connect not complete
 *
 */
int  inform_connect_status(BOOL status);


#endif
