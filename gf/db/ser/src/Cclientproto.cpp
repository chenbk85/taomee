/*
 * =====================================================================================
 *
 *       Filename:  Cclientproto.cgf
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年01月10日 18时22分43秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

/*
 *--------------------------------------------------------------------------------------
 *       Class:  Cclientproto
 *      Method:  Cclientproto
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */
#include "Cclientproto.h"
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include "benchapi.h"
//#include  "tcpip.h"
//#include  "common.h"
//#include "logproto.h"
#include <libtaomee++/utils/tcpip.h>
#include <libtaomee++/inet/pdumanip.hpp>
#include <netinet/in.h>
extern void hex_printf(char * buf,int len);

#define  SEND_DATA_AND_CHECK_SUCC()\
	if((ret=this->net_send())!=SUCC){ return ret; }\
	result=((PROTO_HEADER * )recvbuf)->result;\
	if (result==SUCC )	{

#define  SEND_DATA_AND_CHECK_SUCC_EX(resultvalue )\
	if((ret=this->net_send())!=SUCC){ return ret; }\
	result=((PROTO_HEADER * )recvbuf)->result;\
	if (result==SUCC || result==resultvalue){



#define  RETURN()   \
	}\
	free ( recvbuf);\
	return result;


Cclientproto::Cclientproto (const char * aipaddr, u_short aport)  /* constructor */
{
	this->set_ip_port(aipaddr,aport );
}  /* -----  end of method Cclientproto::Cclientproto  (constructor)  ----- */
void Cclientproto:: set_ip_port (const char * aipaddr, u_short aport)
{
	//safe_copy_string(this->ipaddr,aipaddr );
    if (aipaddr != NULL) {
	    strcpy(this->ipaddr,aipaddr);
    }
	this->port=aport;
    DEBUG_LOG("SWITCH:IP/PORT==[%s:%u]",this->ipaddr, this->port);
	this->sockfd=-1;
	//this->sockfd=open_socket (ipaddr, port);
}

void Cclientproto::set_pub_buf(char *buf,uint16_t cmd_id,int id,short private_size)
{
    switch_header* sh = (switch_header*)buf;
    sh->proto_length = sizeof(switch_header) + private_size;
    DEBUG_LOG ("client send[%d][%u] len=[%u]",cmd_id,id,sh->proto_length);
    sh->cmd_id = cmd_id;
	sh->proto_id=0x12345678;
	sh->id=id;
	sh->result=0;
}
int	Cclientproto::net_send_no_return()
{
	ret= net_io_no_return(sockfd, sendbuf, *((uint32_t*)sendbuf) );	
	int net_errno=errno;
	if ( ret==NET_ERR )
	{ 	//reset socket and reconnect
		//always close it
		DEBUG_LOG ("jim : close socket fd[%d] net_errno:[%d][%s]",sockfd,net_errno,strerror(net_errno));	
		close(sockfd);
		sockfd=-1;
		//是连接中断，不是超时则重新发送
        sockfd = open_socket (ipaddr, port);
		DEBUG_LOG ("jim :re connect [%s][%u]",ipaddr,port );	
    	if (sockfd == -1){
			DEBUG_LOG ("jim :re connect FAIL");	
           return NET_ERR;		
		}
		DEBUG_LOG ("jim :re connect SUCC");	
		ret=net_io_no_return(sockfd, sendbuf,*((uint32_t*)sendbuf));	
    	if(ret==NET_ERR){
			DEBUG_LOG ("jim :re send data err [%d]",errno );	
			close(sockfd);
			sockfd=-1;

           return NET_ERR;		
		}
		return ret;
	}else{
		DEBUG_LOG("发送成功\n");
		return ret;
	}
}




int	Cclientproto::net_send()
{
	ret= net_io(sockfd, sendbuf, *((uint32_t*)sendbuf),  &recvbuf, &rcvlen );	
	//int net_errno=errno;
	if ( ret==NET_ERR )
	{ 	//reset socket and reconnect
		//always close it
	//	DEBUG_LOG ("jim : close socket fd[%d] net_errno:[%d][%s]",net_errno,strerror(net_errno));	
		close(sockfd);
		//是连接中断，不是超时则重新发送
        sockfd = open_socket (ipaddr, port);
	//	DEBUG_LOG ("jim :re connect [%s][%u]",ipaddr,port );	
    	if (sockfd == -1){
	//		DEBUG_LOG ("jim :re connect FAIL");	
           return NET_ERR;		
		}
	//	DEBUG_LOG ("jim :re connect SUCC");	
		ret= net_io(sockfd, sendbuf,*((uint32_t*)sendbuf), &recvbuf, &rcvlen );	
    	if(ret==NET_ERR){
	//		DEBUG_LOG ("jim :re send data err [%d]",errno );	
		   close(sockfd);
		   sockfd=-1;
           return NET_ERR;		
		}
		return ret;
	}else{
		return ret;
	}
}

int Cclientproto::switch_gf_sync_vip(userid_t userid,uint32_t vipflag,uint32_t month_cnt, 
    uint32_t auto_incr, uint32_t end_time,uint32_t start_time,uint8_t vip_type)
{
    if ( (vip_type >> 7) ) {
        DEBUG_LOG("SEND SYSTEM MESSAG year vip uid=[%u] vip_type=[%u]", userid, vip_type);
        char proto_msg[256] = {0};
        memset(proto_msg, 0, 256);
        uint32_t proto_len = this->pack_gf_vip_msg(proto_msg, 2142);
        gf_other_vip_sys_msg_in *in =(gf_other_vip_sys_msg_in *)(sendbuf + sizeof(switch_header));
        set_pub_buf(sendbuf,61004, userid , sizeof(*in)+proto_len );
        in->recvid = userid;
        memcpy(in->msg, proto_msg, proto_len);
        //hex_printf(sendbuf, 22);
        this->net_send_no_return();
    }

    other_sync_vip_in *in=(other_sync_vip_in *)(sendbuf + sizeof(switch_header));
	set_pub_buf(sendbuf,60003, userid ,  sizeof(*in) );
	in->vipflag=vipflag;
    in->vip_month_cnt = month_cnt;
    in->auto_incr=auto_incr;
    in->start_time=start_time;	
    in->end_time=end_time;
    in->vip_type = vip_type;
    //hex_printf(sendbuf, 38);
	return this->net_send_no_return();
}

int Cclientproto::switch_gf_base_svalue(userid_t userid,uint32_t base_svalue,uint16_t chn)
{
	gf_other_sync_base_svalue_in *in=(gf_other_sync_base_svalue_in *)(sendbuf + sizeof(switch_header));
	set_pub_buf(sendbuf,60004, userid ,  sizeof(*in) );
	in->base_svalue=base_svalue;
	in->chn=chn;
	return this->net_send_no_return();

}

int Cclientproto::switch_gf_set_gold(userid_t userid,uint32_t gold)
{
	gf_other_sync_set_gold_in  *in=(gf_other_sync_set_gold_in  *)(sendbuf + sizeof(switch_header));
	set_pub_buf(sendbuf,60005, userid ,  sizeof(*in) );
	in->gold=gold;
	return this->net_send_no_return();

}
int Cclientproto::pack_gf_vip_msg(char* buf, uint32_t type)
{
    int idx = 0;
    //pack heaher
    uint32_t pkg_len = sizeof(switch_header) + sizeof(gf_vip_msg_in);
    taomee::pack(buf, pkg_len, idx);
    taomee::pack(buf, static_cast<uint16_t>(8001), idx);
    taomee::pack(buf, static_cast<uint32_t>(0), idx);
    taomee::pack(buf, static_cast<uint32_t>(0x12345678), idx);
    taomee::pack(buf, static_cast<uint32_t>(0), idx);
    taomee::pack(buf, static_cast<uint32_t>(type), idx);
    //gf_vip_msg_in *msg = (gf_vip_msg_in *)(buf + sizeof(switch_header));
    //set_pub_buf(buf,8001, 0,  sizeof(*msg) );
    //msg->type = 2141;
    //hex_printf(buf, idx);
    return pkg_len;
}
#define PROTO_8001 1
int Cclientproto::switch_gf_vip_sys_msg(userid_t userid,uint32_t len, char* msg)
{
#ifdef PROTO_8001
    char proto_msg[256] = {0};
    memset(proto_msg, 0, 256);
    uint32_t proto_len = this->pack_gf_vip_msg(proto_msg, 2141);
    gf_other_vip_sys_msg_in *in =(gf_other_vip_sys_msg_in *)(sendbuf + sizeof(switch_header));
    set_pub_buf(sendbuf,61004, userid , sizeof(*in)+proto_len );
    in->recvid = userid;
    memcpy(in->msg, proto_msg, proto_len);
    //hex_printf(sendbuf, 22);
    return this->net_send_no_return();
#endif
#ifdef PROTO_8002
    gf_other_vip_sys_msg_in *in =(gf_other_vip_sys_msg_in *)(sendbuf + sizeof(switch_header));
    set_pub_buf(sendbuf,60001, userid ,  sizeof(*in)+len );
    in->recvid = userid;
    in->npc = 0;
    in->msg_tm = (uint32_t)time(NULL);
    in->msglen = len;
    memcpy(in->msg, msg, len);
    //hex_printf(sendbuf, 30);
    return this->net_send_no_return();
#endif
}
/*
int Cclientproto::gf_get_teamid(userid_t userid, uint32_t *p_teamid)
{
	gf_get_team_id_out *out;
	set_pub_buf(sendbuf,PP_GET_TEAMID_CMD, userid , 0 );
	SEND_DATA_AND_CHECK_SUCC();
	out=(gf_get_team_id_out *)(recvbuf+PROTO_HEADER_SIZE);
	*p_teamid=out->teamid;
	RETURN();
}
int Cclientproto::set_pk_team_member_super_nono_contribute(userid_t userid, uint32_t teamid)
{
    set_pk_team_member_super_nono_contribute_in *in=PRI_SEND_IN_POS;
	in->super_nono_contribute=0;
	in->member_id=userid;
	int private_size=sizeof(set_pk_team_member_super_nono_contribute_in);
	set_pub_buf(sendbuf,PP_PK_TEAM_MEMBER_SET_SUPER_NONO_FLAG_CMD,teamid, private_size);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
int Cclientproto::set_gf_nono_super_flag_zero(userid_t userid)
{
    set_gf_nono_super_flag_zero_in *in=PRI_SEND_IN_POS;
	in->super_flag=0;
	int private_size=sizeof(set_gf_nono_super_flag_zero_in);
	set_pub_buf(sendbuf,PP_DDING_SET_SUPER_FLAG_CMD,userid, private_size);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
int Cclientproto::set_gf_vip_flag(userid_t userid,uint32_t flag,uint32_t vip_month_count)
{
	set_gf_vip_flag_in *in=PRI_SEND_IN_POS;
	in->flag=flag;
	in->vip_month_count=vip_month_count;
	int private_size=sizeof(set_gf_vip_flag_in);
	set_pub_buf(sendbuf,PP_SET_VIP_FLAG_CMD,userid, private_size);
	SEND_DATA_AND_CHECK_SUCC();
	RETURN();
}
*/
/*
int Cclientproto::gf_get_nick_friend_list(userid_t userid,gf_get_nick_friend_list_out *p_out)
{
	set_pub_buf(sendbuf,PP_GET_NICK_FRIEND_LIST_CMD, userid , 0 );
	SEND_DATA_AND_CHECK_SUCC();

        uint32_t msglen= ((PROTO_HEADER * )recvbuf)->proto_length;
		uint32_t pri_len= msglen-PROTO_HEADER_SIZE;
	    if (pri_len> sizeof (gf_get_nick_friend_list_out)){
				return PROTO_RETURN_LEN_ERR;
		}
	   memcpy(p_out,recvbuf+PROTO_HEADER_SIZE, pri_len );

//	p_out=(gf_get_nick_friend_list_out *)(recvbuf+PROTO_HEADER_SIZE);
	RETURN();
}

int Cclientproto::gf_msg_add_item(userid_t userid,gf_msg_add_item_in_header *p_in)
{
    gf_msg_add_item_in_header *in=PRI_SEND_IN_POS;
    int private_size=sizeof(*in)+p_in->msglen;
    set_pub_buf(sendbuf, PP_MSG_ADD_ITEM_CMD ,userid, private_size);
    memcpy(in, p_in,sizeof(*p_in )+p_in->msglen );
    SEND_DATA_AND_CHECK_SUCC();
    RETURN();
}
*/


