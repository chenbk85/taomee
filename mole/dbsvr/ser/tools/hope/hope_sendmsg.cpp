#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include "Cclientproto.h"

int main (int argc, char* argv[])
{
    char * comip=argv[1];
    uint16_t comport=atoi(argv[2]);
	uint32_t logdate=atoi(argv[3]);
	uint32_t logmonth=logdate%10000/100;
	uint32_t logday=logdate%10000%100;

	char msgbuf[2000];
    int ret;
    HOPE_GET_HOPE_BY_DATE_OUT_ITEM * p_list, * p_item;
	//userid_t * p_list;
    LIST_COUNT out_header;

    Cclientproto * cp=new  Cclientproto(comip,comport );

	int index=0;
	while (true){
		ret=cp->f_HOPE_GET_HOPE_LIST_BY_DATE(logdate,index, &out_header,&p_list );
		//
		if (ret==SUCC && out_header.count>0 ){
			uint32_t i;
			for (i=0;i<out_header.count;i++){
				p_item=p_list+i;
				if (p_item->recv_id==p_item->send_id){//同一个人
					sprintf (msgbuf , "恭喜你，你在%u月%u日许下的愿望（送给自己 %.30s）实现啦^_^赶快去爱心教堂开启愿望，记得点击神龟许愿池里的石像开启愿望哦！！！如果在一周内未开启，愿望将被自动放弃。",
						 logmonth, logday, p_item->recv_type_name) ;
					ret=cp->user_add_offline_msg(p_item->recv_id, msgbuf );	
				}else{
					//得到接收者nick
					USER_GET_NICK_EMAIL_OUT recv_info;
					ret=cp->f_USER_GET_NICK_EMAIL(p_item->recv_id,&recv_info);
					if (ret==SUCC){
						//发送给许愿者
						sprintf (msgbuf , "恭喜你，你在%u月%u日许下的愿望（送给%.16s: %.30s）实现啦^_^赶快通知他开启愿望去！！！如果在一周内未开启，愿望将被自动放弃。",
						 logmonth, logday,recv_info.nick ,p_item->recv_type_name) ;
						ret=cp->user_add_offline_msg(p_item->send_id, msgbuf );	
						//发送能接收者
						sprintf (msgbuf , "恭喜你，%.16s 在%u月%u日许下的愿望（送给你 %.30s）实现啦^_^赶快去爱心教堂开启愿望，记得点击神龟许愿池里的石像开启愿望哦！！！如果在一周内未开启，愿望将被自动放弃。",
						 p_item->send_nick, logmonth, logday,p_item->recv_type_name) ;
						ret=cp->user_add_offline_msg(p_item->recv_id, msgbuf );
					}
				}	
			}
			free (p_list);
		}else{
			break;
		}

 		index++;
	}


	delete cp;
	return ret;
}
