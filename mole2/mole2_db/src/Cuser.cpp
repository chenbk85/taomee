/*
 * =====================================================================================
 *
 *       Filename:  Cuser.cpp
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

#include "Cuser.h"
#include <time.h>
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include <algorithm>
#include "Ccommon.h"

Cuser::Cuser(mysql_interface * db ) 
	:CtableRoute100x10(db, "MOLE2_USER" , "t_user" , "userid"),
	str_uid("userid"),
	str_nick("nick"),
	str_sign("signature"),
	str_flag("flag"),
	str_color("color"),
	str_regtime("register_time"),
	str_race("race"),
	str_profess("professtion"),
	str_honor("honor"),
	str_xiaomee("xiaomee"),
	str_level("level"),
	str_exp("experience"),
	str_physique("physique"),
	str_strength("strength"),
	str_endurance("endurance"),
	str_quick("quick"),
	str_iq("intelligence"),
	str_attradd("attr_addition"),
	str_hp("hp"),
	str_mp("mp"),
	str_msglist("msglist"),
	str_friendlist("friendlist"),
	str_blacklist("blacklist"),
	str_injury_state("injury_state"),
	str_front("in_front"),
	str_joblevel("joblevel"),
	str_max_attire("max_attire"),
	str_max_medicine("max_medicine"),
	str_max_stuff("max_stuff"),
	str_logout("last_logout"),
	str_axis_x("axis_x"),
	str_axis_y("axis_y"),
	str_mapid("last_mapid"),
	str_maptype("map_type"),
	str_login("login"),
	str_daytime("daytime"),
	str_fly_mapid("fly_mapid"),
	str_expbox("expbox"),
	str_energy("energy"),
	str_skill_expbox("skill_expbox"),
	str_flag_ex("flag_ex"),
	str_ban_flag("ban_flag"),
	str_winbossid("winbossid"),
	str_parent("parent"),
	str_lastonline("last_online"),
	str_total_time("total_time"),
	str_levelup_time("levelup_time"),
	str_sign_count("sign_count"),
	str_shop_name("shop_name")
{

}

int Cuser::get_all_info(userid_t userid, stru_mole2_user_user_info* p_out)
{
		GEN_SQLSTR( this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, \
				%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,\
				%s, %s, %s, %s from %s where %s=%u", 
				this->str_nick,
				this->str_sign,
				this->str_flag,
				this->str_color,
				this->str_regtime,
				this->str_race,
				this->str_profess,
				this->str_honor,
				this->str_xiaomee,
				this->str_level,
				this->str_exp,
				this->str_physique,
				this->str_strength,
				this->str_endurance,
				this->str_quick,
				this->str_iq,
				this->str_attradd,
				this->str_hp,
				this->str_mp,
				this->str_injury_state,
				this->str_front,
				this->str_joblevel,
				this->str_max_attire,
				this->str_max_medicine,
				this->str_max_stuff,
				this->get_table_name(userid),
				this->str_uid, userid );
		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR);
				BIN_CPY_NEXT_FIELD(p_out->nick,sizeof(p_out->nick));
				BIN_CPY_NEXT_FIELD(p_out->signature,sizeof(p_out->signature));
				INT_CPY_NEXT_FIELD(p_out->flag);
				INT_CPY_NEXT_FIELD(p_out->color);
				INT_CPY_NEXT_FIELD(p_out->register_time);
				INT_CPY_NEXT_FIELD(p_out->race);
				INT_CPY_NEXT_FIELD(p_out->professtion);
				INT_CPY_NEXT_FIELD(p_out->honor);
				INT_CPY_NEXT_FIELD(p_out->xiaomee);
				INT_CPY_NEXT_FIELD(p_out->level);
				INT_CPY_NEXT_FIELD(p_out->experience);
				INT_CPY_NEXT_FIELD(p_out->physique);
				INT_CPY_NEXT_FIELD(p_out->strength);
				INT_CPY_NEXT_FIELD(p_out->endurance);
				INT_CPY_NEXT_FIELD(p_out->quick);
				INT_CPY_NEXT_FIELD(p_out->intelligence);
				INT_CPY_NEXT_FIELD(p_out->attr_addition);
				INT_CPY_NEXT_FIELD(p_out->hp);
				INT_CPY_NEXT_FIELD(p_out->mp);
				INT_CPY_NEXT_FIELD(p_out->injury_state);
				INT_CPY_NEXT_FIELD(p_out->in_front);
				INT_CPY_NEXT_FIELD(p_out->joblevel);
				INT_CPY_NEXT_FIELD(p_out->max_attire);
				INT_CPY_NEXT_FIELD(p_out->max_medicine);
				INT_CPY_NEXT_FIELD(p_out->max_stuff);
		STD_QUERY_ONE_END();
}

int Cuser::get_online_login(userid_t userid, mole2_online_login_out* p_out)
{
	memset(p_out->nick,0,sizeof(p_out->nick));
	GEN_SQLSTR(this->sqlstr, "select  "
		"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, "
		"%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s "
		"from %s where %s=%u", 
			this->str_nick,
			this->str_flag,
			this->str_color,
			this->str_regtime,
			this->str_race,
			this->str_profess,
			this->str_honor,
			this->str_xiaomee,
			this->str_level,
			this->str_exp,
			this->str_physique,
			this->str_strength,
			this->str_endurance,
			this->str_quick,
			this->str_iq,
			this->str_attradd,
			this->str_hp,
			this->str_mp,
			this->str_msglist,
			this->str_injury_state,
			this->str_front,
			this->str_joblevel,
			this->str_max_attire,
			this->str_max_medicine,
			this->str_max_stuff,
			this->str_logout,
			this->str_axis_x,
			this->str_axis_y,
			this->str_mapid,
			this->str_daytime,
			this->str_fly_mapid,
			this->str_expbox,
			this->str_energy,
			this->str_skill_expbox,
			this->str_flag_ex,
			this->str_winbossid,
			this->str_parent,
			this->get_table_name(userid),
			this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr,USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD(p_out->nick,sizeof(p_out->nick));
			INT_CPY_NEXT_FIELD(p_out->flag);
			INT_CPY_NEXT_FIELD(p_out->color);
			INT_CPY_NEXT_FIELD(p_out->register_time);
			INT_CPY_NEXT_FIELD(p_out->race);
			INT_CPY_NEXT_FIELD(p_out->professtion);
			INT_CPY_NEXT_FIELD(p_out->honor);
			INT_CPY_NEXT_FIELD(p_out->xiaomee);
			INT_CPY_NEXT_FIELD(p_out->level);
			INT_CPY_NEXT_FIELD(p_out->experience);
			INT_CPY_NEXT_FIELD(p_out->physique);
			INT_CPY_NEXT_FIELD(p_out->strength);
			INT_CPY_NEXT_FIELD(p_out->endurance);
			INT_CPY_NEXT_FIELD(p_out->quick);
			INT_CPY_NEXT_FIELD(p_out->intelligence);
			INT_CPY_NEXT_FIELD(p_out->attr_addition);
			INT_CPY_NEXT_FIELD(p_out->hp);
			INT_CPY_NEXT_FIELD(p_out->mp);
			BIN_CPY_NEXT_FIELD(p_out->msglist,2000);
			INT_CPY_NEXT_FIELD(p_out->injury_state);
			INT_CPY_NEXT_FIELD(p_out->in_front);
			INT_CPY_NEXT_FIELD(p_out->joblevel);
			INT_CPY_NEXT_FIELD(p_out->max_attire);
			INT_CPY_NEXT_FIELD(p_out->max_medicine);
			INT_CPY_NEXT_FIELD(p_out->max_stuff);
			INT_CPY_NEXT_FIELD(p_out->time);
			INT_CPY_NEXT_FIELD(p_out->axis_x);
			INT_CPY_NEXT_FIELD(p_out->axis_y);
			INT_CPY_NEXT_FIELD(p_out->mapid);
			INT_CPY_NEXT_FIELD(p_out->daytime);
			INT_CPY_NEXT_FIELD(p_out->fly_mapid);
			INT_CPY_NEXT_FIELD(p_out->expbox);
			INT_CPY_NEXT_FIELD(p_out->energy);
			INT_CPY_NEXT_FIELD(p_out->skill_expbox);
			INT_CPY_NEXT_FIELD(p_out->flag_ex);
			INT_CPY_NEXT_FIELD(p_out->winbossid);
			INT_CPY_NEXT_FIELD(p_out->parent);
	STD_QUERY_ONE_END();
}

int Cuser::get_all_info(userid_t userid , stru_mole2_user_user_info_ex* p_out )
{
		GEN_SQLSTR( this->sqlstr, "select %s, %s, %s, %s, %s, %s, %s, %s, %s, %s,\
			%s, %s, %s, %s, %s, %s, %s, %s, %s, %s,%s,\
			%s, %s, %s, %s, %s, %s, %s, %s, %s, %s,\
			%s, %s, %s, %s, %s, %s, %s, %s, %s, %s from %s where %s=%u", 
				this->str_nick,
				this->str_sign,
				this->str_flag,
				this->str_color,
				this->str_regtime,
				this->str_race,
				this->str_profess,
				this->str_honor,
				this->str_xiaomee,
				this->str_level,
				this->str_exp,
				this->str_physique,
				this->str_strength,
				this->str_endurance,
				this->str_quick,
				this->str_iq,
				this->str_attradd,
				this->str_hp,
				this->str_mp,
				this->str_injury_state,
				this->str_front,
				this->str_joblevel,
				this->str_max_attire,
				this->str_max_medicine,
				this->str_max_stuff,
				this->str_logout,
				this->str_axis_x,
				this->str_axis_y,
				this->str_mapid,
				this->str_maptype,
				this->str_daytime,
				this->str_fly_mapid,
				this->str_expbox,
				this->str_energy,
				this->str_skill_expbox,
				this->str_flag_ex,
				this->str_winbossid,
				this->str_parent,
				this->str_total_time,
				this->str_levelup_time,
				this->str_sign_count,
				this->get_table_name(userid),
				this->str_uid, userid );
		STD_QUERY_ONE_BEGIN(this->sqlstr ,USER_ID_NOFIND_ERR);
				BIN_CPY_NEXT_FIELD(p_out->userinfo.nick,sizeof(p_out->userinfo.nick));
				BIN_CPY_NEXT_FIELD(p_out->userinfo.signature,sizeof(p_out->userinfo.signature));
				INT_CPY_NEXT_FIELD(p_out->userinfo.flag);
				INT_CPY_NEXT_FIELD(p_out->userinfo.color);
				INT_CPY_NEXT_FIELD(p_out->userinfo.register_time);
				INT_CPY_NEXT_FIELD(p_out->userinfo.race);
				INT_CPY_NEXT_FIELD(p_out->userinfo.professtion);
				INT_CPY_NEXT_FIELD(p_out->userinfo.honor);
				INT_CPY_NEXT_FIELD(p_out->userinfo.xiaomee);
				INT_CPY_NEXT_FIELD(p_out->userinfo.level);
				INT_CPY_NEXT_FIELD(p_out->userinfo.experience);
				INT_CPY_NEXT_FIELD(p_out->userinfo.physique);
				INT_CPY_NEXT_FIELD(p_out->userinfo.strength);
				INT_CPY_NEXT_FIELD(p_out->userinfo.endurance);
				INT_CPY_NEXT_FIELD(p_out->userinfo.quick);
				INT_CPY_NEXT_FIELD(p_out->userinfo.intelligence);
				INT_CPY_NEXT_FIELD(p_out->userinfo.attr_addition);
				INT_CPY_NEXT_FIELD(p_out->userinfo.hp);
				INT_CPY_NEXT_FIELD(p_out->userinfo.mp);
				INT_CPY_NEXT_FIELD(p_out->userinfo.injury_state);
				INT_CPY_NEXT_FIELD(p_out->userinfo.in_front);
				INT_CPY_NEXT_FIELD(p_out->userinfo.joblevel);
				INT_CPY_NEXT_FIELD(p_out->userinfo.max_attire);
				INT_CPY_NEXT_FIELD(p_out->userinfo.max_medicine);
				INT_CPY_NEXT_FIELD(p_out->userinfo.max_stuff);
				INT_CPY_NEXT_FIELD(p_out->time);
				INT_CPY_NEXT_FIELD(p_out->axis_x);
				INT_CPY_NEXT_FIELD(p_out->axis_y);
				INT_CPY_NEXT_FIELD(p_out->mapid);
				INT_CPY_NEXT_FIELD(p_out->maptype);
				INT_CPY_NEXT_FIELD(p_out->daytime);
				INT_CPY_NEXT_FIELD(p_out->fly_mapid);
				INT_CPY_NEXT_FIELD(p_out->expbox);
				INT_CPY_NEXT_FIELD(p_out->energy);
				INT_CPY_NEXT_FIELD(p_out->skill_expbox);
				INT_CPY_NEXT_FIELD(p_out->flag_ex);
				INT_CPY_NEXT_FIELD(p_out->winbossid);
				INT_CPY_NEXT_FIELD(p_out->parent);
				INT_CPY_NEXT_FIELD(p_out->total_time);
				INT_CPY_NEXT_FIELD(p_out->levelup_time);
				INT_CPY_NEXT_FIELD(p_out->sign_count);
		STD_QUERY_ONE_END();
}


int Cuser::get_login_info(userid_t userid, mole2_user_user_login_out_header* p_out_header, 
		mole2_user_user_login_out_item** pp_idlist)
{
	stru_id_list friendlist, blacklist;

	int ret = this->get_friend_black_list(userid, &(p_out_header->vip), &friendlist, &blacklist);
	if (ret == SUCC)
	{
		*pp_idlist = (mole2_user_user_login_out_item *)malloc((friendlist.count + blacklist.count) * sizeof(userid_t));
		if (*pp_idlist == NULL)
			return SYS_ERR;
		int cpy_len = 0;
		char* p_offset = (char*)(*pp_idlist);
		memcpy(p_offset + cpy_len, friendlist.iditem, friendlist.count * sizeof(userid_t));
		cpy_len = friendlist.count * sizeof(userid_t);
		memcpy(p_offset + cpy_len, blacklist.iditem, blacklist.count * sizeof(userid_t));
		cpy_len = blacklist.count * sizeof(userid_t);
		p_out_header->friend_cnt = friendlist.count;
		p_out_header->black_cnt = blacklist.count;
	}
	return ret;
}

int	Cuser::create_role(userid_t userid, mole2_user_user_create_role_in* p_in)
{
	int ret = this->insert(userid, p_in->parent,p_in->nick, p_in->signature, p_in->color, p_in->professtion, time(0));
	return ret;
}

int Cuser::get_offline_msg(userid_t userid, mole2_user_user_get_offline_msg_out* p_out)
{
	GEN_SQLSTR( this->sqlstr, "select %s from %s where %s=%u ", 
			this->str_msglist, this->get_table_name(userid), this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, USER_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (p_out, sizeof (mole2_user_user_get_offline_msg_out) );
	STD_QUERY_ONE_END();
}

int Cuser::set_offline_msg(userid_t userid, mole2_user_user_get_offline_msg_out* msglist)
{
	char mysql_msglist[mysql_str_len(sizeof (mole2_user_user_get_offline_msg_out))];
	set_mysql_string(mysql_msglist,(char*)msglist, msglist->msglen + sizeof(msglist->msglen) + sizeof(msglist->count));
	GEN_SQLSTR( this->sqlstr, "update %s set %s='%s' where %s=%u" ,
		this->get_table_name(userid), 
		this->str_msglist, mysql_msglist,
		this->str_uid, userid );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser::add_friend(userid_t userid, userid_t uid)
{
	uint32_t flag = 0;
	stru_id_list friendlist={};
    stru_id_list blacklist={};
    uint32_t * find_index, *idstart, *idend;
    uint32_t allow_max_count=0;
    int ret;

    ret=this->get_friend_black_list(userid ,&flag  , &friendlist, &blacklist);
    if(ret!=SUCC) return ret;

	if(blacklist.count > 200) blacklist.count = 200;
	if(friendlist.count > 200) friendlist.count = 200;

    //检查是否在blacklist中
    idstart=blacklist.iditem;
    idend=idstart+blacklist.count;

    find_index=std::find( idstart, idend , uid );
    if (find_index!=idend ) {
        //find 
        ret=this->del_id(userid , this->str_blacklist,uid );
        if (ret!=SUCC){ return ret; }
    }

    if ((flag& MOLE2_FLAG_VIP )==MOLE2_FLAG_VIP ){ //vip
        allow_max_count=200;
    }else{
        allow_max_count=100;
    }

    idstart=friendlist.iditem;
    idend=idstart+friendlist.count;

    find_index=std::find( idstart ,  idend ,  uid );
    if (find_index!=idend ) {
        //find 
        return MOLE2_FRIENDID_EXISTED_ERR;
    }else if ( friendlist.count >= allow_max_count){
        //out of max value
        return MOLE2_FRIEND_MAX_ERR;
    }else{
		DEBUG_LOG("before add friend the count is %u", friendlist.count);
        friendlist.iditem[friendlist.count ]=uid;
        friendlist.count++;
        return this->update_idlist(userid,this->str_friendlist,&friendlist);
    }
}

int Cuser::del_friend(userid_t userid, userid_t uid)
{
	return this->del_id(userid, this->str_friendlist, uid);
}
int Cuser::get_friend_black_list(userid_t userid ,uint32_t *p_flag,
       stru_id_list* p_friendlist, stru_id_list* p_blacklist )
{
    //sprintf( this->sqlstr, "select vip, friendlist, blacklist
    sprintf( this->sqlstr, "select flag &0x1, %s, %s from %s where %s=%u ",
			this->str_friendlist, this->str_blacklist,
			this->get_table_name(userid), this->str_uid, userid);

    STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
        //copy idlist  
        INT_CPY_NEXT_FIELD(*p_flag ) ;
        BIN_CPY_NEXT_FIELD(p_friendlist , sizeof(stru_id_list ));
        BIN_CPY_NEXT_FIELD(p_blacklist, sizeof(stru_id_list ));
    STD_QUERY_ONE_END();
}

int Cuser::get_friend_list(userid_t userid, mole2_user_user_get_friend_list_out* p_out)
{
	return this->get_idlist(userid, this->str_friendlist, p_out);
}

int Cuser::check_is_friend(userid_t userid, mole2_user_user_check_is_friend_in* p_in, 
		mole2_user_user_check_is_friend_out* p_out)
{
	int ret;
    stru_id_list idlist={ };
    uint32_t * find_index, *idstart, *idend;
    ret=this->get_idlist(userid, this->str_friendlist,&idlist);
    if (ret==SUCC){

		if(idlist.count > 200) idlist.count = 200;
		
        idstart=idlist.iditem;
        idend=idstart+idlist.count;

        find_index=std::find( idstart ,  idend , p_in->userid );
        if (find_index!=idend ) {
            //find 
            p_out->isfriend=1;
        }else{
            p_out->isfriend=0;
        }
        return SUCC;
    }else{
        return ret;
    }

}

int Cuser::add_blackuser(userid_t userid, userid_t uid)
{
	uint32_t flag = 0;
	stru_id_list friendlist={};
    stru_id_list blacklist={};
    uint32_t * find_index, *idstart, *idend;
    uint32_t allow_max_count=0;
    int ret;

    ret=this->get_friend_black_list(userid ,&flag  , &friendlist, &blacklist);
    if(ret!=SUCC) return ret;

	if(blacklist.count > 200) blacklist.count = 200;
	if(friendlist.count > 200) friendlist.count = 200;

    //检查是否在friendlist中
    idstart=friendlist.iditem;
    idend=idstart+friendlist.count;

    find_index=std::find( idstart, idend , uid );
    if (find_index!=idend ) {
        //find 
        ret=this->del_id(userid , this->str_friendlist,uid );
        if (ret!=SUCC){ return ret; }
    }

    if ((flag& MOLE2_FLAG_VIP )==MOLE2_FLAG_VIP ){ //vip
        allow_max_count=200;
    }else{
        allow_max_count=100;
    }

    idstart=blacklist.iditem;
    idend=idstart+blacklist.count;

    find_index=std::find( idstart ,  idend ,  uid );
    if (find_index!=idend ) {
        //find 
        return MOLE2_BLACKID_EXISTED_ERR;
    }else if (blacklist.count >= allow_max_count){
        //out of max value
        return MOLE2_BLACKID_MAX_ERR;
    }else{
        blacklist.iditem[blacklist.count ]=uid;
        blacklist.count++;
        return this->update_idlist(userid,this->str_blacklist,&blacklist);
    }
}

int Cuser::del_blackuser(userid_t userid, userid_t uid)
{
	return this->del_id(userid, this->str_blacklist, uid);
}

int Cuser::get_black_list(userid_t userid, mole2_user_user_get_black_list_out* p_out)
{
	return this->get_idlist(userid, this->str_blacklist, p_out);
}

int	Cuser::insert(userid_t userid, userid_t parent,char* nick, char* sign, uint32_t color, uint8_t professtion, uint32_t regtime)
{
	char nick_mysql[mysql_str_len(16)];
	char signature_mysql[mysql_str_len(32)];
	set_mysql_string(nick_mysql, (char *)(nick), 16);
	set_mysql_string(signature_mysql, (char *)(sign), 32);
	sprintf( this->sqlstr, "insert into %s(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, \
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s,\
		%s, %s, %s, %s, %s, %s, %s, %s, %s, %s,\
		%s, %s, %s, %s, %s, %s, %s) values( %u,%u,'%s','%s',%u, %u, %u, %u, %u, %u, %u,\
		%u, %u, %u, %u, %u, %u, %u, %u, %u, %u,\
		%s, %s, %s, %u, %u, %u, %u, %u, %u, %u,\
		%u, %u, %u, %u, %u, %u, %u)",
		this->get_table_name(userid),
		this->str_uid, this->str_parent,this->str_nick, this->str_sign, this->str_flag, this->str_color,
		this->str_regtime, this->str_race, this->str_profess, this->str_honor, this->str_xiaomee,
		this->str_level, this->str_exp, this->str_physique, this->str_strength, this->str_endurance,
		this->str_quick, this->str_iq, this->str_attradd, this->str_hp, this->str_mp,
		this->str_friendlist, this->str_blacklist, this->str_msglist, this->str_injury_state, this->str_front,
		this->str_joblevel, this->str_max_attire, this->str_max_medicine, this->str_max_stuff, this->str_logout,
		this->str_axis_x, this->str_axis_y, this->str_mapid, this->str_login, this->str_daytime,
		this->str_fly_mapid, this->str_expbox,
		userid, 
		parent,
		nick_mysql, 
		signature_mysql, 
		1 << 6, 					/* flag  team off*/
		color, 
		regtime, 
		0,							/* race */ 
		professtion, 
		0,							/* honor */ 
		0,							/* xiaomee */ 		//10
		1,							/* level */ 
		0, 							/* experience */
		0, 						/* physique */
		0, 						/* strength */
		0, 						/* endurance */
		0, 						/* quick */
		0, 						/* intelligence */
		0, 							/* attr_addition */
		1, 						/* hp */
		1, 						/* mp */			//20
		"0x00000000",				/* friendlis */
		"0x00000000",				/* blacklist */
		"0x00000000",				/* msglist */
		0,							// injury_state
		0,							/* in_front */
		0,							//joblevel
		0,						// max_attire
		0,						// max_medicine
		0,						//max_stuff
		regtime,						//last_logout			//30
		0,						//axis x
		0,						//axis y
		0,						//last_mapid
		regtime,						//login
		0,						//daytime
		0,						//fly_mapid
		1000);						//expbox
	return this->exec_insert_sql( this->sqlstr, USER_ID_EXISTED_ERR );
}

int Cuser::update_nick(userid_t userid, mole2_user_user_update_nick_in* p_in)
{
	char nick_mysql[mysql_str_len(sizeof(p_in->nick))];
	set_mysql_string(nick_mysql, (char *)(p_in->nick), sizeof(p_in->nick));
	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s' where %s=%u", this->get_table_name(userid), 
			this->str_nick, nick_mysql, this->str_uid, userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR );
}

int Cuser::update_signature(userid_t userid, mole2_user_user_update_signature_in* p_in)
{
	char signature_mysql[mysql_str_len(sizeof(p_in->signature))];
	set_mysql_string(signature_mysql, (char *)(p_in->signature), sizeof(p_in->signature));
	GEN_SQLSTR(this->sqlstr, "update %s set %s='%s' where %s=%u",
			this->get_table_name(userid), 
			this->str_sign,	signature_mysql, 
			this->str_uid,	userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::update_color(userid_t userid, mole2_user_user_update_color_in* p_in)
{
	return this->set_int_value(userid, this->str_color, p_in->color);
}

int Cuser::update_professtion(userid_t userid, mole2_user_user_update_professtion_in* p_in)
{
	return this->set_int_value(userid, this->str_profess, p_in->professtion);
}

int Cuser::add_xiaomee(userid_t userid, int32_t add_value, mole2_user_user_add_xiaomee_out* p_out, int add_limit)
{
	int ret = this->get_int_value(userid, this->str_xiaomee, &p_out->xiaomee);

	if (ret != SUCC)
		return ret;
	p_out->xiaomee = Ccommon::add_limit32(p_out->xiaomee, add_value, 0, MOLE2_MAX_XIAOMEE, &(p_out->add_value));
	DEBUG_LOG("wwww %d %d",p_out->add_value,p_out->xiaomee);
	return this->set_int_value(userid, this->str_xiaomee, p_out->xiaomee);
}

int Cuser::get_level_exp_attr(userid_t userid, uint32_t* p_level, uint32_t* p_exp, uint16_t* p_attr)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s from %s where %s=%u", 
			this->str_level, this->str_exp, this->str_attradd, 
			this->get_table_name(userid), 
			this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_level);
		INT_CPY_NEXT_FIELD(*p_exp);
		INT_CPY_NEXT_FIELD(*p_attr);
	STD_QUERY_ONE_END();
}

int Cuser::set_level_exp_attr(userid_t userid, uint32_t level, uint32_t exp, uint16_t attr)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u where %s=%u",
			this->get_table_name(userid),
			this->str_level,	level,
			this->str_exp,		exp,
			this->str_attradd,	attr,
			this->str_uid,		userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cuser::add_exp(userid_t userid, int32_t exp, mole2_user_user_add_exp_out* p_out)
{
	if (exp < 0) return VALUE_OUT_OF_RANGE_ERR;

	int ret = this->get_level_exp_attr(userid, &(p_out->level), &(p_out->experience), &(p_out->attr_addition));
	if (exp == 0) return ret;

	p_out->experience = Ccommon::add_limit32(p_out->experience, exp, 0, 0xFFFFFFFF);
	uint32_t level = p_out->level;
	while ((level + 1) * (level + 1) * (level + 1) * (level + 1) <= p_out->experience)
	{
		level ++;
	}

	if (level > p_out->level)
	{
		uint16_t attr_addition = p_out->attr_addition + 4 * (level - p_out->level);
		p_out->attr_addition = attr_addition < p_out->attr_addition ? 0xFFFF : attr_addition;
		p_out->level = level;
	}
	
	return this->set_level_exp_attr(userid, p_out->level, p_out->experience, p_out->attr_addition);
}

int Cuser::add_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_in* p_in,
		mole2_user_user_add_attr_addition_out* p_out)
{
	int ret = SYS_ERR;
	if((NULL == p_out) || (ret = this->get_attr_addition(userid, p_out)) != DB_SUCC)
		return ret;
	p_out->attr_addition = Ccommon::add_limit32(p_out->attr_addition, p_in->attr_addition, 0, MOLE2_MAX_ATTR_ADDTION);
	return this->set_attr_addition(userid, p_out);	
}
/*
int Cuser::add_hp_mp(userid_t userid, mole2_user_user_add_hp_mp_in* p_in,
		mole2_user_user_add_hp_mp_out* p_out)
{
	int ret = SYS_ERR;
	if((NULL == p_out) || (ret=this->get_hp_mp(userid, p_out)) != DB_SUCC)
		return ret;
	p_out->hp = Ccommon::add_limit32(p_out->hp, p_in->hp, 0, MOLE2_MAX_HP);
	p_out->mp = Ccommon::add_limit32(p_out->mp, p_in->mp, 0, MOLE2_MAX_MP);
	return this->set_hp_mp(userid, p_out->hp, p_out->mp);
}
*/
/* 
int Cuser::update_hp_mp(userid_t userid, mole2_user_user_set_hp_mp_in* p_in,
			mole2_user_user_set_hp_mp_out* p_out)
{
	p_out->hp = p_in->hp > MOLE2_MAX_HP ? MOLE2_MAX_HP : p_in->hp;
	p_out->mp = p_in->mp > MOLE2_MAX_MP ? MOLE2_MAX_MP : p_in->mp;
	return this->set_hp_mp(userid, p_out);
}
 
int Cuser::add_four_attr(userid_t userid, mole2_user_user_add_four_attr_in* p_in,
		mole2_user_user_add_four_attr_out* p_out)
{
	int ret = SYS_ERR;
	if((NULL == p_out) || (ret = this->get_four_attr(userid, p_out)) != DB_SUCC)
		return ret;
	p_out->earth = Ccommon::add_limit32(p_out->earth, p_in->earth, 0, MOLE2_MAX_EARTH);
	p_out->water = Ccommon::add_limit32(p_out->water, p_in->water, 0, MOLE2_MAX_WATER);
	p_out->fire = Ccommon::add_limit32(p_out->fire, p_in->fire, 0, MOLE2_MAX_FIRE);
	p_out->wind = Ccommon::add_limit32(p_out->wind, p_in->wind, 0, MOLE2_MAX_WIND);
	return this->set_four_attr(userid, p_out);	
}
*/

int Cuser::get_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s=%u", this->str_attradd,
			this->get_table_name(userid), this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->attr_addition);
	STD_QUERY_ONE_END();
}

int Cuser::set_attr_addition(userid_t userid, mole2_user_user_add_attr_addition_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s=%u",this->get_table_name(userid), 
			this->str_attradd,	p_out->attr_addition, 
			this->str_uid,		userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}	

int Cuser::get_base_attr(userid_t userid, mole2_user_user_add_base_attr_out* p_attr)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s, %s, %s from %s where %s=%u", 
			this->str_physique,
			this->str_strength,
			this->str_endurance,
			this->str_quick,
			this->str_iq,
			this->str_attradd,
			this->get_table_name(userid), 
			this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_attr->physique);
		INT_CPY_NEXT_FIELD(p_attr->strength);
		INT_CPY_NEXT_FIELD(p_attr->endurance);
		INT_CPY_NEXT_FIELD(p_attr->quick);
		INT_CPY_NEXT_FIELD(p_attr->intelligence);
		INT_CPY_NEXT_FIELD(p_attr->attr_addition);
	STD_QUERY_ONE_END();
}

int Cuser::set_base_attr(userid_t userid, mole2_user_user_add_base_attr_in* p_in)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u where %s=%u", 
			this->get_table_name(userid), 
			this->str_physique,		p_in->physique,
			this->str_strength,		p_in->strength, 
			this->str_endurance,	p_in->endurance, 
			this->str_quick,		p_in->quick, 
			this->str_iq,			p_in->intelligence, 
			this->str_attradd,		p_in->attr_addition,
			this->str_uid,			userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::get_hp_mp(userid_t userid, mole2_user_user_add_hp_mp_out* p_in)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s=%u", 
			this->str_hp, this->str_mp, this->get_table_name(userid), 
			this->str_uid, userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_in->hp);
		INT_CPY_NEXT_FIELD(p_in->mp);
	STD_QUERY_ONE_END();
}

int Cuser::add_hp_mp(userid_t userid, int hp, int mp, uint32_t injury_state)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%s+%d, %s=%s+%d, %s=%u where %s=%u", 
			this->get_table_name(userid), 
			this->str_hp,	this->str_hp,	hp, 
			this->str_mp,	this->str_mp,	mp,
			this->str_injury_state, injury_state,
			this->str_uid,	userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::set_hp_mp(userid_t userid, uint32_t hp, uint32_t mp)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u where %s=%u", 
			this->get_table_name(userid), 
			this->str_hp,	hp, 
			this->str_mp,	mp,
			this->str_uid,	userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::set_hp_mp_injury(userid_t userid, uint32_t hp, uint32_t mp, uint32_t injury)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u ,%s=%u where %s=%u", 
			this->get_table_name(userid), 
			this->str_hp,	hp, 
			this->str_mp,	mp,
			this->str_injury_state, injury,
			this->str_uid,	userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

/* 
int Cuser::get_four_attr(userid_t userid, mole2_user_user_add_four_attr_out* p_in)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s, %s, %s from %s where %s=%u", 
			this->str_earth,
			this->str_water,
			this->str_fire,
			this->str_wind,
			this->get_table_name(userid), 
			this->str_uid,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_in->earth);
		INT_CPY_NEXT_FIELD(p_in->water);
		INT_CPY_NEXT_FIELD(p_in->fire);
		INT_CPY_NEXT_FIELD(p_in->wind);
	STD_QUERY_ONE_END();

}

int Cuser::set_four_attr(userid_t userid, mole2_user_user_add_four_attr_out* p_out)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u, %s=%u, %s=%u where %s=%u", 
			this->get_table_name(userid), 
			this->str_earth,p_out->earth,
			this->str_water,p_out->water, 
			this->str_fire,	p_out->fire, 
			this->str_wind,	p_out->wind, 
			this->str_uid,	userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}
*/

int Cuser::del_id(userid_t userid ,const char * id_flag_str , userid_t id )
{
    int ret;
    stru_id_list idlist={ };
    uint32_t * new_idend, *idstart, *idend;
    ret=this->get_idlist(userid,id_flag_str,&idlist);
    if (ret==SUCC){
		if(idlist.count > 200) idlist.count = 200;
		
        idstart=idlist.iditem;
        idend=idstart+idlist.count;
        new_idend=std::remove( idstart ,  idend ,  id );
        if (new_idend != idend) {
            idlist.count=new_idend-idstart;
            return this->update_idlist(userid,id_flag_str ,&idlist);
        }else{
            return LIST_ID_NOFIND;
        }

    }else{
        return ret;
    }
}

int Cuser::get_idlist(userid_t userid ,const  char * id_flag_str , stru_id_list * p_idlist )
{
    memset(p_idlist,0,sizeof(*p_idlist) );
    sprintf( this->sqlstr, "select  %s \
            from %s where userid=%u ",
        id_flag_str,this->get_table_name(userid), userid);

    //copy idlist  
    STD_QUERY_ONE_BEGIN(this-> sqlstr,USER_ID_NOFIND_ERR);
        BIN_CPY_NEXT_FIELD(p_idlist, sizeof(*p_idlist ));
    STD_QUERY_ONE_END();
}

int Cuser::update_idlist(userid_t userid , const char * id_flag_str ,  stru_id_list * p_idlist)
{
    char mysql_idlist[mysql_str_len(sizeof(*p_idlist ) )];
    set_mysql_string(mysql_idlist,(char*)p_idlist, 4 +sizeof (userid_t)*(p_idlist->count));
    sprintf( this->sqlstr, " update %s set \
        %s ='%s'\
        where userid=%u " ,
        this->get_table_name(userid),
        id_flag_str,
        mysql_idlist,
        userid );
    return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR );
}

int Cuser::injury_state_update(userid_t userid, uint32_t injury_state)
{
	sprintf(this->sqlstr, "update %s set %s = %u where userid = %u", 
		this->get_table_name(userid),
		str_injury_state,
		injury_state,
		userid
		);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::injury_state_select(uint32_t userid, uint32_t* injury_state)
{
	sprintf(this->sqlstr, "select injury_state from %s where userid = %u",
			this->get_table_name(userid),
			userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*injury_state);
	STD_QUERY_ONE_END();
}

int Cuser::set_pos(uint32_t userid, uint8_t pos)
{
	return this->set_int_value(userid, this->str_front, pos);
}

int Cuser::add_xiaomee(uint32_t userid, int32_t value)
{
/*
	sprintf(this->sqlstr, "update %s set xiaomee = xiaomee + %d where userid = %u",
			this->get_table_name(userid),
			value,
			userid
			);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
*/
    uint32_t xiaomee;
    int ret = this->get_int_value(userid, this->str_xiaomee,&xiaomee);
    if (ret != SUCC)
        return ret;
	//DEBUG_LOG("dddd value%d",value);
    xiaomee = Ccommon::add_limit32(xiaomee, value, 0, MOLE2_MAX_XIAOMEE, &value);
	//DEBUG_LOG("dddd %d",xiaomee);
    return this->set_int_value(userid, this->str_xiaomee, xiaomee);
}

int Cuser::add_exp(uint32_t userid, uint32_t value)
{
	sprintf(this->sqlstr, "update %s set %s = %s + %u where userid = %u",
			this->get_table_name(userid),
			this->str_exp, this->str_exp, value,
			userid
			);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int	Cuser::update_role(userid_t userid, char* nick, uint32_t prof,uint32_t color, uint16_t* p_attr, uint32_t hp, uint32_t mp)
{
	char nick_mysql[mysql_str_len(16)];
	set_mysql_string(nick_mysql, (char *)(nick), 16);
	
	GEN_SQLSTR( this->sqlstr, "update %s set %s='%s', %s=%u, %s=%u, %s=%s | %u, %s=%u, %s=%u, %s=%u, %s=%u, %s=%u,\
			%s=%u, %s=%u, %s=%u, %s=%u where %s = %u",
		this->get_table_name(userid), 
		this->str_nick,			nick_mysql,
		this->str_profess,		prof,
		this->str_color,		color,
		this->str_flag,			this->str_flag,	2,
		this->str_physique,		p_attr[0],
		this->str_strength,		p_attr[1],
		this->str_endurance,	p_attr[2],
		this->str_quick,		p_attr[3],
		this->str_iq,			p_attr[4],
		this->str_hp,			hp,
		this->str_mp,			mp,
		this->str_exp,			0,
		this->str_level,		1,
		this->str_uid,			userid);
	return this->exec_update_sql( this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::levelup_update(uint32_t userid, stru_mole2_user_levelup_info *p_in)
{
	sprintf(this->sqlstr, "update %s set experience = %u, level = %u,\
		attr_addition = %u, hp = %u, mp = %u where userid = %u",
		this->get_table_name(userid),
		p_in->experience,
		p_in->level,
		p_in->addition,
		p_in->hp,
		p_in->mp,
		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::flag_set(uint32_t userid, uint32_t flag, uint32_t flag_ex)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u, %s=%u where %s = %u",
			this->get_table_name(userid),
			this->str_flag,		flag,
			this->str_flag_ex,	flag_ex,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::reward_set(uint32_t userid, uint32_t xiaomee, uint32_t experience,
							uint32_t level, uint32_t attr_addition, uint32_t hp,
							uint32_t mp, uint32_t expbox)
{
	sprintf(this->sqlstr, "update %s set %s=%u,%s=%u,%s=%u,%s=%u,%s=%u,%s=%u,%s=%u where %s=%u",
			this->get_table_name(userid),
			this->str_xiaomee,	xiaomee,
			this->str_exp,		experience,
			this->str_level,	level,
			this->str_attradd,	attr_addition,
			this->str_hp,		hp,
			this->str_mp,		mp,
			this->str_expbox,	expbox,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::reward_set_ex(uint32_t userid, uint32_t xiaomee, uint32_t experience,
							uint32_t level, uint32_t attr_addition, uint32_t hp,
							uint32_t mp, uint32_t expbox, uint32_t skill_expbox,
							uint32_t flag_ex, uint32_t joblevel)
{
	sprintf(this->sqlstr, "update %s set %s=%u,%s=%u,%s=%u,%s=%u,%s=%u,%s=%u,\
				%s=%u,%s=%u,%s=%u, %s=%u where %s=%u",
			this->get_table_name(userid),
			this->str_xiaomee,	xiaomee,
			this->str_exp,		experience,
			this->str_level,	level,
			this->str_attradd,	attr_addition,
			this->str_hp,		hp,
			this->str_mp,		mp,
			this->str_expbox,	expbox,
			this->str_skill_expbox,		skill_expbox,
			this->str_flag_ex,	flag_ex,
			this->str_joblevel,	joblevel,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser::xiaomee_get(uint32_t userid, uint32_t* p_xiaomee)
{
	sprintf(this->sqlstr, "select xiaomee from %s where userid = %u",
		this->get_table_name(userid),
		userid
	);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_xiaomee);
	STD_QUERY_ONE_END();
}

int Cuser::profession_set(userid_t userid, uint8_t profession)
{
	return this->set_int_value(userid, this->str_profess, profession);
}

int Cuser::update_total_time(uint32_t userid,uint32_t timeadd)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %s + %u,%s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_total_time,this->str_total_time,timeadd,this->str_login,(uint32_t)time(NULL),
			this->str_uid,userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::logout_position_set(uint32_t userid,
		uint32_t time,
		uint32_t axis_x,
		uint32_t axis_y,
		uint32_t mapid,
		uint32_t maptype,
		uint32_t daytime,
		uint32_t fly_mapid,
		uint32_t last_online)
{
	if(mapid) {
		GEN_SQLSTR(this->sqlstr, "update %s set %s = %u, %s = %u, %s = %u, %s = %u, %s = %u, %s = %u,%s = %u ,%s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_logout,	time,
			this->str_axis_x,	axis_x,
			this->str_axis_y,	axis_y,
			this->str_mapid,	mapid,
			this->str_maptype,	maptype,
			this->str_daytime,	daytime,
			this->str_fly_mapid, fly_mapid,
			this->str_lastonline,last_online,
			this->str_uid,		userid);
	} else {
		GEN_SQLSTR(this->sqlstr, "update %s set %s = %u, %s = %u, %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_logout,	time,
			this->str_daytime,	daytime,
			this->str_lastonline,last_online,
			this->str_uid,		userid);
	}
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::logout_position_get(uint32_t userid, uint32_t* p_time, uint32_t* p_axis_x, uint32_t* p_axis_y, uint32_t* p_mapid)
{
	GEN_SQLSTR(this->sqlstr, "select %s,%s,%s,%s from %s where %s = %u",
		this->str_logout,
		this->str_axis_x,
		this->str_axis_y,
		this->str_mapid,
		this->get_table_name(userid),
		this->str_uid,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_time);
		INT_CPY_NEXT_FIELD(*p_axis_x);
		INT_CPY_NEXT_FIELD(*p_axis_y);
		INT_CPY_NEXT_FIELD(*p_mapid);
	STD_QUERY_ONE_END();
}

int Cuser::honor_set(uint32_t userid, uint32_t titleid)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_honor,	titleid,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::login_set(uint32_t userid, uint32_t login)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_login,	login,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::login_set(uint32_t userid, uint32_t login, uint32_t daytime)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u, %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_login,	login,
			this->str_daytime,	daytime,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::login_get(uint32_t userid, uint32_t* p_login, uint32_t* p_daytime)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s from %s where %s = %u",
		this->str_login,
		this->str_daytime,
		this->get_table_name(userid),
		this->str_uid,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_login);
		INT_CPY_NEXT_FIELD(*p_daytime);
	STD_QUERY_ONE_END();
}

int Cuser::nick_get(uint32_t userid, char* sz_nick)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s = %u",
		this->str_nick,
		this->get_table_name(userid),
		this->str_uid,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(sz_nick, NICK_LEN);
	STD_QUERY_ONE_END();
}

int Cuser::exp_box_set(userid_t userid, uint32_t expbox)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_expbox,	expbox,
			this->str_uid,		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::exp_box_get(userid_t userid, uint32_t* expbox)
{
	GEN_SQLSTR(this->sqlstr, "select %s from %s where %s = %u",
			this->str_expbox,
			this->get_table_name(userid),
			this->str_uid,		userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*expbox);
	STD_QUERY_ONE_END();
}

int Cuser::exp_info_set(uint32_t userid, uint32_t exp, uint32_t level, uint32_t attr, uint32_t hp, uint32_t mp, uint32_t expbox)
{
	sprintf(this->sqlstr, "update %s set %s=%u, %s=%u,\
		%s=%u, %s=%u, %s=%u, %s=%u where userid = %u",
		this->get_table_name(userid),
		str_exp,	exp,
		str_level,	level,
		str_attradd,	attr,
		str_hp,		hp,
		str_mp,		mp,
		str_expbox,		expbox,
		userid);
	return this->exec_update_sql(sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::for_home_get(uint32_t userid, char* sz_nick, uint32_t* p_expbox,uint32_t* p_joblvl)
{
	GEN_SQLSTR(this->sqlstr, "select %s, %s ,%s from %s where %s = %u",
		this->str_nick,
		this->str_expbox,
		this->str_joblevel,
		this->get_table_name(userid),
		this->str_uid,	userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(sz_nick, NICK_LEN);
		INT_CPY_NEXT_FIELD(*p_expbox);
		INT_CPY_NEXT_FIELD(*p_joblvl);
	STD_QUERY_ONE_END();
}

int Cuser::energy_set(uint32_t userid, uint32_t energy)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u",
			this->get_table_name(userid),
			this->str_energy,	energy,
			this->str_uid,		userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::set_ban_flag(userid_t userid,uint32_t ban_flag)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s = %u,%s = %u where %s = %u",
		this->get_table_name(userid),
		this->str_ban_flag,ban_flag,
		this->str_logout,(uint32_t)time(NULL),
		this->str_uid,userid);
	return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR);
}

int Cuser::check_ban_flag(userid_t userid)
{
	uint32_t logout;
	uint32_t ban_flag;
	uint32_t diff_time;

	GEN_SQLSTR(this->sqlstr,"select %s,%s from %s where %s = %u",
		this->str_logout,
		this->str_ban_flag,
		this->get_table_name(userid),
		this->str_uid,userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(logout);
		INT_CPY_NEXT_FIELD(ban_flag);
	STD_QUERY_ONE_END_WITHOUT_RETURN();

	diff_time = (uint32_t)time(NULL) - logout;

	if(ban_flag & BAN_FOREVER) {
		return BAN_FOREVER_ERR;
	} else if((ban_flag & BAN_14DAYS) && diff_time < 14 * 24 * 3600) {
		return BAN_14DAYS_ERR;
	} else if((ban_flag & BAN_7DAYS) && diff_time < 7 * 24 * 3600) {
		return BAN_7DAYS_ERR;
	} else if((ban_flag & BAN_24HOURS) && diff_time < 24 * 3600) {
		return BAN_24HOURS_ERR;
	} else if(ban_flag){
		this->set_ban_flag(userid,0);
	}

	return SUCC;
}

int Cuser::set_win_bossid(userid_t userid, uint32_t bossid)
{
        GEN_SQLSTR(this->sqlstr, "update %s set %s = %u where %s = %u",
                this->get_table_name(userid),
                this->str_winbossid,bossid,
                this->str_uid,userid);
        return this->exec_update_sql(this->sqlstr,USER_ID_NOFIND_ERR);
}

int Cuser::set_joblevel(userid_t userid, uint32_t joblevel)
{
	GEN_SQLSTR(this->sqlstr, "update %s set %s=%u where %s=%u",
			this->get_table_name(userid),
			this->str_joblevel,	joblevel,
			this->str_uid,		userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::set_field_value(userid_t userid, su_mole2_set_field_value_in *p_in)
{
	char mysql_value[mysql_str_len(sizeof(p_in->value))];
	set_mysql_string(mysql_value, p_in->value, sizeof(p_in->value));

	GEN_SQLSTR(this->sqlstr, "update %s set %s = '%s' where %s = %u", 
				this->get_table_name(userid), p_in->field,	mysql_value, 
				this->str_uid,	userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser::get_shop_name(userid_t userid, char* nick)
{
	//GEN_SQLSTR(this->sqlstr, "select %s from %s where %s = %u",
			//this->str_shop_name,
			//this->get_table_name(userid),
			//this->str_uid,	userid);
	//STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		//BIN_CPY_NEXT_FIELD(nick, NICK_LEN);
	//STD_QUERY_ONE_END();
	return 0;
}

int Cuser::set_shop_name(userid_t userid, char* nick)
{
	//char mysql_value[NICK_LEN];
	//set_mysql_string(mysql_value, nick,NICK_LEN);

	//GEN_SQLSTR(this->sqlstr, "update %s set shop_name = '%s' where %s = %u", 
			//this->get_table_name(userid),	mysql_value, 
			//this->str_uid,	userid);
	//return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
	return 0;
}
