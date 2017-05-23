#ifndef __XMPP_SENDER_H__
#define __XMPP_SENDER_H__


#include "strophe.h"
#include "Xmpp.h"

int xmpp_send_chat_feedback_message(
	const char* const to,
	const char* const id);


void request_friends_status();

int xmpp_add_friends_respond(const char *new_friends, const char *respond);

void test_send_chat_message(char* friend_id, char* body);

int xmpp_send_heartbeat();

int xmpp_declare_online();

int xmpp_create_groupchat(const char* groupchat_jid);


#endif
