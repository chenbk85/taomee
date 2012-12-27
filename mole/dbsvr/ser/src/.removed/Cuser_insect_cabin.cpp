/*
 * =====================================================================================
 *
 *       Filename:  Cuser_insect_cabin.cpp
 *
 *    Description:  蝴蝶等昆虫的信息追踪，记录一个昆虫的信息，出生，状态，工作等等情况。
 *
 *        Version:  1.0
 *        Created:  2009年12月07日 10时21分32秒
 *       Revision:  none
 *       Compiler:  gcc -Wall -Wextra Cuser_butterfly.cpp
 *
 *         Author:  easyeagel (LiuGuangBao), easyeagel@gmx.com
 *        Company:  淘米网络-taomee.com
 *
 * =====================================================================================
 */

#include<ctime>
#include<sstream>

#include"Cuser_insect_cabin.h"

namespace
{
struct TransferUnit
{
	uint32_t insect_id;
	uint32_t time_limit;
};

const uint32_t bufferfly_id=0;

//TODO:把animal_id设置为真实的ID
TransferUnit insect_transfer_time[]=
{
	{bufferfly_id, 24},//蝴蝶
};

int get_time(int x)
{
	//const uint32_t seconds=60*60;
	return insect_transfer_time[x].time_limit;
}

}

/**
 * @brief 在昆虫小屋里插入一个昆虫
 * @para user_id 哪个用户的昆虫
 * @para insect_id 插入哪种昆虫
 * @para status 把当前昆虫的状态设置为
 */
int Cuser_insect_cabin::sql_insert(const userid_t user_id, const uint32_t insect_id, const uint8_t stat)
{
	std::time_t current_time=std::time(0);
	struct tm  t= *(std::gmtime(&current_time));
	uint32_t today=t.tm_yday;
	sprintf(this->sqlstr, "insert into %s"
		"(userid, insectid, begintime, insectstat, numberoftoday, dateoftoday) values"
		"(%u, %u, %u, %u, %u, %u)",
		this->get_table_name(user_id),
		user_id, insect_id, (uint32_t)current_time, stat, 0, today);

	STD_INSERT_RETURN(this->sqlstr, DB_ERR);
}

int Cuser_insect_cabin::sql_select_all(const userid_t user_id, uint32_t* pcount, InsectInfo** list)
{
	sprintf(this->sqlstr, "select autoid, insectid, insectstate, numberoftoday from %s where userid=%u", 
		this->get_table_name(user_id),
		user_id);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, list, pcount);
		INT_CPY_NEXT_FIELD((*list + i)->inner_id);
		INT_CPY_NEXT_FIELD((*list + i)->insect_id);
		INT_CPY_NEXT_FIELD((*list + i)->insect_state);
		INT_CPY_NEXT_FIELD((*list + i)->num_of_today);
	STD_QUERY_WHILE_END();
}

int Cuser_insect_cabin::sql_update_state(const userid_t user_id, const std::vector<uint32_t>& id_list, const uint32_t state)
{
	std::ostringstream stm;
	std::vector<uint32_t>::const_iterator b=id_list.begin();
	for(size_t i=0; i<id_list.size() ; ++b, ++i)
	{
		stm << *b;
		if(i<id_list.size()-1)
		{
			stm << ',';
		}
	}

	sprintf(this->sqlstr, "update %s set insectstate=%u where autoid in (%s)",
		this->get_table_name(user_id),
		state,
		stm.str().c_str());
	
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Cuser_insect_cabin::sql_select(const userid_t user_id, const uint32_t inner_id, InsectInfo& info)
{
	int ret=0;
	sprintf(this->sqlstr, "select autoid, insectid, begintime, insectstate, numberoftoday , dateoftoday from %s where autoid=%u", 
		this->get_table_name(user_id),
		inner_id);

	uint32_t begin_time=0;
	uint32_t today=0;
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(info.inner_id);
		INT_CPY_NEXT_FIELD(info.insect_id);
		INT_CPY_NEXT_FIELD(begin_time);
		INT_CPY_NEXT_FIELD(info.insect_state);
		INT_CPY_NEXT_FIELD(info.num_of_today);
		INT_CPY_NEXT_FIELD(today);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	//针对蝴蝶进行数据更新
	if(bufferfly_id==info.insect_id )
	{
		std::time_t ct=std::time(0);
		if(info.insect_state!=2)//没有成虫
		{
			//处理茧阶段
			uint32_t left=begin_time;
			uint32_t right=begin_time + get_time(bufferfly_id);
			uint32_t old_state=info.insect_state;
			if((uint32_t)ct < left)
			{
				info.insect_state=0;//幼虫
			} else if((uint32_t)ct > left && (uint32_t)ct < right) {
				info.insect_state = 1;//茧
			} else if( (uint32_t)ct > right ) {
				info.insect_state = 2;//成虫
			}	

			if(info.insect_state!= old_state)
			{
				ret=sql_update_state(user_id, info.inner_id, info.insect_state);
				if(SUCC!=ret)
				{
					return ret;
				}
			}
		}
		else
		{
			struct tm  t= *(std::gmtime(&ct));
			if(t.tm_yday!=(int)today)
			{
				today=t.tm_yday;
			}
		}
	}

}

int Cuser_insect_cabin::insect_get_all(const userid_t user_id,
		user_insect_cabin_get_all_out_header* header, user_insect_cabin_get_all_out_item** list)
{
	return sql_select_all(user_id, &(header->count), list);
}

