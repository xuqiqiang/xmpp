#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Xmpp.h"
#include "strophe.h"
#include <pthread.h>
#include <unistd.h>
#include "XmppString.h"
#include "XmppReceiver.h"
#include "XmppSender.h"



int main(void) {


	xmpp_init();

	//regDevInfoToServer("xu1230@tt.com", "123", "202.98.157.54", 5223);

	connect_xmpp("xu123", "123", "tt.com", "202.98.157.54", 5223);
	//connect_xmpp("xu1", "123dsa", "shinelon-pc", "10.3.34.183", 5222);


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
			test_send_chat_message("xu456", input);
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
			xmpp_add_friends(input, XMPP_ADD_FRIEND_INTENT_REQUEST);
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
		else if (strcmp(input, "9") == 0) {	//输入9：
			xmpp_get_vCard(NULL);
		}
		else if (strcmp(input, "a") == 0) {	//输入a：

			char *base = "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";
			xmpp_set_vCard("xuqiqiang", NULL);
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

}
