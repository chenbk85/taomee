/*
 * =====================================================================================
 *
 *       Filename:  Ccup.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "Croominfo.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>
using namespace std;
//#define INIT_SHM_FLAG  -100 
//createDB_ROOMINFO_0
Croominfo::Croominfo (mysql_interface * db ) :CtableRoute10x10( db, "ROOMINFO","t_roominfo","room_id" )
{ 


}


int Croominfo::insert(userid_t userid )
{
	sprintf(this->sqlstr, "insert into %s(room_id) values(%u)",
		this->get_table_name(userid),
		userid );
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}

int Croominfo::get_hot_db(userid_t userid, roominfo_get_hot_out * p_out)
{
	sprintf( this->sqlstr, "select room_hot, room_flower, room_bug from %s where room_id=%d", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->hot);
		INT_CPY_NEXT_FIELD(p_out->flower);
		INT_CPY_NEXT_FIELD(p_out->mud);
	STD_QUERY_ONE_END();
}

int Croominfo::get_hot(userid_t userid, roominfo_get_hot_out * p_out)
{
	int ret=this->get_hot_db(userid,p_out );
	if (ret!=SUCC) return ret;

	if (p_out->mud>VALUE_MAX)
	{
		this->init_mud(userid);			
		p_out->mud=0;
	}
	return ret;
}

int Croominfo::add_value(userid_t userid, char * filed_name )
{
	sprintf( this->sqlstr, " update %s set  %s=%s+1 where room_id=%u " ,
		this->get_table_name(userid), 
		filed_name,filed_name	,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo::get_value(userid_t userid, char * filed_name ,uint32_t  * p_value)
{
	sprintf( this->sqlstr, "select %s from %s where room_id=%d", 
			 filed_name,this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value );
	STD_QUERY_ONE_END();
}

int  Croominfo::vote_room (userid_t userid ,roominfo_vote_room_in *p_in )
{
	char filedname[100];
	if ( p_in->flag==1 ){
		strcpy(filedname, "room_flower" );
	}else{
		strcpy(filedname, "room_bug" );
	}
	return this->add_value(userid, filedname);
}

int Croominfo::get_vote_list(userid_t userid,const char * field_str, roominfo_pug_list_out *p_out )
{
    memset(p_out,0,sizeof(*p_out));
    sprintf( this->sqlstr, "select  %s from %s where room_id=%d",
             field_str ,this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
            BIN_CPY_NEXT_FIELD (p_out, sizeof ( *p_out) );
			if (p_out->count>ROOMINFO_PUG_LIST_LEN){
				p_out->count=0;
			}
    STD_QUERY_ONE_END();
}

int Croominfo::add_vote(userid_t userid, const char * field_str, rooninfo_pug_item *p_item )
{
	int ret;
	uint16_t i;
	roominfo_pug_list_out out;
	ret=this->get_vote_list(userid, field_str, &out);	

	if ( ret!=SUCC) return ret;	

	for (i=0;i<out.count;i++){
		if(out.item[i].userid==p_item->userid){
			break;	
		}
	}
	if( i==out.count)
		out.count++;
	if (i==ROOMINFO_PUG_LIST_LEN) i=ROOMINFO_PUG_LIST_LEN-1;
	for (;i>0;i--){
		memcpy(&(out.item[i]),&(out.item[i-1]),sizeof(out.item[0]));
	}
	memcpy(&(out.item[0]),p_item,sizeof(out.item[0]));
	if (out.count>ROOMINFO_PUG_LIST_LEN ) out.count=ROOMINFO_PUG_LIST_LEN-1;

	return this->update_votelist(userid, field_str, &out );
}

int Croominfo::update_votelist(userid_t userid ,const char * field_str , roominfo_pug_list_out * p_list )
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];

	set_mysql_string(mysql_list,(char*)p_list, 
			4+sizeof(p_list->item[0])* p_list->count);
	sprintf( this->sqlstr, " update %s set %s='%s' \
		where room_id=%u " ,
		this->get_table_name(userid), 
		field_str,
		mysql_list,
		userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo::add_hot(userid_t userid )
{
	sprintf( this->sqlstr, " update %s set room_hot=room_hot+1,\
			weekhot=weekhot+1,dayhot=dayhot+1 \
		where room_id=%u " ,
		this->get_table_name(userid), userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo::init_mud(userid_t userid)
{
    sprintf(this->sqlstr, "update %s set room_bug=0 where room_id=%u", 
			this->get_table_name(userid), userid);
    STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Croominfo::del_mud(userid_t userid)
{
    sprintf(this->sqlstr, "update %s set room_bug=room_bug-1 where room_id=%u", 
			this->get_table_name(userid), userid);
    STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);
}

int Croominfo::set_value(userid_t userid,roominfo_set_info_in *p_in )
{
	sprintf( this->sqlstr, " update %s set  room_flower=%u, room_bug=%u  where room_id=%u " ,
		this->get_table_name(userid), 
			p_in->flower,
			p_in->mud, userid );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo::get_total_time(userid_t userid, uint32_t *p_out )
{
	uint32_t ret = this->get_work_time(userid, p_out);
	if (ret != SUCC) {
		ret = this->insert(userid);
		*p_out = 0;
	}
	return ret;
}

int Croominfo::get_work_time(userid_t userid, uint32_t *p_out )
{
	sprintf( this->sqlstr, "select work_time from %s where  room_id=%d ", 
			 this->get_table_name(userid),
			 userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD (*p_out);
	STD_QUERY_ONE_END();
}

int Croominfo::update_work_time( userid_t userid) 
{
	sprintf( this->sqlstr, " update %s set work_time = work_time + 1 where  room_id=%u" ,
			this->get_table_name(userid), 
			userid
		   );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

int Croominfo::add_field(userid_t userid, const char *field, uint32_t add, uint32_t *p_out)
{
	uint32_t ret = this->change_int_field_without_max_check(userid, field, add, p_out);
	if (ret != SUCC) {
		if (ret == 1105) {
			ret = this->insert(userid);
			if (ret != SUCC) {
				return ret;
			}
			ret = this->change_int_field_without_max_check(userid, field, add, p_out);
			return ret;
		} else {
			return ret;
		}
	}
	return SUCC;
}

int Croominfo::get_score_candy(userid_t userid, roominfo_get_score_candy_out * p_out)
{
	sprintf( this->sqlstr, "select candy from %s where room_id=%d", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->candy);
	STD_QUERY_ONE_END();
}

int Croominfo::add_score(userid_t userid, uint32_t add_score)
{
	uint32_t ret = this->update_score(userid, add_score);
	if (ret != SUCC) {
		if (ret == 1105) {
			ret = this->insert(userid);
			if (ret != SUCC) {
				return ret;
			}
			ret = this->update_score(userid, add_score);
			return ret;
		} else {
			return ret;
		}
	}
	return SUCC;
}

int Croominfo::update_score( userid_t userid, uint32_t add_score) 
{
	sprintf( this->sqlstr, " update %s set score = score + %u, total_score = total_score + %u where  room_id=%u" ,
			this->get_table_name(userid), 
			add_score,
			add_score,
			userid
		   );
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR );	
}

/* @brief 得到合成列表信息 
 * @param userid 米米号 
 * @param p_out 输出的列表信息
 */
int Croominfo::get_merge(userid_t userid, roominfo_get_merge_buf *p_out)
{
	sprintf( this->sqlstr, "select merge_list from %s where room_id=%d", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(roominfo_get_merge_buf));
	STD_QUERY_ONE_END();
}
 
/* @brief 更新合成列表
 * @param userid 米米号
 * @param p_list 要更新的合成列表
 */ 
int Croominfo::update_merge(userid_t userid, roominfo_get_merge_buf *p_list ) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list, (char*)p_list, sizeof(*p_list));
	sprintf( this->sqlstr, " update %s set merge_list ='%s' \
			where  room_id=%u " ,
			this->get_table_name(userid), 
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/* @brief 更新合成表里的信息，如果不存在就插入一条记录 
 * @param userid 米米号  
 * @param p_list 要保存的列表信息
 */
int Croominfo::insert_merge(userid_t userid, roominfo_get_merge_buf *p_list)
{
	uint32_t ret = this->update_merge(userid, p_list);
	if (ret == 1105) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_merge(userid, p_list);
	}
	return ret;
}

/* @brief 检查是否有糖果未领取
 * @param userid 用户的米米号
 * @param p_flag 返回是否有糖果未领取，0表示未领取，1表示已经领取
 */
int Croominfo::check_candy(userid_t userid, uint32_t *p_flag)
{
	roominfo_get_merge_buf out = {};
	this->get_merge(userid, &out);
	if (out.count != 0) {
		*p_flag = 1;
	}
	return SUCC;
}

int Croominfo :: get_candy_score(userid_t userid, roominfo_get_user_all_info_web_out *p_out)
{
	sprintf( this->sqlstr, " select candy_count, prop_count  from %s where room_id=%d", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->candy_count);
		INT_CPY_NEXT_FIELD(p_out->prop_count);
	STD_QUERY_ONE_END();
}

/**
 * @brief 更新糖果数和道具数
 * @param userid 用户的米米号
 * @param candy 是否增加糖果数
 * @param prop 是否增加道具
 */
int Croominfo :: change_candy_prop(userid_t userid, uint32_t flag, uint32_t candy, uint32_t prop, uint32_t *p_flag, uint32_t *p_prop)
{
	uint32_t init_candy = 0, init_prop = 0;
	this->get_candy_prop(userid, &init_candy, &init_prop);
	if (candy == 1) {
		init_candy += 1;
	}
	uint32_t prop_num = 0;
	if (prop == 1) 
	{
		prop_num = (init_prop & 0X3FFFFFFF);
		if (((init_prop & 0x80000000) != 0) && ((init_prop & 0X40000000) == 0)) {
			init_candy += 30;
			if (prop_num > 1) {
				prop_num -=1;
				init_prop -= 1;
			}
			init_prop |= 0X40000000;
		}
		if ((init_prop & 0X80000000) == 0) {
			if ((init_candy + prop_num * 30) >= 100) {
				*p_flag = 1;
			}
		}
		if ((flag == 0) && (*p_flag == 1)) {
			*p_flag = 2;
		} else if (*p_flag == 1) {
			init_prop |= 0X80000000;
		}
	}
	if (*p_flag != 1) {
		if (prop_num >= 3) {
			*p_prop = 2;
		} else {
			if (prop == 1) {
				if (init_candy < 30) {
					*p_prop = 1;
				} else {
					init_candy -= 30;
					init_prop += 1;
				}
			}
		}
	}
	uint32_t ret = this->update_candy_prop(userid, init_candy, init_prop);
	if (ret == USER_ID_NOFIND_ERR) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_candy_prop(userid, init_candy, init_prop);
	}
	return ret;
}

int Croominfo::update_candy_prop( userid_t userid, uint32_t candy_count, uint32_t prop_count) 
{
	sprintf( this->sqlstr, " update %s set candy_count = %u, prop_count = %u where  room_id=%u" ,
			this->get_table_name(userid), 
			candy_count,
			prop_count,
			userid
		   );
	STD_SET_RETURN(this->sqlstr,userid, USER_ID_NOFIND_ERR);	
}

int Croominfo :: get_candy_prop(userid_t userid, uint32_t *p_candy, uint32_t *p_prop)
{
	sprintf( this->sqlstr, " select candy_count, prop_count  from %s where room_id=%d", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_candy);
		INT_CPY_NEXT_FIELD(*p_prop);
	STD_QUERY_ONE_END();
}

/* WEB页面用 */
int Croominfo::update_user_info(userid_t userid, roominfo_set_user_all_info_web_in *p_in)
{
	if (p_in->flag == 1) {
		p_in->prop_count |= 0x80000000;
	}
	sprintf( this->sqlstr, " update %s set candy_count = %u, prop_count = %u where  room_id=%u" ,
			this->get_table_name(userid), 
			p_in->candy_count,
			p_in->prop_count,
			userid
		   );
	STD_SET_RETURN(this->sqlstr,userid, USER_ID_NOFIND_ERR);	
}

int Croominfo :: check_candy_list(userid_t userid, userid_t get_userid)
{
	roominfo_candy_list out={ };
	this->get_candy_list(userid, &out);
	if (out.people_count >= 100) {
		return FETCH_CANDY_OUT_OF_MAX_ERR;
	}
	if (out.fetch_count >= 100) {
		return FETCH_CANDY_OUT_OF_MAX_ERR;
	}
	uint32_t i = 0;
	for (i = 0; i < out.people_count; i++) {
		if (out.item[i].userid == get_userid) {
			break;
		}
	}
	if (i == out.people_count) {
		out.item[out.people_count].userid = get_userid;
		out.item[out.people_count].count = 1;
		out.people_count++;
	} else {
		if (out.item[i].count >= 5) {
			return YOU_HAVE_GOT_MANY_TIME_ERR;
		}
		out.item[i].count++;
	}
	out.fetch_count++;
	uint32_t ret = this->insert_candy_list(userid, &out);
	return ret;
}

int Croominfo :: get_candy_list(userid_t userid, roominfo_candy_list *p_out)
{
	sprintf( this->sqlstr, " select candy_list from %s where room_id=%d", 
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(roominfo_candy_list));
	STD_QUERY_ONE_END();
}

/**
 * @brief 更新合成列表
 * @param userid 米米号
 * @param p_list 要更新的合成列表
 */ 
int Croominfo::update_candy_list(userid_t userid, roominfo_candy_list *p_list) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list, (char*)p_list, sizeof(*p_list));
	sprintf( this->sqlstr, " update %s set candy_list ='%s' \
			where  room_id=%u " ,
			this->get_table_name(userid), 
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/**
 * @brief 更新合成表里的信息，如果不存在就插入一条记录 
 * @param userid 米米号  
 * @param p_list 要保存的列表信息
 */
int Croominfo::insert_candy_list(userid_t userid, roominfo_candy_list *p_list)
{
	uint32_t ret = this->update_candy_list(userid, p_list);
	if (ret == 1105) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_candy_list(userid, p_list);
	}
	return ret;
}

/**
 * @brief 取得数据库中的数据，从整体上进行
 * @para user_id 米米号
 * @para out 用户输出的结构体
 */
int Croominfo::dressing_mark_get_all(const userid_t user_id, DressingMarkRecord& out)
{
	sprintf( this->sqlstr, " select dressing_mark from %s where room_id=%d", 
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(&out, sizeof(out));
	STD_QUERY_ONE_END();
}

/**
 * @brief 把数据写到数据库，从整体上进行
 * @para user_id 米米号
 * @para indata 需要输入的数据
 */
int Croominfo::dressing_mark_set_all(const userid_t user_id, const DressingMarkRecord& indata)
{
	DressingMarkRecord re=indata;
	DEBUG_LOG("dressing_mark_record, next_index %u", re.next_index);

	char mysql_list[mysql_str_len(sizeof (indata))];
	set_mysql_string(mysql_list, (char*)(&indata), sizeof(indata));
	sprintf(this->sqlstr, " update %s set dressing_mark='%s' \
			where  room_id=%u " ,
			this->get_table_name(user_id), 
			mysql_list,
			user_id
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Croominfo::dressing_mark_get(const userid_t user_id, const db_utility::DressingUnit& dress, roominfo_roominfo_dressing_mark_get_out& out)
{
	DressingMarkRecord record={};
	int ret=dressing_mark_get_all(user_id, record);
	if(SUCC!=ret)
	{
		std::memset(&out, 0, sizeof(out));
		return ret;
	}

	const DressingMarkUnit* endp=record.record+8; //只允许有打分8次
	const DressingMarkUnit* findp=::find(record.record, endp, dress);
	if(findp==endp)
	{
		std::memset(&out, 0, sizeof(out));
		return ROOMINFO_ROOMINFO_DRESSING_MARK_NO_UNIT_ERR;
	}

	out=findp->mark;

	return SUCC;
}

int Croominfo::dressing_mark_set(const userid_t user_id, const DressingMarkUnit& unit)
{
	DressingMarkRecord record={};
	int ret=dressing_mark_get_all(user_id, record);
	if(SUCC!=ret)
	{
		return ret;
	}

	//因为数据初始化有问题，所有作为特殊处理
	if (record.next_index > 7)
	{
		std::memset(&record, 0, sizeof(DressingMarkRecord));
	}

	const DressingMarkUnit* endp=record.record+8;
	const DressingMarkUnit* findp=::find(record.record, endp, unit.dress);
	if(findp!=endp) //找到了
	{
		return SUCC;
	}

	record.record[record.next_index] = unit;
	record.next_index = (record.next_index + 1) % 8;

	return dressing_mark_set_all(user_id, record);
}


int Croominfo::duck_statu_get(const userid_t user_id, uint32_t& statu)
{
	sprintf( this->sqlstr, " select duck_statu from %s where room_id=%d", 
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(&statu, sizeof(statu));
	STD_QUERY_ONE_END();
}

int Croominfo::duck_statu_set(const userid_t user_id, const uint32_t statu)
{
	sprintf( this->sqlstr, " update %s set duck_statu=%u where room_id=%d", 
			this->get_table_name(user_id),
			statu,
			user_id
		   );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/**
 * @brief 得到领过袜子的人的ID号
 * @param userid 米米号
 * @param p_out 保存返回的领取列表
 */
int Croominfo::select_socks_sql(userid_t user_id, stru_roominfo_socks *p_out)
{
	sprintf( this->sqlstr, " select socks from %s where room_id=%d", 
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(stru_roominfo_socks));
	STD_QUERY_ONE_END();
}

/**
 * @brief 更新领取过袜子的人的列表 
 * @param usrid 米米号
 * @param p_list 保存领取过袜子的人的ID号 
 */
int Croominfo::update_socks_sql(userid_t userid, stru_roominfo_socks *p_list) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list, (char*)p_list, sizeof(*p_list));
	sprintf( this->sqlstr, " update %s set socks = '%s' where  room_id = %u" ,
			this->get_table_name(userid), 
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/**
 * @brief 判断这个用户是否领取过袜子
 * @param userid 用户的米米号
 */
int Croominfo::get_socks(userid_t userid, userid_t other_userid)
{
	stru_roominfo_socks temp = { };
	uint32_t ret = this->select_socks_sql(userid, &temp);
	if (ret != SUCC) {
		ret = this->insert(userid);
	    if (ret != SUCC) {
		    return ret;
	    }
   } 	   
   for(uint32_t i = 0; i < temp.count; i++) {
	   if (temp.items[i] == other_userid) {
		   return HAVE_BEEN_GOT_SOCKES_ERR;
	   }
   }
	if (temp.count >= 99) {
		temp.count = 0;
	}
	temp.items[temp.count] = other_userid;
	temp.count++;
	ret = this->update_socks_sql(userid, &temp);
   return ret;
}

/**
 * @brief 更新心愿字节 
 * @param usrid 米米号
 * @param p_list 保存心愿的结构体 
 */
int Croominfo::update_wish_sql(userid_t userid, stru_roominfo_wish *p_list) 
{
	char mysql_list[mysql_str_len(sizeof (*p_list))];
	set_mysql_string(mysql_list, (char*)p_list, sizeof(*p_list));
	sprintf( this->sqlstr, " update %s set wish = '%s' where  room_id = %u" ,
			this->get_table_name(userid), 
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}


/**
 * @brief 得到心愿内容
 * @param userid 米米号
 * @param p_out 保存返回的内容
 */
int Croominfo::select_wish_sql(userid_t user_id, stru_roominfo_wish *p_out)
{
	sprintf( this->sqlstr, " select wish from %s where room_id=%d", 
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(stru_roominfo_socks));
	STD_QUERY_ONE_END();
}

/**
 * @brief 更新某个字段的数值 
 * @param usrid 米米号
 * @param p_field 字段的名称
 * @param p_list 要更新字段的数据
 * @param len 要更字段的数值的结构体的长度
 */
int Croominfo::update_sql(userid_t userid, const char *p_field, void *p_list, size_t len) 
{
	char mysql_list[mysql_str_len(len)];
	set_mysql_string(mysql_list, (char*)p_list, len);
	sprintf( this->sqlstr, "update %s set %s = '%s' where  room_id = %u",
			this->get_table_name(userid), 
			p_field,
			mysql_list,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}


/**
 * @brief 得到某个字段的数值 
 * @param usrid 米米号
 * @param p_field 字段的名称
 * @param p_list 获取字段的数据
 * @param len 要获取字段的数值的结构体的长度
 */
int Croominfo::select_sql(userid_t user_id, const char *p_field, void *p_out, size_t len)
{
	sprintf( this->sqlstr, "select %s from %s where room_id=%d", 
			p_field,
			this->get_table_name(user_id),
			user_id
		   );
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, len);
	STD_QUERY_ONE_END();
}

/**
 * @brief 记录谁送过礼物
 * @param userid 米米号
 * @param p_list 送礼物的USERID和昵称
 */
int Croominfo::add_gift_userid(userid_t userid, roominfo_record_give_gift_userid_in *p_list)
{
	roominfo_give_gift_userid temp = { };
	uint32_t ret = this->select_sql(userid, "gift_userid", &temp, sizeof(roominfo_give_gift_userid));
	if (ret != SUCC) {
		if (ret == USER_ID_NOFIND_ERR) {
			ret = this->insert(userid);
			if (ret != SUCC) {
				return ret;
			}
		} else {
			return ret;
		}
	}
	if (temp.count >= 100) {
		return VALUE_OUT_OF_RANGE_ERR;
	}	
	memcpy(&temp.items[temp.count], p_list, sizeof(*p_list));
	temp.count++;
	ret = update_sql(userid,"gift_userid", &temp, temp.get_real_len());
	return ret;
}

/**
 * @brief 获取那个用户送过礼物，并且重记录中删除这个用户
 * @param userid 米米号
 * @param p_out 返回送礼物的USERID和昵称。
 */
int Croominfo::del_gift_userid(userid_t userid, roominfo_get_give_gift_userid_out *p_out)
{
	roominfo_give_gift_userid temp = { };
	uint32_t ret = this->select_sql(userid,"gift_userid", &temp, sizeof(roominfo_give_gift_userid));
	if ((ret != SUCC) && (ret != USER_ID_NOFIND_ERR)) {
		return ret;
	}
	if (temp.count == 0) {
		return NO_BODY_GIVE_GIFT_ERR;
	}
	temp.count--;
	memcpy(p_out, &temp.items[temp.count], sizeof(roominfo_record_give_gift_userid_in));	
	ret = this->update_sql(userid, "gift_userid", &temp, sizeof(roominfo_give_gift_userid));
	return ret;
}

/**
 * @breif 更新两个字段的数值
 * @param userid 米米号
 * @param p_col1 字段一的名称
 * @param p_col2 字段二的名称
 * @param col1_value 字段一的数值
 * @param col2_value 字段二的数值
 */
int Croominfo::update_two_col_sql(userid_t userid, const char *p_col1, const char *p_col2,
	          uint32_t col1_value, uint32_t col2_value)
{
	sprintf( this->sqlstr, " update %s set %s = %u, %s = %u where  room_id = %u" ,
			this->get_table_name(userid), 
			p_col1,
			col1_value,
			p_col2,
			col2_value,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/**
 * @brief 取两个字段的数值
 * @param userid 用户的米米号
 * @param p_col1 字段一的名称
 * @param p_col2 字段二的名称
 * @param p_col1_value 字段一的返回数值
 * @param p_col2_value 字段二的返回数值
 */
int Croominfo::select_two_col_sql(userid_t userid, const char *p_col1, const char *p_col2,
	           uint32_t *p_col1_value, uint32_t *p_col2_value)
{
	sprintf( this->sqlstr, "select %s, %s from %s where room_id=%d", 
			p_col1,
			p_col2,
			this->get_table_name(userid),
			userid
		   );
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_col1_value);
		INT_CPY_NEXT_FIELD(*p_col2_value);
	STD_QUERY_ONE_END();
}

/**
 * @brief 增加能量之星，VIP每天上限为99，非VIP上限为50
 * @param userid 米米号
 * @param add_value 增加的数值
 * @param vip_flag 是否是VIP标志
 * @param p_out 返回值，返回总数，当天的总数，实际增加的数量 
 */
int Croominfo::add_energy_star(userid_t userid, uint32_t add_value, uint32_t vip_flag, roominfo_add_energy_star_out *p_out)
{
	uint32_t ret = this->select_two_col_sql(userid, "energy_star", "day_star", &p_out->total_star, &p_out->day_star);
	if ((ret != SUCC) && (ret != USER_ID_NOFIND_ERR)) {
		return ret;
	}
	p_out->day_star += add_value;
	p_out->total_star += add_value;
	p_out->real_add = add_value;
	/* vip用户 */
	if (vip_flag) {
		if (p_out->day_star > 99) {
			p_out->real_add = add_value - (p_out->day_star - 99);
			p_out->total_star -= (p_out->day_star - 99);
			p_out->day_star = 99;
		} 
	} else {
		if (p_out->day_star > 50) {
			p_out->real_add = add_value - (p_out->day_star - 50);
			p_out->total_star -= (p_out->day_star - 50);
			p_out->day_star = 50;
		}
	}
	if (p_out->real_add == 0) {
		return ENERG_STAR_OUT_OF_RANGE_ERR;
	}
	ret = this->update_two_col_sql(userid, "energy_star", "day_star", p_out->total_star, p_out->day_star);
	if (ret == USER_ID_NOFIND_ERR) {
		ret = this->insert(userid);
		if (ret != SUCC) {
			return ret;
		}
		ret = this->update_two_col_sql(userid, "energy_star", "day_star", p_out->total_star, p_out->day_star);
	}
	return ret;
}

int Croominfo::get_scores(userid_t userid, uint32_t& score, uint32_t& today_score)
{
	sprintf( this->sqlstr, "select score, today_score from %s where room_id=%d", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(score);
		INT_CPY_NEXT_FIELD(today_score);
	STD_QUERY_ONE_END();
}

int Croominfo::get_hotlist_date(userid_t userid, uint32_t &date, roominfo_hot_list *visits)
{
	sprintf(this->sqlstr, "select date, hotlist from %s where room_id = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(date);
        BIN_CPY_NEXT_FIELD ((char*)visits, sizeof(roominfo_hot_list) );
	STD_QUERY_ONE_END();
}

int Croominfo::update_hotlist(userid_t userid, roominfo_hot_list *visits)
{
	char mysql_hot[mysql_str_len(sizeof(roominfo_hot_list))];
	memset(mysql_hot, 0, sizeof(mysql_hot));
	set_mysql_string(mysql_hot, (char*)visits, sizeof(roominfo_hot_list));
	sprintf(this->sqlstr, "update %s set hotlist = '%s' where room_id = %u",
			this->get_table_name(userid),
			mysql_hot,
			userid
			);

    STD_SET_RETURN_EX(this->sqlstr, USER_ID_EXISTED_ERR);
}
/*
int Croominfo::reward_num_update(userid_t userid, int reward_num)
{
	sprintf( this->sqlstr, " update %s set reward_num = reward_num + %d where  room_id = %u" ,
			this->get_table_name(userid), 
			reward_num,
			userid
		  );
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

int Croominfo::reward_num_select(userid_t userid, uint32_t *p_reward_num)
{
	sprintf( this->sqlstr, "select reward_num from %s where room_id=%d", 
			this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_reward_num);
	STD_QUERY_ONE_END();
}*/
