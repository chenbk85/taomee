/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_pet_attire.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 13时47分01秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_PET_ATTIRE_INCL
#define    CUSER_PET_ATTIRE_INCL

#include <sstream>

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "Citem_change_log.h"



class Cuser_pet_attire:public CtableRoute100x10
{
private:
	Citem_change_log *p_item_change_log;
public:
	Cuser_pet_attire(mysql_interface * db, Citem_change_log *p_log ); 
	int get (userid_t userid, uint32_t petid, uint32_t attireid,
			uint32_t * usedcount,uint32_t * count  );

	int get_list_by_attireid_interval(userid_t userid, uint32_t petid, 
			uint32_t attireid_start, uint32_t attireid_end,uint8_t usedflag,
			uint32_t *count, attire_count ** list	);

	int insert(userid_t userid, uint32_t petid,uint32_t attireid, uint32_t usedcount,uint32_t count);

	int remove(userid_t userid, uint32_t petid, uint32_t attireid);

	/**
	 * @brief 把指定物品的个数与使用数设置为给定值
	 */
	int update( userid_t userid, uint32_t petid, uint32_t attireid, uint32_t  usedcount,uint32_t count  );

	/**
	 * @brief 把物品的数据增加或减少指定数目
	 * @param int addcount 需要增加的数目，负数表示减少
	 */
	int update_count( userid_t userid, uint32_t petid,uint32_t attireid,int addcount );

	/**
	 * @brief 把物品的数据增加或减少指定数目，如果数据库不存在该，那么插入
	 * @param int addcount 需要增加的数目，负数表示减少
	 */
	int update_count_ex( userid_t userid, uint32_t petid,uint32_t attireid,int addcount );

	/**
	 * @brief 尝试更新指定的行，如果该不存在，则插入该行
	 */
	int update_ex( userid_t userid, uint32_t petid,uint32_t attireid, uint32_t  usedcount,uint32_t count,
			uint32_t is_vip_opt_type = 0);

	int update_common(userid_t userid, uint32_t petid,user_set_attire_in *item, user_set_attire_out *p_out, 
			uint32_t is_vip_opt_type = 0);

	int get_used_list(userid_t userid, uint32_t *count, user_get_pet_attire_used_all_out_item  ** list);

	int get_all(userid_t userid, uint32_t *count, user_get_pet_attire_all_out_item ** list   );

	int remove_pet_attire(userid_t userid, uint32_t petid , uint32_t is_vip_opt_type = 0);

	int set_no_vip(userid_t userid);

	int get_pet_now_colth(const userid_t userid, const uint32_t petid, uint32_t* count,
			user_pet_attire_set_used_out_item** list, const bool is_colth);

	int set_pet_used_attire(const userid_t userid, const uint32_t petid, const uint32_t count,
			const user_pet_attire_set_used_in_item* list, const bool is_colth);
	int get_pet_items(userid_t userid, uint32_t attireid_start, uint32_t attireid_end,
			uint32_t *count,  attire_count **list);

	/**
	 * @brief 得到各个不连续物品ID的数目
	 * @param userid 米米号
	 * @param p_in 输入的要查询的ID号
	 * @param count 输入的ID号的个数
	 * @param pp_out 输出查询的结构(物品的ID号和对应的个数)
	 * @param p_out_count 查询到的物品的个数
	 */
	int get_discontinuous_attire(userid_t userid, user_pet_get_count_with_attireid_in_item* p_in, uint32_t count,
			user_pet_get_count_with_attireid_out_item** pp_out, uint32_t *p_out_count)
	{
		char formatstr[6000]={};
		char valuestr[100]={};
		if (count>200 || count<1)
		{
			return VALUE_OUT_OF_RANGE_ERR;
		}
		for(uint32_t i = 0; i < count; i++)
		{
			sprintf (valuestr,"%d,", (p_in + i)->attireid);
			strcat(formatstr, valuestr );
		}
		//去掉最后一个","
		formatstr[strlen(formatstr)-1]='\0';
		
		sprintf( this->sqlstr,  "select attireid, count from %s where attireid in (%s) and count > 0 and userid = %u", 
				this->get_table_name(userid),
				formatstr,
				userid
			   ); 
			STD_QUERY_WHILE_BEGIN( this->sqlstr, pp_out, p_out_count) ;
				INT_CPY_NEXT_FIELD( (*pp_out + i)->attireid); 
				INT_CPY_NEXT_FIELD((*pp_out + i)->count); 
			STD_QUERY_WHILE_END();
	}

	int add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count);

private:

	char *msglog_file;

};

#endif   /* ----- #ifndef CUSER_PET_ATTIRE_INCL  ----- */
