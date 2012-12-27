/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_class.h
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

#ifndef  CUSER_CLASS_INC
#define  CUSER_CLASS_INC
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "func_comm.h"
#include "Citem_change_log.h"



#define  CLASS_HOME_STR 		"home" 
#define  CLASS_MEMBERLIST_STR 		"member_list" 
#define  CLASS_ACCESS_FLAG_STR "class_access_flag" 
#define  CLASS_CLASS_MEDAL_STR "class_medal" 

struct stru_opt_log_item{
	uint32_t	userid;
	uint32_t	id_ex;
	char 		value[25];
}__attribute__((packed));

#define  STRU_OPT_LOG_LIST_LEN 50
struct stru_opt_log_list{
	uint32_t			count;
	stru_opt_log_item 	items[STRU_OPT_LOG_LIST_LEN];
	inline void insert(stru_opt_log_item *p_item ){
		this->count++;
		if (this->count>STRU_OPT_LOG_LIST_LEN ) this->count=STRU_OPT_LOG_LIST_LEN ;
		//整体向后移动
		uint32_t i=this->count; 
		if (i==STRU_OPT_LOG_LIST_LEN ) i= STRU_OPT_LOG_LIST_LEN-1;
		for (;i>0;i--){
			memcpy(&(this->items[i]),&(this->items[i-1]),sizeof(this->items[0]));
		}
		//加入新项
		memcpy(&(this->items[0]),p_item,sizeof(this->items[0]));
	}
}__attribute__((packed));


class Cuser_class :public CtableRoute100x10 {
	private:
		Citem_change_log *p_item_change_log;
				//返回实际删除的个数
		inline int del_homeattire_noused( noused_homeattirelist *p_list, 
				attire_noused_item *p_item ); 

				//返回实际删除的个数
		inline int del_homeattire_used(home_attirelist *p_list , home_attire_item * p_item );
		inline int add_homeattire_noused(noused_homeattirelist *p_list,
				attire_count_with_max * p_item);

		int update_used_homeattirelist( const char * type_str, userid_t classid ,   
			home_attirelist * p_usedlist );
	public:
		Cuser_class(mysql_interface * db , Citem_change_log *p_log); 

		int insert(userid_t classid, user_class_create_in  * p_in );
		int del(userid_t classid );

		int get_info( userid_t classid , stru_user_class_info *p_out );


		//attire list
		int get_attirelist( const char * type_str,userid_t classid,home_attirelist * p_home_attirelist);
		int swap_homeattire(const char * type_str, userid_t classid , 
				uint32_t oldcount,attire_count* p_oldlist,
				uint32_t newcount,attire_count_with_max * p_newlist );
		
		int add_home_attire(const char * type_str, userid_t classid,attire_count_with_max * p_item, uint32_t is_vip_opt_type = 0);


		int update_noused_homeattirelist( const char * type_str,userid_t classid, noused_homeattirelist * p_list );
		int get_noused_home_attirelist(const char * type_str, userid_t classid, noused_homeattirelist * p_list );

		int  get_homeattirelist(const char * type_str, userid_t classid, home_attirelist * p_usedlist, 
			noused_homeattirelist* p_nousedlist);

		int update_homeattirelist(const char * type_str, userid_t classid,  
			home_attirelist * p_usedlist,noused_homeattirelist * p_nousedlist );

		void get_home_attire_total(ATTIRE_MAPITEM  * p_itemmap,
			   	home_attirelist * p_usedlist,
				noused_homeattirelist * p_nousedlist );
		int del_home_attire_noused(const char * type_str, userid_t classid , attire_count_with_max  * p_item);

		int get_attire_count(const char * type_str, userid_t classid, uint32_t  attireid,
				uint32_t count_flag,  uint32_t * p_count );

		int home_set_attire_noused(const char * type_str, userid_t classid , uint32_t attireid);

		int home_attire_change(const char * type_str, userid_t classid , user_home_attire_change_in *p_in );

		int del_home_attire_used(const char * type_str, userid_t classid , uint32_t attireid );

		int update_used_homeattirelist_with_check(const char * type_str, userid_t classid ,   
			home_attirelist * p_new_usedlist );

		int get_idlist(userid_t classid ,const  char * id_flag_str , id_list * idlist );

		int add_id(userid_t classid ,const  char * id_flag_str , userid_t id );
		int update_idlist(userid_t classid , const char * id_flag_str ,  id_list * idlist );

		int del_id(userid_t classid ,const char * id_flag_str , userid_t id );

		int get_flag_info( userid_t classid , user_class_get_flag_info_out *p_out );
		int get_simple_info( userid_t classid , user_class_get_simple_info_out  *p_out );

		int change_info(userid_t classid,  user_class_change_info_in * in);

		int get_benefactor_list(userid_t userid, stru_opt_log_list *p_out );

		int update_benefactor_list(userid_t userid ,stru_opt_log_list * p_list );

		int benefactor_list_add(userid_t userid , stru_opt_log_item * p_item );

		int update_homeattire_all(const char * type_str,  userid_t classid ,  
			home_attirelist * p_usedlist  );

		int set_medal(userid_t classid, class_medal *p_list);

		int get_medal(userid_t classid, class_medal *p_list);

		int add_medal(userid_t classid, uint32_t type, uint32_t attire_id);

		int set_medal_info(userid_t classid , user_class_set_medal_info_in *p_in,
		user_class_set_medal_info_out *p_out  );

		int get_attire_list_count(uint32_t classid, uint32_t attireid, uint32_t count_flag, uint32_t *p_count);

		int get_one_attire_count(home_attirelist old_usedlist, noused_homeattirelist old_nousedlist,
				                 uint32_t attireid, uint32_t count_flag, uint32_t *p_count);


};


#endif   /* ----- #ifndef CUSER_CLASS_INC  ----- */

