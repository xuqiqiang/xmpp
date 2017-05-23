#include "xmpp.h"

#define IPP_SUCCESS 0;
#define IPP_ERROR   -1;



long get_current_time()
{
	struct timeval time;

	gettimeofday(&time,NULL);

	return time.tv_sec;
}

int set_socket_noblock(int fd)
{
	int flags;

	if(fd < 0){
		return IPP_ERROR;
	}

	flags = fcntl(fd, F_GETFL, 0);
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);/*return -1 when error*/
}


int set_socket_timeout(int fd,int time)
{
	int32_t					ret = -1;
	struct timeval 			timeout;

	if(fd<0 || time < 0){
		return IPP_ERROR;
	}

	timeout.tv_sec = time;
	timeout.tv_usec =0;

	ret = setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
	if(ret < 0){

		printf("err: setsockopt() recv SO_RCVTIMEO timeout %d\n",errno);
		return IPP_ERROR;
	}

	ret = setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
	if(ret < 0){
		printf("err: setsockopt() send SO_SNDTIMEO timeout\n");
		return IPP_ERROR;
	}

	return IPP_SUCCESS;
}


int socket_send(const int s, const char *buf, const int len,const int flag)
{
	int send_byte = 0;
	int send_count = 0;
	if(s<0 || buf==NULL || len<=0)
		return -1;


	while(send_count < len){
		send_byte = send(s,buf+send_count,len-send_count,0);
		if(send_byte > 0){
			send_count += send_byte;
		}
		if(send_byte == -1){
			if(errno==EAGAIN || errno==EINTR){
				usleep(10);
				continue;
			}else{
//				LOGE("err:socket_send(): %s:%d,errno=%d\n",__func__,__LINE__,errno);
				printf("err:socket_send(): %s:%d,errno=%d\n");
				return -1;
			}
		}
	}

	return 0;
}









