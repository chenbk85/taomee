/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_gen_vip.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/08/2009 09:21:10 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Croommsg_bibo.h"
/* @brief 构造函数
 * @param db 数据库类指针
 */
Croommsg_bibo:: Croommsg_bibo(mysql_interface *db)
	              : CtableRoute10x10(db, "ROOMMSG", "t_roommsg_bibo", "userid")
{

}

/* @brief 插入一条记录 
 * @param userid 用户的米米号
 * @param p_in bibo的相关信息
 */
int Croommsg_bibo:: insert(userid_t userid, roommsg_insert_bibo_in *p_in)
{
	char bibo_title[mysql_str_len(sizeof(p_in->title))];
	set_mysql_string(bibo_title, p_in->title, sizeof(p_in->title));

	char bibo_mysql[mysql_str_len(sizeof(p_in->bibo_content))];
	set_mysql_string(bibo_mysql, p_in->bibo_content, sizeof(p_in->bibo_content));
	
	sprintf(this->sqlstr, "insert into %s values (%u, %u, %u, '%s', '%s')",
			 this->get_table_name(userid),
			 userid,
			 p_in->date,
			 p_in->pic,
			 bibo_title,
			 bibo_mysql
     		);
	STD_SET_RETURN_EX(this->sqlstr, DB_ERR);
}

/* @brief 更行bibo的相关信息
 * @param 
 */
int Croommsg_bibo::update(userid_t userid, roommsg_insert_bibo_in *p_in)
{

	char bibo_title[mysql_str_len(sizeof(p_in->title))];
	set_mysql_string(bibo_title, p_in->title, sizeof(p_in->title));

	char bibo_content[mysql_str_len(sizeof(p_in->bibo_content))];
	set_mysql_string(bibo_content, p_in->bibo_content, sizeof(p_in->bibo_content));
	
	sprintf( this->sqlstr, "update %s set pic = %u, title = '%s', content = '%s' where userid = %u and date = %u",
			 this->get_table_name(userid),
			 p_in->pic,
			 bibo_title,
			 bibo_content,
			 userid,
			 p_in->date
     		);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Croommsg_bibo :: bibo_edit(userid_t userid, roommsg_insert_bibo_in *p_in)
{
	uint32_t date = get_date(time(NULL));
	if (p_in->date < date) {
		return YOU_TIME_OLD_THEM_CURRENT_ERR;
	}
	uint32_t ret = this->insert(userid, p_in);
	if (ret != SUCC) {
		ret = this->update(userid, p_in);
	}
	return ret;
}

int Croommsg_bibo :: del(userid_t userid, uint32_t date) 
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and date = %u",
			this->get_table_name(userid),
			userid,
			date
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Croommsg_bibo :: get_list(userid_t userid, roommsg_get_bibo_list_in *p_in, roommsg_get_bibo_list_out_item **pp_list,
		             uint32_t *p_count)
{
	sprintf(this->sqlstr, "select date, pic, title, content from %s where\
			userid = %u and date >= %u and date <= %u",
			this->get_table_name(userid),
			userid,
			p_in->date_start,
			p_in->date_end
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->date);
		INT_CPY_NEXT_FIELD((*pp_list + i)->pic);
		BIN_CPY_NEXT_FIELD((*pp_list + i)->title, sizeof((*pp_list)->title));
		BIN_CPY_NEXT_FIELD((*pp_list + i)->bibo_content, sizeof((*pp_list)->bibo_content));
	STD_QUERY_WHILE_END();
}


