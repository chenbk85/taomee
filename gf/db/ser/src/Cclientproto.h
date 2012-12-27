/*
 * =====================================================================================
 * 
 *       Filename:  Cclientproto.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月10日 18时22分49秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */

#ifndef  CCLIENTPROTO_INC
#define  CCLIENTPROTO_INC
/*
 * =====================================================================================
 *        Class:  Cclientproto
 *  Description:  
 * =====================================================================================
 */

#include <string.h>
//#include "common.h"
//#include "Csync_log.h"
#include "proto_header.h"
#include "proto.h"
#include "dbser/db_error_base.h"
/*
#define PP_GET_TEAMID_CMD                   (0x1888 )
#define PP_PK_TEAM_MEMBER_SET_SUPER_NONO_FLAG_CMD   (0x5830 |NEED_UPDATE )
#define PP_DDING_SET_SUPER_FLAG_CMD             (0x1882 |NEED_UPDATE )
#define PP_SET_VIP_FLAG_CMD                 (0x1889 |NEED_UPDATE )
#define PP_GET_NICK_FRIEND_LIST_CMD         (0x1829 )
#define PP_MSG_ADD_ITEM_CMD                 (0x1B01 |NEED_UPDATE)
*/
#define ID_LIST_MAX 200
#define userid_t uint32_t



typedef struct  as_msg{
		uint32_t msglen;
		uint8_t version;
		uint32_t v1;
		uint32_t v2;
		uint32_t v3;
		uint32_t v4;
		uint32_t v_ex;
		uint32_t v5;
		uint32_t v6;
		char     nick[16];
		uint32_t v7;
		uint32_t logtime;
		uint32_t v8;
		uint32_t as_msglen;
		char msg[2000];
} __attribute__((packed)) AS_MSG;

typedef struct self_header{
    uint32_t proto_length; //报文总长度
    uint32_t proto_id;//序列号，需要原样返回
    uint16_t cmd_id; //命令号
    int32_t result; //返回值
    userid_t id;  /*一般是米米号*/
    //uint32_t role_tm;
}__attribute__((packed)) switch_header;

typedef struct  switch_msg{
		uint32_t serverid;
		uint32_t userid;
		uint32_t v;
		int16_t msglen;
		char msg[2000];
}__attribute__((packed)) SWITCH_MSG;
/*
struct gf_get_team_id_out{
	uint32_t teamid;
}__attribute__((packed));
struct set_pk_team_member_super_nono_contribute_in{
	uint32_t super_nono_contribute;
	uint32_t member_id;
}__attribute__((packed));
struct set_gf_nono_super_flag_zero_in{
	uint32_t super_flag;
}__attribute__((packed));
*/
struct other_sync_vip_in{
    uint32_t vipflag;
    uint32_t vip_month_cnt;
	uint32_t auto_incr;
	uint32_t start_time;
	uint32_t end_time;
    uint8_t  vip_type;
}__attribute__((packed));
struct set_gf_vip_flag_in{
	uint32_t flag;
	uint32_t vip_month_count;
}__attribute__((packed));
struct gf_other_sync_base_svalue_in{
	uint32_t base_svalue;
	uint16_t chn;
}__attribute__((packed));

struct gf_other_sync_set_gold_in{
	uint32_t gold;
}__attribute__((packed));

struct gf_vip_msg_in{
    uint32_t    type;
    uint32_t    uid;
    uint32_t    role_time;
    uint32_t    role_type;
    uint8_t     nick[16];
    uint32_t    accept;
    uint32_t    serverid;
    uint32_t    map_type;
    uint32_t    map_lowid;
    uint8_t     map_name[64];
}__attribute__((packed));

struct gf_other_vip_sys_msg_in{
    uint32_t recvid;
    //uint32_t npc;
    //uint32_t msg_tm;
    //uint32_t msglen;
    char    msg[];
}__attribute__((packed));
/*  struct stru_id_list{
    uint32_t    count;
	userid_t    item[ID_LIST_MAX];
}__attribute__((packed));
struct gf_get_nick_friend_list_out{
   char nick[16];
   stru_id_list friendlist;   
}__attribute__((packed));
struct gf_msg_add_item_in_header{
    uint32_t senderid;
    char sendernick[16];
    uint32_t msg_time;
    uint32_t tmpltid;
    uint32_t msglen;
};
*/
class Cclientproto{
	public:
    	char sendbuf[4096];
    	int switch_gf_sync_vip(userid_t userid,uint32_t vipflag, uint32_t month_cnt,
            uint32_t auto_incr, uint32_t end_time,uint32_t start_time, uint8_t vip_type);
		int switch_gf_base_svalue(userid_t userid,uint32_t base_svalue,uint16_t chn);
        Cclientproto (const char * aipaddr, u_short aport);  /* constructor */
        Cclientproto (){ }	/* constructor */

        void set_ip_port (const char * aipaddr, u_short aport); 
	    void set_pub_buf(char *buf,uint16_t cmd_id,int id,short private_size);
        int gf_get_teamid(userid_t userid, uint32_t *p_teamid);
		int set_pk_team_member_super_nono_contribute(userid_t userid, uint32_t teamid);
		int set_gf_nono_super_flag_zero(userid_t userid);
		int set_gf_vip_flag(userid_t userid,uint32_t flag,uint32_t vip_month_count);
        int switch_gf_set_gold(userid_t userid,uint32_t gold);
        int switch_gf_vip_sys_msg(userid_t userid,uint32_t len, char* msg);
        //int gf_get_nick_friend_list(userid_t userid,gf_get_nick_friend_list_out *p_out);
		//int gf_msg_add_item(userid_t userid,gf_msg_add_item_in_header *p_in);
  protected:
    	int net_send();
	    int	net_send_no_return();

  private:
        int pack_gf_vip_msg(char* msg, uint32_t type);
    	int  sndlen;
	    char * recvbuf;
	    int  rcvlen;
	    int  ret;
    	short  result;
	    int sockfd ;
	    u_short port;
	    char ipaddr[16];
}; /* -----  end of class  Cclientproto  ----- */

#endif   /* ----- #ifndef CCLIENTPROTO_INC  ----- */




