/*
 * =====================================================================================
 * 
 *       Filename:  Cmsgboard.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CMSGBOARD_INCL
#define  CMSGBOARD_INCL
#include "Ctable.h"
#include "Csem.h"
#include "proto.h"
//缓存消息个数
#define MSG_CACHE_MAX  			20
//缓存列表个数
#define MSGBOARD_LIST_CACHE_MAX 1000 

#define FLAG_INPUT  	0
#define FLAG_CHECK_OK  	1
#define FLAG_DELETE  	2

typedef struct msgboard { /*保存前二十条记录*/
	bool	inited; /*初始化是否完成*/
	uint32_t count_in_db; 
	uint32_t startindex; 
	stru_msgboard_item msgboard_item [MSG_CACHE_MAX]; 
}__attribute__((packed)) MSGBOARD;


typedef struct msgboard_list { 
	MSGBOARD item[MSGBOARD_LIST_CACHE_MAX]; 
}__attribute__((packed)) MSGBOARD_LIST;



class Cmsgboard : public Ctable{
	protected:
		int shmid;
		MSGBOARD_LIST * cache_list;
		Csem sem;
		inline int check_boardid(uint32_t boardid);
		int initlist(uint32_t boardid);
	public:
		int getmsglist_from_db(uint32_t boardid,uint32_t flag, uint32_t start, uint32_t count, 
				uint32_t *p_count, stru_msgboard_item** pp_list  );

		Cmsgboard(mysql_interface * db ); 
		int  clear_by_delete_flag();
		int insert(uint32_t boardid, msgboard_item_max *msg ,  uint32_t msglen, uint32_t *p_msgid);
		int getmsglist_lock(uint32_t boardid,uint32_t pageid,msgboard_get_msglist_out *p_out);
		int getmsglist(uint32_t boardid,uint32_t pageid, msgboard_get_msglist_out *p_out);

		int getmsglist_by_userid( userid_t userid, uint32_t boardid,
		uint32_t start, uint32_t count, uint32_t *p_count, stru_msgboard_item** pp_list );
		int addmsg_lock(uint32_t boardid, msgboard_item_max *msg,	
				uint32_t msg_len,uint32_t *p_msgid );
		int getmsg_by_msgid(uint32_t msgid,uint32_t *p_boardid , msgboard_item_max *p_msg);
		int update_flag(uint32_t msgid,  uint32_t  flag);
		int addhot(uint32_t boardid , uint32_t msgid );
		int set_cache_is_inited(uint32_t boardid, bool flag  );
		int get_msgcount(uint32_t boardid,uint32_t *p_count );
		int check_msg(uint32_t boardid ,uint32_t userid, char * mysql_msg);
		//update cache init flag
		int update_hot_in_cache(uint32_t boardid,uint32_t  msgid );

		int getmsg_by_msgid_nocheckflag(uint32_t msgid, msgboard_get_msg_nocheck_out *p_msg);
		int update_msg(uint32_t msgid, char *  msg );

};

#endif   /* ----- #ifndef CMSGBOARD_INCL  ----- */

