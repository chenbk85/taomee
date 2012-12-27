/*
 * =====================================================================================
 *
 *       Filename:  Cuser_pet_pet.cpp
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

#include "Cuser_pet.h"
//user_pet
Cuser_pet::Cuser_pet(mysql_interface * db ) 
	:CtableRoute100x10( db,"USER","t_user_pet","petid")
{ 

}
int Cuser_pet::remove(userid_t userid, uint32_t petid)
{
	sprintf( this->sqlstr, " delete from  %s \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		userid, petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::get_count(userid_t userid, uint32_t *p_count)
{

	sprintf( this->sqlstr, "select count(*) from %s \
			where userid=%u", 
			this->get_table_name(userid),userid); 
	STD_QUERY_ONE_BEGIN(this-> sqlstr,DB_ERR);
			INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cuser_pet::get(userid_t userid, uint32_t petid, uint32_t *p_count, 
		user_get_user_pet_out_item **pp_out_item )
{
	char petid_str [100];
	if (petid==0){//查询所有pet
		strcpy(petid_str,"");
	}else{
		sprintf(petid_str," and  petid =%u ", petid );
	 }

	sprintf( this->sqlstr, "select petid,"
		"flag, "
		"flag2, "
		"birthday, "
		"logic_birthday, "
		"nick, "
		"color, "
		"sicktime, "
		"pos_x, "
		"pos_y, "
		"hungry, "
		"thirsty, "
		"sanitary, "
		"spirit, "
		"endtime, "
		"starttime, "
		"taskid, "
		"skill, "
		"stamp, "
		"sick_type, "
		"fire_skill_bitflag, "
		"water_skill_bitflag, "
		"wood_skill_bitflag, "
		"skill_flag, "
		"change_value, "
		"hot_skill_one, "
		"hot_skill_two, "
		"hot_skill_three, "
		"pet_flag "
		"from %s "
	"where userid=%u  %s ", 
	this->get_table_name(userid),
	userid,
	petid_str); 

	STD_QUERY_WHILE_BEGIN( this->sqlstr,pp_out_item, p_count ) ;
			(*pp_out_item+i)->petid=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->flag =atoi_safe(NEXT_FIELD); 
			(*pp_out_item+i)->flag2 =atoi_safe(NEXT_FIELD); 
			(*pp_out_item+i)->birthday =atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->logic_birthday =atoi_safe(NEXT_FIELD ); 
			strncpy((*pp_out_item+i)->nick ,NEXT_FIELD,NICK_LEN ); 
			(*pp_out_item+i)->color=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->sicktime=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->pos_x =atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->pos_y =atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->hungry=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->thirsty=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->sanitary=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->spirit=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->endtime=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->starttime=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->taskid=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->skill=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->stamp=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->sick_type=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->fire_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->water_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->wood_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->skill_flag=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->change_value=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->hot_skill_one=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->hot_skill_two=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->hot_skill_three=atoi_safe(NEXT_FIELD ); 
			(*pp_out_item+i)->pet_flag = atoi_safe(NEXT_FIELD ); 
	STD_QUERY_WHILE_END();
}

int Cuser_pet::add(userid_t userid,  user_add_user_pet_in *p_in, uint32_t  *p_petid)
{
	uint32_t pet_count ,ret ;
	ret=this->get_count(userid,&pet_count);
	if (ret!=SUCC) return ret;
	if (pet_count>=4) return USER_PET_MAX_ERR; 
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_in->nick,NICK_LEN);

	sprintf(this->sqlstr, "insert into %s(userid,\
			flag,\
			flag2,\
			birthday,\
			logic_birthday,\
			nick,\
			color,\
			sicktime,\
			pos_x,\
			pos_y,\
			hungry,\
			thirsty,\
			sanitary,\
			spirit,\
			endtime,\
			starttime,\
			taskid,\
			skill,\
			stamp,\
			sick_type\
	) values(%u,%u,%u,%u,%u, '%s',%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)", 
			this->get_table_name(userid), 
			userid,
			p_in->flag,
			p_in->flag2,
			p_in->birthday,
			p_in->logic_birthday,
			nick_mysql, //%s
			p_in->color,
			p_in->sicktime,
			p_in-> pos_x,
			p_in-> pos_y,
			p_in->hungry,
			p_in->thirsty,
			p_in->sanitary,
			p_in->spirit,
			p_in->endtime,
			p_in->starttime,
			p_in->taskid,
			p_in->skill,
			p_in->stamp,
			p_in->sick_type
	   	);
		STD_INSERT_GET_ID(this->sqlstr, USER_ID_EXISTED_ERR, *p_petid );	
		return SUCC;
}

int Cuser_pet::set_pos(userid_t userid,  pet_pos_item *p_in)
{
	sprintf( this->sqlstr, " update %s set \
		pos_x=%u ,\
		pos_y=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in-> pos_x, p_in->pos_y,
		userid, p_in->petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::set_skill(userid_t userid,user_set_user_pet_skill_in   *p_in)
{
	sprintf( this->sqlstr, " update %s set \
		skill=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->skill,
		userid, p_in->petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}


int Cuser_pet::set_skill_flag(userid_t userid, user_pet_set_skill_flag_in   *p_in)
{
	sprintf( this->sqlstr, " update %s set \
		skill_flag=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->skill_flag,
		userid, p_in->petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::set_change_value(userid_t userid, user_pet_set_change_value_in   *p_in)
{
	sprintf( this->sqlstr, " update %s set \
		change_value=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->change_value,
		userid, p_in->petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::set_hot_skill(userid_t userid, user_pet_set_hot_skill_in   *p_in)
{
	sprintf( this->sqlstr, " update %s set \
		hot_skill_one=%u, \
		hot_skill_two=%u, \
		hot_skill_three=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->hot_skill_one,
		p_in->hot_skill_two,
		p_in->hot_skill_three,
		userid, p_in->petid );
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}


int Cuser_pet::set_info(userid_t userid,  
	   	user_set_user_pet_info_in *p_in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_in->nick,NICK_LEN);

	sprintf( this->sqlstr, " update %s set \
		flag=%u ,\
		flag2=%u ,\
		birthday=%u ,\
		nick='%s', \
		color=%u ,\
		sicktime=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->flag,p_in->flag2,p_in->birthday, nick_mysql,p_in->color,
		p_in->sicktime,
		userid, p_in->petid 
		);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::set_all(userid_t userid,  
	   	user_set_user_pet_all_in *p_in)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,p_in->nick,NICK_LEN);

	sprintf( this->sqlstr, " update %s set \
		flag=%u ,\
		flag2=%u ,\
		birthday=%u ,\
		logic_birthday=%u ,\
		nick='%s', \
		color=%u ,\
		sicktime=%u ,\
		hungry=%u ,\
		thirsty=%u ,\
		sanitary=%u, \
		spirit=%u, \
		stamp=%u ,\
		pos_x=%u ,\
		pos_y=%u ,\
		starttime=%u ,\
		taskid=%u ,\
		skill=%u ,\
		sick_type=%u ,\
		endtime=%u, \
		fire_skill_bitflag=%u, \
		water_skill_bitflag=%u, \
		wood_skill_bitflag=%u, \
		skill_flag=%u, \
		change_value=%u, \
		hot_skill_one=%u, \
		hot_skill_two=%u, \
		hot_skill_three=%u \
		where userid=%u and petid=%u " ,

		this->get_table_name(userid), 
		p_in->flag,
		p_in->flag2,
		p_in->birthday, 
		p_in->logic_birthday, 
		nick_mysql,
		p_in->color,
		p_in->sicktime,
		p_in->hungry,
		p_in->thirsty,
		p_in->sanitary,
		p_in->spirit,
		p_in->stamp,
		p_in-> pos_x, 
		p_in->pos_y,
		p_in->starttime,
		p_in->taskid,
		p_in->skill,
		p_in->sick_type,
		p_in->endtime,
		p_in->fire_skill_bitflag,
		p_in->water_skill_bitflag,
		p_in->wood_skill_bitflag,
		p_in->skill_flag,
		p_in->change_value,
		p_in->hot_skill_one,
		p_in->hot_skill_two,
		p_in->hot_skill_three,
		userid, 
		p_in->petid 
		);
	STD_SET_RETURN_EX(this->sqlstr,SUCC);	
}

int Cuser_pet::set_comm(userid_t userid,  
	   	user_set_user_pet_comm_in *p_in )
{
	p_in->hungry=get_valid_value(p_in->hungry ,0, ATTR_VALUE_MAX);
	p_in->thirsty=get_valid_value(p_in->thirsty, 0, ATTR_VALUE_MAX);
	p_in->sanitary=get_valid_value(p_in->sanitary, 0, ATTR_VALUE_MAX);
	p_in->spirit=get_valid_value(p_in->spirit, 0, ATTR_VALUE_MAX);
	
	sprintf( this->sqlstr, " update %s set \
		flag=%u,\
		flag2=%u,\
		sicktime=%u ,\
		hungry=%u ,\
		thirsty=%u ,\
		sanitary=%u, \
		spirit=%u, \
		endtime=%u,\
		starttime=%u,\
		taskid=%u,\
		stamp=%u \
		where userid=%u and petid=%u " ,
		this->get_table_name(userid), 
		p_in->flag,
		p_in->flag2,
		p_in->sicktime,
		p_in->hungry,
		p_in->thirsty,
		p_in->sanitary,
		p_in->spirit,
		p_in->endtime,
		p_in->starttime,
		p_in->taskid,
		p_in->stamp,
		userid, p_in->petid 
		);
	STD_SET_RETURN_EX(this->sqlstr, SUCC );	
}

int Cuser_pet::set_no_vip(userid_t userid)
{
	sprintf( this->sqlstr, " update %s set \
		flag2=flag2&0xFFFFFFFE, skill_flag = skill_flag&0xFFFFFFF8\
		where userid=%u and (flag2 & 0x01)=0x01 " ,
		this->get_table_name(userid), userid);
	STD_SET_LIST_RETURN(this->sqlstr);	
}

/**
 * @brief 得到一个宠物的全部信息
 * @param userid 用户ID号
 * @param petid  宠物的ID号
 * @param p_out_item 保存返回信息
 */
int Cuser_pet :: get_one_pet(userid_t userid, uint32_t petid, stru_user_pet *p_out_item)
{
	sprintf( this->sqlstr, "select petid, "
		"flag ,"
		"flag2 ,"
		"birthday ,"
		"logic_birthday ,"
		"nick ,"
		"color ,"
		"sicktime ,"
		"pos_x ,"
		"pos_y ,"
		"hungry ,"
		"thirsty ,"
		"sanitary ,"
		"spirit ,"
		"endtime ,"
		"starttime ,"
		"taskid ,"
		"skill ,"
		"stamp ,"
		"sick_type ,"
		"fire_skill_bitflag, "
		"water_skill_bitflag, "
		"wood_skill_bitflag, "
		"skill_flag, "
		"change_value, "
		"hot_skill_one, "
		"hot_skill_two, "
		"hot_skill_three "
		"from %s where userid=%u and petid = %u ", 
	this->get_table_name(userid),
	userid,
	petid); 
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,RECORD_NOT_EXIST_ERR) ;
			(p_out_item)->petid=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->flag =atoi_safe(NEXT_FIELD); 
			(p_out_item)->flag2 =atoi_safe(NEXT_FIELD); 
			(p_out_item)->birthday =atoi_safe(NEXT_FIELD ); 
			(p_out_item)->logic_birthday =atoi_safe(NEXT_FIELD ); 
			strncpy(p_out_item->nick ,NEXT_FIELD,NICK_LEN ); 
			(p_out_item)->color=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->sicktime=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->pos_x =atoi_safe(NEXT_FIELD ); 
			(p_out_item)->pos_y =atoi_safe(NEXT_FIELD ); 
			(p_out_item)->hungry=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->thirsty=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->sanitary=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->spirit=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->endtime=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->starttime=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->taskid=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->skill=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->stamp=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->sick_type=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->fire_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->water_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->wood_skill_bitflag=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->skill_flag=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->change_value=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->hot_skill_one=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->hot_skill_two=atoi_safe(NEXT_FIELD ); 
			(p_out_item)->hot_skill_three=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_ONE_END();
}

int Cuser_pet :: get_flag2(userid_t userid, uint32_t petid, uint32_t *p_out)
{
	sprintf( this->sqlstr, "select flag2 from %s where userid=%u and petid = %u ", 
			this->get_table_name(userid), userid, petid); 
	
	STD_QUERY_ONE_BEGIN(this->sqlstr,RECORD_NOT_EXIST_ERR) ;
			*p_out=atoi_safe(NEXT_FIELD ); 
	STD_QUERY_ONE_END();
}

/**
 * @brief 设置宠物FLAG标志
 * @param userid 用户米米号
 * @param petid  宠物ID号
 * @param flag   要设置的FLAG值
 */
int Cuser_pet :: set_flag(userid_t userid, uint32_t petid, uint32_t flag)
{
	sprintf(this->sqlstr, "update %s set flag = %u\
			               where userid=%u and petid=%u",
		this->get_table_name(userid),
		flag,
		userid,
		petid
	);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 设置宠物FLAG2标志
 *
 */
int Cuser_pet :: set_flag2(userid_t userid, uint32_t petid, uint32_t flag2)
{
	sprintf(this->sqlstr, "update %s set flag2 = %u\
			               where userid=%u and petid=%u",
		this->get_table_name(userid),
		flag2,
		userid,
		petid
	);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 设置宠物的名称
 *
 */
int Cuser_pet :: set_nick(userid_t userid, uint32_t petid,  char *nick)
{
    char nick_str[mysql_str_len(NICK_LEN)];
    set_mysql_string(nick_str, nick, NICK_LEN);
    sprintf( this->sqlstr, "update %s set nick= '%s' \
                            where userid=%u and petid=%u ",
        this->get_table_name(userid),
        nick_str,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 设置宠物的生日
 *
 */
int Cuser_pet :: set_birthday(userid_t userid, uint32_t petid,  uint32_t birthday)
{
    sprintf( this->sqlstr, "update %s set birthday=%u \
                            where userid=%u and petid=%u ",
        this->get_table_name(userid),
        birthday,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief  设置宠物的颜色
 */
int Cuser_pet :: set_color(userid_t userid, uint32_t petid, uint32_t color)
{
    sprintf( this->sqlstr, "update %s set color=%u \
                            where userid=%u and petid=%u ",
        this->get_table_name(userid),
        color,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief  设置宠物的生病时间
 */
int Cuser_pet :: set_sicktime(userid_t userid, uint32_t petid, uint32_t sicktime)
{
    sprintf( this->sqlstr, "update %s set sicktime=%u \
                            where userid=%u and petid=%u ",
        this->get_table_name(userid),
        sicktime,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief  设置宠物的终止时间
 */
int Cuser_pet :: set_endtime(userid_t userid, uint32_t petid, uint32_t endtime)
{
    sprintf( this->sqlstr, "update %s set endtime=%u \
                            where userid=%u and petid=%u ",
        this->get_table_name(userid),
        endtime,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief  更新宠物的心情值等
 */
int Cuser_pet :: update_life(userid_t userid, uint32_t petid, user_pet_set_life_in *change, user_pet_set_life_out *out)
{
	this->cal(&out->hungry,    change->change_hungry);
	this->cal(&out->thirsty,   change->change_thirsty);
	this->cal(&out->sanitary,  change->change_sanitary);
	this->cal(&out->spirit,    change->change_spirit);

    sprintf(this->sqlstr, "update %s set hungry = %u, thirsty = %u, sanitary = %u, spirit = %u, stamp = %u \
			               where userid = %u and petid =%u",
        this->get_table_name(userid),
		out->hungry,
		out->thirsty,
		out->sanitary,
		out->spirit,
		change->stamp,
        userid,
        petid
	    );
    STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 计算宠物的心情值等，其值应在0~100范围之内
 */
int Cuser_pet :: cal(uint8_t *value, int32_t change_value)
{
	int32_t temp_value;
	temp_value = *value + change_value;
	if (temp_value >= 100 && temp_value < 10000) {
		*value = 100;
	} else if (temp_value >= 10000 || temp_value < 0) {
		*value = 0;
	} else {
		*value = temp_value;
	}
	return SUCC;
}

/**
 * @brief  得到宠物的心情值等
 */
int Cuser_pet :: get_life(userid_t userid, uint32_t petid,  user_pet_set_life_out *out)
{

    sprintf( this->sqlstr, "select hungry, thirsty, sanitary, spirit, stamp from %s\
			                where userid=%u and petid=%u ",
        this->get_table_name(userid),
        userid,
        petid
	    );
	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(out->hungry);
        INT_CPY_NEXT_FIELD(out->thirsty);
        INT_CPY_NEXT_FIELD(out->sanitary);
        INT_CPY_NEXT_FIELD(out->spirit);
		INT_CPY_NEXT_FIELD(out->stamp);
    STD_QUERY_ONE_END();
}

/**
 * @brief 得到心情值-》计算心情值-》更新心情值
 */
int Cuser_pet :: set_life(userid_t userid, uint32_t petid, user_pet_set_life_in *change, user_pet_set_life_out *out)
{
	int ret;
	ret = this->get_life(userid, petid, out);
    if (ret != SUCC) {
		return ret;
	}        
	ret=this->update_life(userid, petid, change, out);
	return ret;
}

/**
 * @brief 更新心情值-》得到宠物的全部信息
 */
int Cuser_pet :: set_get_pet(userid_t userid, user_pet_set_life_in *change, user_get_user_pet_out_item *out)
{
	user_pet_set_life_out pet_out;
	int ret;

	ret = this->set_life(userid, change->petid, change, &pet_out);
	if (ret != SUCC) {
		return ret;
	}

	ret = this->get_one_pet(userid, change->petid, out);
	return ret;
}

/**
 * @brief 喂养宠物，更新相应的值
 */
int Cuser_pet :: feed_pet(userid_t userid, user_pet_feed_pet_in *in)
{
	sprintf(this->sqlstr, "update %s set flag = %u, sicktime = %u, stamp = %u \
							where userid = %u and petid = %u",
						   this->get_table_name(userid),
						   in->flag,
						   in->sicktime,
						   in->stamp,
						   userid,
						   in->petid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 宠物托管/领会，更新相应的值 
 */
int Cuser_pet :: retrieve_pet(userid_t userid, user_pet_retrieve_pet_in *in)
{
	sprintf(this->sqlstr, "update %s set flag = %u, endtime = %u, stamp = %u \
						   where userid = %u and petid = %u",
						   this->get_table_name(userid),
						   in->flag,
						   in->endtime,
						   in->stamp,
						   userid,
						   in->petid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 接宠物任务，更新相应的值
 */
int Cuser_pet :: get_pet(userid_t userid, user_pet_get_pet_in *in)
{
	sprintf(this->sqlstr, "update %s set flag = %u, endtime = %u, starttime = %u, taskid = %u, stamp = %u \
							where userid = %u  and petid = %u",
						   this->get_table_name(userid),
							in->flag,
							in->endtime,
							in->starttime,
							in->taskid,
							in->stamp,
							userid,
							in->petid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

/**
 * @brief 宠物出游，访友等，更新相应的值
 */
int Cuser_pet :: travel_pet(userid_t userid, user_pet_travel_pet_in *in)
{
	sprintf(this->sqlstr, "update %s set flag = %u, endtime = %u where userid = %u and petid = %u",							
						  	this->get_table_name(userid),
							in->flag,
							in->endtime,
							userid, 
							in->petid
			);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Cuser_pet :: set_sick_type(userid_t userid, uint32_t pet_id, uint32_t sick_type)
{
	sprintf(this->sqlstr, "update %s set sick_type = %u where petid = %u",
			this->get_table_name(userid),
			sick_type,
			pet_id
			);
	STD_SET_RETURN_EX(this->sqlstr, RECORD_NOT_EXIST_ERR);
}


int Cuser_pet::set_pet_em_userid(userid_t userid, uint32_t petid, uint32_t em_userid)
{
	sprintf( this->sqlstr, "update %s set em_userid=%u \
							where userid=%u and petid=%u ",
		this->get_table_name(userid),
		em_userid,
		userid,
		petid
		);
	STD_SET_RETURN_EX(this->sqlstr, SUCC);
}

int Cuser_pet::get_user_pet_employ_info_list(userid_t userid, user_pet_employ_info_list_out_item **pp_list, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select petid, em_userid\
						   from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_list, p_count);
		INT_CPY_NEXT_FIELD((*pp_list + i)->petid);
		INT_CPY_NEXT_FIELD((*pp_list + i)->em_userid);
	STD_QUERY_WHILE_END();
}


int Cuser_pet::pet_flag_update(uint32_t userid, uint32_t petid, uint32_t pet_flag)
{
	sprintf( this->sqlstr, "update %s set pet_flag = %u\
			where userid = %u and petid = %u ",
		this->get_table_name(userid),
		pet_flag,
		userid,
		petid
		);
	STD_SET_RETURN_EX(this->sqlstr, USER_PET_SET_ERR);
}

/* 
  *@brief 判断用户是否拥有神力拉姆
*/
int Cuser_pet::is_sulamu(uint32_t userid, uint32_t &is_sulamu)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u and skill_flag=7 and flag2=1",
		this->get_table_name(userid),
		userid
		);

	STD_QUERY_ONE_BEGIN(this->sqlstr, RECORD_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(is_sulamu);
	STD_QUERY_ONE_END();
}
