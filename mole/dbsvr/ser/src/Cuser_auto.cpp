/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_card.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *
 * =====================================================================================
 */

#include "Cuser_auto.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include "msglog.h"

/* 汽车总类的ID号, 汽车的初始油量，汽车的引擎，汽车的颜色，汽车的道具孔数目 */
auto_attire auto_info[] =  {
							 {1300001, 10,  1,  1, 0},
							 {1300002, 10,  1,  1, 0},
							 {1300003, 10,  1,  1, 0},
							 {1300004, 10,  1,  1, 0},
							 {1300005, 10,  1,  1, 0},
							 {1300006, 10,  1,  1, 0},
							 {1300007, 20,  1,  1, 0},
							 {1300008, 10,  1,  1, 0},
							 {1300009, 9,   1,  1, 0},
							 {1300010, 10,  1,  1, 0},
							 {1300011, 10,  1,  1, 0}, 
							 {1300012, 10,  1,  1, 0}, 
							 {1300013, 10,  1,  1, 0}, 
							 {1300014, 10,  1,  1, 0}, 
							};
/* 引擎ID号，对应的油耗 */
engine_and_oil engine_oil[] = {
								{1, 1}
							  };

#define ENGINE_NUM 	(sizeof(engine_oil) / sizeof(engine_oil[1]))
#define BASE_ENGINE 1

#define AUTO_NUM ((sizeof(auto_info) / sizeof(auto_info[1])))

#define BASE_AUTO_ID 1300001 

/* @brief 类构造函数
 * @param db 数据库句柄
 */
Cuser_auto::Cuser_auto(mysql_interface * db, Citem_change_log *p_log ) 
	:CtableRoute100x10( db, "USER", "t_user_auto", "userid")
{ 
	
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
	this->p_item_change_log = p_log;

}

/* @brief 插入一条汽车的记录
 * @param userid 米米号
 * @param p_in 汽车的传入的基本属性
 * @param p_id 返回新插入的记录的ID号
 */
int Cuser_auto::insert(userid_t userid, user_auto_insert_in *p_in, uint32_t *p_id)
{
	
	uint32_t index = p_in->auto_id - BASE_AUTO_ID;
	if (index >= AUTO_NUM || (p_in->auto_id == 1300005)) {
		return VALUE_OUT_OF_RANGE_ERR;
	}

	this->add_attire_msglog(userid, p_in->auto_id, 1);

	user_auto_propery propery = { };
	propery.count = auto_info[index].hole_num;
	char propery_mysql[mysql_str_len(sizeof (propery))];
	set_mysql_string(propery_mysql, (char *)(&propery), sizeof(propery));

	uint32_t now = time(NULL);
	sprintf(this->sqlstr, "insert into %s values(NULL, %u, %u, %u, %u, %u, %u, '%s', 0x0000000000000000)",
			this->get_table_name(userid),
			userid,
			p_in->auto_id,
			auto_info[index].oil,
			auto_info[index].engine,
			auto_info[index].color,
			now,
			propery_mysql
			);
	STD_INSERT_GET_ID(this->sqlstr, DB_ERR, *p_id);	
}

/* @brief 得到某个用户总共的汽车总数 
 * @param userid 米米号
 * @param p_out 返回汽车的总数
 */
int Cuser_auto::get_count(userid_t userid, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/* @brief 插入一条机车记录，并且校验是否超过最大记录数 
 * @param userid 米米号
 * @param p_in 机车的输入基本属性
 * @param p_id 返回汽车的ID号
 */
int Cuser_auto::add(userid_t userid, user_auto_insert_in *p_in, uint32_t *p_id, uint32_t is_vip_opt_type)
{
	uint32_t count = 0;
	uint32_t ret = this->get_count(userid, &count);
	if (ret != SUCC) {
		return ret;
	}
	if (count >= 100) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	 
	uint32_t temp_count = 0;
	ret = this->get_auto_class(userid, p_in->auto_id, &temp_count);
	if (ret != SUCC) {
		return ret;
	}
	if (temp_count != 0) {
		return YOU_HAVE_THIS_KIND_AUTO_ERR;
	}
	ret = this->insert(userid, p_in, p_id);
	/*
	 * 114D协议,下面是道具增减统计支持
	 */
	if(ret == SUCC){
		ret = this->p_item_change_log->add(time(NULL), is_vip_opt_type, userid, p_in->auto_id, 1);
	}
	return ret;
}


/* @brief 得到某个字段的值 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_type 选择的字段名称
 * @param p_out 返回选择的字段的值
 */
int Cuser_auto::get_attire(uint32_t id, userid_t userid, char *p_type, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select %s from %s where id = %u",
			p_type,
			this->get_table_name(userid),
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/* @brief 更新某个字段的值
 * @param id 汽车的ID号
 * @param useid 米米号
 * @param type 1表示更新引擎，2表示更新颜色
 * @param new_type 更新的新值
 * @param p_out 旧值
 */
int Cuser_auto::update_attire(uint32_t id, userid_t userid, uint32_t type, uint32_t new_type, uint32_t *p_out)
{
	char buf[10];
	*p_out = 0;
	switch(type) {
		case 1:
			strcpy(buf, "engine");
			if ((new_type - BASE_ENGINE) >= ENGINE_NUM) {
				return VALUE_OUT_OF_RANGE_ERR;
			}
			break;
		case 2:
			strcpy(buf, "color");
			break;
		default:
			return VALUE_OUT_OF_RANGE_ERR;
			break;
	}
	if (type == 1) {
		uint32_t ret = this->get_attire(id, userid, buf, p_out);
		if (ret != SUCC) {
			return ret;
		}
	}
	sprintf(this->sqlstr, "update %s set %s = %u where id = %u",
			this->get_table_name(userid),
			buf,
			new_type,
			id
			);
	STD_SET_RETURN(this->sqlstr, userid, USER_ID_NOFIND_ERR);	
}

/* @brief 得到道具孔的信息 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_out 返回汽车的道具孔信息
 */
int Cuser_auto::get_propery(uint32_t id, userid_t userid, user_auto_propery *p_out)
{
	sprintf(this->sqlstr, "select property from %s where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out, sizeof(user_auto_propery));
	STD_QUERY_ONE_END();
}

/* @brief 更新道具孔信息 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_in 要更新的汽车孔的信息
 */
int Cuser_auto::update_propery(uint32_t id, userid_t userid, user_auto_propery *p_in)
{
	
	char propery_mysql[mysql_str_len(sizeof (p_in))];
	set_mysql_string(propery_mysql, (char *)(p_in), sizeof(p_in));
	sprintf(this->sqlstr, "update %s set property = '%s' where id = %u",
			this->get_table_name(userid),
			propery_mysql,
			id
			);
	STD_SET_RETURN(this->sqlstr,userid, USER_ID_NOFIND_ERR);
}

/* @brief 增加道具孔，增加道具，减少道具，更换道具 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param type 1表示增加道具孔，2表示增加道具，3表示减少道具，4表示交换道具
 * @param propery_id 要改变的道具孔
 * @param new_type 要改变的道具ID号
 * @param p_old 旧的道具ID号
 */
int Cuser_auto::change_propery(uint32_t id, userid_t userid, uint32_t type,
		uint32_t propery_id, uint32_t new_propery, uint32_t *p_old)
{
	user_auto_propery propery = {};
	uint32_t ret = get_propery(id, userid, &propery);
	if (ret != SUCC) {
		return ret;
	}
	switch(type) {
		case 1:/* 增加道具孔 */
			if (propery.count >= 4) {
				return VALUE_OUT_OF_RANGE_ERR;
			}
			propery.count++;
			*p_old = 0;
			break;
		case 2:/* 往道具孔里加道具 */
			if (propery_id > propery.count) {
				return VALUE_OUT_OF_RANGE_ERR;
			}
			if (propery.item[propery_id -1] != 0) {
				return USER_AUTO_ATTIRE_EXIST_ERR;
			}
			propery.item[propery_id - 1] = new_propery;
			*p_old = 0;
			break;
		case 3:/* 去除道具 */
			if (propery_id > propery.count) {
				return VALUE_OUT_OF_RANGE_ERR;
			}
			if (propery.item[propery_id - 1] == 0) {
				return USER_AUTO_ATTIRE_NOT_EXIST_ERR;
			}
			*p_old = propery.item[propery_id - 1];
			propery.item[propery_id - 1] = 0;
			break;
		case 4:/*交换道具*/
			if (propery_id > propery.count) {
				return VALUE_OUT_OF_RANGE_ERR;
			}
			if (propery.item[propery_id - 1] == 0) {
				return USER_AUTO_ATTIRE_NOT_EXIST_ERR;
			}
			*p_old = propery.item[propery_id - 1];
			propery.item[propery_id - 1] = new_propery;
			break;
		default:
			return VALUE_OUT_OF_RANGE_ERR;
			break;
		}
	ret = this->update_propery(id, userid, &propery);
	return ret;
}

/* @brief 得到某个字段的值
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_field 要得到的字段的值
 * @param p_out 输出字段的值
 */
int Cuser_auto::get_field(uint32_t id, userid_t userid, char *p_field, uint32_t *p_out)
{
	sprintf(this->sqlstr, "select %s from %s where id = %u",
			this->get_table_name(userid),
			p_field,
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_out);
	STD_QUERY_ONE_END();
}

/* @brief 更新某个字段的值 
 * @param id 汽车的ID号
 * @param p_field 要更新的美国字段的值
 * @param value 更新的值
 */
int Cuser_auto::update_field(uint32_t id, userid_t userid, char *p_field, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %u where id = %u",
			this->get_table_name(userid),
			p_field,
			value,
			id
			);
	STD_SET_RETURN(this->sqlstr, userid,USER_ID_NOFIND_ERR );	
}

/* @brief 得到oil,time, engine字段的值
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_oil 返回油量
 * @param p_time 返回上次计算的时间
 * @param p_engine 返回引擎值
 */
int Cuser_auto::get_oil_time(uint32_t id, userid_t userid, uint32_t *p_oil, uint32_t *p_time,
		                     uint32_t *p_engine, uint32_t *p_auto)
{
	sprintf(this->sqlstr, "select auto_id, oil, oil_time, engine from %s where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_auto);
		INT_CPY_NEXT_FIELD(*p_oil);
		INT_CPY_NEXT_FIELD(*p_time);
		INT_CPY_NEXT_FIELD(*p_engine);
	STD_QUERY_ONE_END();
}

/* @brief 更新oil,time字段的值 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param oil 更新油值
 * @param time 更新时间值
 */
int Cuser_auto::update_oil_time(uint32_t id, userid_t userid, uint32_t oil, uint32_t time)
{
	sprintf(this->sqlstr, "update %s set oil = %u, oil_time = %u where id = %u",
			this->get_table_name(userid),
			oil,
			time,
			id
			);
	STD_SET_RETURN(this->sqlstr, userid,USER_ID_NOFIND_ERR );	
}


/* @brief 更新oil,time并且计算出消耗的豆豆数值 
 * @param id 汽车的ID号
 * @param userid 用户的米米号
 * @param auto_id 汽车种类的ID号
 * @param p_xiaomee 返回要消耗的豆豆
 */
int Cuser_auto::add_oil(uint32_t id, userid_t userid, uint32_t *p_xiaomee)
{
	uint32_t oil = 0;
	uint32_t old_time = 0;
	uint32_t engine = 0;
	uint32_t auto_id = 0;
	uint32_t ret = this->get_oil_time(id, userid, &oil, &old_time, &engine, &auto_id);
	if (ret != SUCC) {
		return ret;
	}
	uint32_t engine_index = engine - BASE_ENGINE;
	if (engine_index >= ENGINE_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t now = time(NULL);
	uint32_t day = (now - old_time) / (3600 * 24);
	uint32_t index = auto_id - BASE_AUTO_ID;
	if (index >= AUTO_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	if ((day * engine_oil[engine_index].oil) >= oil) {
		*p_xiaomee = auto_info[index].oil * 20 + 100;
	} else {
		*p_xiaomee = (auto_info[index].oil - (oil - day * engine_oil[index].oil)) * 20;
	}
	ret = this->update_oil_time(id, userid, auto_info[index].oil, now);
	return ret;
}

/* @brief 计算消耗的油 
 * @param id
 * @param userid
 * @param auto_id
 */
int Cuser_auto::cal_oil(uint32_t id, userid_t userid, uint32_t auto_id, uint32_t *old_time, uint32_t *oil, uint32_t engine)
{
	uint32_t now = time(NULL);
	uint32_t day = (now - *old_time) / (3600 * 24);
	uint32_t index = engine - BASE_ENGINE;
	if (index >= ENGINE_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	if ((day * engine_oil[index].oil) >= *oil) {
		*oil = 0;
	} else {
		*oil -= day * engine_oil[index].oil;
	}
	*old_time += day * 3600 * 24;
	uint32_t ret = this->update_oil_time(id, userid, *oil, *old_time);
	return ret;
}

/* @brief 得到用户的全部汽车的信息 
 * @param userid 米米号
 * @param pp_list 返回用户汽车的信息
 * @param p_count 记录的条数
 */
int Cuser_auto::get_all_auto(userid_t userid, user_auto_get_all_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select id, auto_id, oil, engine, color, oil_time,\
			               property, adorn from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->auto_id);
		INT_CPY_NEXT_FIELD((*pp_list + i)->oil);
		INT_CPY_NEXT_FIELD((*pp_list + i)->engine);
		INT_CPY_NEXT_FIELD((*pp_list + i)->color);
		INT_CPY_NEXT_FIELD((*pp_list + i)->oil_time);
		BIN_CPY_NEXT_FIELD(&(*pp_list + i)->propery, sizeof(user_auto_propery));
		BIN_CPY_NEXT_FIELD(&(*pp_list + i)->adorn, sizeof(uint32_t) * 2);
	STD_QUERY_WHILE_END();
}

/* @brief 得到用户某一汽车的信息
 * @param userid 用户的米米号 
 * @param id 汽车的ID号
 * @param p_list 返回汽车的信息
 */
int Cuser_auto::get_one_auto_info(userid_t userid, uint32_t id, user_auto_get_one_info_out *p_list)
{
	sprintf(this->sqlstr, "select id, auto_id, oil, engine, color, oil_time,\
			               property, adorn from %s where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, AUTO_IS_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(p_list->id);
		INT_CPY_NEXT_FIELD(p_list->auto_id);
		INT_CPY_NEXT_FIELD(p_list->oil);
		INT_CPY_NEXT_FIELD(p_list->engine);
		INT_CPY_NEXT_FIELD(p_list->color);
		INT_CPY_NEXT_FIELD(p_list->oil_time);
		BIN_CPY_NEXT_FIELD(&(p_list)->propery, sizeof(user_auto_propery));
		BIN_CPY_NEXT_FIELD(&(p_list)->adorn, sizeof(uint32_t) * 2);
	STD_QUERY_ONE_END();
}


/* @brief 得到用户某一汽车的装饰
 * @param userid 用户的米米号 
 * @param id 汽车的ID号
 * @param p_list 返回汽车的信息
 */
int Cuser_auto::get_adron(userid_t userid, uint32_t id, stru_adron *p_list)
{
	sprintf(this->sqlstr, "select adorn from %s where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, AUTO_IS_NOT_EXIST_ERR);
		BIN_CPY_NEXT_FIELD(p_list, sizeof(stru_adron));
	STD_QUERY_ONE_END();
}


/* @brief 更新更新汽车的装饰 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param p_in 要更新的信息
 */
int Cuser_auto::update_adron(uint32_t id, userid_t userid, stru_adron *p_in)
{
	
	char adron_mysql[mysql_str_len(sizeof (p_in))];
	set_mysql_string(adron_mysql, (char *)(p_in), sizeof(p_in));
	sprintf(this->sqlstr, "update %s set adorn = '%s' where id = %u",
			this->get_table_name(userid),
			adron_mysql,
			id
			);
	STD_SET_RETURN(this->sqlstr,userid, USER_ID_NOFIND_ERR);
}



/* @brief 增加装饰，减少装饰，更换装饰 
 * @param id 汽车的ID号
 * @param userid 米米号
 * @param type 1表示增加装饰，2表示减少装饰，3表示交换装饰
 * @param propery_id 要改变的装饰孔
 * @param new_type 要改变的装饰ID号
 * @param p_old 旧的装饰ID号
 */
int Cuser_auto::change_adron(uint32_t id, userid_t userid, uint32_t type,
		uint32_t adron_id, uint32_t new_adron, uint32_t *p_old)
{
	stru_adron adron_info = {};
	uint32_t ret = get_adron(userid, id, &adron_info);
	if (ret != SUCC) {
		return ret;
	}
	if (adron_id > 2) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	switch(type) {
		case 1:/* 增加装饰 */
			if (adron_info.adron[adron_id - 1] != 0) {
				return ADORN_HAVE_EXIST_ERR;
			}
			adron_info.adron[adron_id - 1] = new_adron;
			*p_old = 0;
			break;
		case 2:/* 减少装饰 */
			if (adron_info.adron[adron_id -1] == 0) {
				return USER_AUTO_ATTIRE_NOT_EXIST_ERR;
			}
			*p_old = adron_info.adron[adron_id -1];
			adron_info.adron[adron_id - 1] = 0;
			break;
		case 3:/*交换装饰*/
			if (adron_info.adron[adron_id - 1] == 0) {
				return USER_AUTO_ATTIRE_NOT_EXIST_ERR;
			}
			*p_old = adron_info.adron[adron_id - 1];
			adron_info.adron[adron_id - 1] = new_adron;
			break;
		default:
			return VALUE_OUT_OF_RANGE_ERR;
			break;
		}
	ret = this->update_adron(id, userid, &adron_info);
	return ret;
}

/* @brief 得到用户某一汽车的装饰
 * @param userid 用户的米米号 
 * @param auto_id 汽车的种类的ID号
 * @param p_count 返回汽车的某类的数量
 */
int Cuser_auto::get_auto_class(userid_t userid, uint32_t auto_id, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid=%u and auto_id = %u",
			this->get_table_name(userid),
			userid,
			auto_id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, AUTO_IS_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

/* @brief 检查用户是否拥有这个ID号 
 * @param userid 用户的ID号
 * @param show_id 要设置的展示的车的ID号
 * @param exist 是否存在展示的车的ID号
 */
int Cuser_auto::check_id_exist(userid_t userid, uint32_t show_id, bool *exist)
{
	sprintf (this->sqlstr,"select id from %s where id = %u and userid = %u", \
			 this->get_table_name(userid),
			 show_id,
			 userid
			 );
	*exist=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		*exist=true;
	STD_QUERY_ONE_END();
}

/* @brief 得到更重汽车的ID号
 * @param userid 用户的米米号 
 * @param id 汽车的ID号
 * @param p_list 返回汽车的信息
 */

int Cuser_auto::get_auto_num_by_class(userid_t userid, user_get_attire_list_out_item **pp_list, uint32_t *p_count,
							 uint32_t startid, uint32_t endid)
{
	sprintf(this->sqlstr, "select auto_id, count(*) from %s where userid = %u\
			and auto_id>=%u and auto_id < %u group by auto_id",
			this->get_table_name(userid),
			userid,
			startid,
			endid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->attireid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->count);
	STD_QUERY_WHILE_END();
}

/* @brief 删除车 
 * @param userid 用户的米米号
 * @param id 车的ID号
 */
int Cuser_auto::del_auto(userid_t userid, uint32_t id)
{
	sprintf(this->sqlstr, "delete from %s where id = %u and userid = %u",
			this->get_table_name(userid),
			id,
			userid
			);
	STD_SET_RETURN(this->sqlstr,userid, AUTO_IS_NOT_EXIST_ERR);
}
/* @brief 修改汽车的信息，WEB页面用
 */
int Cuser_auto :: update_web(userid_t userid, uint32_t id, uint32_t oil, uint32_t engine, 
		                     uint32_t color, uint32_t oil_time)
{
	sprintf(this->sqlstr, "update %s set oil = %u, engine = %u, color = %u, oil_time = %u where id = %u",
			this->get_table_name(userid),
			oil,
			engine,
			color,
			oil_time,
			id
			);
	STD_SET_RETURN(this->sqlstr,userid, USER_ID_NOFIND_ERR);
}

/* @brief 得到某个汽车的初始油耗
 * @param auto_id 汽车种类的ID号
 * @param p_total_oil 返回汽车油耗量的初始值
 */
int Cuser_auto :: get_init_oil(uint32_t auto_id, uint32_t *p_total_oil)
{
	uint32_t index = auto_id - BASE_AUTO_ID;
	if (index >= AUTO_NUM) {
		return VALUE_OUT_OF_RANGE_ERR;
	}	
	*p_total_oil = auto_info[index].oil; 
	return SUCC;
}

int Cuser_auto::add_attire_msglog(userid_t userid, uint32_t attireid, uint32_t count)
{
	uint32_t log_type = 0;
	if(attireid >= 1300001 && attireid <= 1300999)//交通工具
	{
		log_type = 0x02000000+attireid-1300000;
	}

	if (log_type > 0)
	{
		struct USERID_NUM{
			uint32_t id;
			uint32_t num;
		};

		USERID_NUM s_userid_num = {};
		s_userid_num.id = userid ;
		s_userid_num.num = count;

		msglog(this->msglog_file, log_type, time(NULL), &s_userid_num, sizeof(s_userid_num));

	}
	
	return SUCC;
}




