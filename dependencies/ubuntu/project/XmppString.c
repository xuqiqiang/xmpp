#include "XmppString.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern char server_name[STR_SIZE];

long rand_seed = 0;

status getJidFromSring(char* jid, const char* str){
	if (jid == NULL){
		return ER;
	}


	int i;

	for (i = 0; str[i] != '\0'; i++){
		if (str[i] == '/')
			break;
	}

	strncpy(jid, str, i);
	jid[i] = '\0';

	return OK;
}


status getNameFromJid(char* name, const char* jid){
	if (name == NULL){
		return ER;
	}


	int i;

	for (i = 0; jid[i] != '\0'; i++){
		if (jid[i] == '@')
			break;
	}

	strncpy(name, jid, i);
	name[i] = '\0';

	return OK;
}


status getJidFromName(char* jid, const char* name){
	if (jid == NULL){
		return ER;
	}

	strcpy(jid, name);
	strcat(jid, "@");
	strcat(jid, server_name);

	return OK;
}


status str_cat(char* des, const char* res)
{
	if (strlen(res) > STR_SIZE - 1 - strlen(des))
		LOGD("Exceeds the maximum length!\n");
	strncat(des, res, STR_SIZE - 1 - strlen(des));
	return 1;
}

BOOL str_equal(const char* des, const char* res){
	if (des == NULL || res == NULL){
		return false;
	}

	if (!strcmp(des, res))
		return true;
	else
		return false;
}



status create_random_num(char* random)
{
	if (random == NULL){
		return ER;
	}
	int j;

	long seed = rand_seed;

	for (j = 0; j < N; j++)
	{
		random[j] = 'a' + seed % 10;
		seed /= 10;
	}

	rand_seed++;
	random[j] = '\0';

	return OK;
}


const char* getSafeString(const char *str){
	if (str == NULL){
		return "123";
	}
	return str;
}