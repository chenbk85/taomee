/*
 * =====================================================================================
 * 
 *	   Filename:  proto.h
 * 
 *	Description:  
 * 
 *		Version:  1.0
 *		Created:  2007年11月01日 14时10分21秒 CST
 *	   Revision:  none
 *	   Compiler:  gcc
 * 
 *		 Author:  xcwen (xcwen), xcwenn@gmail.com
 *		Company:  NULL
 * 
 * =====================================================================================
 */

#ifndef  PROTO_INC
#define  PROTO_INC
#ifndef  MK_PROTO_DOC 
#include "proto_header.h"
#endif
//在cmdid 中数据库识别标志
#define RROTO_ROUTE_FIELD 				0xFC00 //使用前6个bit

//是否需要保存交易报文
//有涉及到更新数据库数据的协议，都要保存 
#define NEED_UPDATE							0x0100

#define DO_EX								0x0800


//删除角色

#define gf_add_del_role_cmd					(0x0EA1)

#define gf_del_allrole_overload_cmd			(0x0EA2)



#define gf_del_role_cmd						(0x06F1 | NEED_UPDATE)

#define OFFLINE_MSG_MAX_LEN 100
#define MSG_LIST_BUF_SIZE  2000 

#define NICK_LEN	16
#define  FRIEND_COUNT_MAX   200 
#define ID_LIST_MAX 200
#define BACK_ID_LIST_MAX 2000
#define ACCESS_COUNT_MAX 100
#define VALUE_MAX 100000000 
#define  MSG_MAX_LEN 2000

struct	stru_count{
	uint32_t	count;
}__attribute__((packed));

struct stru_id{
	userid_t	id;
}__attribute__((packed));

struct stru_id_list{
	uint32_t    count;       
   	userid_t    item[ID_LIST_MAX];
}__attribute__((packed));

struct  gf_reg_in{
	uint32_t sex;
	uint32_t birthday;
	char nick[NICK_LEN];
}__attribute__((packed));


struct gf_role_deleted_stru{
	userid_t userid;
	uint32_t role_regtime;
	uint32_t del_time;
}__attribute__((packed));

struct gf_del_allrole_overload_in{
	uint32_t time;
}__attribute__((packed));



#endif
