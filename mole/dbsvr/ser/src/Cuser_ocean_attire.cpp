/*
 * =====================================================================================
 *
 *       Filename:  Cuser_ocean_attire.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/23/2012 03:17:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_ocean_attire.h"
#include "msglog.h"
#include <sstream>

int Cuser_ocean_attire::add_attire_msglog(userid_t userid, uint32_t itemid, 
		uint32_t count, uint32_t type)
{
	uint32_t msg_id = 0;
	if(type == 1){//获取
		if(itemid >= 1593000 && itemid <= 1593029){
			msg_id = 0x0409BDE3 + itemid - 1593000;
		}
	}
	else{//消耗
		if(itemid >= 1613108 && itemid <= 1613113){
			msg_id = 0x0409BDAF + itemid - 1613108;
		}
	}

	if(msg_id > 0){
		struct MSG{
			uint32_t uid;
			uint32_t cnt;
		};
		MSG msg = { 0 };
		msg.uid = userid;
		msg.cnt = count;

		msglog(this->msglog_file, msg_id, time(NULL), &msg, sizeof(msg));
	}

	return 0;	
}

Cuser_ocean_attire::Cuser_ocean_attire(mysql_interface *db, Citem_change_log *p_log)
	:CtableRoute100x10(db, "USER", "t_user_ocean_attire", "userid")
{
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;
}


int Cuser_ocean_attire::get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count - used_count from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_ocean_attire::get_count(userid_t userid, uint32_t itemid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR) ;
		count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_ONE_END();
}

int Cuser_ocean_attire::take_off(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "update %s set used_count = if(used_count >= 1, used_count-1, 0) where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_attire::put_on(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "update %s set used_count = used_count + 1 where userid = %u and itemid = %u",
            this->get_table_name(userid),
            userid,
            itemid
            );
    
    STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_attire::add_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit,
		uint32_t is_vip_opt_type)
{
	int ret = this->inner_add(userid, itemid, count, limit);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, count);
	}
	return ret;
}

int Cuser_ocean_attire::inner_add(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit)
{
	if(count == 0) 
	return SUCC;
	if(count > limit){
		return EXCEED_TOP_LIMIT_ERR;
	}
	uint32_t old_count = 0;
	int32_t ret = get_count(userid, itemid, old_count);
	add_attire_msglog(userid, itemid, count, 1);
	
	if(ret == SUCC){
		if(limit <= old_count){
			return VALUE_OUT_OF_RANGE_ERR;
		}
		sprintf(this->sqlstr, "update %s  set count = count + %u where userid = %u and itemid = %u",
				this->get_table_name(userid),
				count,
				userid,
				itemid
				);
	}
	else{
		sprintf(this->sqlstr, "insert into %s values(%u, %u, 0, %u)",
				this->get_table_name(userid),
				userid,
				itemid,
				count);
	}

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	
}

int  Cuser_ocean_attire:: get_items_range(userid_t userid, 
		uint32_t start, uint32_t end,uint8_t usedflag,
		uint32_t *count, attire_count ** list)	
{
	char formatstr[2000] ;
	if (usedflag==0){ //nouse count return
		strcpy(formatstr, "select itemid, count-used_count as unusedcount \
				from %s where userid=%u and itemid>=%u and itemid<%u \
				having unusedcount>0  order by itemid " );
	}else if (usedflag==1){ //use count return
		strcpy(formatstr,	"select itemid,used_count from %s \
			where userid=%u and itemid>=%u and itemid<%u and used_count>0 order by itemid ");
	}else if(usedflag == 2){
		strcpy(formatstr,	"select itemid, count from %s \
			where userid=%u and itemid>=%u and itemid<%u order by itemid ");
	}
	else{
		return  ENUM_OUT_OF_RANGE_ERR ;
	}
	sprintf( this->sqlstr,formatstr ,this->get_table_name(userid), userid, start, end); 
	
		
	STD_QUERY_WHILE_BEGIN( this->sqlstr,list, count ) ;
		(*list+i)->attireid=atoi_safe(NEXT_FIELD); 
		(*list+i)->count=atoi_safe(NEXT_FIELD); 
	STD_QUERY_WHILE_END();
}

int Cuser_ocean_attire::remove(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_ocean_attire::sub_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t is_vip_opt_type)
{
	int ret = inner_sub(userid, itemid, count);
	if(ret == SUCC){
		return this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, -count);
	}
	return ret;
}

int Cuser_ocean_attire::inner_sub(userid_t userid, uint32_t itemid, uint32_t count)
{
	if(count == 0)
		return SUCC;
	uint32_t old_count = 0;
	get_noused_count(userid, itemid, old_count);
	if( old_count < count ){
		return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}

	add_attire_msglog(userid, itemid, count, 0);

	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and itemid = %u",
			this->get_table_name(userid),
			old_count - count,
			userid,
			itemid
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	

}

int Cuser_ocean_attire::get_interval_item(userid_t userid, uint32_t *p_in_item, uint32_t in_count, 
		attire_count** pp_out_item, uint32_t *out_count)
{
	std::ostringstream in_str;
    for(uint32_t i = 0; i < in_count; ++i) {
			if(i < in_count-1){
				in_str << *(p_in_item+i) << ",";
			}
			else{
				in_str << *(p_in_item+i);
			}
    }
	
	sprintf(this->sqlstr, "select itemid, count-used_count from %s where userid = %u and itemid in(%s)",
			this->get_table_name(userid),
			userid,
			in_str.str().c_str()
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out_item, out_count);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_out_item + i)->count);
	STD_QUERY_WHILE_END();
}

int Cuser_ocean_attire::check_items_enough(userid_t userid, uint32_t* material_items, uint32_t in_count,
		uint32_t* material_count)
{
	attire_count* p_item = 0;
	uint32_t cnt = 0;
	get_interval_item(userid, material_items, in_count, &p_item, &cnt);
	if(cnt < in_count){
		return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}
	else{
		for(uint32_t k = 0; k < cnt; ++k){
			uint32_t* iter = std::find(material_items, material_items+cnt, (p_item+k)->attireid);
			if(iter == material_items+cnt){
				 if(p_item != 0){
					 free(p_item);
				 }
				 return ATTIRE_COUNT_NO_ENOUGH_ERR;
			}
			if((p_item+k)->count < *(material_count+(iter-material_items))){
				if(p_item != 0){
					free(p_item);
					return ATTIRE_COUNT_NO_ENOUGH_ERR;
				}
			}
		}//for
	}

	return SUCC;
}
                    
