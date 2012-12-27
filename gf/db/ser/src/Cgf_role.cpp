#include <algorithm>
#include "db_error.h"
#include "Cgf_role.h"

using namespace std;


const char* ROLE_XIAOMEE_COLUMN = "xiaomee";
const char* ROLE_EXPLOIT_COLUMN = "pvp_fight";
const char* ROLE_EXP_COLUMN = "exp";
const char* ROLE_SKILL_POINT_COLUMN = "skill_point";
const char* ROLE_OL_COUNT = "Ol_count";
const char* ROLE_FUMO_POINTS_COLUMN = "fumo_points";
const char* ROLE_HONOR_COLUMN = "pvp_honour";
const char* ROLE_ALLOCATOR_EXP_COLUMN = "alloter_exp";
const char* ROLE_GF_COUPONS_COLUMN = "gf_conpons";




extern inline void hex_printf(char * buf,int len);
//加入
Cgf_role::Cgf_role(mysql_interface * db ) 
	:CtableRoute( db,"GF" ,"t_gf_role","userid")
{ 
	role_max_num = config_get_intval("ROLE_MAX_NUM", 4);
	init_xiaomee = config_get_intval("INIT_XIAOMEE", 0);
//	init_hp		 = config_get_intval("INIT_HP",0);
//	init_mp		 = config_get_intval("INIT_MP",0);
//	init_skill_point	 = config_get_intval("INIT_SKILL_POINT",0);
//	init_level			 = config_get_intval("INIT_LEVEL",1);
//	init_exp			 = config_get_intval("INIT_EXP",0);
	
}

int Cgf_role::set_role_phy_del(userid_t userid)
{
	GEN_SQLSTR(sqlstr, "update %s set status = %u where \
					userid=%u and status<3 and del_tm <> 0 and del_tm < (unix_timestamp(now()) - 60*60*24*14)", 
					get_table_name(userid),
					ROLE_STATUS_DEL,
					userid);
	return exec_update_sql(sqlstr, SUCC);
}

int Cgf_role::get_role_list(userid_t userid, uint32_t* p_count, gf_get_role_list_out_item_1** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select role_regtime,role_type,del_tm,Ol_last,level,exp,nick from %s \
		where userid=%u and status=%u order by role_regtime;",
		this->get_table_name(userid),userid,ROLE_STATUS_NOR);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_time );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->del_tm );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->Ol_last );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->level );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->exp );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->nick,sizeof((*pp_list+i)->nick) );
		
	STD_QUERY_WHILE_END();
}

int Cgf_role::get_logic_del_role_list(userid_t userid, uint32_t* p_count, gf_get_logic_del_role_list** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select role_regtime,del_tm from %s \
		where userid=%u and status=%u",
		this->get_table_name(userid), userid, ROLE_STATUS_DEL);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_time );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->del_tm );
	STD_QUERY_WHILE_END();
}

int Cgf_role::get_all_role_list(userid_t userid, uint32_t* p_count, gf_get_role_list_ex_out_item_1** pp_list)
{
	GEN_SQLSTR(this->sqlstr,"select role_regtime,role_type,level,status,nick from %s \
		where userid=%u order by role_regtime;",
		this->get_table_name(userid), userid);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_time );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->role_type );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->level );
		INT_CPY_NEXT_FIELD( (*pp_list+i)->status );
		BIN_CPY_NEXT_FIELD( (*pp_list+i)->nick, sizeof((*pp_list+i)->nick) );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->vip );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->Ol_count );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->Ol_today );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->Ol_last );
//		INT_CPY_NEXT_FIELD( (*pp_list+i)->Ol_time );
		
	STD_QUERY_WHILE_END();
}


int Cgf_role::update_home_active_point(userid_t userid , uint32_t role_regtime, uint32_t point, uint32_t last_tm)
{
	GEN_SQLSTR(sqlstr, "update %s set home_active_point = %u, home_last_tm = %u where \
						userid=%u and role_regtime=%u", 
						get_table_name(userid),
						point,
						last_tm,
						userid,
						role_regtime);
	return exec_update_sql(sqlstr, SUCC);
}

/**
 *@fn 取得角色信息
 *@brief cmd route interface
 */
int Cgf_role::get_role_info(userid_t userid, uint32_t role_regtime, 
	stru_role_info* p_out, stru_msg_list* p_msg)
{
	GEN_SQLSTR(this->sqlstr,"select role_regtime,role_type,power_user,Ol_count,nick,nick_change_tm,skill_point,map_id, \
        xpos,ypos,level,exp,alloter_exp,hp,mp,xiaomee,pvp_honour,pvp_fight,pvp_win,pvp_fail,pvp_winning_streak, \
        fumo_points,fumo_points_today,fumo_tower_top,fumo_tower_used_tm,unique_item_bit,msglist,itembind, \
        double_exp_time, day_flag, max_times_chapter,show_state,strengthen_count, achieve_point, last_add_ap_time, \
		achieve_title, open_box_times, home_active_point, home_last_tm, vitality_point,team_id,god_guard \
        from %s where userid=%u and role_regtime=%u ;",
		this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->role_regtime);
		INT_CPY_NEXT_FIELD(p_out->role_type);
		INT_CPY_NEXT_FIELD(p_out->power_user);
		INT_CPY_NEXT_FIELD(p_out->Ol_count);
		BIN_CPY_NEXT_FIELD(p_out->nick, sizeof(p_out->nick));
		INT_CPY_NEXT_FIELD(p_out->nick_change_tm);
		INT_CPY_NEXT_FIELD(p_out->skill_point);
		INT_CPY_NEXT_FIELD(p_out->map_id);
		INT_CPY_NEXT_FIELD(p_out->xpos);
		INT_CPY_NEXT_FIELD(p_out->ypos);
		INT_CPY_NEXT_FIELD(p_out->level);
		INT_CPY_NEXT_FIELD(p_out->exp);
		INT_CPY_NEXT_FIELD(p_out->alloter_exp);
		INT_CPY_NEXT_FIELD(p_out->hp);
		INT_CPY_NEXT_FIELD(p_out->mp);

		INT_CPY_NEXT_FIELD(p_out->xiaomee);
		INT_CPY_NEXT_FIELD(p_out->honour);
		INT_CPY_NEXT_FIELD(p_out->fight);
		INT_CPY_NEXT_FIELD(p_out->win);
		INT_CPY_NEXT_FIELD(p_out->fail);
		INT_CPY_NEXT_FIELD(p_out->winning_streak);
		INT_CPY_NEXT_FIELD(p_out->fumo_points);
		INT_CPY_NEXT_FIELD(p_out->fumo_points_today);
		INT_CPY_NEXT_FIELD(p_out->fumo_tower_top);
		INT_CPY_NEXT_FIELD(p_out->fumo_tower_used_tm);
		BIN_CPY_NEXT_FIELD(p_out->uniqueitem, sizeof(p_out->uniqueitem));
		BIN_CPY_NEXT_FIELD (p_msg, sizeof (typeof(*p_msg)));
		BIN_CPY_NEXT_FIELD (p_out->itembind, sizeof (p_out->itembind));
		INT_CPY_NEXT_FIELD(p_out->double_exp_time);
		INT_CPY_NEXT_FIELD(p_out->day_flag);
		INT_CPY_NEXT_FIELD(p_out->max_times_chapter);
		INT_CPY_NEXT_FIELD(p_out->show_state);
		INT_CPY_NEXT_FIELD(p_out->strengthen_cnt);
		INT_CPY_NEXT_FIELD(p_out->achieve_point);
		INT_CPY_NEXT_FIELD(p_out->last_update_tm);
		INT_CPY_NEXT_FIELD(p_out->achieve_title);
		INT_CPY_NEXT_FIELD(p_out->open_box_times);
		INT_CPY_NEXT_FIELD(p_out->home_active_point);
		INT_CPY_NEXT_FIELD(p_out->home_last_tm);
		INT_CPY_NEXT_FIELD(p_out->vitality_point);
		INT_CPY_NEXT_FIELD(p_out->team_id);
        BIN_CPY_NEXT_FIELD (p_out->god_guard, sizeof(p_out->god_guard));
	STD_QUERY_ONE_END_WITHOUT_RETURN();

    DEBUG_LOG("LOGIN: nick=%s,skillp=%u,map_id=%u,lv=%u,exp=%u, double time=%u,fumo_tower_top=%u", 
        p_out->nick, p_out->skill_point, p_out->map_id, 
        p_out->level,p_out->exp, p_out->double_exp_time,
        p_out->fumo_tower_top);
	return SUCC;

}


int Cgf_role::get_role_partial_info(userid_t userid, uint32_t role_regtime, get_user_partial_userpart* p_info) 
{
	if (role_regtime == 0)
		GEN_SQLSTR(this->sqlstr,"select userid,role_type,role_regtime, power_user, level,nick \
				from %s where userid=%u order by Ol_last desc limit 1; ",
				this->get_table_name(userid), userid);
	else
		GEN_SQLSTR(this->sqlstr,"select userid,role_type,role_regtime, power_user, level,nick \
				from %s where userid=%u and role_regtime=%u; ",
				this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_info->userid);
		INT_CPY_NEXT_FIELD(p_info->role_type);
		INT_CPY_NEXT_FIELD(p_info->role_regtime);
		INT_CPY_NEXT_FIELD(p_info->power_user);
		INT_CPY_NEXT_FIELD(p_info->lv);
		BIN_CPY_NEXT_FIELD(p_info->nick, sizeof(p_info->nick));
	//	INT_CPY_NEXT_FIELD(p_info->skill_point);
		
	STD_QUERY_ONE_END();		
}


int Cgf_role::get_role_power_user_flag(userid_t userid, uint32_t role_regtime, uint32_t* power_user) 
{
	GEN_SQLSTR(this->sqlstr,"select power_user	from %s where userid=%u and role_regtime=%u; ",
			this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*power_user);
	STD_QUERY_ONE_END();		
}

int Cgf_role::get_role_info_for_special_set(userid_t userid, uint32_t role_regtime, uint32_t* p_lv, uint32_t* p_exp, uint32_t* p_upd_flg) 
{

	GEN_SQLSTR(this->sqlstr,"select level, exp, upd_flg from %s where userid=%u and role_regtime=%u; ",
			this->get_table_name(userid), userid, role_regtime);
	
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_lv);
		INT_CPY_NEXT_FIELD(*p_exp);
		INT_CPY_NEXT_FIELD(*p_upd_flg);
	STD_QUERY_ONE_END();		
}


int Cgf_role::get_role_detail_info(userid_t userid, uint32_t role_regtime, gf_get_role_detail_info_out* p_info) 
{
	if (role_regtime==0)
	{
		GEN_SQLSTR(this->sqlstr,"select level,exp,pvp_honour, \
            pvp_fight, pvp_win, pvp_fail, pvp_winning_streak,achieve_title \
			from %s where userid=%u order by Ol_last desc limit 1; ",
			this->get_table_name(userid), userid);
	}
	else
	{
		GEN_SQLSTR(this->sqlstr,"select level,exp,pvp_honour, \
            pvp_fight, pvp_win, pvp_fail, pvp_winning_streak,achieve_title \
			from %s where userid=%u and role_regtime=%u; ",
			this->get_table_name(userid), userid, role_regtime);
	}
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);

	p_info->userid = userid;
	INT_CPY_NEXT_FIELD(p_info->level);
	INT_CPY_NEXT_FIELD(p_info->exp);
	//INT_CPY_NEXT_FIELD(p_info->hp);
	//INT_CPY_NEXT_FIELD(p_info->mp);
	INT_CPY_NEXT_FIELD(p_info->honour);
	INT_CPY_NEXT_FIELD(p_info->fight);
	INT_CPY_NEXT_FIELD(p_info->win);
	INT_CPY_NEXT_FIELD(p_info->fail);
	INT_CPY_NEXT_FIELD(p_info->winning_streak);
	INT_CPY_NEXT_FIELD(p_info->achieve_title);

	STD_QUERY_ONE_END();

}

int Cgf_role::add_offline_msg(userid_t userid , uint32_t role_regtime,  stru_msg_item *item )
{
	int ret;
	stru_msg_list msglist={0};
	
	ret=this->get_msglist(userid, role_regtime, &msglist);
	DEBUG_LOG("-----------add_offline_msg: %d :  %d",msglist.len,msglist.count);
	if (ret!=SUCC)
	{	
		return ret;
	}
	
	if (msglist.len < msg_list_head_len)
	{
		msglist.len = msg_list_head_len;
	}

	if (msglist.len+item->msglen> MSG_LIST_BUF_SIZE)
		return  GF_OFFLINE_MSG_MAX_ERR ;


	memcpy (((char*)(&msglist))+msglist.len, (char*)item,item->msglen);
	msglist.len+=item->msglen;
	msglist.count++;
	return this->update_msglist(userid, role_regtime,&msglist);
}

int Cgf_role::get_msglist(userid_t userid , uint32_t role_regtime,  stru_msg_list * msglist )
{
	sprintf( this->sqlstr, "select msglist from %s where userid=%u and role_regtime=%u",
			this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (msglist, sizeof (stru_msg_list) );
	STD_QUERY_ONE_END();
}

int Cgf_role::update_msglist(userid_t userid , uint32_t role_regtime,  stru_msg_list * msglist )
{   
	char mysql_msglist[mysql_str_len(sizeof (stru_msg_list))];
	
	set_mysql_string(mysql_msglist,(char*)msglist, msglist->len );
	sprintf( this->sqlstr, " update %s set \
		msglist='%s'\
		where userid=%u and role_regtime=%u " ,
		this->get_table_name(userid),
		mysql_msglist,
		userid,role_regtime );
	return this->exec_update_sql(this->sqlstr,ROLE_ID_NOFIND_ERR );
}   


int Cgf_role::update_nick(userid_t userid , uint32_t role_regtime, char* nick)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
    uint32_t cur_time = time(NULL);
	set_mysql_string(nick_mysql, nick, NICK_LEN);
	GEN_SQLSTR(this->sqlstr,"update %s set nick='%s', nick_change_tm=%u \
			where userid=%u and role_regtime=%u;",
			this->get_table_name(userid), nick_mysql, cur_time, userid, role_regtime);
	return this->exec_update_sql( this->sqlstr, ROLE_ID_NOFIND_ERR );

}

int Cgf_role::set_client_buf(userid_t userid , uint32_t role_regtime, uint8_t* buf)
{
	char buf_mysql[mysql_str_len(max_client_buf_len)];
	set_mysql_string(buf_mysql, (char*)buf, max_client_buf_len);
	GEN_SQLSTR(this->sqlstr,"update %s set client_buf='%s' \
			where userid=%u and role_regtime=%u;",
			this->get_table_name(userid), buf_mysql, userid, role_regtime);
	return this->exec_update_sql( this->sqlstr, ROLE_ID_NOFIND_ERR );

}

int Cgf_role::reset_pvp_fight(userid_t userid , uint32_t role_regtime)
{
	uint32_t pvp_fight = 1000000;
	GEN_SQLSTR(this->sqlstr,"update %s set pvp_fight=%u \
			where userid=%u and role_regtime=%u and pvp_fight>%u;",
			this->get_table_name(userid), pvp_fight, userid, role_regtime, pvp_fight);
	return this->exec_update_sql( this->sqlstr, ROLE_ID_NOFIND_ERR );

}


int Cgf_role::get_client_buf(userid_t userid , uint32_t role_regtime, uint8_t* buf)
{
	sprintf( this->sqlstr, "select client_buf from %s where userid=%u and role_regtime=%u",
			this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (buf, max_client_buf_len );
	STD_QUERY_ONE_END();
}


/**
 * @brief get role nick /last login
 */
int Cgf_role::get_last_login_nick(userid_t userid, char* nick )
{
	sprintf( this->sqlstr, "select nick from %s where userid=%u order by Ol_last desc limit 1",
			this->get_table_name(userid), userid);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
			BIN_CPY_NEXT_FIELD (nick, NICK_LEN );
	STD_QUERY_ONE_END();
}

int Cgf_role::is_role_num_max(userid_t userid)
{
	int ret = 0;
	uint32_t role_num = 0;
	ret = get_role_num(userid, ROLE_STATUS_NOR_BLOCK, &role_num);
	if (ret != SUCC)
	{
		return ret;
	}
	if (role_num >= this->role_max_num)
	{
		return GF_ROLE_NUM_MAX_ERR;
	}
	return ret;
}

int Cgf_role::get_role_num(userid_t userid,uint32_t status,uint32_t* p_num)
{
	if (status==ROLE_STATUS_ALL)
	{
		GEN_SQLSTR(this->sqlstr,"select count(role_regtime) \
				from %s where userid=%u ;",
				this->get_table_name(userid), userid);
	}
	else if (status==ROLE_STATUS_NOR_BLOCK)
	{
		GEN_SQLSTR(this->sqlstr,"select count(role_regtime) \
				from %s where userid=%u and (status=%u or status=%u) ;",
				this->get_table_name(userid), userid,ROLE_STATUS_NOR,ROLE_STATUS_BLOCK);		
	}
	else
	{
		GEN_SQLSTR(this->sqlstr,"select count(role_regtime) \
				from %s where userid=%u status=%u ;",
				this->get_table_name(userid), userid,status);	

	}
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( (*p_num) );
	STD_QUERY_ONE_END();	

}

int Cgf_role::get_all_active_role(userid_t userid, uint32_t * p_num)
{
	GEN_SQLSTR(this->sqlstr, "select count(role_regtime) from %s where userid=%u and del_tm=0;",
		this->get_table_name(userid), userid);

	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( (*p_num) );
	STD_QUERY_ONE_END();	
}


int Cgf_role::add_role(userid_t userid,uint32_t role_regtime,gf_add_role_in* p_in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql, p_in->nick, NICK_LEN);
	
	GEN_SQLSTR(this->sqlstr,"insert into %s (userid,role_regtime,role_type,Ol_last,nick\
		,level,hp,mp,xiaomee,status) \
		values (%u,%u,%u,%u,'%s',\
		%u,%u,%u,%u,%u)",
		this->get_table_name(userid), userid, role_regtime, p_in->role_type, role_regtime, nick_mysql
			, p_in->level, p_in->hp, p_in->mp, init_xiaomee, ROLE_STATUS_NOR);
	return this->exec_insert_sql(this->sqlstr, ROLE_ID_EXISTED_ERR);	
}

int Cgf_role::del_role(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"delete from %s where userid = %u and role_regtime=%u and status=%u;",
		this->get_table_name(userid), userid, role_regtime, ROLE_STATUS_DEL);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_role::set_role_delflg(userid_t userid,uint32_t role_regtime, uint32_t status )
{
	GEN_SQLSTR(this->sqlstr,"update %s set status=%u where userid = %u and role_regtime=%u ",
		this->get_table_name(userid), status, userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_role::gray_del_role(userid_t userid,uint32_t role_regtime)
{
	uint32_t db_lv=0;
	uint32_t ret = this->get_int_value(userid, role_regtime, "level", &db_lv);
	if (ret)
	{
		return ret;
	}

	uint32_t all_role_count = 0;
	ret = this->get_all_active_role(userid, &all_role_count);
	if (ret) 
	{
		return ret;
	}

	if (all_role_count < 2) {
		return ROLE_ID_NO_VALID_ROLE_2_DELETE;  
	}

	//if (db_lv < 10) 
	{
	//	return logic_del_role(userid, role_regtime);
	}
	//else 
	{
		GEN_SQLSTR(this->sqlstr,"update %s set del_tm=UNIX_TIMESTAMP() where userid = %u and role_regtime=%u and status<>2",
			this->get_table_name(userid), userid, role_regtime);
		return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
	}
	return 0;
}

int Cgf_role::logic_del_role(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"update %s set del_tm=UNIX_TIMESTAMP(), status=%u where userid = %u and role_regtime=%u ",
		this->get_table_name(userid), ROLE_STATUS_DEL, userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_role::resume_gray_role(userid_t userid,uint32_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"update %s set del_tm=0 where userid = %u and role_regtime=%u and  status <> 2",
		this->get_table_name(userid), userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);	
}

int Cgf_role::get_role_status(userid_t userid,uint32_t role_regtime,uint32_t* p_status)
{
	GEN_SQLSTR(this->sqlstr,"select status \
			from %s where userid=%u and role_regtime=%u ",
			this->get_table_name(userid), userid,role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( (*p_status) );
	STD_QUERY_ONE_END();		
}

int Cgf_role::get_skill_point(userid_t userid,uint32_t role_regtime,uint32_t* p_num)
{
	GEN_SQLSTR(this->sqlstr,"select skill_point \
			from %s where userid=%u and role_regtime=%u ",
			this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD( (*p_num) );
	STD_QUERY_ONE_END();		
}
int Cgf_role::reduce_skill_point(userid_t userid, uint32_t role_regtime, uint32_t num, uint32_t* p_left)
{
	uint32_t db_point = 0;	
	int ret = this->get_skill_point(userid, role_regtime, &db_point);
	if (ret!=SUCC)
	{
		return ret;
	}
	if (num > db_point)
	{
		return GF_SKILL_POINT_NOENOUGH_ERR;
	}

	*p_left = db_point - num;
	if (num == 0)
		return SUCC;
	ret = this->set_int_value(userid, role_regtime, "skill_point", *p_left);
	if (ret!=SUCC)
	{
		return ret;
	}
	return ret;
}


int Cgf_role::is_role_status_nor(userid_t userid,uint32_t role_regtime)
{
	uint32_t status=0;
	int db_ret = get_role_status(userid, role_regtime, &status);
	if (db_ret != SUCC)
		return db_ret;
	if (status != ROLE_STATUS_NOR)
	{
		return ROLE_ID_NOFIND_ERR;
	}
	return db_ret;
}

int Cgf_role::get_int_value(userid_t userid , uint32_t role_regtime, 
		const char * field_type ,  uint32_t * p_value)
{
	sprintf( this->sqlstr, "select  %s from %s where %s=%u and role_regtime = %u", 
		field_type , this->get_table_name(userid), this->id_name, userid,role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_value );
	STD_QUERY_ONE_END();
}

int Cgf_role::get_xiaomee_exp(userid_t userid , uint32_t role_regtime,
		uint32_t* p_xiaomee, uint32_t* p_exp)
{
	sprintf( this->sqlstr, "select xiaomee,exp from %s where %s=%u and role_regtime = %u", 
		this->get_table_name(userid), this->id_name, userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_xiaomee );
		INT_CPY_NEXT_FIELD(*p_exp );
	STD_QUERY_ONE_END();
}

int Cgf_role::get_xiaomee_exp_skpt(userid_t userid , uint32_t role_regtime,
		uint32_t* p_xiaomee, uint32_t* p_exp, uint32_t *p_allocator_exp,  uint32_t* p_skill_pt, uint32_t* fumo_pt, uint32_t* honor)
{
	sprintf( this->sqlstr, "select xiaomee,exp, alloter_exp, skill_point,fumo_points,pvp_honour \
        from %s where %s=%u and role_regtime = %u", 
		this->get_table_name(userid), this->id_name, userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_xiaomee );
		INT_CPY_NEXT_FIELD(*p_exp );
		INT_CPY_NEXT_FIELD(*p_allocator_exp);
		INT_CPY_NEXT_FIELD(*p_skill_pt );
		INT_CPY_NEXT_FIELD(*fumo_pt );
		INT_CPY_NEXT_FIELD(*honor );
	STD_QUERY_ONE_END();
}

int Cgf_role::set_role_base_info(userid_t userid, uint32_t role_regtime
	, gf_set_role_base_info_in_header* p_in)
{
	GEN_SQLSTR(this->sqlstr,"update %s set \
		skill_point=(case when level < %u \
		then skill_point+%u-level else skill_point end), \
		level=(case when level > %u then level else %u end), exp=%u, alloter_exp=%u,double_exp_time=%u, xiaomee=xiaomee+%u,\
        fumo_points=fumo_points+%u, \
        fumo_points_today=fumo_points_today+%u , fumo_tower_top=%u, fumo_tower_used_tm=%u \
        where userid=%u and role_regtime=%u; ",
		this->get_table_name(userid), p_in->level, p_in->level, p_in->level, p_in->level, 
		p_in->exp, p_in->alloter_exp,
        p_in->dexp_tm, p_in->coins, 
        p_in->fumo_points, p_in->fumo_points, p_in->fumo_tower_top, p_in->fumo_tower_used_tm, 
        userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);		
}

int Cgf_role::update_unique_itembit(userid_t userid, uint32_t role_regtime, uint8_t* bit)
{
    char unique_item[mysql_str_len(UNIQUE_ITEM_LEN)];
    set_mysql_string(unique_item, (char*)(bit), UNIQUE_ITEM_LEN);

    GEN_SQLSTR(this->sqlstr, "update %s set unique_item_bit='%s' \
        where userid=%u and role_regtime=%u;",
        this->get_table_name(userid), unique_item, userid, role_regtime);
    return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::set_role_base_info_2(userid_t userid, uint32_t role_regtime
		, gf_set_role_base_info_2_in_header* p_in)
{
	GEN_SQLSTR(this->sqlstr,"update %s set skill_point=(case when level < %u then skill_point+%u-level else skill_point end), \
			level=%u, exp=%u, xiaomee=%u where userid=%u and role_regtime=%u;", 
            this->get_table_name(userid), p_in->level, p_in->level, p_in->level, p_in->exp, p_in->coins, userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::set_role_pvp_info(userid_t userid, uint32_t role_regtime
		, gf_set_role_pvp_info_in* p_in)
{
	GEN_SQLSTR(this->sqlstr,"update %s set pvp_honour=%u, pvp_fight=pvp_fight+%u, pvp_win=pvp_win+%u, pvp_fail=pvp_fail+%u, pvp_winning_streak=%u where userid=%u and role_regtime=%u; ",
			this->get_table_name(userid), p_in->honour, p_in->fight, p_in->win,
            p_in->fail, p_in->winning_streak ,userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::set_role_god_guard_info(userid_t userid, uint32_t role_regtime, gf_group_t *group)
{
    char group_mysql[mysql_str_len(80)];
    memset(group_mysql, 0x00, mysql_str_len(80));
    set_mysql_string(group_mysql, (char *)(group),  sizeof(gf_group_t));

	GEN_SQLSTR(this->sqlstr,"update %s set god_guard='%s' where userid=%u and role_regtime=%u; ",
			this->get_table_name(userid), group_mysql, userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::increase_int_value(userid_t userid,uint32_t role_regtime,const char* column_type, uint32_t value)
{
	GEN_SQLSTR(this->sqlstr,"update %s set %s=%s+%u \
		where userid=%u and role_regtime=%u; ",
		this->get_table_name(userid), column_type, column_type, value, userid, role_regtime);
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);		
}

int Cgf_role::logout(userid_t userid,uint32_t role_regtime, gf_logout_in* p_in)
{
    
    DEBUG_LOG("role LOGOUT: mapid=%u xpos=%u ypos=%u",p_in->mapid,p_in->xpos,p_in->ypos);
    if ( p_in->mapid == 0) {
        GEN_SQLSTR(this->sqlstr,"update %s set Ol_count=Ol_count+1,Ol_last=%u \
            where userid=%u and role_regtime=%u;",
            this->get_table_name(userid), (uint32_t )time(NULL), userid, role_regtime);
    } else {
        GEN_SQLSTR(this->sqlstr,"update %s set map_id=%u, xpos=%u, ypos=%u, Ol_count=Ol_count+1,\
            Ol_last=%u where userid=%u and role_regtime=%u; ",
            this->get_table_name(userid), p_in->mapid, p_in->xpos, p_in->ypos,
            (uint32_t )time(NULL), userid, role_regtime);
    }
	return this->exec_update_sql (this->sqlstr, ROLE_ID_NOFIND_ERR);		
}

int Cgf_role::increase_int_value_with_ret(userid_t userid,uint32_t role_regtime,const char* column_type, uint32_t value, uint32_t* p_left)
{
	uint32_t db_num=0;
	uint32_t ret = this->get_int_value(userid, role_regtime, column_type, &db_num);
	if (ret != SUCC	)
		return ret;

	ret=this->set_int_value(userid, role_regtime, column_type,(db_num + value));
	if (ret!=SUCC)
	{
		return ret;
	}
	
	if (p_left)
		*p_left = db_num + value;
	return ret;	
}

int Cgf_role::set_int_value(userid_t userid, uint32_t role_regtime, const char * field_type , uint32_t  value)
{
	sprintf( this->sqlstr, "update %s set %s =%u where %s=%u and role_regtime=%u" ,
		this->get_table_name(userid), field_type, value ,this->id_name, userid, role_regtime );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cgf_role::set_int_add_value(userid_t userid, uint32_t role_regtime, uint32_t value,const char* field_type)
{
	sprintf( this->sqlstr, "update %s set %s=%s+%u where %s=%u and role_regtime=%u" ,
		this->get_table_name(userid), field_type, field_type, value ,this->id_name, userid, role_regtime );
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);	
}

int Cgf_role::reduce_int_value(userid_t userid, uint32_t role_regtime,uint32_t err_id,const char* column_type, uint32_t value, uint32_t* p_left)
{
	uint32_t db_num=0;
	uint32_t ret = this->get_int_value(userid, role_regtime, column_type, &db_num);
	if (ret != SUCC	)
	{
		return ret;
	}
    DEBUG_LOG("test reduce value:%u db_num:%u", value, db_num);
	if (db_num < value)
	{
		return err_id;
	}
	else
	{
		if (value)
		{
			ret=this->set_int_value(userid, role_regtime, column_type, (db_num-value));
			if (ret!=SUCC)
			{
				return ret;
			}
		}
	}
	if (p_left)
		*p_left = db_num-value;
	return ret;
}

int Cgf_role::update_itembind(userid_t userid, uint32_t role_regtime, gf_set_role_itembind_in* p_in)
{
	char bind_mysql[mysql_str_len(ITEM_BIND_LEN)];

	set_mysql_string(bind_mysql, 
			(char*)(p_in->buf), ITEM_BIND_LEN);
	
	GEN_SQLSTR( this->sqlstr, "update %s set itembind='%s' \
		where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), bind_mysql, userid, role_regtime);
	return this->exec_update_sql(this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::update_skillbind(userid_t userid, uint32_t role_regtime, const char* buf, uint32_t len)
{
	char bind_mysql[mysql_str_len(SKILL_BIND_LEN)];

	set_mysql_string(bind_mysql, 
			buf, len > SKILL_BIND_LEN? SKILL_BIND_LEN : len);
	//hex_printf(buf, len);
	GEN_SQLSTR( this->sqlstr, "update %s set skill_bind='%s' \
		where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), bind_mysql, userid, role_regtime);
	return this->exec_update_sql(this->sqlstr, ROLE_ID_NOFIND_ERR);
}

int Cgf_role::get_itembind(userid_t userid, uint32_t role_regtime, gf_set_role_itembind_out* p_in)
{
	GEN_SQLSTR(this->sqlstr," select itembind from %s \
		where userid=%u and role_regtime=%u ; ",
		this->get_table_name(userid), userid, role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_in->buf, sizeof(p_in->buf) );
	STD_QUERY_ONE_END();
}

int Cgf_role::get_skillbind(userid_t userid, uint32_t role_regtime, char* buf)
{
	GEN_SQLSTR(this->sqlstr," select skill_bind from %s \
		where userid=%u and role_regtime=%u ; ",
		this->get_table_name(userid),userid,role_regtime);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(buf, SKILL_BIND_LEN);
		//hex_printf(buf, SKILL_BIND_LEN);
	STD_QUERY_ONE_END();
}

int Cgf_role::get_coin_exp(userid_t userid, uint32_t role_regtime, uint32_t* p_coins, uint32_t* p_exp)
{
	GEN_SQLSTR(this->sqlstr," select xiaomee,exp from %s \
		where userid=%u and role_regtime=%u ; ",
		this->get_table_name(userid),userid,role_regtime);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_coins);
		INT_CPY_NEXT_FIELD(*p_exp);
	STD_QUERY_ONE_END();
}

int Cgf_role::get_coins(userid_t userid, uint32_t role_regtime, uint32_t* p_coins)
{
	GEN_SQLSTR(this->sqlstr," select xiaomee from %s \
		where userid=%u and role_regtime=%u ; ",
		this->get_table_name(userid), userid, role_regtime);

	STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_coins);
	STD_QUERY_ONE_END();
}

int Cgf_role::set_coins(userid_t userid, uint32_t role_regtime, uint32_t xiaomee)
{
	GEN_SQLSTR(this->sqlstr, "update %s set xiaomee=%u where userid=%u and role_regtime=%u;",
			this->get_table_name(userid), xiaomee, userid, role_regtime);
	return this->exec_update_sql(sqlstr, SUCC);
}

int Cgf_role::get_role_pvp_info(userid_t userid, uint32_t role_regtime, pvp_header *p_pvp)
{
	GEN_SQLSTR(this->sqlstr," select pvp_honour, pvp_fight, pvp_win, pvp_fail, pvp_winning_streak \
        from %s where userid=%u and role_regtime=%u ; ",
		this->get_table_name(userid), userid, role_regtime);

	STD_QUERY_ONE_BEGIN(this->sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_pvp->honour);
		INT_CPY_NEXT_FIELD(p_pvp->fight);
		INT_CPY_NEXT_FIELD(p_pvp->win);
		INT_CPY_NEXT_FIELD(p_pvp->fail);
		INT_CPY_NEXT_FIELD(p_pvp->winning_streak);
	STD_QUERY_ONE_END();
}

int Cgf_role::get_role_Ol_last(userid_t userid, uint32_t role_regtime, uint32_t* Ol_last)
{
    GEN_SQLSTR(this->sqlstr," select Ol_last from %s where userid=%u and role_regtime=%u ;",
        this->get_table_name(userid), userid, role_regtime);

    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*Ol_last);
    STD_QUERY_ONE_END();
}

int Cgf_role::set_role_Ol_last(userid_t userid, uint32_t role_regtime, uint32_t Ol_last)
{
	uint32_t set_time = 0;
	if (Ol_last) {
		set_time = Ol_last;
	} else {
		set_time = (uint32_t )time(NULL);
	}
	
	GEN_SQLSTR(this->sqlstr," update %s set Ol_last=%u where userid=%u and role_regtime=%u ;",
	        this->get_table_name(userid), set_time, userid, role_regtime);
	
    return this->exec_update_list_sql (this->sqlstr, SUCC); 
}

/* 
int Cgf_role::get_itembind(userid_t userid,uint32_t role_regtime,gf_get_role_itembind_out* p_out)
{
	GEN_SQLSTR( this->sqlstr, "select itembind from %s \
		where userid=%u and role_regtime=%u;",
		this->get_table_name(userid),userid,role_regtime);
	STD_QUERY_ONE_BEGIN(this-> sqlstr,ROLE_ID_NOFIND_ERR);
		BIN_CPY_NEXT_FIELD(p_out->buf ,ITEM_BIND_LEN);
	STD_QUERY_ONE_END();
}
*/
int Cgf_role::del_daily_loop(userid_t userid, userid_t role_regtime)
{
	GEN_SQLSTR(this->sqlstr,"update %s set fumo_points_today=0, day_flag=0, double_exp_time=0, \
        strengthen_count=0 where userid=%u and role_regtime=%u;",
		this->get_table_name(userid),userid,role_regtime);
	return this->exec_update_list_sql (this->sqlstr, SUCC); 
}

int Cgf_role::get_role_double_exp_data(userid_t userid, userid_t role_regtime, uint32_t* flag,
    uint32_t *dexp_tm)
{
    GEN_SQLSTR(this->sqlstr,"select double_exp_time, day_flag from %s \
        where userid=%u and role_regtime=%u;",
		this->get_table_name(userid),userid,role_regtime);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*dexp_tm);
        INT_CPY_NEXT_FIELD(*flag);
    STD_QUERY_ONE_END();
}

int Cgf_role::set_role_double_exp_data(userid_t userid, userid_t role_regtime, uint32_t flag,
    uint32_t dexp_tm)
{
    GEN_SQLSTR(this->sqlstr,"update %s set day_flag=day_flag+%u, double_exp_time=double_exp_time+%u \
        where userid=%u and role_regtime=%u;",
		this->get_table_name(userid), flag, dexp_tm, userid,role_regtime);
	return this->exec_update_list_sql (this->sqlstr, SUCC); 
}

int Cgf_role::get_user_max_level(userid_t userid, uint32_t* lv)
{
    GEN_SQLSTR(this->sqlstr,"select max(level) from %s \
        where userid=%u",
		this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(*lv);
    STD_QUERY_ONE_END();
}

int Cgf_role::set_role_show_state(userid_t userid, userid_t role_regtime, uint32_t state)
{
	GEN_SQLSTR(sqlstr, "update %s set show_state = %u where userid = %u and  role_regtime = %u",
			  get_table_name(userid), state, userid, role_regtime);
	return exec_update_list_sql(sqlstr, SUCC);
}

int Cgf_role::set_role_open_box_times(userid_t userid, userid_t role_regtime, uint32_t total_times)
{
	GEN_SQLSTR(sqlstr, "update %s set open_box_times = %u where userid = %u and  role_regtime = %u",
			get_table_name(userid), total_times, userid, role_regtime);
	return exec_update_list_sql(sqlstr, SUCC);
}


int Cgf_role::get_role_base_info_for_boss(userid_t userid, userid_t* role_regtime, gf_get_role_base_info_for_boss_out_header* p_out)
{
    GEN_SQLSTR(this->sqlstr,"select role_regtime,role_type,level,pvp_honour,nick from %s \
        where userid=%u order by Ol_last desc limit 1",
		this->get_table_name(userid),userid);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*role_regtime);
        INT_CPY_NEXT_FIELD(p_out->role_type);
		INT_CPY_NEXT_FIELD(p_out->level);
		INT_CPY_NEXT_FIELD(p_out->honor);
		BIN_CPY_NEXT_FIELD (p_out->nick, NICK_LEN);
    STD_QUERY_ONE_END();

}

int Cgf_role::get_community_info(userid_t userid, userid_t role_regtime, gf_get_player_community_info_out* p_out)
{
    GEN_SQLSTR(this->sqlstr,"select team_id,achieve_point,fumo_points from %s \
        where userid=%u and role_regtime=%u",
		this->get_table_name(userid),userid, role_regtime);
    STD_QUERY_ONE_BEGIN(this-> sqlstr, ROLE_ID_NOFIND_ERR);
        INT_CPY_NEXT_FIELD(p_out->team_id);
		INT_CPY_NEXT_FIELD(p_out->achieve_cnt);
		INT_CPY_NEXT_FIELD(p_out->fumo_points);
    STD_QUERY_ONE_END();

}

int Cgf_role::set_role_vitality_point(userid_t userid,uint32_t role_regtime, uint32_t point)
{
	GEN_SQLSTR(sqlstr, "update %s set vitality_point = %u where userid = %u and role_regtime = %u",
			get_table_name(userid),
			point,
			userid,
			role_regtime
	);
	return exec_update_list_sql(sqlstr, SUCC);
}

int Cgf_role::get_role_offline_info(userid_t userid, userid_t role_regtime, gf_get_offline_info_out * out)
{
	GEN_SQLSTR(sqlstr, "select user_off_tm, summon_off_tm, off_coins, off_fumo from %s \
			where userid=%u and role_regtime=%u",
			get_table_name(userid), userid, role_regtime);
    STD_QUERY_ONE_BEGIN(this->sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(out->user_off_tm);
		INT_CPY_NEXT_FIELD(out->summon_off_tm);
		INT_CPY_NEXT_FIELD(out->off_coins);
		INT_CPY_NEXT_FIELD(out->off_fumo);
	STD_QUERY_ONE_END();
		
}

int Cgf_role::set_role_offline_info(userid_t userid, userid_t role_regtime, gf_set_offline_info_in * in) 
{
	GEN_SQLSTR(sqlstr, "update %s set user_off_tm=%u, summon_off_tm=%u, off_coins=%u, off_fumo=%u, exp=exp+%u, \
			level=%u  where userid=%u and role_regtime=%u", 
			get_table_name(userid),
			in->user_off_tm,
			in->summon_off_tm,
			in->off_coins,
			in->off_fumo,
			in->player_add_exp,
			in->player_lv,
			userid,
			role_regtime);
	return exec_update_list_sql(sqlstr, ROLE_ID_NOFIND_ERR);
}

/**
 *@fn 
 *@brief cmd route interface
 */
int Cgf_role::get_role_need_physics_del(uint32_t userid, uint32_t* p_userid, uint32_t* p_role_tm)
{
	GEN_SQLSTR(this->sqlstr,"select userid, role_regtime from %s where \
		userid=%u and status = %u and del_tm > 0 and del_tm < (unix_timestamp(now()) - 60*60*24*30) limit 1",
		this->get_table_name(userid), userid, ROLE_STATUS_DEL);
	
	STD_QUERY_ONE_BEGIN(this->sqlstr, ROLE_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(*p_userid);
		INT_CPY_NEXT_FIELD(*p_role_tm);
	STD_QUERY_ONE_END();

}

int Cgf_role::get_simple_role_info(userid_t userid, uint32_t * cnt, gf_list_simple_role_info_out_item ** out)
{
	GEN_SQLSTR(this->sqlstr, "select userid, role_regtime, role_type, level, nick from %s where \
			userid=%u and del_tm=0 order by level desc", this->get_table_name(userid), userid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, out, cnt);
		INT_CPY_NEXT_FIELD( (*out+i)->userid );
		INT_CPY_NEXT_FIELD( (*out+i)->role_regtime );
		INT_CPY_NEXT_FIELD( (*out+i)->role_type );
		INT_CPY_NEXT_FIELD( (*out+i)->level );
		BIN_CPY_NEXT_FIELD( (*out+i)->nick_name, sizeof((*out+i)->nick_name) );
	STD_QUERY_WHILE_END();
}

