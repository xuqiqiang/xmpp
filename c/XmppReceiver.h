#ifndef __XMPP_RECEIVER_H__
#define __XMPP_RECEIVER_H__

#include "strophe.h"
#include "Xmpp.h"

status handle_message(xmpp_stanza_t * const stanza);

status handle_iq(xmpp_stanza_t * const stanza);

status handle_presence(xmpp_stanza_t * const stanza);


int on_receive_message_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata);

int on_receive_iq_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata);

int on_receive_presence_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata);

int get_send_chat_feedback_handler(xmpp_conn_t * const conn,
	xmpp_stanza_t * const stanza,
	void * const userdata);

#endif