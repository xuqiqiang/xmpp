#ifndef __XMPP_H__
#define __XMPP_H__


#include "Friend_list.h"



#define N 7//固定长度为7

#define true 1
#define false 0


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


#define STR_SIZE 10000

#define MAX_SIZE 10000000







//void set_sendBroadcast(void *mySendBroadcast);

int isConnect();

char* create_random_num(char* ch);

void xmpp_init();

int connect_xmpp(const char* strUserName, const char* strPassWord, const char* strServerIP, int serverAltport);

int xmpp_send_chat_message(const char* chat_object, const char* body, const char* id);

int xmpp_set_status(const char* status);

int xmpp_get_friends_list();

int xmpp_add_friends(const char* new_friends);

int xmpp_delete_friends(const char* gDelFriend_id);

int stop_xmpp();

int xmpp_request_game(const char* new_friend);

int xmpp_accept_game(const char* new_friend);

int xmpp_send_game_message(const char* new_friend, const char* content);


int xmpp_send_picture(const char* chat_object, const char* name, const char* content, const char* id);

void xmpp_release();

#endif