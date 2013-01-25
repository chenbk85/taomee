/*
 * =====================================================================================
 *
 *       Filename:  Cpet.cpp
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
 *  common.h
 * =====================================================================================
 */

#include "Cattire.h"
#include <time.h> 
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "util.h"
#include <map>
using std::map;

#define MOLE2_BODY_GRID_BASE	10000

#define MOLE2_STORAGE_GRID_BASE	20000

#define MOLE2_STORAGE_GRID_END	30000

Cattire::Cattire(mysql_interface * db , Citem_change_log * p_log ) 
	:CtableRoute(db , "MOLE2_USER" , "t_attire" , "userid","gettime"),
	str_userid("userid"),
	str_gettime("gettime"),
	str_attireid("attire_id"),
	str_gridid("grid_id"),
	str_level("level"),
	str_mduration("mduration"),
	str_duration("duration"),
	str_hpmax("hpmax"),
	str_mpmax("mpmax"),
	str_atk("atk"),
	str_matk("matk"),
	str_def("def"),
	str_mdef("mdef"),
	str_speed("speed"),
	str_spirit("spirit"),
	str_resume("resume"),
	str_hit("hit"),
	str_dodge("dodge"),
	str_crit("crit"),
	str_fightback("fightback"),
	str_rpoison("rpoison"),
	str_rlithification("rlithification"),
	str_rlethargy("rlethargy"),
	str_rinebriation("rinebriation"),
	str_rconfusion("rconfusion"),
	str_roblivion("roblivion"),
	str_quality("hole_cnt"),
	str_validday("hole1"),
	str_crystal_attr("hole2"),
	str_bless_type("hole3")
{ 
	this->p_item_change_log=p_log;

}

int Cattire::insert(userid_t userid, uint32_t time, stru_mole2_user_attire_info* p_in)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u, %u, %u, %u, %u, %u, %u, %u,\
		%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u) ",
			this->get_table_name(userid),
			userid,
			time,
			p_in->attire_id,
			p_in->gridid,
			p_in->level,
			p_in->mduration,
			p_in->duration,
			p_in->hpmax,
			p_in->mpmax,
			p_in->atk,
			p_in->matk,
			p_in->def,
			p_in->mdef,
			p_in->speed,
			p_in->spirit,
			p_in->resume,
			p_in->hit,
			p_in->dodge,
			p_in->crit,
			p_in->fightback,
			p_in->rpoison,
			p_in->rlithification,
			p_in->rlethargy,
			p_in->rinebriation,
			p_in->rconfusion,
			p_in->roblivion,
			p_in->quality,
			p_in->validday,
			p_in->crystal_attr,
			p_in->bless_type);
	return this->exec_insert_sql( this->sqlstr, MOLE2_ATTIRE_KEY_EXISTED_ERR);
}

int Cattire::del_attire(userid_t userid, uint32_t gettime, uint32_t *p_attireid,bool is_vip_opt )
{
	int ret;

	//得到attireid
	ret=this->get_int_value(userid,gettime,this->str_attireid ,p_attireid );
	if (ret!=SUCC) return ret;

	ret=this->del(userid,gettime );
	if (ret!=SUCC) return ret;

	DEBUG_LOG("del cloth[uid=%u gettime=%u attireid=%u]",userid,gettime,*p_attireid);
	ret=this->p_item_change_log->add(time(NULL),is_vip_opt ,userid,*p_attireid,-1);


	return ret;
}
int Cattire::del(userid_t userid, uint32_t gettime)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s=%u and %s=%u",
			this->get_table_name(userid), 
			this->str_userid,	userid, 
			this->str_gettime,	gettime);
	return this->exec_update_sql(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
}

int Cattire::get_list_count(userid_t userid,MOLE2_ATTIRE_POS_TYPE type,uint32_t &count)
{
	if(MOLE2_ATTIRE_IN_BAG == type) {
		GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and %s < %u", 
				this->get_table_name(userid), 
				this->str_userid,	userid,
				this->str_gridid,MOLE2_BODY_GRID_BASE);
	} else if(MOLE2_ATTIRE_ON_BODY == type) {
		GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and %s >= %u and %s < %u",
				this->get_table_name(userid), 
				this->str_userid, userid, 
				this->str_gridid, MOLE2_BODY_GRID_BASE,
				this->str_gridid, MOLE2_STORAGE_GRID_BASE);
	} else if(MOLE2_ATTIRE_IN_STORAGE == type) {
		GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and %s >= %u and %s < %u",
				this->get_table_name(userid), 
				this->str_userid, userid, 
				this->str_gridid, MOLE2_STORAGE_GRID_BASE,
				this->str_gridid, MOLE2_STORAGE_GRID_END);
	} else if (MOLE2_ATTIRE_BODY_BAG == type) {
				GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and %s < %u", 
				this->get_table_name(userid), 
				this->str_userid,	userid,
				this->str_gridid,	MOLE2_STORAGE_GRID_BASE);
	} else {
		count = 0;
		return SUCC;
	}
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cattire::set_field_value(uint32_t userid, su_mole2_set_field_value_in* p_in){
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where %s = %u and %s = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				str_userid,	userid,
				str_gettime, p_in->opt_id);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}


int Cattire::getlist(userid_t userid, MOLE2_ATTIRE_POS_TYPE type,uint32_t start,uint32_t count,
		   stru_mole2_user_attire_info_1** p_out_item, uint32_t* p_out_count)
{
	if(MOLE2_ATTIRE_IN_BAG == type) {
		GEN_SQLSTR(this->sqlstr, "select * from %s where %s=%u and %s < %u limit %u,%u", 
				this->get_table_name(userid), 
				this->str_userid,	userid,
				this->str_gridid,MOLE2_BODY_GRID_BASE,start,count);
	} else if(MOLE2_ATTIRE_ON_BODY == type) {
		GEN_SQLSTR(this->sqlstr, "select * from %s where %s=%u and %s >= %u and %s < %u limit %u,%u",
				this->get_table_name(userid), 
				this->str_userid, userid, 
				this->str_gridid, MOLE2_BODY_GRID_BASE,
				this->str_gridid, MOLE2_STORAGE_GRID_BASE,start,count);
	} else if(MOLE2_ATTIRE_IN_STORAGE == type) {
		GEN_SQLSTR(this->sqlstr, "select * from %s where %s=%u and %s >= %u and %s < %u limit %u,%u",
				this->get_table_name(userid), 
				this->str_userid, userid, 
				this->str_gridid, MOLE2_STORAGE_GRID_BASE,
				this->str_gridid, MOLE2_STORAGE_GRID_END,start,count);
	} else if (MOLE2_ATTIRE_BODY_BAG == type) {
				GEN_SQLSTR(this->sqlstr, "select * from %s where %s=%u and %s < %u limit %u,%u", 
				this->get_table_name(userid), 
				this->str_userid,	userid,
				this->str_gridid,	MOLE2_STORAGE_GRID_BASE,start,count);
	} else {
		*p_out_count = 0;
		return SUCC;
	}
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out_item, p_out_count);
		INT_CPY_NEXT_FIELD(userid);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->gettime);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.attire_id);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.gridid);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.level);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.mduration);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.duration);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.hpmax);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.mpmax);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.atk);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.matk);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.def);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.mdef);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.speed);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.spirit);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.resume);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.hit);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.dodge);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.crit);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.fightback);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.rpoison);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.rlithification);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.rlethargy);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.rinebriation);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.rconfusion);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.roblivion);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.quality);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.validday);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.crystal_attr);
		INT_CPY_NEXT_FIELD((*p_out_item+i)->attire_info.bless_type);
	STD_QUERY_WHILE_END();
}
int Cattire::get_attire_list(userid_t userid, stru_mole2_user_attire_info_1** p_out_item,
		uint32_t* p_out_count)
{
	return getlist(userid, MOLE2_ATTIRE_ON_BODY,0,1000,p_out_item, p_out_count);
}
int Cattire::get_attire_list_ex(userid_t userid, 
			stru_mole2_user_attire_info_1** p_out_item, uint32_t* p_out_count)
{
	return this->getlist(userid, MOLE2_ATTIRE_IN_BAG,0,1000,p_out_item, p_out_count);
}

int Cattire::get_attire_list_1(userid_t userid, stru_mole2_user_attire_info_1** p_out_item,
		uint32_t* p_out_count)
{
	return getlist(userid, MOLE2_ATTIRE_BODY_BAG,0,1000,p_out_item, p_out_count);
}


int Cattire::storage_list_get(userid_t userid, uint32_t start,uint32_t count,
	stru_mole2_user_attire_info_1** p_out_item, user_attire_in_storage_list_get_out_header* out_hdr)
{
	uint32_t total = 0;
	this->get_list_count(userid,MOLE2_ATTIRE_IN_STORAGE, total);
	out_hdr->start = start;
	out_hdr->total = total;
	return this->getlist(userid, MOLE2_ATTIRE_IN_STORAGE, start,count, p_out_item, &out_hdr->count);
}

int Cattire::get_attire_id(userid_t userid,uint32_t key, uint32_t *p_attire_id )
{
	GEN_SQLSTR(this->sqlstr, "select attire_id from %s where %s = %u and %s = %u", 
			this->get_table_name(userid),
			this->str_userid,	userid, 
			this->str_gettime,	key);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_attire_id  );
	STD_QUERY_ONE_END();
}

int Cattire::get_attire(userid_t userid,uint32_t key, 
		stru_mole2_user_attire_info_1* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select * from %s where %s = %u and %s = %u", 
			this->get_table_name(userid),
			this->str_userid,	userid, 
			this->str_gettime,	key);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(userid);
		INT_CPY_NEXT_FIELD(p_out->gettime);
		INT_CPY_NEXT_FIELD(p_out->attire_info.attire_id);
		INT_CPY_NEXT_FIELD(p_out->attire_info.gridid);
		INT_CPY_NEXT_FIELD(p_out->attire_info.level);
		INT_CPY_NEXT_FIELD(p_out->attire_info.mduration);
		INT_CPY_NEXT_FIELD(p_out->attire_info.duration);
		INT_CPY_NEXT_FIELD(p_out->attire_info.hpmax);
		INT_CPY_NEXT_FIELD(p_out->attire_info.mpmax);
		INT_CPY_NEXT_FIELD(p_out->attire_info.atk);
		INT_CPY_NEXT_FIELD(p_out->attire_info.matk);
		INT_CPY_NEXT_FIELD(p_out->attire_info.def);
		INT_CPY_NEXT_FIELD(p_out->attire_info.mdef);
		INT_CPY_NEXT_FIELD(p_out->attire_info.speed);
		INT_CPY_NEXT_FIELD(p_out->attire_info.spirit);
		INT_CPY_NEXT_FIELD(p_out->attire_info.resume);
		INT_CPY_NEXT_FIELD(p_out->attire_info.hit);
		INT_CPY_NEXT_FIELD(p_out->attire_info.dodge);
		INT_CPY_NEXT_FIELD(p_out->attire_info.crit);
		INT_CPY_NEXT_FIELD(p_out->attire_info.fightback);
		INT_CPY_NEXT_FIELD(p_out->attire_info.rpoison);
		INT_CPY_NEXT_FIELD(p_out->attire_info.rlithification);
		INT_CPY_NEXT_FIELD(p_out->attire_info.rlethargy);
		INT_CPY_NEXT_FIELD(p_out->attire_info.rinebriation);
		INT_CPY_NEXT_FIELD(p_out->attire_info.rconfusion);
		INT_CPY_NEXT_FIELD(p_out->attire_info.roblivion);
		INT_CPY_NEXT_FIELD(p_out->attire_info.quality);
		INT_CPY_NEXT_FIELD(p_out->attire_info.validday);
		INT_CPY_NEXT_FIELD(p_out->attire_info.crystal_attr);
		INT_CPY_NEXT_FIELD(p_out->attire_info.bless_type);
	STD_QUERY_ONE_END();
}
int Cattire::update_gridid(userid_t userid, uint32_t gettime, uint32_t gridid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s=%u and %s=%u",
			this->get_table_name(userid), 
			this->str_gridid,	gridid, 
			this->str_userid,	userid, 
			this->str_gettime,	gettime);
	return this->exec_update_sql(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
}

int Cattire::set_attr(userid_t userid, uint32_t key, 
		mole2_user_attire_attr_add_list_out_item* p_item)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, \
			%s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,\
			%s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u where %s=%u and %s=%u",
		   	this->get_table_name(userid),
			this->str_level,		p_item->attire_info.level,
			this->str_duration,		p_item->attire_info.mduration,
			this->str_duration,		p_item->attire_info.duration,
			this->str_hpmax,		p_item->attire_info.hpmax,
			this->str_mpmax,		p_item->attire_info.mpmax,
			this->str_atk,			p_item->attire_info.atk,
			this->str_matk,			p_item->attire_info.matk,
			this->str_def,			p_item->attire_info.def,
			this->str_mdef,			p_item->attire_info.mdef,
			this->str_speed,		p_item->attire_info.speed,
			this->str_spirit,		p_item->attire_info.spirit,
			this->str_resume,		p_item->attire_info.resume,
			this->str_hit,			p_item->attire_info.hit,
			this->str_dodge,		p_item->attire_info.dodge,
			this->str_crit,			p_item->attire_info.crit,
			this->str_fightback,	p_item->attire_info.fightback,
			this->str_rpoison,		p_item->attire_info.rpoison,
			this->str_rlithification,	p_item->attire_info.rlithification,
			this->str_rlethargy,		p_item->attire_info.rlethargy,
			this->str_rinebriation,		p_item->attire_info.rinebriation,
			this->str_rconfusion,		p_item->attire_info.rconfusion,
			this->str_roblivion,		p_item->attire_info.roblivion,
			this->str_userid,			userid,
			this->str_gettime,			key);
	return this->exec_update_sql(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
}

int Cattire::set_body_null(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s=%u and %s>%u and %s<%u",
			this->get_table_name(userid), 
			this->str_gridid,	0, 
			this->str_userid,	userid,	
			this->str_gridid,	MOLE2_BODY_GRID_BASE,
			this->str_gridid,	MOLE2_STORAGE_GRID_BASE);
	return this->exec_update_list_sql(this->sqlstr,SUCC);
}

int Cattire::update_gridid_list(userid_t userid,
		mole2_user_attire_update_gridid_list_in_item* p_in_item, uint32_t in_count)
{
	int ret;
	

	//将身上装扮清空
	ret = this->set_body_null(userid);
	if (ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}

	//换装：修改装扮格子信息
	for(uint32_t i = 0; i < in_count; i ++)
	{
		ret = this->set_int_value(userid, p_in_item[i].gettime, this->str_gridid, p_in_item[i].gridid);
		if(ret != SUCC)
		{
			STD_ROLLBACK();
			return ret;
		}
	}


	return ret;
}


int Cattire::get_attire_simple(uint32_t userid, uint32_t gettime, uint32_t* p_attire_id, uint32_t* p_level)
{
	sprintf(this->sqlstr, "select %s, %s from %s where %s = %u and %s = %u",
		this->str_gettime,
		this->str_level,
		this->get_table_name(userid), 
		this->str_userid,	userid,
		this->str_gettime,	gettime);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_attire_id);
		INT_CPY_NEXT_FIELD(*p_level);
		//DEBUG_LOG("attire_id:%u\nlevel:%u",*p_attire_id, *p_level);
	STD_QUERY_ONE_END();
}

int Cattire::attire_add(userid_t userid, stru_mole2_user_attire_info* p_in, uint32_t* p_gettime ,bool is_vip_opt )
{
	int ret;
	uint32_t now = time(0);
	for (uint32_t i = 0; i < 48; i++) {
		ret = this->insert(userid, now, p_in);
		if (ret == DB_SUCC) break;
		++now;
	}
	if (ret != SUCC) {
		return ret;
	}
	if (p_gettime != NULL) {
		*p_gettime = now;

	}
	if (ret!=SUCC) return ret;

	stru_game_change_add log_item;
	log_item.userid= userid; 
	log_item.opt_type=1001 ;//修改物品
	log_item.ex_v1=p_in->attire_id ;
	log_item.ex_v2= 1;
	game_change_add(log_item );
	
	ret=this->p_item_change_log->add(now,is_vip_opt ,userid,p_in->attire_id , 1);

		
	return ret;
}

int Cattire::duration_set(uint32_t userid, uint32_t attireid, uint16_t duration)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s=%u and %s=%u",
			this->get_table_name(userid),
			this->str_duration, duration,
			this->str_userid,	userid,
			this->str_gettime,	attireid);
	return this->exec_update_sql(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
}

int Cattire::mending_set(uint32_t userid, uint32_t attireid, uint16_t mduration, uint16_t duration)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u where %s=%u and %s=%u",
		this->get_table_name(userid),
		this->str_mduration,	mduration,
		this->str_duration,		duration,
		this->str_userid,		userid,
		this->str_gettime,		attireid);
	return this->exec_update_sql(this->sqlstr, MOLE2_ATTIRE_KEY_NOFIND_ERR);
}

int Cattire::get_cloths(userid_t userid, std::vector<stru_cloth_info> &cloths,uint32_t grid_min,uint32_t grid_max)
{
	GEN_SQLSTR(this->sqlstr, "select \
		%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s \
		from %s where %s=%u and %s >= %u and %s < %u",
		this->str_gettime,
		this->str_attireid,
		this->str_gridid,
		this->str_level,
		this->str_mduration,
		this->str_duration,
		this->str_hpmax,
		this->str_mpmax,
		this->str_atk,
		this->str_matk,
		this->str_def,
		this->str_mdef,
		this->str_speed,
		this->str_spirit,
		this->str_resume,
		this->str_hit,
		this->str_dodge,
		this->str_crit,
		this->str_fightback,
		this->str_rpoison,
		this->str_rlithification,
		this->str_rlethargy,
		this->str_rinebriation,
		this->str_rconfusion,
		this->str_roblivion,
		this->str_quality,
		this->str_validday,
		this->str_crystal_attr,
		this->str_bless_type,
		this->get_table_name(userid),this->str_userid, userid, this->str_gridid, grid_min,this->str_gridid, grid_max);

	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, cloths);
		INT_CPY_NEXT_FIELD(item.gettime);
		INT_CPY_NEXT_FIELD(item.clothid);
		INT_CPY_NEXT_FIELD(item.gridid);
		INT_CPY_NEXT_FIELD(item.level);
		INT_CPY_NEXT_FIELD(item.mduration);
		INT_CPY_NEXT_FIELD(item.duration);
		INT_CPY_NEXT_FIELD(item.hpmax);
		INT_CPY_NEXT_FIELD(item.mpmax);
		INT_CPY_NEXT_FIELD(item.atk);
		INT_CPY_NEXT_FIELD(item.matk);
		INT_CPY_NEXT_FIELD(item.defense);
		INT_CPY_NEXT_FIELD(item.mdef);
		INT_CPY_NEXT_FIELD(item.speed);
		INT_CPY_NEXT_FIELD(item.spirit);
		INT_CPY_NEXT_FIELD(item.resume);
		INT_CPY_NEXT_FIELD(item.hit);
		INT_CPY_NEXT_FIELD(item.dodge);
		INT_CPY_NEXT_FIELD(item.crit);
		INT_CPY_NEXT_FIELD(item.fightback);
		INT_CPY_NEXT_FIELD(item.rpoison);
		INT_CPY_NEXT_FIELD(item.rlithification);
		INT_CPY_NEXT_FIELD(item.rlethargy);
		INT_CPY_NEXT_FIELD(item.rinebriation);
		INT_CPY_NEXT_FIELD(item.rconfusion);
		INT_CPY_NEXT_FIELD(item.roblivion);
		INT_CPY_NEXT_FIELD(item.quality);
		INT_CPY_NEXT_FIELD(item.validday);
		INT_CPY_NEXT_FIELD(item.crystal_attr);
		INT_CPY_NEXT_FIELD(item.bless_type);
	STD_QUERY_WHILE_END_NEW();
}

int Cattire::get_count_by_type(userid_t userid,uint32_t clothid,uint32_t &count)
{
	GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and attire_id=%u",
		this->get_table_name(userid),this->str_userid, userid,clothid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

