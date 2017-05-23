#ifndef __Friend_list_H__
#define __Friend_list_H__

#define SUM_MAX 1000
#define LENGTH 100

typedef struct {
	//char ** list;
	char list[SUM_MAX][LENGTH];
	int friend_sum;
} Friend_list;



void init_friend_list(Friend_list *friend_list);

void add_friend(Friend_list *friend_list, const char *friend_jid);

void clear_friend_list(Friend_list *friend_list);


//void free_friend_list(Friend_list *friend_list);


#endif