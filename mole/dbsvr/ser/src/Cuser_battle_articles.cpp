/*
 * =====================================================================================
 *
 *       Filename:  Cuser_battle_articles.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/20/2011 01:56:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include "Cuser_battle_articles.h"
#include "msglog.h"

Cuser_battle_articles::Cuser_battle_articles(mysql_interface *db, Citem_change_log *p_log)
	:CtableRoute100x10(db, "USER", "t_user_battle_articles", "userid")
{

	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;
}

int Cuser_battle_articles::get_noused_count(userid_t userid, uint32_t itemid, uint32_t &count)
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

int Cuser_battle_articles::get_count(userid_t userid, uint32_t itemid, uint32_t &count)
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

int Cuser_battle_articles::add_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit,
		uint32_t is_vip_opt_type)
{
	int ret = this->inner_add(userid, itemid, count, limit);
	if(ret == SUCC){
		DEBUG_LOG("#add# itemid: %u  count:%u", itemid, count);
		this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, count);
	}
	return ret;
}

int Cuser_battle_articles::inner_add(userid_t userid, uint32_t itemid, uint32_t count, uint32_t limit)
{
	if(count == 0) 
	return SUCC;
	if(count > limit){
		return EXCEED_TOP_LIMIT_ERR;
	}
	uint32_t old_count = 0;
	int32_t ret = get_count(userid, itemid, old_count);
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

	//获取战斗物品统计	
	struct userid_num_t {
		userid_t userid;
		uint32_t count;
	};

	uint32_t type_id = 0;
	if(itemid == 1301001){
		type_id = 0x0409BBC9;
	}
	else if(itemid <= 1301023){
		type_id = 0x0409BBC9 + (itemid - 1301001)*2;
	}
	else if(itemid == 1301024){
		type_id = 0x0409B446; 
	}
	else if(itemid <= 1301027){
		type_id = 0x0409BBF9 + (itemid - 1301025)*2;
	}
	else if(itemid >= 1553151 && itemid <= 1553155){
		type_id = 0x0409B456 + (itemid - 1553151);
	}
	if(type_id != 0){
		userid_num_t package_cont = {userid, count};
		msglog(this->msglog_file, type_id, time(NULL), &package_cont, sizeof(package_cont));
	}

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	

}

int  Cuser_battle_articles:: get_items_range(userid_t userid, 
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
			where userid=%u and itemid>=%u and itemid<%u  order by itemid ");
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

int Cuser_battle_articles::remove(userid_t userid, uint32_t itemid)
{
	sprintf(this->sqlstr, "delete from %s where userid = %u and itemid = %u",
			this->get_table_name(userid),
			userid,
			itemid
			);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}
int Cuser_battle_articles::sub_count(userid_t userid, uint32_t itemid, uint32_t count, uint32_t is_vip_opt_type)
{
	int ret = inner_sub(userid, itemid, count);
	if(ret == SUCC){
		DEBUG_LOG("#sub# itemid: %u  count:%u", itemid, count);
		this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, itemid, -count);
	}
	return ret;
}
int Cuser_battle_articles::inner_sub(userid_t userid, uint32_t itemid, uint32_t count)
{
	if(count == 0)
		return SUCC;
	uint32_t old_count = 0;
	get_noused_count(userid, itemid, old_count);
	if( old_count < count ){
		return ATTIRE_COUNT_NO_ENOUGH_ERR;
	}

	//消耗战斗物品统计	
	struct userid_num_con_t {
		userid_t userid;
		uint32_t count;
	};

	uint32_t type_id = 0;
	if(itemid == 1301001){
		type_id = 0x0409BBCA;
	}
	else if(itemid <= 1301023){
		type_id = 0x0409BBCA + (itemid - 1301001)*2;
	}
	else if(itemid == 1301024){
		type_id = 0x0409B447;
	}
	else if(itemid <= 1301027){
		type_id = 0x0409BBFA + (itemid - 1301025)*2;
	}
	if(type_id != 0){
		userid_num_con_t package_cont = {userid, count};
		msglog(this->msglog_file, type_id, time(NULL), &package_cont, sizeof(package_cont));
	}

	sprintf(this->sqlstr, "update %s set count = %u where userid = %u and itemid = %u",
			this->get_table_name(userid),
			old_count - count,
			userid,
			itemid
			);

	STD_SET_RETURN_EX(this->sqlstr,USER_ID_NOFIND_ERR);	

}
int Cuser_battle_articles::add_achievement_level(userid_t userid, uint32_t new_level, uint32_t old_level,
		uint32_t *count, user_battle_over_info_out_item_1* &p_offset)
{
    uint32_t cardid = 0;
    switch(new_level){
        case 10:
            {
                if(old_level < 10){
                    cardid = 1553024;
                }
                break;
            }
        case 20:
            {
                if(old_level < 20){
                     cardid = 1553025;
                }
                break;
            }
        case 30:
            {
                if(old_level < 30){
                    cardid = 1553026;
                }
                break;
            }
        case 40:
            {
                if(old_level < 40){
                    cardid = 1553027;
                }
                break;
            }
    }
    if(cardid != 0){
		 add_count(userid, cardid, 1, 99);		
		 p_offset->cardid = cardid;
		 ++p_offset;
		 ++(*count);
    }
    return 0;
}

int Cuser_battle_articles::get_pass_boss_achivement_card(userid_t userid, uint32_t pass_count, 
		uint32_t comp_degree, uint32_t barrier_id, uint32_t *count, 
		user_battle_over_info_out_item_1* &p_offset)
{
	uint32_t barrier_card[] = {
		1553028, 1553029, 1553030, 1553031, 1553035, 1553036, 1553037, 1553038, 1553073, 1553074, 1553075,
			1553076, 1553077, 1553078, 1553079, 1553080, 1553104, 1553105, 1553106, 1553107, 1553108, 1553109,
		   	1553110,1553111
	};
	uint32_t other_card[] = {
		1553133,1553134,1553135,1553136,1553137,1553138,1553139,1553140
	};
	if(pass_count < 100 && (pass_count + comp_degree) >= 100){
		if(barrier_id < 35){

			add_count(userid, barrier_card[barrier_id - 1], 1, 99);
			p_offset->cardid = barrier_card[barrier_id - 1];
		}
		else if(barrier_id >= 35 && barrier_id <= 42){
			add_count(userid, other_card[barrier_id - 35], 1, 99);
			p_offset->cardid = other_card[barrier_id - 35];
		}
		++p_offset;
		++(*count);
	}

	return 0; 
}
