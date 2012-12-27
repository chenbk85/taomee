/*
 * =====================================================================================
 *
 *       Filename:  Csend_email.cgf
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
//#include "common.h"
#include <libtaomee++/utils/tcpip.h>
#include "proto.h"
#include "benchapi.h"
#include "proto_header.h"
#include "db_error_base.h"

Csend_email::Csend_email(Cclientproto *cp  )
{ 
	this->cp=cp;
	this->switch_cp=new Cclientproto(config_get_strval("SWITCH_IP"),
					config_get_intval("SWITCH_PORT",0 ) );

	memset(this->time_stamp_ip,0,sizeof(this->time_stamp_ip));
	
	char * time_stamp_ip_v=config_get_strval("TIME_STAMP_IP");
	//safe_copy_string (this->time_stamp_ip ,time_stamp_ip_v );
    if (time_stamp_ip_v != NULL)
	    memcpy (this->time_stamp_ip, time_stamp_ip_v, sizeof(time_stamp_ip_v)+1);
	this->time_stamp_port=config_get_intval("TIME_STAMP_PORT",0 ) ;
	DEBUG_LOG("TIME_STAMP:IP/PORT==[%s:%d]",this->time_stamp_ip,this->time_stamp_port);
	this->time_stamp_sockid=-1;			
}

int Csend_email::send_time_stamp( userid_t userid )
{
	if (this->time_stamp_sockid==-1){
		this->time_stamp_sockid=open_socket_udp(this->time_stamp_ip,this->time_stamp_port,
				&(this->time_stamp_addr));
	}
 	PROTO_HEADER   t;
	t.cmd_id=7;
	t.proto_length=22;
	t.proto_id=0;
	t.id=userid;
    t.role_tm=0;
	t.result=0;
	sendto(this->time_stamp_sockid,&t,sizeof(t),0, 
		(struct sockaddr *)&(this->time_stamp_addr),sizeof(this->time_stamp_addr ));
	return 0;
}

int Csend_email::gf_sync_vip( userid_t userid,uint32_t vipflag,uint32_t vip_month_count,
		uint32_t auto_incr,uint32_t end_time,uint32_t start_time,uint8_t vip_type)
{
	//this->send_time_stamp(userid);
    DEBUG_LOG("sync vip: uid=[%u] flag=[%u] month=[%u] auto_incr=[%u] vip_type=[%u]", 
        userid, vipflag, vip_month_count, auto_incr, vip_type);
	int ret = this->switch_cp->switch_gf_sync_vip(userid,vipflag,vip_month_count,
        auto_incr,end_time,start_time,vip_type);
    if (ret != SUCC) {
        return ret;
    }

	if ( vipflag==0 ) { //清除vip操作
	    //uint32_t teamid=0;
		//this->cp->gf_get_teamid(userid,&teamid);
		//this->cp->set_pk_team_member_super_nono_contribute(userid,teamid);
	}
    /*
	if(vipflag==1){
    	//以下设置VIP时，才发数据...
		char buf[4096];
		char msg[4096];
		gf_msg_add_item_in_header *e_in;
		e_in = (gf_msg_add_item_in_header *) buf ;	

		gf_get_nick_friend_list_out u_info;
		int ret= cp->gf_get_nick_friend_list(userid,&u_info);
		if (ret!=SUCC) return ret;

		if (u_info.friendlist.count>200) return  PROTO_LEN_ERR;
		e_in->msg_time= time(NULL);
		e_in->senderid=7;
		e_in->tmpltid=10007;
		strcpy( e_in->sendernick,"肖恩");
		sprintf(msg,"亲爱的小赛尔，您的好友%.16s(%u)已经开通了超能NoNo！",
		u_info.nick, userid );
		e_in->msglen=strlen(msg);
		memcpy(buf+sizeof(*e_in),msg, e_in->msglen);
       //给好友发送消息
		for(uint32_t i=0;i<u_info.friendlist.count;i++){
			cp->gf_msg_add_item(u_info.friendlist.item[i],e_in);		
		}

	}
    */
/*  if (vipflag != 0) {
		//给自己发送离线信息
		if ((month == 3) || (month == 6) || (month == 12) || (month == 24)) {
			int r=switch_cp->f_switch_add_user_msg(userid, "小摩尔陪超级拉姆度过了许多美好时光，正是因为你的细心呵护，你的超级拉姆升级啦！快快去爱心教堂领取大礼包吧!");
			DEBUG_LOG("===== ret:r:%d",r);
		}
	}*/

    return SUCC;
}
int Csend_email::gf_sync_base_svalue(userid_t userid,uint32_t base_svalue,uint16_t chn)
{
    DEBUG_LOG("sync x value: uid=[%u] base_svalue=[%u] chn_id=[%u]",userid,base_svalue,chn);
	this->switch_cp->switch_gf_base_svalue(userid,base_svalue,chn);
	return SUCC;
}
int Csend_email::gf_sync_set_gold(userid_t userid,uint32_t gold)
{
    this->switch_cp->switch_gf_set_gold(userid,gold);
	return SUCC;

}
int Csend_email::gf_vip_sys_msg(userid_t userid,uint32_t len, char* msg)
{
    this->switch_cp->switch_gf_vip_sys_msg(userid, len, msg);
    return SUCC;
}

