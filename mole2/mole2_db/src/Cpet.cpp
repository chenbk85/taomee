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

#include "Cpet.h"
#include <time.h> 
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"


#define STORAGE_SPLIT_BASE	10000

Cpet::Cpet(mysql_interface * db ) 
	:CtableRoute(db , "MOLE2_USER" , "t_pet" , "userid","petid"),
	struid("userid"),
	strpetid("petid"),
	strpettype("pettype"),
	strrace("race"), 
	strflag("flag"),
	strnick("nick"),
	strlevel("level"),
	strexp("experience"), 
	strphyinit("physiqueinit"),
	strstrinit("strengthinit"),
	strendinit("enduranceinit"),
	strquickinit("quickinit"),
	striqinit("iqinit"),
	strphyparam("physiqueparam"),
	strstrparam("strengthparam"),
	strendparam("enduranceparam"),
	strquickparam("quickparam"),
	striqparam("iqparam"),
	strphyadd("physique_add"), 
	strstradd("strength_add"),
	strendadd("endurance_add"),
	strquickadd("quick_add"),
	striqadd("iq_add"),
	strattradd("attr_addition"),
	strhp("hp"),
	strmp("mp"), 
	strearth("earth"), 
	strwater("water"),
	strfire("fire"),
	strwind("wind"),
	strinjury("injury_state"),
	strlocation("location")
{ 

}

int Cpet::get_pet_info(userid_t userid, uint32_t petid, 
		stru_mole2_user_pet_info_1* p_out)
{
		GEN_SQLSTR( this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
				%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
				%s, %s, %s, %s, %s from %s where %s=%u and %s=%u",
				this->strpetid,
				this->strpettype,
				this->strrace,
				this->strflag,
				this->strnick,
				this->strlevel,
				this->strexp,
				this->strphyinit,
				this->strstrinit,
				this->strendinit,
				this->strquickinit,
				this->striqinit,
				this->strphyparam,
				this->strstrparam,
				this->strendparam,
				this->strquickparam,
				this->striqparam,
				this->strphyadd,
				this->strstradd,
				this->strendadd,
				this->strquickadd,
				this->striqadd,
				this->strattradd,
				this->strhp,
				this->strmp,
				this->strearth,
				this->strwater,
				this->strfire,
				this->strwind,
				this->strinjury,
				this->strlocation,
				this->get_table_name(userid), 
				this->struid, userid, 
				this->strpetid, petid);
		STD_QUERY_ONE_BEGIN(this->sqlstr ,MOLE2_PETID_NOFIND_ERR );
				INT_CPY_NEXT_FIELD(p_out->gettime);
				INT_CPY_NEXT_FIELD(p_out->pet_info.pettype);
				INT_CPY_NEXT_FIELD(p_out->pet_info.race);
				INT_CPY_NEXT_FIELD(p_out->pet_info.flag);
				BIN_CPY_NEXT_FIELD(p_out->pet_info.nick, sizeof(p_out->pet_info.nick));
				INT_CPY_NEXT_FIELD(p_out->pet_info.level);
				INT_CPY_NEXT_FIELD(p_out->pet_info.exp);
				INT_CPY_NEXT_FIELD(p_out->pet_info.physiqueinit);
				INT_CPY_NEXT_FIELD(p_out->pet_info.strengthinit);
				INT_CPY_NEXT_FIELD(p_out->pet_info.enduranceinit);
				INT_CPY_NEXT_FIELD(p_out->pet_info.quickinit);
				INT_CPY_NEXT_FIELD(p_out->pet_info.iqinit);
				INT_CPY_NEXT_FIELD(p_out->pet_info.physiqueparam);
				INT_CPY_NEXT_FIELD(p_out->pet_info.strengthparam);
				INT_CPY_NEXT_FIELD(p_out->pet_info.enduranceparam);
				INT_CPY_NEXT_FIELD(p_out->pet_info.quickparam);
				INT_CPY_NEXT_FIELD(p_out->pet_info.iqparam);
				INT_CPY_NEXT_FIELD(p_out->pet_info.physique_add);
				INT_CPY_NEXT_FIELD(p_out->pet_info.strength_add);
				INT_CPY_NEXT_FIELD(p_out->pet_info.endurance_add);
				INT_CPY_NEXT_FIELD(p_out->pet_info.quick_add);
				INT_CPY_NEXT_FIELD(p_out->pet_info.iq_add);
				INT_CPY_NEXT_FIELD(p_out->pet_info.attr_addition);
				INT_CPY_NEXT_FIELD(p_out->pet_info.hp);
				INT_CPY_NEXT_FIELD(p_out->pet_info.mp);
				INT_CPY_NEXT_FIELD(p_out->pet_info.earth);
				INT_CPY_NEXT_FIELD(p_out->pet_info.water);
				INT_CPY_NEXT_FIELD(p_out->pet_info.fire);
				INT_CPY_NEXT_FIELD(p_out->pet_info.wind);
				INT_CPY_NEXT_FIELD(p_out->pet_info.injury_state);
				INT_CPY_NEXT_FIELD(p_out->pet_info.location);
		STD_QUERY_ONE_END();
}

int	Cpet::get_pet_list_on_body(userid_t userid, stru_mole2_user_pet_info_1** pp_item, uint32_t* p_count)
{
	 GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s from %s where %s = %u and %s > %u and %s < %u",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strflag,
		this->strnick,
		this->strlevel,
		this->strexp,
		this->strphyinit,
		this->strstrinit,
		this->strendinit,
		this->strquickinit,
		this->striqinit,
		this->strphyparam,
		this->strstrparam,
		this->strendparam,
		this->strquickparam,
		this->striqparam,
		this->strphyadd,
		this->strstradd,
		this->strendadd,
		this->strquickadd,
		this->striqadd,
		this->strattradd,
		this->strhp,
		this->strmp,
		this->strearth,
		this->strwater,
		this->strfire,
		this->strwind,
		this->strinjury,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		this->strlocation,	0,
		this->strlocation,	STORAGE_SPLIT_BASE);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_item+i)->gettime);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.pettype);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.race);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.flag);
		BIN_CPY_NEXT_FIELD((*pp_item+i)->pet_info.nick, sizeof((*pp_item+i)->pet_info.nick));
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.level);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.exp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physique_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strength_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.endurance_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quick_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iq_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.attr_addition);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.hp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.mp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.earth);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.water);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.fire);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.wind);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.injury_state);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.location);
	STD_QUERY_WHILE_END();
}

int	Cpet::get_pets(userid_t userid, std::vector<stru_pet_info> &pets,uint32_t location_min,uint32_t location_max)
{
	GEN_SQLSTR(this->sqlstr, "select \
	 	%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s \
	 	from %s where %s = %u and %s > %u and %s < %u",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strflag,
		this->strnick,
		this->strlevel,
		this->strexp,
		this->strphyinit,
		this->strstrinit,
		this->strendinit,
		this->strquickinit,
		this->striqinit,
		this->strphyparam,
		this->strstrparam,
		this->strendparam,
		this->strquickparam,
		this->striqparam,
		this->strphyadd,
		this->strstradd,
		this->strendadd,
		this->strquickadd,
		this->striqadd,
		this->strattradd,
		this->strhp,
		this->strmp,
		this->strearth,
		this->strwater,
		this->strfire,
		this->strwind,
		this->strinjury,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		this->strlocation,	location_min,
		this->strlocation,	location_max);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, pets);
		memset(item.nick,0,sizeof(item.nick));
		INT_CPY_NEXT_FIELD(item.gettime);
		INT_CPY_NEXT_FIELD(item.pettype);
		INT_CPY_NEXT_FIELD(item.race);
		INT_CPY_NEXT_FIELD(item.flag);
		BIN_CPY_NEXT_FIELD(item.nick, sizeof(item.nick));
		INT_CPY_NEXT_FIELD(item.level);
		INT_CPY_NEXT_FIELD(item.exp);
		INT_CPY_NEXT_FIELD(item.physiqueinit);
		INT_CPY_NEXT_FIELD(item.strengthinit);
		INT_CPY_NEXT_FIELD(item.enduranceinit);
		INT_CPY_NEXT_FIELD(item.quickinit);
		INT_CPY_NEXT_FIELD(item.iqinit);
		INT_CPY_NEXT_FIELD(item.physiqueparam);
		INT_CPY_NEXT_FIELD(item.strengthparam);
		INT_CPY_NEXT_FIELD(item.enduranceparam);
		INT_CPY_NEXT_FIELD(item.quickparam);
		INT_CPY_NEXT_FIELD(item.iqparam);
		INT_CPY_NEXT_FIELD(item.physique_add);
		INT_CPY_NEXT_FIELD(item.strength_add);
		INT_CPY_NEXT_FIELD(item.endurance_add);
		INT_CPY_NEXT_FIELD(item.quick_add);
		INT_CPY_NEXT_FIELD(item.iq_add);
		INT_CPY_NEXT_FIELD(item.attr_addition);
		INT_CPY_NEXT_FIELD(item.hp);
		INT_CPY_NEXT_FIELD(item.mp);
		INT_CPY_NEXT_FIELD(item.earth);
		INT_CPY_NEXT_FIELD(item.water);
		INT_CPY_NEXT_FIELD(item.fire);
		INT_CPY_NEXT_FIELD(item.wind);
		INT_CPY_NEXT_FIELD(item.injury_state);
		INT_CPY_NEXT_FIELD(item.location);
	STD_QUERY_WHILE_END_NEW();
}

int	Cpet::get_pet_list_on_body(userid_t userid, stru_mole2_user_pet_info_2** pp_item, uint32_t* p_count)
{
	 GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s from %s where %s = %u and %s > %u and %s < %u",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strflag,
		this->strnick,
		this->strlevel,
		this->strexp,
		this->strphyinit,
		this->strstrinit,
		this->strendinit,
		this->strquickinit,
		this->striqinit,
		this->strphyparam,
		this->strstrparam,
		this->strendparam,
		this->strquickparam,
		this->striqparam,
		this->strphyadd,
		this->strstradd,
		this->strendadd,
		this->strquickadd,
		this->striqadd,
		this->strattradd,
		this->strhp,
		this->strmp,
		this->strearth,
		this->strwater,
		this->strfire,
		this->strwind,
		this->strinjury,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		this->strlocation,	0,
		this->strlocation,	STORAGE_SPLIT_BASE);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_item+i)->gettime);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.pettype);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.race);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.flag);
		BIN_CPY_NEXT_FIELD((*pp_item+i)->pet_info.nick, sizeof((*pp_item+i)->pet_info.nick));
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.level);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.exp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physique_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strength_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.endurance_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quick_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iq_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.attr_addition);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.hp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.mp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.earth);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.water);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.fire);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.wind);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.injury_state);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.location);
	STD_QUERY_WHILE_END();
}

int	Cpet::get_pet_list_in_store(userid_t userid, pet_in_store_list_get_out_item** pp_item, uint32_t* p_count,uint32_t start,uint32_t cnt)
{
	 GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s from %s where %s = %u and %s >= %u and %s < %u limit %u,%u",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strnick,
		this->strlevel,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		this->strlocation,	STORAGE_SPLIT_BASE,
		this->strlocation,	STORAGE_SPLIT_END,
		start,cnt);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_item + i)->petid);
		INT_CPY_NEXT_FIELD((*pp_item + i)->pettype);
		INT_CPY_NEXT_FIELD((*pp_item + i)->race);		
		BIN_CPY_NEXT_FIELD((*pp_item + i)->nick, sizeof((*pp_item+i)->nick));
		INT_CPY_NEXT_FIELD((*pp_item + i)->level);		
		INT_CPY_NEXT_FIELD((*pp_item + i)->location);
	STD_QUERY_WHILE_END();
}

int	Cpet::get_pet_list_in_store(userid_t userid, stru_mole2_user_pet_info_1** pp_item, uint32_t* p_count,uint32_t start,uint32_t cnt)
{
	 GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s from %s where %s = %u and %s >= %u and %s < %u limit %u,%u",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strflag,
		this->strnick,
		this->strlevel,
		this->strexp,
		this->strphyinit,
		this->strstrinit,
		this->strendinit,
		this->strquickinit,
		this->striqinit,
		this->strphyparam,
		this->strstrparam,
		this->strendparam,
		this->strquickparam,
		this->striqparam,
		this->strphyadd,
		this->strstradd,
		this->strendadd,
		this->strquickadd,
		this->striqadd,
		this->strattradd,
		this->strhp,
		this->strmp,
		this->strearth,
		this->strwater,
		this->strfire,
		this->strwind,
		this->strinjury,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		this->strlocation,	STORAGE_SPLIT_BASE,
		this->strlocation,	STORAGE_SPLIT_END,
		start,cnt);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		INT_CPY_NEXT_FIELD((*pp_item+i)->gettime);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.pettype);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.race);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.flag);
		BIN_CPY_NEXT_FIELD((*pp_item+i)->pet_info.nick, sizeof((*pp_item+i)->pet_info.nick));
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.level);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.exp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqinit);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physiqueparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strengthparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.enduranceparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quickparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iqparam);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.physique_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.strength_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.endurance_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.quick_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.iq_add);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.attr_addition);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.hp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.mp);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.earth);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.water);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.fire);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.wind);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.injury_state);
		INT_CPY_NEXT_FIELD((*pp_item+i)->pet_info.location);
	STD_QUERY_WHILE_END();
}

int Cpet::pet_add(userid_t userid, stru_mole2_user_pet_info* p_in, uint32_t* p_gettime)
{
	int ret;
	uint32_t now = (uint32_t)time(0) - 1;
	do {
		now++;
		ret = this->insert(userid, now, p_in);
	} while(ret != SUCC && ret != DB_ERR);
	if (p_gettime != NULL) *p_gettime = now;
	return ret;
}

int Cpet::insert(userid_t userid, uint32_t time, stru_mole2_user_pet_info* p_in)
{
	char nick_mysql[mysql_str_len(sizeof(p_in->nick))];
	set_mysql_string(nick_mysql, (char *)(p_in->nick), sizeof(p_in->nick));
	GEN_SQLSTR( this->sqlstr, "insert into %s values( %u, %u, %u, %u, %u, '%s', \
		%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, \
		%u, %u, %u, %u, %u, %u, %u, %u, %u)", this->get_table_name(userid), 
		userid, 
		time,
	   	p_in->pettype,
		p_in->race,
		p_in->flag,
		nick_mysql,
		p_in->level, 
		p_in->exp, /* p_in->exp, */ 
		p_in->physiqueinit, 
		p_in->strengthinit, 
		p_in->enduranceinit, 
		p_in->quickinit, 
		p_in->iqinit, 
		p_in->physiqueparam, 
		p_in->strengthparam,
	   	p_in->enduranceparam, 
		p_in->quickparam, 
		p_in->iqparam, 
		0,	/* physique_add */
		0,	/* strength_add */
		0,	/* endurance_add */
		0,	/* quick_add */
		0,	/* iq_add */
		p_in->attr_addition, 
		p_in->hp, 
		p_in->mp, 
		p_in->earth, 
		p_in->water, 
		p_in->fire, 
		p_in->wind,
		0,
		p_in->location
	);
	return this->exec_insert_sql( this->sqlstr, MOLE2_PETID_EXISTED_ERR );
}

int Cpet::update(userid_t userid, uint32_t time, stru_mole2_user_pet_info* p_in)
{
	char nick_mysql[mysql_str_len(sizeof(p_in->nick))];
	set_mysql_string(nick_mysql, (char *)(p_in->nick), sizeof(p_in->nick));
	GEN_SQLSTR( this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s='%s', \
		%s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, \
		%s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,%s=%u, %s=%u, %s=%u, %s=%u, \
		%s=%u, %s=%u, %s=%u, %s=%u where %s=%u and %s=%u", this->get_table_name(userid), 
	   	this->strpettype,p_in->pettype,
		this->strrace,p_in->race,
		this->strflag,p_in->flag,
		this->strnick,nick_mysql,
		this->strlevel,p_in->level, 
		this->strexp,p_in->exp, /* p_in->exp, */ 
		this->strphyinit,p_in->physiqueinit, 
		this->strstrinit,p_in->strengthinit, 
		this->strendinit,p_in->enduranceinit, 
		this->strquickinit,p_in->quickinit, 
		this->striqinit,p_in->iqinit, 
		this->strphyparam,p_in->physiqueparam, 
		this->strstrparam,p_in->strengthparam,
	   	this->strendparam,p_in->enduranceparam, 
		this->strquickparam,p_in->quickparam, 
		this->striqparam,p_in->iqparam, 
		this->strphyadd,p_in->physique_add,
		this->strstradd,p_in->strength_add,
		this->strendadd,p_in->endurance_add,
		this->strquickadd,p_in->quick_add,
		this->striqadd,p_in->iq_add,
		this->strattradd,p_in->attr_addition, 
		this->strhp,p_in->hp, 
		this->strmp,p_in->mp, 
		this->strearth,p_in->earth, 
		this->strwater,p_in->water, 
		this->strfire,p_in->fire, 
		this->strwind,p_in->wind,
		this->strinjury,p_in->injury_state,
		this->strlocation,p_in->location,
		this->struid,userid,
		this->strpetid,time
	);
	return this->exec_update_sql( this->sqlstr, MOLE2_PETID_EXISTED_ERR );
}

int Cpet::update_nick(userid_t userid, mole2_user_pet_update_nick_in* p_in,
		mole2_user_pet_update_nick_out* p_out)
{
	int ret = this->set_nick(userid, p_in);
	this->get_nick(userid, p_in->petid, p_out);
	return ret;
}

int Cpet::add_exp(userid_t userid, mole2_user_pet_add_exp_in* p_in,
		mole2_user_pet_add_exp_out* p_out)
{
	p_out->petid = p_in->petid;
	return this->change_int_value(userid, p_in->petid, this->strexp, p_in->exp, 
			MOLE2_MAX_EXPERIENCE, &(p_out->exp), 0, true);
}

int Cpet::get_base_attr_initial(userid_t userid, mole2_user_pet_get_base_attr_initial_in* p_in,
			mole2_user_pet_get_base_attr_initial_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s from %s where %s=%u and %s=%u", 
			this->strpetid,
			this->strphyinit,
			this->strstrinit,
			this->strendinit,
			this->strquickinit,
			this->striqinit,
			this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	p_in->petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->physiqueinit);
		INT_CPY_NEXT_FIELD(p_out->strengthinit);
		INT_CPY_NEXT_FIELD(p_out->enduranceinit);
		INT_CPY_NEXT_FIELD(p_out->quickinit);
		INT_CPY_NEXT_FIELD(p_out->iqinit);
	STD_QUERY_ONE_END();
}

int Cpet::add_base_attr_initial(userid_t userid,
			mole2_user_pet_add_base_attr_initial_in* p_in,
			mole2_user_pet_get_base_attr_initial_out* p_out)
{
	int ret;
	this->get_base_attr_initial(userid, 
			reinterpret_cast<mole2_user_pet_get_base_attr_initial_in *>(p_in), p_out);
	return ret;
}

int Cpet::get_base_attr_param(userid_t userid, mole2_user_pet_get_base_attr_param_in* p_in,
			mole2_user_pet_get_base_attr_param_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s from %s where %s=%u and %s=%u",	
			this->strpetid,
			this->strphyparam,
			this->strstrparam,
			this->strendparam,
			this->strquickparam,
			this->striqparam,
			this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	p_in->petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->physiqueparam);
		INT_CPY_NEXT_FIELD(p_out->strengthparam);
		INT_CPY_NEXT_FIELD(p_out->enduranceparam);
		INT_CPY_NEXT_FIELD(p_out->quickparam);
		INT_CPY_NEXT_FIELD(p_out->iqparam);
	STD_QUERY_ONE_END();
}

int Cpet::set_attr_addition(userid_t userid, mole2_user_pet_add_attr_addition_in* p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strphyadd,	p_in->physique, 
			this->strstradd,	p_in->strength,
			this->strendadd,	p_in->endurance,
			this->strquickadd,	p_in->quick,
			this->striqadd,		p_in->iq,
			this->strattradd,	p_in->attr_addition,
			this->struid,		userid,
			this->strpetid,		p_in->petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::get_flag(userid_t userid, uint32_t petid, mole2_user_pet_get_flag_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s=%u",
			this->strpetid, this->strflag,	this->get_table_name(userid), 
			this->struid,	userid,
			this->strpetid,	petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->flag);
	STD_QUERY_ONE_END();
}

int Cpet::get_four_attr(userid_t userid, mole2_user_pet_get_four_attr_in* p_in,
		mole2_user_pet_get_four_attr_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s from %s where %s=%u and %s=%u", 
			this->strpetid,
			this->strearth,
			this->strwater,
			this->strfire,
			this->strwind,
			this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	p_in->petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->earth);
		INT_CPY_NEXT_FIELD(p_out->water);
		INT_CPY_NEXT_FIELD(p_out->fire);
		INT_CPY_NEXT_FIELD(p_out->wind);
	STD_QUERY_ONE_END();
}

int Cpet::set_flag(userid_t userid, uint32_t petid, uint32_t flag)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s|%u where %s=%u and %s=%u", 
			this->get_table_name(userid),
			this->strflag,	strflag,	flag, 
			this->struid,	userid, 
			this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::unset_flag(userid_t userid, uint32_t petid, uint32_t flag)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s&%u where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strflag,	this->strflag,	~flag,
			this->struid,	userid, 
			this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::update_carry_flag(userid_t userid, mole2_user_pet_update_carry_flag_in* p_in,
		mole2_user_pet_update_carry_flag_out* p_out)
{
	if(p_in->flag == 1)
	{
		this->unset_pet_rdy(userid);
		this->set_flag(userid, p_in->petid, MOLE2_PET_CARRY_FLAG);
	}
	else if(p_in->flag == 0)
		this->unset_flag(userid, p_in->petid, MOLE2_PET_CARRY_FLAG);
	return this->get_pet_info(userid, p_in->petid, p_out);
}

int	Cpet::get_nick(userid_t userid, uint32_t petid, mole2_user_pet_update_nick_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s=%u", 
			this->strpetid, this->strnick, this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		BIN_CPY_NEXT_FIELD(p_out->nick, sizeof(p_out->nick));
	STD_QUERY_ONE_END();
}

int Cpet::set_nick(userid_t userid, mole2_user_pet_update_nick_in* p_in)
{
	char nick_mysql[mysql_str_len(sizeof(p_in->nick))];
	set_mysql_string(nick_mysql, (char *)(p_in->nick),sizeof(p_in->nick));
	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s' where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strnick,	nick_mysql, 
			this->struid,	userid, 
			this->strpetid,	p_in->petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::get_attr_addition(userid_t userid, uint32_t petid, mole2_user_pet_add_attr_addition_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u and %s=%u",
			this->strpetid, this->strattradd, this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->attr_addition);
	STD_QUERY_ONE_END();
}

int Cpet::get_attr_add(userid_t userid, mole2_user_pet_get_attr_add_in* p_in,
	   mole2_user_pet_get_attr_add_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, physique_add, strength_add, endurance_add, \
			quick_add, iq_add from %s where userid=%u and petid=%u", 
			this->strpetid, this->get_table_name(userid), userid, p_in->petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->physique_add);
		INT_CPY_NEXT_FIELD(p_out->strength_add);
		INT_CPY_NEXT_FIELD(p_out->endurance_add);
		INT_CPY_NEXT_FIELD(p_out->quick_add);
		INT_CPY_NEXT_FIELD(p_out->iq_add);
	STD_QUERY_ONE_END();
}

int Cpet::get_hp_mp(userid_t userid, uint32_t petid, mole2_user_pet_add_hp_mp_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u and %s=%u", 
			this->strpetid, this->strhp, this->strmp, this->get_table_name(userid), 
			this->struid,	userid, 
			this->strpetid,	petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->petid);
		INT_CPY_NEXT_FIELD(p_out->hp);
		INT_CPY_NEXT_FIELD(p_out->mp);
	STD_QUERY_ONE_END();
}

int Cpet::add_hp_mp(userid_t userid, uint32_t petid, int hp, int mp, uint32_t injury_state)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s+%d, %s=%s+%d, %s=%u where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strhp,	this->strhp,hp, 
			this->strmp,	this->strmp,mp,
			this->strinjury,	injury_state,
			this->struid,	userid, 
			this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::set_hp_mp(userid_t userid, uint32_t petid, uint32_t hp, uint32_t mp)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strhp,	hp, 
			this->strmp,	mp,
			this->struid,	userid, 
			this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::set_hp_mp_injury(userid_t userid, uint32_t petid, int hp, int mp, uint32_t injury)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u where %s=%u and %s=%u", 
			this->get_table_name(userid), 
			this->strhp,	hp, 
			this->strmp,	mp,
			this->strinjury,injury,
			this->struid,	userid, 
			this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::injury_state_update(userid_t userid, uint32_t petid, uint32_t injury_state)
{
	sprintf(this->sqlstr, "update %s set injury_state = %u where userid = %u and petid = %u", 
		this->get_table_name(userid),
		injury_state,
		userid,
		petid
	);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cpet::injury_state_select(uint32_t userid, uint32_t petid, uint32_t* injury_state)
{
	sprintf(this->sqlstr, "select injury_state from %s where userid = %u and petid = %u",
			this->get_table_name(userid),
			userid,
			petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*injury_state);
	STD_QUERY_ONE_END();
}
int Cpet::unset_pet_rdy(uint32_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s&%u where %s=%u and %s&%u=%u",
			this->get_table_name(userid), 
			this->strflag,	this->strflag,	~MOLE2_PET_CARRY_FLAG,
			this->struid,	userid,
			this->strflag,	MOLE2_PET_CARRY_FLAG,	MOLE2_PET_CARRY_FLAG);
	return this->exec_update_sql(this->sqlstr, SUCC);
}

int Cpet::levelup_update(uint32_t userid, user_pet_levelup_set_in *p_in)
{
	sprintf(this->sqlstr, "update %s set experience = %u, level = %u,\
		attr_addition = %u, hp = %u, mp = %u where userid = %u and petid = %u",
		this->get_table_name(userid),
		p_in->experience,
		p_in->level,
		p_in->addition,
		p_in->hp,
		p_in->mp,
		userid,
		p_in->petid
		);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}



int Cpet::flag_get(userid_t userid, uint32_t petid, uint32_t* p_flag)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u and %s=%u",
		this->strflag,
		this->get_table_name(userid), 
		this->struid,	userid,
		this->strpetid,	petid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_flag);
	STD_QUERY_ONE_END();
}

int Cpet::location_set(userid_t userid, uint32_t petid, uint32_t location)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u and %s = %u", 
				this->get_table_name(userid), 
				this->strlocation,	location, 
				this->struid,	userid,
				this->strpetid, petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::follow_pet_clear(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s & %u where %s = %u and %s <> %u and %s < %u", 
				this->get_table_name(userid), 
				this->strflag,	this->strflag,	~MOLE2_PET_FOLLOW_FLAG, 
				this->struid,	userid,
				this->strlocation,	0,
				this->strlocation,	STORAGE_SPLIT_BASE);
	return this->exec_update_list_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::follow_pet_set(userid_t userid, uint32_t petid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s | %u where %s = %u and %s = %u", 
				this->get_table_name(userid), 
				this->strflag,	this->strflag,	MOLE2_PET_FOLLOW_FLAG, 
				this->struid,	userid,
				this->strpetid, petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::set_attributes(userid_t userid, mole2_set_pet_attributes_in* p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set"
		" %s=%u, %s=%u,"
		" %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,"
		" %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,"
		" %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,"
		" %s=%u, %s=%u, %s=%u"
		" where %s = %u and %s = %u",
		this->get_table_name(userid), 
		this->strlevel,		p_in->level,
		this->strexp,		p_in->exp,
		this->strphyinit,	p_in->physiqueinit,
		this->strstrinit,	p_in->strengthinit,
		this->strendinit,	p_in->enduranceinit,
		this->strquickinit, p_in->quickinit,
		this->striqinit,	p_in->iqinit,
       	this->strphyparam,  p_in->physiqueparam,
        this->strstrparam,  p_in->strengthparam,
		this->strendparam,  p_in->enduranceparam,
		this->strquickparam,p_in->quickparam,
		this->striqparam,   p_in->iqparam,
		this->strphyadd,	p_in->physiqueadd,
		this->strstradd,	p_in->strengthadd,
		this->strendadd,	p_in->enduranceadd,
		this->strquickadd,	p_in->quickadd,
		this->striqadd,		p_in->iqadd,
		this->strattradd,	p_in->attradd,
		this->strhp, 		p_in->hp,
		this->strmp, 		p_in->mp,
		this->struid,		userid,
		this->strpetid,		p_in->petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::pet_cnt_get(userid_t userid, uint32_t* p_count,uint32_t location)
{
	GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s=%u and location >= %u and location < 20000",
			this->get_table_name(userid), 
			this->struid,	userid,location);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}


int Cpet::del_pet(userid_t userid, uint32_t petid)
{
	GEN_SQLSTR(this->sqlstr, "delete from %s where %s = %u and %s = %u",
		this->get_table_name(userid), 
		this->struid,	userid,
		this->strpetid,	petid);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::get_all_pet_type(userid_t userid, get_all_pet_type_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select DISTINCT pettype from %s where userid=%u and location < 30000",
		this->get_table_name(userid),userid);

	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,p_out->petlist);
		INT_CPY_NEXT_FIELD(item);
	STD_QUERY_WHILE_END_NEW();
}

int Cpet::chk_pet(userid_t userid, uint32_t petid, uint32_t pettype)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s = %u and %s = %u and %s = %u",
		this->strpetid, this->get_table_name(userid), 
		this->struid, userid,
		this->strpetid, petid, 
		this->strpettype, pettype);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(petid);
	STD_QUERY_ONE_END();
}

int Cpet::set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in)
{
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where %s = %u and %s = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				this->struid,	userid,
				this->strpetid, p_in->opt_id);
	return this->exec_update_sql(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
}

int Cpet::get_type_pet_count(userid_t userid, uint32_t* p_count,uint32_t pettype)
{
	GEN_SQLSTR(this->sqlstr, "select count(*) from %s where %s = %u and %s = %u and location < 30000",
			this->get_table_name(userid), 
			this->struid, userid, this->strpettype, pettype);
	STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_PETID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cpet::get_type_pets(userid_t userid, std::vector<stru_pet_info> &pets,uint32_t start,uint32_t limit,uint32_t pettype)
{
	GEN_SQLSTR(this->sqlstr, "select \
			%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
			%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s \
			from %s where %s = %u and %s = %u and location < 30000 limit %u,%u",
			this->strpetid,
			this->strpettype,
			this->strrace,
			this->strflag,
			this->strnick,
			this->strlevel,
			this->strexp,
			this->strphyinit,
			this->strstrinit,
			this->strendinit,
			this->strquickinit,
			this->striqinit,
			this->strphyparam,
			this->strstrparam,
			this->strendparam,
			this->strquickparam,
			this->striqparam,
			this->strphyadd,
			this->strstradd,
			this->strendadd,
			this->strquickadd,
			this->striqadd,
			this->strattradd,
			this->strhp,
			this->strmp,
			this->strearth,
			this->strwater,
			this->strfire,
			this->strwind,
			this->strinjury,
			this->strlocation,
			this->get_table_name(userid),
			this->struid,		userid,
			this->strpettype,	pettype, start,	limit);
		STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, pets);
			memset(item.nick,0,sizeof(item.nick));
			INT_CPY_NEXT_FIELD(item.gettime);
			INT_CPY_NEXT_FIELD(item.pettype);
			INT_CPY_NEXT_FIELD(item.race);
			INT_CPY_NEXT_FIELD(item.flag);
			BIN_CPY_NEXT_FIELD(item.nick, sizeof(item.nick));
			INT_CPY_NEXT_FIELD(item.level);
			INT_CPY_NEXT_FIELD(item.exp);
			INT_CPY_NEXT_FIELD(item.physiqueinit);
			INT_CPY_NEXT_FIELD(item.strengthinit);
			INT_CPY_NEXT_FIELD(item.enduranceinit);
			INT_CPY_NEXT_FIELD(item.quickinit);
			INT_CPY_NEXT_FIELD(item.iqinit);
			INT_CPY_NEXT_FIELD(item.physiqueparam);
			INT_CPY_NEXT_FIELD(item.strengthparam);
			INT_CPY_NEXT_FIELD(item.enduranceparam);
			INT_CPY_NEXT_FIELD(item.quickparam);
			INT_CPY_NEXT_FIELD(item.iqparam);
			INT_CPY_NEXT_FIELD(item.physique_add);
			INT_CPY_NEXT_FIELD(item.strength_add);
			INT_CPY_NEXT_FIELD(item.endurance_add);
			INT_CPY_NEXT_FIELD(item.quick_add);
			INT_CPY_NEXT_FIELD(item.iq_add);
			INT_CPY_NEXT_FIELD(item.attr_addition);
			INT_CPY_NEXT_FIELD(item.hp);
			INT_CPY_NEXT_FIELD(item.mp);
			INT_CPY_NEXT_FIELD(item.earth);
			INT_CPY_NEXT_FIELD(item.water);
			INT_CPY_NEXT_FIELD(item.fire);
			INT_CPY_NEXT_FIELD(item.wind);
			INT_CPY_NEXT_FIELD(item.injury_state);
			INT_CPY_NEXT_FIELD(item.location);
		STD_QUERY_WHILE_END_NEW();
}

int	Cpet::get_last_pets(userid_t userid, std::vector<stru_pet_info> &pets)
{
	uint32_t last_time=time(NULL)-86400*31;
	GEN_SQLSTR(this->sqlstr, "select \
	 	%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s \
	 	from %s where %s = %u  and petid >%u order by petid desc ",
		this->strpetid,
		this->strpettype,
		this->strrace,
		this->strflag,
		this->strnick,
		this->strlevel,
		this->strexp,
		this->strphyinit,
		this->strstrinit,
		this->strendinit,
		this->strquickinit,
		this->striqinit,
		this->strphyparam,
		this->strstrparam,
		this->strendparam,
		this->strquickparam,
		this->striqparam,
		this->strphyadd,
		this->strstradd,
		this->strendadd,
		this->strquickadd,
		this->striqadd,
		this->strattradd,
		this->strhp,
		this->strmp,
		this->strearth,
		this->strwater,
		this->strfire,
		this->strwind,
		this->strinjury,
		this->strlocation,
		this->get_table_name(userid),
		this->struid,		userid,
		last_time);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, pets);
		memset(item.nick,0,sizeof(item.nick));
		INT_CPY_NEXT_FIELD(item.gettime);
		INT_CPY_NEXT_FIELD(item.pettype);
		INT_CPY_NEXT_FIELD(item.race);
		INT_CPY_NEXT_FIELD(item.flag);
		BIN_CPY_NEXT_FIELD(item.nick, sizeof(item.nick));
		INT_CPY_NEXT_FIELD(item.level);
		INT_CPY_NEXT_FIELD(item.exp);
		INT_CPY_NEXT_FIELD(item.physiqueinit);
		INT_CPY_NEXT_FIELD(item.strengthinit);
		INT_CPY_NEXT_FIELD(item.enduranceinit);
		INT_CPY_NEXT_FIELD(item.quickinit);
		INT_CPY_NEXT_FIELD(item.iqinit);
		INT_CPY_NEXT_FIELD(item.physiqueparam);
		INT_CPY_NEXT_FIELD(item.strengthparam);
		INT_CPY_NEXT_FIELD(item.enduranceparam);
		INT_CPY_NEXT_FIELD(item.quickparam);
		INT_CPY_NEXT_FIELD(item.iqparam);
		INT_CPY_NEXT_FIELD(item.physique_add);
		INT_CPY_NEXT_FIELD(item.strength_add);
		INT_CPY_NEXT_FIELD(item.endurance_add);
		INT_CPY_NEXT_FIELD(item.quick_add);
		INT_CPY_NEXT_FIELD(item.iq_add);
		INT_CPY_NEXT_FIELD(item.attr_addition);
		INT_CPY_NEXT_FIELD(item.hp);
		INT_CPY_NEXT_FIELD(item.mp);
		INT_CPY_NEXT_FIELD(item.earth);
		INT_CPY_NEXT_FIELD(item.water);
		INT_CPY_NEXT_FIELD(item.fire);
		INT_CPY_NEXT_FIELD(item.wind);
		INT_CPY_NEXT_FIELD(item.injury_state);
		INT_CPY_NEXT_FIELD(item.location);
	STD_QUERY_WHILE_END_NEW();
}
