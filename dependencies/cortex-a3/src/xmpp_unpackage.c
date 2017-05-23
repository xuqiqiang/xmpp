#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "xmpp.h"



int deep=0;

int find_XMPP_START(char* xmpp){

	if(*xmpp=='<'&&*(xmpp+1)!='/')
		return 1;
	else return 0;
}
int find_XMPP_END_A(char* xmpp){

	if(*xmpp=='<'&&*(xmpp+1)=='/')
	{

	
//printf("%d %s xmpp %s \n",__LINE__,__FUNCTION__,xmpp);

		return 1;
		}
	else return 0;
}
int find_XMPP_END_B(char* xmpp){

	if(*xmpp=='/'&&*(xmpp+1)=='>')
	{
		//	printf("%d %s xmpp %s \n",__LINE__,__FUNCTION__,xmpp);
		return 1;
		
	}
	else return 0;
}
int find_XMPP_CONNECT(char* xmpp){

	if(*xmpp=='<'&&*(xmpp+1)=='?')
	{
		//	printf("%d %s xmpp %s \n",__LINE__,__FUNCTION__,xmpp);
		return 1;

	}
	else{
		return 0;
	}
}



int  checkpackagelen(   char* xmpp,int len){

if(*xmpp=='/'&&*(xmpp+1)=='>')
{
xmpp++;


return len+1;
}

if(*xmpp=='<'&&*(xmpp+1)=='/')
{

 int index=0;
  while(*xmpp!='>')
  {index++;
	  xmpp++;
  }

	//len=+index;
	return len+index;
}

return len;

}
int handlexmpp(char* xmpp,int xmpplen){
	//printf("%s %d \n",__func__,__LINE__);
	int start=0;
	int packagelen=0;
	char* startbuffer=0;
	char* oldxmpp=xmpp;
	while(xmpp!=&oldxmpp[xmpplen-1])
	{
		//len--;
		packagelen++;
		
		
		if(find_XMPP_CONNECT(xmpp)>0){

			char* streampt=strstr(xmpp,"<stream:features>");

			if(streampt==0)
				return xmpplen;

                     start=0;
					 startbuffer=0;
					 packagelen=0;
               xmpp=streampt;


		}


			if(find_XMPP_START(xmpp)>0){
			if(deep==0&&start==0)
			{packagelen=1;
			start=1;
			startbuffer=xmpp;
			
			}
			//	printf("%d %s deep %d \n",__LINE__,__FUNCTION__,deep);
			deep+=1;
			}
			else if(find_XMPP_END_A(xmpp)>0||find_XMPP_END_B(xmpp)>0){

				deep-=1;
				//printf("%d %s deep %d \n",__LINE__,__FUNCTION__,deep);
				if(start==1&&deep==0){
                      packagelen= checkpackagelen(xmpp,packagelen);
					          
                       start=0;
					char* single=(char*)malloc(packagelen+1);
					memset(single,0,packagelen+1);
					memcpy(single,startbuffer,packagelen);
					printf("single %s \n",single);

					handle_server_data(single);
					//printf("handle_server_data end  \n");

					free(single);
					startbuffer=0;
					packagelen=0;

				}
				

			}
		

		
		xmpp++;
	}
	//printf("%s %d \n",__func__,__LINE__);
	int dataunhandlelen=0;
	if(startbuffer!=0){
	 dataunhandlelen=&oldxmpp[xmpplen-1]-startbuffer+1;
	//printf("%s %d dataunhandlelen %d\n",__func__,__LINE__,dataunhandlelen);
	memcpy(oldxmpp,startbuffer,dataunhandlelen);

	//printf("%s %d \n",__func__,__LINE__);
printf("startbuffer %s len %d \n",oldxmpp,dataunhandlelen);
	}
	memset(oldxmpp+dataunhandlelen,0,xmpplen-dataunhandlelen);


return dataunhandlelen;





}



