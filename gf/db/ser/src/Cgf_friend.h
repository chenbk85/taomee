/*
 * =====================================================================================
 * 
 *       Filename:  Cgf_friend.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2010年02月03日 19时52分58秒 CST
 *       Revision:  1.0
 *       Compiler:  gcc
 * 
 *         Author:  ianyang
 *        Company:  TAOMEE
 * 普通:100个好友 ..100个黑名单
 vip:200个好友  .. 100个黑名单
 * =====================================================================================
 */

#ifndef  GF_CGF_FRIEND_H
#define  GF_CGF_FRIEND_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 
using namespace std;


#define  PP_FLAG_VIP             0x00000001
#define  PP_FLAG_IS_VIP_OLD      0x00000002
#define  VIP_FRIENDLIST_MAX_NUM       200
#define  NOR_FRIENDLIST_MAX_NUM       100
#define  BLACKLIST_MAX_NUM            100
#define  FRIEND_LIST_STR            "friendlist" 
#define  BLACK_LIST_STR		        "blacklist" 
#define  BACKFRIEND_LIST_STR		"backfriendlist"


enum FRIEND_TYPE {TYPE_FRIEND=1 ,    /*好友*/
					TYPE_BLACK,         /*黑名单*/
					TYPE_MASTER,        /*师傅*/
                    TYPE_PRENTICE ,     /*徒弟*/
                    TYPE_HUSBAND,       /*丈夫*/
                    TYPE_WIFE,			 /*妻子*/
                    TYPE_FRIENDALL};        


class Cgf_friend :public CtableRoute{
	public:
 		Cgf_friend(mysql_interface * db);
		int add(userid_t userid, uint32_t fid, uint32_t vip_type, FRIEND_TYPE friend_type);
		int del(userid_t userid, uint32_t fid, FRIEND_TYPE friend_type);
		int del( userid_t userid, uint32_t fid );
		int get_friendid_list(userid_t userid, FRIEND_TYPE friend_type, uint32_t* p_count,stru_friedid_list** p_out_item);
		uint32_t get_friend_max(uint32_t vip_type);
		uint32_t get_black_max(uint32_t vip_type);
		int get_friendid_list_type(userid_t userid, FRIEND_TYPE friend_type, uint32_t* p_count,
			gf_get_friendlist_type_out_item** pp_list);
		int clear_role_friend(userid_t userid);
		int get_friend_count(userid_t userid,FRIEND_TYPE friend_type,uint32_t* p_count);

	private:
		
		int insert(userid_t userid, uint32_t fid, FRIEND_TYPE friend_type);
		int is_friend_id_exist(userid_t userid, uint32_t fid, FRIEND_TYPE friend_type, bool* p_existed);
		int update_friend_type( userid_t userid, uint32_t fid,  FRIEND_TYPE from_type, FRIEND_TYPE to_type);

		int ret;
	public:
};


#endif   /* ----- #ifndef GF_CGF_FRIEND_H  ----- */

