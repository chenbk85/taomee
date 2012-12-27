/*
 * =====================================================================================
 *
 *       Filename:  Cuser_temp_item.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 13时46分59秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include "Cuser_temp_item.h"
#include "msglog.h"

Cuser_temp_item::Cuser_temp_item(mysql_interface * db, Citem_change_log *p_log) 
	:CtableRoute100x10( db,"USER","t_user_temp_item","userid")
{
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;
}

int Cuser_temp_item::get_count(userid_t userid, uint32_t itemid,uint32_t &count)
{
	count=0; 
	sprintf(this->sqlstr, "select count from %s where userid=%u and itemid=%u", 
		this->get_table_name(userid),userid, itemid); 

	STD_QUERY_ONE_BEGIN(this->sqlstr,-1) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_temp_item::add_count(userid_t userid, uint32_t itemid,uint32_t count,uint32_t limit, uint32_t is_vip_opt_type)
{
	int ret = inner_add(userid, itemid, count, limit);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, count);
	}
	return ret;

}

int Cuser_temp_item::inner_add(userid_t userid, uint32_t itemid,uint32_t count,uint32_t limit)
{

	int ret;
    uint32_t old_count = 0;
    if(count == 0) return SUCC;
	if(count > limit){
		return EXCEED_TOP_LIMIT_ERR;
	}
    ret = this->get_count(userid,itemid,old_count);
    if(ret == SUCC) {
        if(limit <= old_count) return VALUE_OUT_OF_RANGE_ERR;
        sprintf(this->sqlstr, "update %s set count=count+%u where userid=%u and itemid=%u",
                        this->get_table_name(userid),count,userid,itemid);
    } else {
        sprintf(this->sqlstr, "insert into %s values(%u,%u,%u)",
                        this->get_table_name(userid),userid,itemid,count);
    }
    struct statistic_ty{
            uint32_t userid;
            uint32_t num;
        };
    statistic_ty stru = {}; 
    stru.userid = userid;
    stru.num = count;

    if(itemid == 1351053){
        msglog(this->msglog_file, 0x0406FAD2, time(NULL), &stru, sizeof(stru));
    }
    else{
        msglog(this->msglog_file, 0x04040C01 + (itemid - 1351000), time(NULL), &stru, sizeof(stru));
    }

    STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);
}
int Cuser_temp_item::sub_count(userid_t userid, uint32_t itemid,uint32_t count, uint32_t is_vip_opt_type)
{
	int ret = inner_sub(userid, itemid, count);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, -count);
	}
	return ret;
}

int Cuser_temp_item::inner_sub(userid_t userid, uint32_t itemid, uint32_t count)
{
    int ret;
    uint32_t old_count = 0;
    if(count == 0) return SUCC;
    ret = this->get_count(userid,itemid,old_count);
    if(ret != SUCC || old_count < count) return ATTIRE_COUNT_NO_ENOUGH_ERR;
    sprintf(this->sqlstr, "update %s set count=count-%u where userid=%u and itemid=%u",
                this->get_table_name(userid),count,userid,itemid);
    STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);
}

int  Cuser_temp_item::get_items_count(userid_t userid, uint32_t start, uint32_t end, uint32_t *count, attire_count** list)	
{
	sprintf(this->sqlstr,"select itemid, count from %s where userid=%u and itemid>=%u and itemid<%u and count > 0",
		this->get_table_name(userid),userid, start, end); 
		
	STD_QUERY_WHILE_BEGIN(this->sqlstr,list, count );
		(*list+i)->attireid=atoi_safe(NEXT_FIELD); 
		(*list+i)->count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}
