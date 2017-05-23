#include "Friend_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xmpp.h"

extern void(*sendBroadcast)(int name_id, const char* data);

void init_call_back(void(*call_back)(int name_id, const char* data)){
    sendBroadcast=call_back;
}