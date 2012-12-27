/*
 * =====================================================================================
 *
 *       Filename:  Csend_email.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Csend_email.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Csend_email::Csend_email(Cclientproto *cp  )
{ 
	this->cp=cp;
	this->switch_cp=new Cclientproto(config_get_strval("SWITCH_IP"),
					config_get_intval("SWITCH_PORT",0 ) );

	memset(this->time_stamp_ip,0,sizeof(this->time_stamp_ip));
	
	char * time_stamp_ip_v=config_get_strval("TIME_STAMP_IP");
	safe_copy_string (this->time_stamp_ip ,time_stamp_ip_v );
	DEBUG_LOG("stampip==%s",this->time_stamp_ip);
	this->time_stamp_port=config_get_intval("TIME_STAMP_PORT",0 ) ;
	this->time_stamp_sockid=-1;			
}

int Csend_email::send_time_stamp( userid_t userid )
{
	if (this->time_stamp_sockid==-1){
		this->time_stamp_sockid=open_socket_udp(this->time_stamp_ip,this->time_stamp_port,
				&(this->time_stamp_addr));
	}
	struct  time_stamp_proto_stru t;
	t.len=htonl(17);
	t.version=htonl(1);
	t.cmdid=htonl(7);
	t.userid=htonl(userid);
	t.result=0;
	sendto(this->time_stamp_sockid,&t,sizeof(t),0, 
		(struct sockaddr *)&(this->time_stamp_addr),sizeof(this->time_stamp_addr ));
	return 0;
}

int Csend_email::send_email( userid_t userid,uint32_t vipflag, uint32_t month)
{
	this->send_time_stamp(userid);
	this->switch_cp->f_other_sync_vip(userid,vipflag);
	if (vipflag == 0) {
		return SUCC;
	}
	//以下设置VIP时，才发数据...
	if ((vipflag == 1) || (vipflag == 2)) {
		char buf[4096];
		char msg[4096];
		emailsys_add_email_in_header * e_in;
		e_in = (emailsys_add_email_in_header *) buf ;	

		userinfo_get_nick_flist_out u_info;
		int ret= cp->f_userinfo_get_nick_flist(userid,&u_info);
		if (ret!=SUCC) return ret;

		if (u_info.fiend_list.count>200) return  PROTO_LEN_ERR;
		e_in->sendtime= time(NULL);
		e_in->senderid=0;
		e_in->type=1000028;
		strcpy( e_in->sendernick,"克劳神父");
		sprintf(msg,"亲爱的小摩尔，你的好友%.16s(%u)已经拥有了庄园的守护精灵“超级拉姆”，以后遇到困难可以找他帮忙哦！",
		u_info.nick, userid );
		e_in->msglen=strlen(msg);
		memcpy(buf+sizeof(*e_in),msg, e_in->msglen);
		DEBUG_LOG("self receive");
		int r=switch_cp->f_switch_add_user_msg(userid, "　你的拉姆现在可以成为超级拉姆了。快带上一只前往爱心教堂克劳那里，他会帮你实现拥有超级拉姆的愿望。拥有了超级拉姆，你就可以带着它去庄园中寻找各种神奇道具了。");
		DEBUG_LOG("ret:r:%d",r);

		//给好友发送邮件消息
		for(uint32_t i=0;i<u_info.fiend_list.count;i++){
			cp->f_emailsys_add_email(u_info.fiend_list.item[i],e_in);		
		}
	}
	//if ((vipflag != 0) && (vipflag != 6)) {
		////给自己发送离线信息
		//if ((month == 3) || (month == 6) || (month == 12) || (month == 24)) {
			//int r=switch_cp->f_switch_add_user_msg(userid, "小摩尔陪超级拉姆度过了许多美好时光，正是因为你的细心呵护，你的超级拉姆升级啦！快快去城堡露天花园领取大礼包吧!");
			//DEBUG_LOG("===== ret:r:%d",r);
		//}
	//}
	return SUCC;
}
int Csend_email::pp_sync_vip( userid_t userid,uint32_t vipflag)
{
	this->send_time_stamp(userid);
	this->switch_cp->switch_pp_sync_vip(userid,vipflag);
	if ( vipflag!=1 ) {
			
	}else{

	}

	return SUCC;
}
