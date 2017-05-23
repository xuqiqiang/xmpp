#include "Jni_call.h"
#include "XmppString.h"
#include "Xmpp.h"
#include <stdio.h>


JavaVM* g_jvm = 0;
jclass TestProvider;

extern void(*sendBroadcast)(int name_id, const char* data);

//extern void(*on_get_register_result)(int result);

extern int thread_user_start;

void initEnv(JNIEnv* env){
	(*env)->GetJavaVM(env, &g_jvm);

	//TestProvider = (*env)->FindClass(env, "com/snailstdio/MyLib/net/Xmpp");//com/xuqiqiang/xmpp/Xmpp");
	jclass tmp = (*env)->FindClass(env, XMPP_JAVA_PATH);
	TestProvider = (jclass)(*env)->NewGlobalRef(env, tmp);
}


/*void on_get_register_result_func(int result){
	if (result == XMPP_REGISTER_SUCCESSFUL){
		sendBroadcast(ON_XMPP_REGISTER_SUCCESSFUL_ID, NULL);
	}
	else if (result == XMPP_REGISTER_NOT_SUCCESSFUL){
		sendBroadcast(ON_XMPP_REGISTER_NOT_SUCCESSFUL_ID, NULL);
	}
	else if (result == XMPP_REGISTER_ALREADY_EXIST){
		sendBroadcast(ON_XMPP_REGISTER_ALREADY_EXIST_ID, NULL);
	}
	else if (result == XMPP_REGISTER_NETWORK_CUT_DOWN){
		sendBroadcast(ON_XMPP_REGISTER_NETWORK_CUT_DOWN_ID, NULL);
	}
}*/

void mySendBroadcast(int name_id, const char* data){

	//LOGD("%s %s:%d\n", data, __func__, __LINE__);

	JNIEnv *env_now = NULL;
	if (g_jvm == NULL) {
		return;
	}
	if ((*g_jvm)->AttachCurrentThread(g_jvm, &env_now, 0) != JNI_OK) {
		return;
	}
	if (env_now == NULL) {
		LOGD("env_now NULL");
		return;
	}
	if (TestProvider == NULL) {

		LOGD("TestProvider NULL");
		return;
	}
	jmethodID fun = (*env_now)->GetStaticMethodID(env_now, TestProvider, "sendBroadcast", "(ILjava/lang/String;)Ljava/lang/String;");// (Ljava / lang / String;)Ljava / lang / String; ");
	(*env_now)->CallStaticObjectMethod(env_now, TestProvider, fun, name_id, (*env_now)->NewStringUTF(env_now, (char*)data));
	(*g_jvm)->DetachCurrentThread(g_jvm);
}

void Java_com_xuqiqiang_xmpp_Xmpp_initXmpp(JNIEnv* env, jobject thiz){
	xmpp_init();

	initEnv(env);
	sendBroadcast = mySendBroadcast;
}



void Java_com_xuqiqiang_xmpp_Xmpp_register(JNIEnv* env, jobject thiz, jstring strUserName, jstring strPassWord, jstring strServerIP, jint serverAltport)
{

	const char *c_UserName = (*env)->GetStringUTFChars(env, strUserName, false);
	const char *c_PassWord = (*env)->GetStringUTFChars(env, strPassWord, false);
	const char *c_ServXmpp = (*env)->GetStringUTFChars(env, strServerIP, false);


	regDevInfoToServer(c_UserName, c_PassWord, c_ServXmpp, serverAltport);

	LOGD("c_UserName:%s", c_UserName);
	LOGD("c_ServXmpp:%s", c_ServXmpp);

	(*env)->ReleaseStringUTFChars(env, strUserName, c_UserName);
	(*env)->ReleaseStringUTFChars(env, strPassWord, c_PassWord);
	(*env)->ReleaseStringUTFChars(env, strServerIP, c_ServXmpp);
}


void Java_com_xuqiqiang_xmpp_Xmpp_connectXmpp(JNIEnv* env, jobject thiz, jstring strUserName, jstring strPassWord, jstring strServerName, jstring strServerIP, jint serverAltport)
{

	//(*env)->GetJavaVM(env, &g_jvm);

	//TestProvider = (*env)->FindClass(env, "com/xuqiqiang/xmpp/Xmpp");


	const char *c_UserName = (*env)->GetStringUTFChars(env, strUserName, false);
	const char *c_PassWord = (*env)->GetStringUTFChars(env, strPassWord, false);
	const char *c_ServName = (*env)->GetStringUTFChars(env, strServerName, false);
	const char *c_ServXmpp = (*env)->GetStringUTFChars(env, strServerIP, false);

	LOGD("%s:%d\n", __func__, __LINE__);
	connect_xmpp(c_UserName, c_PassWord, c_ServName, c_ServXmpp, serverAltport);
	LOGD("%s:%d\n", __func__, __LINE__);

	(*env)->ReleaseStringUTFChars(env, strUserName, c_UserName);
	(*env)->ReleaseStringUTFChars(env, strPassWord, c_PassWord);
	(*env)->ReleaseStringUTFChars(env, strServerName, c_ServName);
	(*env)->ReleaseStringUTFChars(env, strServerIP, c_ServXmpp);
}

jstring Java_com_xuqiqiang_xmpp_Xmpp_sendChatMessage(JNIEnv* env, jobject thiz, jstring chat_object, jstring content)
{

	if (!isConnect()){
		LOGD("disconnect!\n");
		return (*env)->NewStringUTF(env, (char*)"");
	}

	const char *c_chat_object = (*env)->GetStringUTFChars(env, chat_object, false);
	const char *c_content = (*env)->GetStringUTFChars(env, content, false);


	char random_id[N + 1] = { 0 };

	create_random_num(random_id);

	if (random_id == NULL) {
		LOGD("create random id error!");
		(*env)->ReleaseStringUTFChars(env, chat_object, c_chat_object);
		(*env)->ReleaseStringUTFChars(env, content, c_content);
		return (*env)->NewStringUTF(env, (char*)"");
	}

	xmpp_send_chat_message(c_chat_object, c_content, random_id);

	(*env)->ReleaseStringUTFChars(env, chat_object, c_chat_object);
	(*env)->ReleaseStringUTFChars(env, content, c_content);

	return (*env)->NewStringUTF(env, (char*)random_id);

}

void Java_com_xuqiqiang_xmpp_Xmpp_setStatus(JNIEnv* env, jobject thiz, jstring status)
{
	const char *c_status = (*env)->GetStringUTFChars(env, status, false);
	xmpp_set_status(c_status);

	LOGD("setStatus:%s", c_status);

	(*env)->ReleaseStringUTFChars(env, status, c_status);
}

void Java_com_xuqiqiang_xmpp_Xmpp_getFriendsList(JNIEnv* env, jobject thiz)
{
	xmpp_get_friends_list();
}

void Java_com_xuqiqiang_xmpp_Xmpp_addFriend(JNIEnv* env, jobject thiz, jstring friend_jid)
{
	LOGD("%s:%d\n", __func__, __LINE__);
	
	const char *c_friend_jid = (*env)->GetStringUTFChars(env, friend_jid, false);
LOGD("c_friend_jid:%s\n", c_friend_jid);
	xmpp_add_friends(c_friend_jid, XMPP_ADD_FRIEND_INTENT_REQUEST);

	(*env)->ReleaseStringUTFChars(env, friend_jid, c_friend_jid);
}

void Java_com_xuqiqiang_xmpp_Xmpp_addFriendRespond(JNIEnv* env, jobject thiz, jstring friend_jid, jint Respond)
{
	const char *c_friend_jid = (*env)->GetStringUTFChars(env, friend_jid, false);

	if (Respond == 0){
		xmpp_add_friends_respond(c_friend_jid, XMPP_ADD_FRIEND_RESPOND_AGREE);
		xmpp_add_friends(c_friend_jid, XMPP_ADD_FRIEND_INTENT_AGREE);
	}
	else{
		xmpp_add_friends_respond(c_friend_jid, XMPP_ADD_FRIEND_RESPOND_DISAGREE);
	}

	(*env)->ReleaseStringUTFChars(env, friend_jid, c_friend_jid);
}

void Java_com_xuqiqiang_xmpp_Xmpp_deleteFriend(JNIEnv* env, jobject thiz, jstring friend_name)
{
	const char *c_friend_name = (*env)->GetStringUTFChars(env, friend_name, false);
	LOGD("c_friend_name:%s\n", c_friend_name);
	xmpp_delete_friends(c_friend_name);

	(*env)->ReleaseStringUTFChars(env, friend_name, c_friend_name);
}


void Java_com_xuqiqiang_xmpp_Xmpp_stopXmpp(JNIEnv* env, jobject thiz)
{
	stop_xmpp();
}

void Java_com_xuqiqiang_xmpp_Xmpp_releaseXmpp(JNIEnv* env, jobject thiz){
	xmpp_release();
}



jstring Java_com_xuqiqiang_xmpp_Xmpp_sendPicture(JNIEnv* env, jobject thiz, jstring chat_object, jstring name, jstring content)
{

	if (!isConnect()){
		LOGD("disconnect!\n");
		return (*env)->NewStringUTF(env, (char*)"");
	}

	const char *c_chat_object = (*env)->GetStringUTFChars(env, chat_object, false);
	const char *c_name = (*env)->GetStringUTFChars(env, name, false);
	const char *c_content = (*env)->GetStringUTFChars(env, content, false);


	char random_id[N + 1] = { 0 };

	create_random_num(random_id);

	if (random_id == NULL) {
		LOGD("create random id error!");
		(*env)->ReleaseStringUTFChars(env, chat_object, c_chat_object);
		(*env)->ReleaseStringUTFChars(env, name, c_name);
		(*env)->ReleaseStringUTFChars(env, content, c_content);
		return (*env)->NewStringUTF(env, (char*)"");
	}

	xmpp_send_picture(c_chat_object, c_name, c_content, random_id);

	(*env)->ReleaseStringUTFChars(env, chat_object, c_chat_object);
	(*env)->ReleaseStringUTFChars(env, name, c_name);
	(*env)->ReleaseStringUTFChars(env, content, c_content);

	return (*env)->NewStringUTF(env, (char*)random_id);
}



void Java_com_xuqiqiang_xmpp_Xmpp_getVCard(JNIEnv* env, jobject thiz, jstring username)
{
	const char *c_username = NULL;
	if (username != NULL){
		LOGD("photo is NULL\n");
		c_username = (*env)->GetStringUTFChars(env, username, false);
	}
	else{
		LOGD("photo is not NULL\n");
	}

	xmpp_get_vCard(c_username);

	if (username != NULL){
		(*env)->ReleaseStringUTFChars(env, username, c_username);
	}
}


void Java_com_xuqiqiang_xmpp_Xmpp_setVCard(JNIEnv* env, jobject thiz, jstring nick_name, jstring photo)
{
	const char *c_nick_name = (*env)->GetStringUTFChars(env, nick_name, false);
	const char *c_photo = NULL;
	if (photo != NULL){
		LOGD("photo is NULL\n");
		c_photo = (*env)->GetStringUTFChars(env, photo, false);
	}
	else{
		LOGD("photo is not NULL\n");
	}

	LOGD("c_nick_name:%s\n", c_nick_name);
	xmpp_set_vCard(c_nick_name, c_photo);

	(*env)->ReleaseStringUTFChars(env, nick_name, c_nick_name);
	if (c_photo != NULL){
		(*env)->ReleaseStringUTFChars(env, photo, c_photo);
	}
}


void Java_com_xuqiqiang_xmpp_Xmpp_requestGame(JNIEnv* env, jobject thiz, jstring friend_name)
{
	const char *c_friend_name = (*env)->GetStringUTFChars(env, friend_name, false);
	xmpp_request_game(c_friend_name);
	(*env)->ReleaseStringUTFChars(env, friend_name, c_friend_name);
}


void Java_com_xuqiqiang_xmpp_Xmpp_acceptGame(JNIEnv* env, jobject thiz, jstring friend_jid)
{
	const char *c_friend_jid = (*env)->GetStringUTFChars(env, friend_jid, false);
	xmpp_accept_game(c_friend_jid);
	(*env)->ReleaseStringUTFChars(env, friend_jid, c_friend_jid);
}

void Java_com_xuqiqiang_xmpp_Xmpp_sendGameMessage(JNIEnv* env, jobject thiz, jstring friend_jid, jstring content)
{
	const char *c_friend_jid = (*env)->GetStringUTFChars(env, friend_jid, false);
	const char *c_content = (*env)->GetStringUTFChars(env, content, false);
	xmpp_send_game_message(c_friend_jid, c_content);

	(*env)->ReleaseStringUTFChars(env, friend_jid, c_friend_jid);
	(*env)->ReleaseStringUTFChars(env, content, c_content);
}


