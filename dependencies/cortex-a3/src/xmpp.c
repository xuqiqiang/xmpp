#include "xmpp.h"

#define RECV_SERV_BUF_SIZE (1024*1024)
#define N 7//固定长度为7
#define STR_LEN 1024//固定长度为7
static char gServXmpp[64] = { 0 };
static char gServXmppIP[64] = { 0 };
static short gServXmppPort = 0;

static char gUserName[64] = { 0 };
static char gPassWord[64] = { 0 };
static char gNewFriendName[64] = { 0 };
static char gFriendName[64] = { 0 };
static char gDelFriendName[64] = { 0 };

static char gChatBody[1024 * 4] = { 0 };
static int gServLiveTime = 0;
pthread_t gServTid = 0;
pthread_mutex_t gServLock;
pthread_t gHeartTid = 0;

int gServConnected = 0;/*-1:error,0:disconnect,1:connect */
int gServSock = -1;
int gIsInited = 1;
int gStatus = 0; //used for add and delete friends
int gDelStatus = 0;

extern int deep;

/*IOS
 int gSecondStatus = 0;
 IOS*/

unsigned int get_ip4(char *host) {
	char dns_buff[8192];
	struct hostent hostinfo, *phost;
	struct sockaddr_in addr4;
	int i, flag, rc;

	time_t now;
	unsigned int ip4;

	printf("%s:%d\n", __func__, __LINE__);

	bzero(&addr4, sizeof(struct sockaddr_in));

	printf("%s:%d\n", __func__, __LINE__);

	if (1 == inet_pton(AF_INET, host, &(addr4.sin_addr)))
		return (addr4.sin_addr.s_addr);

	printf("%s:%d\n", __func__, __LINE__);

	if (0 == gethostbyname_r(host, &hostinfo, dns_buff, 8192, &phost, &rc)) {
		printf("%s:%d\n", __func__, __LINE__);
		ip4 = *(unsigned int *) (hostinfo.h_addr);
	} else {
		ip4 = 0;
	}

	printf("%s:%d\n", __func__, __LINE__);

	return (ip4);

}

int xmpp_get_ipaddr(char *domainname, char *ip) {
	struct addrinfo hints, *res, *cur;
	struct sockaddr_in *addr;

	int ret;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;

	if ((ret = getaddrinfo(domainname, NULL, &hints, &res)) != 0) {
		printf("\n[Error %d] get ip addr Error for %s, %s \n", ret, domainname,
				gai_strerror(ret));
		return -1;
	}

	for (cur = res; cur != NULL; cur = cur->ai_next) {
		addr = (struct sockaddr_in *) cur->ai_addr;
		inet_ntop(AF_INET, &addr->sin_addr, ip, 16);
	}
	freeaddrinfo(res);

	return 0;
}

int parse_ip_addr_from_string(const char *string, char *ip, short *port) {
	char *p = NULL;
	struct in_addr addr;
	char str_tmp[256] = { 0 };

	printf("%s:%d\n", __func__, __LINE__);

	if (string == NULL || ip == NULL)
		return -1;

	printf("%s:%d\n", __func__, __LINE__);

	p = strchr(string, ':');

	printf("%s:%d\n", __func__, __LINE__);

	*port = atoi(p + 1);

	printf("%s:%d\n", __func__, __LINE__);

	memcpy(str_tmp, string, p - string);

	printf("%s:%d\n", __func__, __LINE__);

	/*addr.s_addr = get_ip4(str_tmp);
	 if (addr.s_addr == 0) {
	 return -1;
	 }

	 printf("%s:%d\n", __func__, __LINE__);

	 char* ipstr = inet_ntoa(addr);*/

	//char ipstr[100];

	//strcpy(ip, ipstr);
	//printf("ip=%s,port=%d\n", ip, *port);

	/*char str[STR_LEN];
	 sprintf(str,"ip=%s,port=%d\n", ip, *port);
	 show_log(str, __func__, __LINE__);*/
	return xmpp_get_ipaddr(str_tmp, ip);
}

/*
 *produce a random string number to be used
 *the num len is N
 *return : the random num
 */

char* create_random_num(char* ch) {
	int flag;
	int j, k = 0;

	srand((unsigned int) time((time_t*) NULL));

	for (j = 0; j < N; j++) {
		flag = rand() % 2;
		if (flag)
			ch[k++] = 'A' + rand() % 26;
		else
			ch[k++] = 'a' + rand() % 26;
	}

	ch[k] = '\0';

	return ch;
}

/* IOS
 * zhudong dele the add friends
 *
 int certain_yesorno_on_add_friends(BOOL tag){
 if(tag == 1){
 printf("-----------------add success");
 send_on_confirm_add_friends();
 gSecondStatus = 1;
 return 1;
 }

 if(tag == 0){
 printf("-----------------del friends");
 send_on_delete_friends();
 gSecondStatus = 0;
 return 0;
 }


 return 0;
 }
 IOS*/

/*IOS   get friends status like online,offline
 int handle_on_get_friends_status(char* p,int str_len){

 char*end_node = NULL;
 char*cur_node = NULL;
 char* pre_node = NULL;
 int clean_len = 0;

 int i = 0;
 int data_len = 0;
 BOOL ret = TRUE;
 if(NULL == p ||  str_len <= 0 ){
 printf("err:at get friends status");
 return 0;
 }
 printf("%s:%d\n", __func__, __LINE__);
 pfriendstatus friends_status;
 friends_status = malloc(sizeof(friendstatus)*20);
 memset(friends_status,0,sizeof(friendstatus)*20);
 printf("%s:%d\n", __func__, __LINE__);
 while(ret){

 if(strstr(p,"from") != NULL && strstr(p,"unavailable") == NULL){ //take friends item out
 char *pre_node1 = "from=\"";
 int len = strlen(pre_node1);

 pre_node = strstr(p,pre_node1);
 end_node = pre_node + len;
 clean_len = end_node - p;
 memset(p,0,clean_len);
 memcpy(p,end_node,str_len - clean_len);
 memset(p+(str_len - clean_len),0,clean_len);

 if(strstr(p,"@") != NULL){
 cur_node = strstr(p,"@");
 data_len = cur_node - p;
 memcpy(friends_status[i].friend_item,p,data_len);
 }



 if(strstr(p,"<status>") != NULL){
 char* pre_node1 = "<status>";
 int len1 = strlen(pre_node1);

 pre_node = strstr(p,pre_node1);
 end_node = pre_node + len1;
 clean_len = end_node - p;
 memset(p,0,clean_len);
 memcpy(p,end_node,str_len - clean_len);
 memset(p+(str_len - clean_len),0,clean_len);

 if(strstr(p,"</status>") != NULL){
 cur_node = strstr(p,"</status>");
 data_len = cur_node - p;
 memcpy(friends_status[i].friend_status,p,data_len);
 i++;
 }

 }
 } //first if


 //handle on take offline message
 else if(strstr(p,"unavailable") != NULL){  printf("%s:%d\n", __func__, __LINE__);

 if(strstr(p,"from") != NULL){
 char *pre_node1 = "from=\"";
 int len = strlen(pre_node1);

 pre_node = strstr(p,pre_node1);
 end_node = pre_node + len;
 clean_len = end_node - p;
 memset(p,0,clean_len);
 memcpy(p,end_node,str_len - clean_len);
 memset(p+(str_len - clean_len),0,clean_len);

 if(strstr(p,"@") != NULL){
 cur_node = strstr(p,"@");
 data_len = cur_node - p;
 memcpy(friends_status[i].friend_item,p,data_len);
 }

 }
 char* temp = "unavailable";
 memcpy(friends_status[i].friend_status,temp,strlen(temp));
 ret = FALSE;
 i++;
 }


 else {
 ret = FALSE;
 printf("%s,%d",__func__,__LINE__);
 }

 } //while


 recv_friends_online_status(friends_status,i);
 free(friends_status);
 return 0;
 }

 */

/*
 * send a message to confirm add a friends
 */
int on_confirm_add_friends() {

	int obj_len = 0;
	char obj_str_group[512] = { 0 };
	strcat(
			obj_str_group,
			"<iq id=\"MU55B-46\" type=\"set\"><query xmlns=\"jabber:iq:roster\"><item jid=\"");
	strcat(obj_str_group, gNewFriendName);
	strcat(obj_str_group, "@tt.com\" name=\"");
	strcat(obj_str_group, gNewFriendName);
	strcat(obj_str_group, "\"><group>Friends</group></item></query></iq>");
	char* obj_str = obj_str_group;

	printf("first time add send obj_str : %s\n", obj_str);
	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		//printf("first :on confirm_add_friends send success...................");

		show_log(
				"first :on confirm_add_friends send success...................",
				__func__, __LINE__);
		gStatus = 1; //monitor next add friends  message

	} else {
		//printf("first:on confirm_add_friends send error...................");
		show_log("first:on confirm_add_friends send error...................",
				__func__, __LINE__);
	}
	free(send_buf);
	return 0;

}
/*send to confirm add friends senconds step */
int send_on_confirm_add_friends() {
	int obj_len = 0;
	if (gNewFriendName == 0) {
		//printf("gNewFriendName == 0; %s,%d\n",__func__,__LINE__);
		show_log("gNewFriendName == 0", __func__, __LINE__);
		return 0;
	}
	char obj_str_group[256] = { 0 };
	strcat(obj_str_group, "<presence id=\"MU55B-47\" to=\"");
	strcat(obj_str_group, gNewFriendName);
	strcat(obj_str_group, "@tt.com\" type=\"subscribe\"/>");
	char* obj_str = NULL;
	obj_str = obj_str_group;

	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		//printf("second time:send_on_confirm_add_friends send success...................\n");
		show_log(
				"second time:send_on_confirm_add_friends send success...................",
				__func__, __LINE__);
	} else {
		//printf("second time:send_on_confirm_add_friends send error...................\n");
		show_log(
				"second time:send_on_confirm_add_friends send error...................",
				__func__, __LINE__);
	}
	free(send_buf);
	return 0;

}
/*send to confirm add friends third step */
int send_on_confirm_add_friends_third_time() {
	int obj_len = 0;
	if (gNewFriendName == 0) {
		//printf("gNewFriendName == 0; %s,%d\n",__func__,__LINE__);
		show_log("gNewFriendName == 0", __func__, __LINE__);
		return 0;
	}
	char obj_str_group[256] = { 0 };
	strcat(obj_str_group, "<presence id=\"MU55B-48\" to=\"");
	strcat(obj_str_group, gNewFriendName);
	strcat(obj_str_group, "@tt.com\" type=\"subscribed\"/>");

	char* obj_str = NULL;
	obj_str = obj_str_group;

	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		//printf("third time:send_on_confirm_add_friends_third_time send success...................");
		show_log(
				"third time:send_on_confirm_add_friends_third_time send success...................",
				__func__, __LINE__);
	} else {
		//printf("third time:send_on_confirm_add_friends_third_time send error...................");
		show_log(
				"third time:send_on_confirm_add_friends_third_time send error...................",
				__func__, __LINE__);
	}
	free(send_buf);
	return 0;

}

int declare_online() { //
	int obj_len = 0;

	const char
			* obj_str =
					"<presence id=\"HOXUk-19\"><status>空闲</status><priority>1</priority><show>chat</show></presence>";
	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	//	memcpy(send_buf, &obj_len, 2);
	memcpy(send_buf, obj_str, obj_len);
	//	memcpy(send_buf + 4+obj_len, obj_str1, obj_len1);
	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		//printf("declare_online send success...................\n");
		show_log("declare_online send success...................", __func__,
				__LINE__);
	} else {
		//printf("declare_online send error...................\n");
		show_log("declare_online send error...................", __func__,
				__LINE__);
	}
	free(send_buf);
	return 0;

}

int send_heartbeat() {
	int ret = 0;

	const char* obj_str =
			"<iq id=\"NSken-4\" type=\"get\"><ping xmlns='urn:xmpp:ping' /></iq>";

	int obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	//	memcpy(send_buf, &obj_len, 2);
	memcpy(send_buf, obj_str, obj_len);
	printf("send_heartbeat(),size=%d,data=%s\n", obj_len, send_buf);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		if (socket_send(gServSock, send_buf, obj_len, 0) < 0) {
			printf("err:socket_send(),errno=%d\n", errno);
			gServConnected = -1;
			ret = -1;
		}
		pthread_mutex_unlock(&gServLock);
	} else {
		ret = -1;
	}

	free(send_buf);
	return ret;
}
/*send some stuff every 10 second*/
void* heart_thread(void *arg) {
	struct timeval tv;
	struct timeval time;
	unsigned int last_beat_time;

	gettimeofday(&tv, 0);
	last_beat_time = tv.tv_sec;
	gServLiveTime = tv.tv_sec;
	printf("heart thread create success \n");

	while (gIsInited == 1) {

		gettimeofday(&time, 0);

		if (time.tv_sec - last_beat_time > 10) {/*send every 30s*/
			send_heartbeat();
			last_beat_time = time.tv_sec;
		}

		if (time.tv_sec - gServLiveTime > 60) { /*the server is dead*/

			//printf(
			//		"The Xmpp Servier is dead,socket try to connect again and again");

			show_log(
					"The Xmpp Servier is dead,socket try to connect again and again",
					__func__, __LINE__);
			gServConnected = -1;
		}

		if (gServConnected == -1) {/*the thread will exit*/
			//printf("The network is off ,heart-beat thread exit");
			show_log("The network is off ,heart-beat thread exit", __func__,
					__LINE__);
			pthread_exit(NULL);
		}

		sleep(1);/*1s*/
	}

	return (void*)0;
}

int regDevInfoToServer() {
	int ret = -1;
	int obj_len = 0;

	char obj_str_group[512] = { 0 };
	strcat(obj_str_group,
			"<iq id=\"RGzjR-1\" type=\"set\"><query xmlns=\"jabber:iq:auth\"><username>");

	strcat(obj_str_group, gUserName);

	strcat(obj_str_group, "</username><password>");
	strcat(obj_str_group, gPassWord);
	strcat(obj_str_group, "</password><resource>tt.com</resource></query></iq>");
	char* obj_str = obj_str_group;
	obj_len = strlen(obj_str);

	printf("msg info register obj_str :  %s\n", obj_str);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		ret = 0;
	} else {

		ret = -1;
	}
	free(send_buf);
	return ret;
}
/*
 * zhudong dele the add friends
 */
//int certain_yesorno_on_add_friends(BOOL tag){
//	if(tag == 1){
//		printf("-----------------add success");
//		send_on_confirm_add_friends();
//		gSecondStatus = 1;
//		return 1;
//	}
//
//	if(tag == 0){
//		printf("-----------------del friends");
//		send_on_delete_friends();
//		gSecondStatus = 0;
//		return 0;
//	}
//
//
//	return 0;
//}


/*
 * get the new friends name so that is easy to operate
 */
int handle_on_get_new_friends_name(char* data) {

	char* p = data;
	char* retNode = NULL;
	char*retNode_end = NULL;
	char*retNode_end_end = NULL;

	int str_len = strlen(p);
	printf("p------------> %s;%s:%d\n", p, __func__, __LINE__);
	if (data == NULL || str_len < 0) {
		printf("%s:%d\n", __func__, __LINE__);
		return 0;
	}

	printf("%s:%d\n", __func__, __LINE__);
	if (strstr(p, "from=\"") != NULL) {
		retNode = strstr(p, "from=\"");
		int len = strlen("from=\"");
		retNode_end = retNode + len;

		int clean_len = retNode_end - p;
		if (clean_len < 0) {
			memset(p, 0, str_len);
			return 0;
		}
		memset(p, 0, clean_len);
		memcpy(p, retNode_end, str_len - clean_len);

		memset(p + (str_len - clean_len), 0, clean_len); //2.19pm


	}
	printf("%s:%d\n", __func__, __LINE__);
	if (strstr(p, "@tt.com\"") != NULL) {
		retNode_end_end = strstr(p, "@tt.com\"");
		int len2 = retNode_end_end - p;
		memcpy(gNewFriendName, p, len2);
		//printf("gNewFriendName------------> %s:%s:%d\n",gNewFriendName,__func__,__LINE__);

		char str[STR_LEN];
		sprintf(str, "gNewFriendName------------> %s", gNewFriendName);
		show_log(str, __func__, __LINE__);

		on_confirm_add_friends();
		memset(p, 0, str_len);
	}

	return 0;
}

/*
 * used when server send a chat message ,the client send a message back, notice that client have recv the message
 *
 */

int send_on_chat_message() { //add an OK for test ,now not used
	int obj_len = 0;
	//	char* CallBack = "OK";								//here you need to add your call back content
	//	 strcpy(gChatBackBody,cell);

	char obj_str1[1024] = { 0 };
	strcat(obj_str1, "<message to=\"");
	strcat(obj_str1, gFriendName);
	strcat(obj_str1, "@tt.com\" xmlns=\"jabber:client\" id=\"E54XC-8\"  from=");

	strcat(obj_str1, gUserName);
	strcat(obj_str1, "@tt.com/tt.com\" type=\"chat\"><body>");

	strcat(obj_str1, "</body><thread>CC8rG0</thread></message>");
	char* obj_str = obj_str1;

	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	memcpy(send_buf, obj_str, obj_len);
	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		printf("send_on_chat_message send success...................\n");
	} else {
		printf("send_on_chat_message send error...................\n");
	}
	free(send_buf);
	return 0;

}

/* to take a friends name to be delete and send a message to delete it */
int handle_take_delete_friends_list(char* data) {
	char*p = data;
	printf("%s:%d\n", __func__, __LINE__);
	char* preNode = NULL; //point to the start of the handle msg
	char* endNode = NULL; //point to the end of the handle msg
	char* curNode = NULL;
	int str_len = strlen(p);
	if (strstr(p, "from=\"") != NULL) {
		preNode = strstr(p, "from=\"");
		int len = strlen("from=\"");
		endNode = preNode + len;
		int clean_len = endNode - preNode;
		memset(p, 0, clean_len);
		memcpy(p, endNode, str_len - clean_len);

		memset(p + (str_len - clean_len), 0, clean_len); //2.19pm

		//		if(clean_len >= str_len-clean_len){
		//			memset(endNode,0,str_len-clean_len);
		//		}else {
		//			char* newNode = NULL;
		//			newNode = p + (str_len-clean_len);
		//			memset(newNode,0,clean_len);
		//		}

		if (strstr(p, "@tt.com\"") != NULL) {
			curNode = strstr(p, "@tt.com\"");

			if (endNode != NULL) {
				int len1 = curNode - p;
				memcpy(gDelFriendName, p, len1); //printf(":%s:%s:%d\n", gDelFriendName,__func__, __LINE__);

				char str[STR_LEN];
				sprintf(str, "gDelFriendName------------> %s", gDelFriendName);
				show_log(str, __func__, __LINE__);

				send_on_delete_friends();

				memset(p, 0, str_len);
			}

		}

	}

	return 0;
}
/*
 * parse the friends list out to hanlde
 * get the friends group and name
 */
int handle_take_friends_list(char* data) {

	char* p = data;
	printf("%s:%d\n", __func__, __LINE__);
	char* preNode = NULL; //point to the start of the handle msg
	char* endNode = NULL; //point to the end of the handle msg
	int data_len = 0;
	char* curNode = NULL;
	int i = 0;
	BOOL ret = TRUE;
	int str_len = strlen(p);
	if (p == NULL || str_len < 0) {
		return 0;
	}

	pfriendslistbuffer buffer;

	buffer = malloc(sizeof(friendslistbuffer) * 20);
	memset(buffer, 0, sizeof(friendslistbuffer) * 20);

	int clean_len = 0;

	while (ret) {

		if (strstr(p, "jid=\"") != NULL && strstr(p, "name=\"") != NULL
				&& strstr(p, "jid=\"") < strstr(p, "name=\"")) {
			//	if(strstr(p,"jid=\"") != NULL){

			preNode = strstr(p, "jid=\"");
			int len1 = strlen("jid=\"");
			endNode = preNode + len1;
			clean_len = endNode - p;
			memset(p, 0, clean_len);
			memcpy(p, endNode, str_len - clean_len);
			//			int mid_len = str_len-clean_len -clean_len;
			char* newNode = NULL;
			newNode = p + str_len - clean_len;
			memset(newNode, 0, clean_len);

			if (strstr(p, "\"") != NULL) {
				curNode = strstr(p, "\"");

				data_len = curNode - p;
				memcpy(buffer[i].jid, p, data_len);
				printf("bufferdata--jid--------------------> :%s\n ",
						buffer[i].jid);

			}

			//		}
			printf("%s:%d\n", __func__, __LINE__);
			//	if(strstr(p,"name=\"") != NULL){

			preNode = strstr(p, "name=\"");
			int len2 = strlen("name=\"");
			endNode = preNode + len2; //point to the end of the handle msg
			clean_len = endNode - p;
			memset(p, 0, clean_len);
			memcpy(p, endNode, str_len - clean_len);

			//				int ret_len = str_len-clean_len -clean_len;
			char* newNode1 = NULL;
			newNode1 = p + str_len - clean_len;
			memset(newNode1, 0, clean_len);

			if (strstr(p, "\"") != NULL) {
				curNode = strstr(p, "\"");
				data_len = curNode - p;
				memcpy(buffer[i].name, p, data_len);

				printf("bufferdata--name--------------------> :%s\n ",
						buffer[i].name);
				printf("buffer-------------->%s:%s:%s:%d:%d\n", buffer[i].jid,
						buffer[i].name, __func__, __LINE__, i);
				i++;

			}

			//			}
		}

		else if (strstr(p, "jid=\"") != NULL && strstr(p, "name=\"") != NULL
				&& strstr(p, "jid=\"") > strstr(p, "name=\"")) {

			preNode = strstr(p, "name=\"");
			int len2 = strlen("name=\"");
			endNode = preNode + len2; //point to the end of the handle msg
			clean_len = endNode - p;
			memset(p, 0, clean_len);
			memcpy(p, endNode, str_len - clean_len);

			//					int ret_len = str_len-clean_len -clean_len;
			char* newNode1 = NULL;
			newNode1 = p + str_len - clean_len;
			memset(newNode1, 0, clean_len);

			if (strstr(p, "\"") != NULL) {
				curNode = strstr(p, "\"");
				data_len = curNode - p;
				memcpy(buffer[i].name, p, data_len);

				printf("bufferdata--name--------------------> :%s\n ",
						buffer[i].name);
				printf("buffer-------------->%s:%s:%s:%d:%d\n", buffer[i].jid,
						buffer[i].name, __func__, __LINE__, i);

			}

			preNode = strstr(p, "jid=\"");
			int len1 = strlen("jid=\"");
			endNode = preNode + len1;
			clean_len = endNode - p;
			memset(p, 0, clean_len);
			memcpy(p, endNode, str_len - clean_len);
			//					int mid_len = str_len-clean_len -clean_len;
			char* newNode = NULL;
			newNode = p + str_len - clean_len;
			memset(newNode, 0, clean_len);

			if (strstr(p, "\"") != NULL) {
				curNode = strstr(p, "\"");

				data_len = curNode - p;
				memcpy(buffer[i].jid, p, data_len);
				printf("bufferdata--jid--------------------> :%s\n ",
						buffer[i].jid);
				i++;

			}

		}

		else {
			ret = FALSE;
			printf("%s%d    %s\n", __func__, __LINE__, p);
		}

	}

	//	recv_friends_list_message(buffer,i-1); // send to upper level
	recv_friends_list_message(buffer, i);
	printf("%s:%d\n", __func__, __LINE__);
	free(buffer);
	return 0;
}

//
//int handle_on_get_friends_status(char* p,int str_len){
//
//	char*end_node = NULL;
//	char*cur_node = NULL;
//	char* pre_node = NULL;
//	int clean_len = 0;
//
//	int i = 0;
//	int data_len = 0;
//	BOOL ret = TRUE;
//	if(NULL == p ||  str_len <= 0 ){
//		printf("err:at get friends status");
//		return 0;
//	}
//	printf("%s:%d\n", __func__, __LINE__);
//	pfriendstatus friends_status;
//	friends_status = malloc(sizeof(friendstatus)*20);
//	memset(friends_status,0,sizeof(friendstatus)*20);
//	printf("%s:%d\n", __func__, __LINE__);
//while(ret){
//
//	if(strstr(p,"from") != NULL && strstr(p,"unavailable") == NULL){ //take friends item out
//		char *pre_node1 = "from=\"";
//		int len = strlen(pre_node1);
//
//		pre_node = strstr(p,pre_node1);
//		end_node = pre_node + len;
//		clean_len = end_node - p;
//		memset(p,0,clean_len);
//		memcpy(p,end_node,str_len - clean_len);
//		memset(p+(str_len - clean_len),0,clean_len);
//
//		if(strstr(p,"@") != NULL){
//			cur_node = strstr(p,"@");
//			data_len = cur_node - p;
//			memcpy(friends_status[i].friend_item,p,data_len);
//		}
//
//
//
//	if(strstr(p,"<status>") != NULL){
//		char* pre_node1 = "<status>";
//		int len1 = strlen(pre_node1);
//
//		pre_node = strstr(p,pre_node1);
//		end_node = pre_node + len1;
//		clean_len = end_node - p;
//		memset(p,0,clean_len);
//		memcpy(p,end_node,str_len - clean_len);
//		memset(p+(str_len - clean_len),0,clean_len);
//
//		if(strstr(p,"</status>") != NULL){
//			cur_node = strstr(p,"</status>");
//			data_len = cur_node - p;
//			memcpy(friends_status[i].friend_status,p,data_len);
//			i++;
//		 }
//
//	 }
//    } //first if
//
//
//	//handle on take offline message
//	else if(strstr(p,"unavailable") != NULL){  printf("%s:%d\n", __func__, __LINE__);
//
//		if(strstr(p,"from") != NULL){
//		    char *pre_node1 = "from=\"";
//			int len = strlen(pre_node1);
//
//			pre_node = strstr(p,pre_node1);
//			end_node = pre_node + len;
//			clean_len = end_node - p;
//			memset(p,0,clean_len);
//			memcpy(p,end_node,str_len - clean_len);
//			memset(p+(str_len - clean_len),0,clean_len);
//
//					if(strstr(p,"@") != NULL){
//						cur_node = strstr(p,"@");
//						data_len = cur_node - p;
//						memcpy(friends_status[i].friend_item,p,data_len);
//					}
//
//		}
//		char* temp = "unavailable";
//		memcpy(friends_status[i].friend_status,temp,strlen(temp));
//		ret = FALSE;
//		i++;
//	}
//
//
//	else {
//		ret = FALSE;
//		printf("%s,%d",__func__,__LINE__);
//	}
//
// } //while
//
//
//	recv_friends_online_status(friends_status,i);
//	free(friends_status);
//	return 0;
//}

/*
 *get the message content
 */
int handle_on_chat_message(char* data) {
	char* p = data;
	char* preNode = NULL;
	char* endNode = NULL;
	int data_len = 0;
	char* curNode = NULL;
	int clear_len = 0;
	//	char* cell = NULL;
	int str_len = strlen(p);
	printf("%s:%d\n", __func__, __LINE__);
	if (p == NULL || str_len < 0) {
		return 0;
	}

	pchatbuffer chat_buffer;
	chat_buffer = malloc(sizeof(chatbuffer));
	memset(chat_buffer, 0, sizeof(chatbuffer));

	printf("%s:%d\n", __func__, __LINE__);

	if (strstr(p, "to=\"") != NULL && strstr(p, "from=\"") != NULL && strstr(p,
			"to=\"") < strstr(p, "from=\"")) { //compare the address front or after
		/*get the content to=" first*/

		preNode = strstr(p, "to=\"");
		int len = strlen("to=\"");
		endNode = preNode + len;
		printf("%s:%d\n", __func__, __LINE__);

		clear_len = endNode - p; //clear and make sure the buffer only include the needed parase data
		memset(p, 0, clear_len);
		memcpy(p, endNode, str_len - clear_len);
		//		int mret_len = str_len-clear_len -clear_len;

		char* mnewNode = NULL;
		mnewNode = p + str_len - clear_len; //2--13
		memset(mnewNode, 0, clear_len);

		if (strstr(p, "\"") != NULL) {
			curNode = strstr(p, "\"");
			data_len = curNode - p;
			memcpy(chat_buffer->to, p, data_len);
			printf("chat_buffer->to: %s\n", chat_buffer->to);

		}

		/*get the content to=" first*/
		preNode = strstr(p, "from=\"");
		int len2 = strlen("from=\"");
		endNode = preNode + len2;

		clear_len = endNode - p;
		memset(p, 0, clear_len);
		memcpy(p, endNode, str_len - clear_len);
		//			int mid_len = str_len-clear_len -clear_len;
		char* newNode = NULL;
		//			newNode = endNode+mid_len;
		newNode = p + str_len - clear_len;
		memset(newNode, 0, clear_len);

		//		    if(strstr(p,"\"") != NULL){
		//		    			curNode = strstr(p,"\"");

		if (strstr(p, "@") != NULL) { //2014.2.11guolu
			curNode = strstr(p, "@");
			if (endNode != NULL) {
				data_len = curNode - p;

				memcpy(chat_buffer->from, p, data_len);
				printf("chat_buffer->from: %s\n", chat_buffer->from);
			}
		}

	} //compare the address front or after

	if (strstr(p, "to=\"") != NULL && strstr(p, "from=\"") != NULL && strstr(p,
			"to=\"") > strstr(p, "from=\"")) {
		/*get the content to=" first */
		preNode = strstr(p, "from=\"");
		int len2 = strlen("from=\"");
		endNode = preNode + len2;

		clear_len = endNode - p;
		memset(p, 0, clear_len);
		memcpy(p, endNode, str_len - clear_len);
		//		int mid_len = str_len-clear_len -clear_len;
		char* newNode = NULL;
		//		newNode = endNode+mid_len;
		newNode = p + str_len - clear_len;
		memset(newNode, 0, clear_len);

		if (strstr(p, "@") != NULL) { //2014-2-11 strstr(p,"\"") != NULL
			curNode = strstr(p, "@");
			if (endNode != NULL) {
				data_len = curNode - p;

				memcpy(chat_buffer->from, p, data_len);
				printf("chat_buffer->from: %s\n", chat_buffer->from);
			}
		}

		/*get the content to=" second*/
		preNode = strstr(p, "to=\"");
		int len = strlen("to=\"");
		endNode = preNode + len;

		clear_len = endNode - p; //clear and make sure the buffer only include the needed parase data
		memset(p, 0, clear_len);
		memcpy(p, endNode, str_len - clear_len);
		//			int mret_len = str_len-clear_len -clear_len;
		char* mnewNode = NULL;
		//			mnewNode = endNode+mret_len;
		mnewNode = p + str_len - clear_len;
		memset(mnewNode, 0, clear_len);

		if (strstr(p, "\"") != NULL) {
			curNode = strstr(p, "\"");
			data_len = curNode - p;
			memcpy(chat_buffer->to, p, data_len);
			printf("chat_buffer->to: %s\n", chat_buffer->to);

		}

	}

	if (strstr(p, "<body>") != NULL) {
		preNode = strstr(p, "<body>");
		int len4 = strlen("<body>");
		endNode = preNode + len4;

		clear_len = endNode - p;
		memset(p, 0, clear_len);
		memcpy(p, endNode, str_len - clear_len);

		//			int ret_len = str_len-clear_len -clear_len;
		char* newNode1 = NULL;
		//			newNode1 = endNode+ret_len;
		newNode1 = p + str_len - clear_len;
		memset(newNode1, 0, clear_len);

		if (strstr(p, "</body>") != NULL) {
			curNode = strstr(p, "</body>");
			data_len = curNode - p;
			memcpy(chat_buffer->body, p, data_len);
			printf("chat_buffer->body: %s\n", chat_buffer->body);
			memset(p, 0, str_len); //add at 2014/2/19
		}

	}

	//printf("========================================================================\n\n");
	//printf("接收到来自%s的消息:%s\n\n",chat_buffer->from,chat_buffer->body);
	//printf("========================================================================\n");

	char str[STR_LEN];
	//sprintf(str,"接收到来自%s的消息:%s",chat_buffer->from,chat_buffer->body);
	sprintf(str, "receive message from %s :%s", chat_buffer->from,
			chat_buffer->body);
	show_log(str, __func__, __LINE__);

	//printf("chat_buffer->from:%s;chat_buffer->to: %s;chat_buffer->body: %s\n",chat_buffer->from,chat_buffer->to,chat_buffer->body);
	recv_chat_message(chat_buffer);
	free(chat_buffer);

	return 0;

}

int stop_xmpp() {

	gIsInited=0;
	char obj_str[1024];
	strcpy(obj_str, "</stream:stream>");
	int obj_len = strlen(obj_str);

	printf("msg info add_friend send... obj_str :  %s\n", obj_str);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
	} else {
		printf("stop xmpp...................\n");
	}
	free(send_buf);
	return 0;
}

int InitInfoToServer() {
	int ret = -1;
	int obj_len = 0;
	const char* obj_str = NULL;
	obj_str
			= "<stream:stream to=\"tt.com\" xmlns=\"jabber:client\" xmlns:stream=\"http://etherx.jabber.org/streams\" version=\"1.0\">";
	obj_len = strlen(obj_str);

	printf("msg info init obj_str :  %s\n", obj_str);
	printf("msg info init obj_len :  %d\n", obj_len);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		//printf("InitInfoToServer send success...................");

		show_log("InitInfoToServer send success...................", __func__,
				__LINE__);
		pthread_mutex_unlock(&gServLock);
		ret = 0;
	} else {
		ret = -1;
	}

	free(send_buf);
	return ret;
}

int get_friends_list() {

	int obj_len = 0;
	const char* obj_str = NULL;
	obj_str
			= "<iq id=\"s1dWX-4\" type=\"get\"><query xmlns=\"jabber:iq:roster\"></query></iq> ";
	obj_len = strlen(obj_str);

	printf("msg info get_friends_list send... obj_str :  %s\n", obj_str);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		printf("get_friends_list send success...................");
	} else {
		//printf("get_friends_list send error...................");
		show_log("get_friends_list send error...................", __func__,
				__LINE__);
	}


	free(send_buf);

	return 0;

}

int add_friends(char* newFriends) {

	int obj_len = 0;

	char obj_str[1024];
	strcpy(obj_str,
			"<presence xmlns=\"jabber:client\" type=\"subscribe\" from=\"");
	strcat(obj_str, gUserName);
	strcat(obj_str, "@tt.com\" to=\"");
	strcat(obj_str, newFriends);
	strcat(obj_str, "@tt.com\"/>");
	//const char* obj_str = "<presence to=\"cena1@tt.com\" xmlns=\"jabber:client\" from=\"wade12@tt.com\" type=\"subscribe\"/>";
	obj_len = strlen(obj_str);

	printf("msg info add_friend send... obj_str :  %s\n", obj_str);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);

	memcpy(send_buf, obj_str, obj_len);

	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		printf("add_friends send success...................");
	} else {
		printf("add_friends send error...................");
	}
	free(send_buf);
	return 0;
}

int send_on_delete_friends() {

	int obj_len = 0;
	if (gDelFriendName == NULL) {
		printf("err :gDelFriendName is NULL");
		return 0;
	}
	char obj_group[512] = { 0 };
	strcat(
			obj_group,
			"<iq id=\"O34qx-33\" type=\"set\"><query xmlns=\"jabber:iq:roster\"><item jid=\"");
	strcat(obj_group, gDelFriendName);
	strcat(obj_group, "@tt.com\" name=\"");
	strcat(obj_group, gDelFriendName);
	strcat(obj_group,
			"\" subscription=\"remove\"><group>Friends</group></item></query></iq>");
	char* obj_str = NULL;
	obj_str = obj_group;

	obj_len = strlen(obj_str);

	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	memcpy(send_buf, obj_str, obj_len);
	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		printf("delete_friends messgae send success...................");

		gDelStatus = 1;

	} else {
		//printf("delete_friends send error...................");
		show_log("delete_friends send error...................", __func__,
				__LINE__);
	}
	free(send_buf);
	return 0;
}

/*
 * send chat message by the user's will through  command
 */
int send_chat_message(char* chat_object, char* body) {

	int obj_len = 0;
	int ret = -1;
	if (body == NULL) {
		printf("please enter the chat content");
		return 0;
	}

	char* random_id = NULL;
	char ch[N + 1] = { 0 };
	random_id = create_random_num(ch);

	if (random_id == NULL) {
		printf("create num fail!!");
		return 0;
	}

	strcpy(gFriendName, chat_object);
	strcpy(gChatBody, body);

	char obj_str_group[1024 * 5] = { 0 };
	strcat(obj_str_group, "<message to=\"");
	strcat(obj_str_group, gFriendName);
	strcat(obj_str_group, "@tt.com\" xmlns=\"jabber:client\" id=\"");
	strcat(obj_str_group, random_id);
	strcat(obj_str_group, "\"  from=\"");

	strcat(obj_str_group, gUserName);
	strcat(obj_str_group, "@tt.com/tt.com\" type=\"chat\"><body>");
	strcat(obj_str_group, gChatBody);
	strcat(obj_str_group, "</body><thread>CC8rG0</thread></message>");

	char* obj_str = obj_str_group;
	obj_len = strlen(obj_str);

	printf("msg info send_chat_message() send... obj_str :  %s\n", obj_str);
	char* send_buf = malloc(obj_len + 1);/*1 byte more for printing*/
	memset(send_buf, 0, obj_len + 1);
	memcpy(send_buf, obj_str, obj_len);
	if (gServSock != -1 && gServConnected == 1) {/*send the data*/
		pthread_mutex_lock(&gServLock);
		socket_send(gServSock, send_buf, obj_len, 0);
		pthread_mutex_unlock(&gServLock);
		ret = 0;
		printf("send_chat_message send success...................\n");

		char str[STR_LEN];
		//sprintf(str,"向%s发送消息:%s", chat_object, body);
		sprintf(str, "send message to %s:%s", chat_object, body);
		show_log(str, __func__, __LINE__);

	} else {
		//printf("send_chat_message send error...................\n");
		show_log("send_chat_message send error...................", __func__,
				__LINE__);
		ret = -1;
	}
	free(send_buf);

	//printf("========================================================================\n\n");
	//printf("向%s发送消息:%s\n\n", chat_object, body);
	//printf("========================================================================\n");


	return ret;

}

int handel_iq_data(char* data) {
	char* p = data;
	pthread_attr_t attr;
	printf("%s,%d\n", __func__, __LINE__);

	if (strstr(p, "Authentication successful") != NULL) {
		printf("register success!!!!  \n");
		declare_online();

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		if (pthread_create(&gHeartTid, &attr, heart_thread, 0) != 0) {
			printf("err:ptread_create():%s:%d\n", __func__, __LINE__);
			return 0;
		}
		pthread_attr_destroy(&attr);

	}

	/* handle on add  a new friends name*/
	else if (strstr(p, "type=\"subscribe\"") != NULL) {
		char* p_start = strstr(p, "<presence");
		if (p_start != NULL)
			handle_on_get_new_friends_name(p_start);
	}

	/*listen to the delete friend order and give a feedback msg to delete friends from list,  as about*/
	else if (strstr(p, "type=\"unsubscribe\"") != NULL) {
		char* p_start = strstr(p, "<presence");
		if (p_start != NULL)
			handle_take_delete_friends_list(p_start);
	}

	/*add friends second step*/
	else if (strstr(p, "ask=\"subscribe\"") == NULL && gStatus == 1) {
		send_on_confirm_add_friends();

		/*IOS
		 memcpy(gDelFriendName,gNewFriendName,512);
		 BOOL yesorno = TRUE;

		 yesorno =ios_on_certain_yesorno_on_add_friends(); // get the comand from upper

		 certain_yesorno_on_add_friends(yesorno);  // handle on add or del friends

		 IOS*/

		gStatus = 0;
	}
	/*remove friends*/
	else if (strstr(p, "subscription=\"remove\"") != NULL) {
		gDelStatus = 0;
	}
	/*get friends list*/
	else if (strstr(p, "id=\"s1dWX-4\"") != NULL) {
		char* p_start = strstr(p, "<iq");
		if (p_start != NULL)
			handle_take_friends_list(p_start);
	}
	/*after delete friends there will be a heart beat ,clean*/
	else if (gDelStatus == 1) {
		gDelStatus = 0;
	}
	/* third time handle the feedback add friends message send by platform*/
	else if (strstr(p, "id=\"MU55B-46\"") != NULL) {
		send_on_confirm_add_friends_third_time();

		/*IOS-------------

		 if(gSecondStatus == 1)
		 send_on_confirm_add_friends_third_time();

		 IOS***********/
	} else
	/*  handle the error code that send by platform 2.20*/
	if (strstr(p, "error") != NULL) {
		//printf("error : unknow error type ");
		show_log("error : unknow error type ", __func__, __LINE__);
	}
	/*  handle off line message left 2014-1-23*/
	else if (strstr(p, "offline") != NULL) {
		//printf("removed offline message, for it's unused ");
		show_log("removed offline message, for it's unused ", __func__,
				__LINE__);
	}

	else {
		printf("heart beat live...\n");
		//show_log("heart beat live...", __func__, __LINE__);
	}

	return 0;
}

int handle_presence_data(char* data) {
	char* p = data;
	/* handle (declare and other online message) online message type :    clean*/

	if (strstr(p, "</show></presence>") != NULL || strstr(p,
			"</priority></presence>") != NULL) {

		/*IOS
		 need to be handle the online friends list to upper  for ios
		 char* p_start = strstr(p,"<presence");
		 if(strstr(p,"<presence") != NULL)
		 handle_on_get_friends_status(p_start);
		 */

		//	inform_connect_status(TRUE);
		printf("clean online message OK!\n");

	}

	else if (strstr(p, "type=\"unavailable\"") != NULL) {
		/*IOS
		 need to be handle the online friends list to upper  for ios
		 char* p_start = strstr(p,"<presence");
		 if(strstr(p,"<presence") != NULL)
		 handle_on_get_friends_status(p_start);
		 */
		printf("clean offline message OK!\n");
	}
	/* handle on add  a new friends name*/
	else if (strstr(p, "type=\"subscribe\"") != NULL) {
		char* p_start = strstr(p, "<presence");
		if (p_start != NULL)
			handle_on_get_new_friends_name(p_start);
	}

	/*listen to the delete friend order and give a feedback msg to delete friends from list,  as about*/
	else if (strstr(p, "type=\"unsubscribe\"") != NULL) {
		char* p_start = strstr(p, "<presence");
		if (p_start != NULL)
			handle_take_delete_friends_list(p_start);
	}

	//the delete friends feedback message
	else if (strstr(p, "delay") != NULL || strstr(p, "type=\"unsubscribed\"")
			!= NULL) {
		printf("do clean delete friends or other things!\n");
	}

	/*  handle the error code that send by platform 2.20*/
	else if (strstr(p, "error") != NULL) {
		//printf("error : unknow error type ");
		show_log("error : unknow error type ", __func__, __LINE__);
	}

	/*  handle off line message left 2014-1-23*/
	else if (strstr(p, "offline") != NULL) {
		//printf("removed offline message, for it's unused ");
		show_log("removed offline message, for it's unused ", __func__,
				__LINE__);
	}

	else {
		printf("do clean done!");
	}
	return 1;
}

int handle_message_data(char* data) {
	char*p = data;
	if (strstr(p, "</message>") != NULL) {
		char*p_start = strstr(p, "<message");
		if (p_start != NULL)
			handle_on_chat_message(p_start);
	}

	else
	/*  handle off line message left 2014-1-23*/
	if (strstr(p, "offline") != NULL) {
		printf("removed offline message, for it's unused ");

	}
	/*  handle the error code that send by platform 2.20*/
	else if (strstr(p, "error") != NULL) {
		printf("error : unknow error type ");
	}

	return 1;

}

/*
 * handle server data ,a complete package
 */

void handle_server_data(char* data) {

	char *p = data;

	if (strstr(p, "iq-register") != NULL) {
		printf("init msg recv success!\n ");
		regDevInfoToServer();
	}

	else if (strstr(p, "<iq") != NULL) {
		handel_iq_data(p);
	}

	else if (strstr(p, "<presence") != NULL) {
		handle_presence_data(p);
	}

	else if (strstr(p, "<message") != NULL) {
		handle_message_data(p);
	}

	else if (strstr(p, "</stream:stream>") != NULL) {
		//printf("register failed! register ID is disabled");
		show_log("register not successful! register ID is disabled", __func__, __LINE__);
	}

	else if (strstr(p, "error") != NULL) {
		//printf("message stream error ,try again\n");
		show_log("message stream error ,try again", __func__, __LINE__);
	}

	else {
		//printf("Unknown message type\n");
		show_log("Unknown message type", __func__, __LINE__);
	}

}

void* server_thread() {

	printf("%s:%d\n", __func__, __LINE__);
	int tries = 0;
	int ret = 0;
	int opt = 1;
	int recv_len = 0;
	int left_len = 0;
	char* recv_buf = NULL;

	struct timeval tv;
	fd_set fd_read;
	gServConnected = -1;

	struct sockaddr_in serv_addr;

	printf("%s:%d\n", __func__, __LINE__);
	recv_buf = (char*) malloc(RECV_SERV_BUF_SIZE);
	memset(recv_buf, 0, RECV_SERV_BUF_SIZE);
	while (gIsInited) {
		printf("%s:%d\n", __func__, __LINE__);
		ret = parse_ip_addr_from_string(gServXmpp, gServXmppIP, &gServXmppPort);
		if (ret == 0)
			break;
		sleep(1);
	}

	printf("%s:%d\n", __func__, __LINE__);

	start: recv_len = 0;
	left_len = 0;
	deep = 0;//未知效果，xmpp问题，2014-8-14
	memset(recv_buf, 0, RECV_SERV_BUF_SIZE);

	printf("%s:%d\n", __func__, __LINE__);
	gServSock = socket(AF_INET, SOCK_STREAM, 0);
	if (gServSock < 0) {
		//printf("err:socket():%s:%d\n", __func__, __LINE__);
		show_log("err:socket()", __func__, __LINE__);
		goto start;
	}

	printf("%s:%d\n", __func__, __LINE__);

	if (ret == -1) {
		//printf("err:parse_ip_addr_from_string:%s:%d\n", __func__, __LINE__);
		show_log("err:parse_ip_addr_from_string", __func__, __LINE__);
	}

	setsockopt(gServSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	set_socket_timeout(gServSock, 30);/*30s*/
	set_socket_noblock(gServSock);/*set the socket to no-block type*/

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(gServXmppIP);
	serv_addr.sin_port = htons(gServXmppPort);

	retry: tries = 5;

	do {
		ret = connect(gServSock, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr));
		tries--;
		usleep(100000);
	} while (ret != 0 && tries > 0);

	if (ret == -1) {
		gServConnected = -1;
		sleep(1);
		goto retry;
	} else if (ret == 0) {
		gServConnected = 1;
		gServLiveTime = get_current_time();
	} else if (gIsInited == 0) {
		//printf("...connect err");


		show_log("...connect err", __func__, __LINE__);
		goto end;
	}

	if (InitInfoToServer() != 0) {
		//printf("err:InitInfoToServer():%s:%d\n", __func__, __LINE__);
		show_log("err:InitInfoToServer()", __func__, __LINE__);
	}

	FD_ZERO(&fd_read);
	FD_SET(gServSock,&fd_read);
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	while (gIsInited == 1) {
		FD_SET(gServSock,&fd_read);
		/*the argument is a must, or select() will error*/
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		ret = select(gServSock + 1, &fd_read, NULL, NULL, &tv);
		if (ret < 0) {
			show_log("err: select() error", __func__, __LINE__);
			perror("select");
			continue;
		} else if (ret > 0) { /*something can be read*/
			if (FD_ISSET(gServSock,&fd_read)) {

				recv_len = recv(gServSock, recv_buf + left_len,
						RECV_SERV_BUF_SIZE - left_len, 0);

				if (recv_len > 0) { /*something that can be handle*/
					printf("server packet p=######  %s  ######\n", recv_buf);

					/*char str[STR_LEN];
					 sprintf(str,"server packet p=######  %s  ######",recv_buf);
					 show_log(str, __func__, __LINE__);*/

					gServLiveTime = get_current_time();

					left_len = handlexmpp(recv_buf, left_len + recv_len); // unpackage


				}

				else if (recv_len == 0) {
					gServConnected = -1;
					//printf("info: recv_len = 0 ,network is cut off:%s:%d\n", __func__,__LINE__);
					show_log("info: recv_len = 0 ,network is cut off",
							__func__, __LINE__);
				}

				else {/*recv_len<0*/

					if (errno == EINTR || errno == EWOULDBLOCK || errno
							== EAGAIN) {
						//printf("info: recv() encounter with some problem:%s:%d\n",
						//									__func__, __LINE__);
						show_log("info: recv() encounter with some problem",
								__func__, __LINE__);
					} else {
						gServConnected = -1;
						//printf("info: network is cut off:%s:%d\n", __func__,
						//									__LINE__);
						show_log("info: network is cut off", __func__, __LINE__);
					}

				}
			}

		} else {/*select() time out*/
			//				printf("select timeout...\n");
		}

		if (gServConnected == -1) {/*network is been cut off, try to connect again*/
			close(gServSock);
			show_log("network is been cut off", __func__, __LINE__);
			sleep(10);/*wait for 30s because of the Server*/
			// connect_xmpp again;
			goto start;
		}

	}

	end: if (recv_buf != NULL)
		free(recv_buf);
	close(gServSock);
	gServConnected = 0;
	gServSock = -1;
	show_log("info: server_thread exit", __func__, __LINE__);

	return (void*) 0;
}

void connect_xmpp(char* strServXmpp, char* strUserName, char* strPassWord) {

	strcpy(gUserName, strUserName);
	strcpy(gPassWord, strPassWord);
	strcpy(gServXmpp, strServXmpp);

	pthread_attr_t a; //线程属性
	pthread_attr_init(&a); //初始化线程属性
	pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED); //设置线程属性

	if (pthread_create(&gServTid, &a, server_thread, 0) != 0) {
		show_log("err: xmpp_thread_start error!", __func__, __LINE__);
	} else {
		show_log("info:xmpp_thread_start success!", __func__, __LINE__);
	}

}

void show_log(const char* message, const char* func, int line) {
	printf("======================================================\n");
	printf("%s:%d\n", func, line);
	printf("%s\n", message);
	printf("======================================================\n");
}

int recv_chat_message(pchatbuffer chat_buffer) {
	printf("recv_chat_message:%s\n", chat_buffer->body);
	return 0;
}

int recv_friends_list_message(pfriendslistbuffer data, int len) {
	int i;
	for (i = 0; i < len; i++)
		printf("recv_friends_list_message:%s\n", data[i].jid);
	return 0;
}

int main() {
	connect_xmpp("202.98.157.54:5223", "xu123", "123");
	char str[100];
	while (gets(str)) {

		if (strcmp(str, "0") == 0) { //输入0：修改当前状态
			declare_online();
		}
		else if (strcmp(str, "1") == 0) { //输入0：修改当前状态
			send_chat_message("xu456","hello!");
		}
		else if (strcmp(str, "2") == 0) { //输入0：修改当前状态
			get_friends_list();
		} else if (strcmp(str, "3") == 0) { //输入0：修改当前状态
			add_friends("xu456");
		} else if (strcmp(str, "5") == 0) { //输入0：修改当前状态
			send_heartbeat();
		} else if (strcmp(str, "7") == 0) { //输入0：修改当前状态
			stop_xmpp();
			break;
		}
	}

	printf("释放内存中...\n");
	sleep(2);

	return 0;
}

