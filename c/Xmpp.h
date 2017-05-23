#ifndef __XMPP_H__
#define __XMPP_H__

#include "XmppString.h"

#include "Friend_list.h"








#define ON_LOGIN_ID 0
#define ON_GET_FRIENDS_LIST_ID 1
#define ON_RECEIVE_MESSAGE_ID 2
#define ON_RECEIVE_FEEDBACK_ID 3
#define ON_GET_FRIENDS_STATUS_ID 4

#define ON_REQUEST_GAME_ID 5
#define ON_ACCEPT_GAME_ID 6
#define ON_RECEIVE_GAME_MESSAGE_ID 7

#define ON_XMPP_REGISTER_SUCCESSFUL_ID 8
#define ON_XMPP_REGISTER_NOT_SUCCESSFUL_ID 9
#define ON_XMPP_REGISTER_ALREADY_EXIST_ID 10
#define ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID 11

#define ON_RECEIVE_PICTURE_ID 12

#define ON_ADD_FRIEND_ID 13
#define ON_ADD_FRIEND_RESPOND_AGREE_ID 14
#define ON_ADD_FRIEND_RESPOND_DISAGREE_ID 15


#define ON_NEED_SET_VCARD_ID 16
#define ON_GET_VCARD_ID 17
#define ON_SET_VCARD_SUCCESSFUL_ID 18


#define ON_AUTH_ERROR_ID 19
#define ON_NETWORK_DISCONNECT_ID 20





#define XMPP_STATUS_ONLINE "在线"

//请求订阅你的出席信息
#define XMPP_ADD_FRIEND_REQUEST "subscribe"

//是否同意订阅你的出席信息
#define XMPP_ADD_FRIEND_RESPOND "XMPP_ADD_FRIEND_RESPOND"
#define XMPP_ADD_FRIEND_RESPOND_AGREE "subscribed"
#define XMPP_ADD_FRIEND_RESPOND_DISAGREE "unsubscribed"

//添加好友的意图
#define XMPP_ADD_FRIEND_INTENT_REQUEST "request"
#define XMPP_ADD_FRIEND_INTENT_AGREE "agree"


#define XMPP_GET_FRIEND_LIST_ID "s1dWX-4"

#define XMPP_GET_VCARD_ID "s1dWX-6"

#define XMPP_SET_VCARD_ID "s1dWX-7"

#define REQUEST_FRIENDS_STATUS_ID "O15qx-15"	//请求获得好友状态


#define ID_REQUEST_GAME "O17qx-17"	//请求游戏

#define ID_ACCEPT_GAME "O18qx-18"	//接受游戏

#define ID_SEND_GAME_MESSAGE "O20qx-20"	//发送游戏信息


int isConnect();



void xmpp_init();

status connect_xmpp(const char* strUserName, const char* strPassWord, const char* strServerName, const char* strServerIP, int serverAltport);

status xmpp_send_chat_message(const char* chat_object_name, const char* body, const char* id);

status xmpp_set_status(const char* status);

status xmpp_get_friends_list();

//将对方加入通讯录
status xmpp_add_friends_to_roster(const char *new_friend_name);

//订阅对方的出席信息
status xmpp_add_friends(const char* new_friend_name, const char *intent);

status xmpp_delete_friends(const char* del_friend_name);

status stop_xmpp();

status xmpp_request_game(const char* friend_name);

status xmpp_accept_game(const char* friend_name);

status xmpp_send_game_message(const char* friend_name, const char* content);


status xmpp_send_picture(const char* friend_name, const char* name, const char* content, const char* id);

void xmpp_release();

#endif