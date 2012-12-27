
#include "Cuser_dragon.h"
#include "proto.h"
#include "common.h"
#include "benchapi.h"
#include <msglog.h>

static def_dragon_info dragon_tab[] = {
#ifndef TW_VER
	{190681,1350001,1000,(char*)"辛迪亚"},
	{190690,1350002,10000,(char*)"幽灵巨龙"},
	{190709,1350003,7000,(char*)"独角萨伦龙"},
	{190708,1350004,12000,(char*)"哈米龙"},
	{190710,1350005,10000,(char*)"杰尼哈瑞龙"},
	{190711,1350006,10000,(char*)"火翼巨龙"},
	{190712,1350007,12000,(char*)"柯多拉巨龙"},
	{190713,1350008,12000,(char*)"蒂斯特巨龙"},
	{190714,1350009,12000,(char*)"夜煞龙"},
	{190715,1350010,12000,(char*)"彩蛋龙"},
	{0     ,1350011,0,    (char*)"冰焰骏马"},
	{0     ,1350012,0,    (char*)"卡酷年坐骑"},
	{0     ,1350013,0,    (char*)"雪雪"},
	{0     ,1350014,0,    (char*)"爱莲娜"},
	{0     ,1350015,0,    (char*)"尼尼莫莫"},
	{0     ,1350016,0,    (char*)"茸茸斯达"},
	{0     ,1350017,0,    (char*)"金球萤火"},
	{0     ,1350018,0,    (char*)"爱心美刺"},
	{0     ,1350019,0,    (char*)"红孩儿"},
	{0     ,1350020,0,    (char*)"远古猛犸"},
	{0     ,1350021,0,    (char*)"爱莲女王"},
	{0     ,1350022,0,    (char*)"小分"},
	{0     ,1350023,0,    (char*)"双峰奇驼"},
	{0     ,1350024,0,    (char*)"彩羽伯爵"},
	{0     ,1350025,0,    (char*)"青焰伯爵"},
	{0     ,1350026,0,    (char*)"三眼金蟾"},
	{0     ,1350027,0,    (char*)"霹雳雷龙"},
	{0     ,1350028,0,    (char*)"霹雳火龙"},
	{0     ,1350029,0,    (char*)"白玉虎"},
	{0     ,1350030,0,    (char*)"四足岩虎"},
	{0     ,1350031,0,    (char*)"待定"},
	{0     ,1350032,0,    (char*)"吧啦鸟"},
	{0     ,1350033,0,    (char*)"狸猫苗苗"},
	{0     ,1350034,0,    (char*)"火刺猬"},
	{0     ,1350035,0,    (char*)"超人鼠"},
	{0     ,1350036,0,    (char*)"荷鲁斯"},
	{0     ,1350037,0,    (char*)"斯芬克斯"},
	{0     ,1350038,0,    (char*)"五斑灵鹿"},
	{0     ,1350039,0,    (char*)"六角羚羊"},
	{0     ,1350040,0,    (char*)"七彩鹦鹉"},
	{0     ,1350041,0,    (char*)"金福蛛"},
	{0     ,1350042,0,    (char*)"祥云九尾"},
	{190716,1350043,12000,(char*)"火焰精灵龙"},
	{0     ,1350044,0,	  (char*)"冰雪精灵龙"},
	{0     ,1350045,0,	  (char*)"雪麒麟"},
	{0     ,1350046,0,	  (char*)"华冠海豚"},
	{0     ,1350047,0,	  (char*)"火焰金牛"},
#else
	{190681,1350001,1000,(char*)"辛迪亞"},
	{190690,1350002,10000,(char*)"幽靈巨龍"},
	{190709,1350003,7000,(char*)"獨角薩倫龍"},
	{190708,1350004,12000,(char*)"哈米龍"},
	{190710,1350005,10000,(char*)"杰尼哈瑞龍"},
	{190711,1350006,10000,(char*)"火翼巨龍"},
	{190712,1350007,12000,(char*)"柯多拉巨龍"},
	{190713,1350008,12000,(char*)"蒂斯特巨龍"},
	{190714,1350009,12000,(char*)"夜煞龍"},
	{190715,1350010,12000,(char*)"彩蛋龍"},
	{0     ,1350011,0,    (char*)"冰燄駿馬"},
	{0     ,1350012,0,    (char*)"卡酷年坐騎"},
	{0     ,1350013,0,    (char*)"雪雪"},
	{0     ,1350014,0,    (char*)"愛蓮娜"},
	{0     ,1350015,0,    (char*)"尼尼莫莫"},
	{0     ,1350016,0,    (char*)"茸茸斯達"},
	{0     ,1350017,0,    (char*)"金球螢火"},
	{0     ,1350018,0,    (char*)"愛心美刺"},
	{0     ,1350019,0,    (char*)"紅孩兒"},
	{0     ,1350020,0,    (char*)"遠古猛犸"},
	{0     ,1350021,0,    (char*)"愛蓮女王"},
	{0     ,1350022,0,    (char*)"小分"},
	{0     ,1350023,0,    (char*)"雙峰奇駝"},
	{0     ,1350024,0,    (char*)"彩羽伯爵"},
	{0     ,1350025,0,    (char*)"青焰伯爵"},
	{0     ,1350026,0,    (char*)"三眼金蟾"},
	{0     ,1350027,0,    (char*)"霹靂雷龍"},
	{0     ,1350028,0,    (char*)"霹靂火龍"},
	{0     ,1350029,0,    (char*)"白玉虎"},
	{0     ,1350030,0,    (char*)"四足岩虎"},
	{0     ,1350031,0,    (char*)"待定"},
	{0     ,1350032,0,    (char*)"吧啦鳥"},
	{0     ,1350033,0,    (char*)"狸貓苗苗"},
	{0     ,1350034,0,    (char*)"火刺猬"},
	{0     ,1350035,0,    (char*)"超人鼠"},
	{0     ,1350036,0,    (char*)"荷魯斯"},
	{0     ,1350037,0,    (char*)"斯芬克斯"},
	{0     ,1350038,0,    (char*)"五斑靈鹿"},
	{0     ,1350039,0,    (char*)"六角羚羊"},
	{0     ,1350040,0,    (char*)"七彩鸚鵡"},
	{0     ,1350041,0,    (char*)"金福蛛"},
	{0     ,1350042,0,    (char*)"祥雲九尾"},
	{190716,1350043,12000,(char*)"火焰精靈龍"},
	{0     ,1350044,0,	  (char*)"冰雪精靈龍"},
	{0     ,1350045,0,	  (char*)"雪麒麟"},
	{0     ,1350046,0,	  (char*)"華冠海豚"},
	{0     ,1350047,0,	  (char*)"火焰金牛"},
#endif
};



Cuser_dragon::Cuser_dragon(mysql_interface * db)
	:CtableRoute100x10(db, "USER", "t_user_dragon", "userid")
{
	this->msglog_file = config_get_strval("MSG_LOG_FILE");
}

int Cuser_dragon::log_dragon(userid_t userid,uint32_t dragonid,int num)
{
	uint32_t buff[2] ;

	if(num > 0) {
		buff[0] = 1;
		buff[1] = 0;
	} else {
		buff[0] = 0;
		buff[1] = 1;
	}

	msglog(this->msglog_file,0x0201F000 + dragonid - 1350000,time(NULL),buff,sizeof(buff));

	return 0;
}

uint32_t Cuser_dragon::get_perm_count(uint32_t mole_exp,uint32_t upgrade,uint32_t isvip)
{
	uint32_t count = 2;
	int lvl = get_mole_level(mole_exp);

	if(lvl >= 50) {
		count += 1;
		if(isvip) count += 1;
		if(lvl >= 100) {
			count += 1;
			if(isvip) count += 1;
		}
	}
	
	if(upgrade) {
		if(lvl >= 30) {
			count += 1;
			if(isvip) count += 1;
			if(lvl >= 40) {
				if(isvip) count += 1;
				if(lvl >= 70) {
					count += 1;
					if(isvip) count += 1;
				}
			}
		}	
	}

	return count;
}

uint32_t Cuser_dragon::get_item_growth(uint32_t itemid)
{
	struct item_growth {
		uint32_t itemid;
		uint32_t growth;
	} item_tab[] = {
		{190673,200},
		{190688,600}
	};

    int i = 0;
	int count = sizeof(item_tab) / sizeof(item_tab[0]);
    while(i < count ) {
		if(item_tab[i].itemid == itemid) return item_tab[i].growth;
		i++;
	}

	return 0;
}

def_dragon_info* Cuser_dragon::get_def_dragon_info(uint32_t dragonid)
{
	int i = 0;
	int count = sizeof(dragon_tab) / sizeof(dragon_tab[0]);
	while(i < count ) {
		if(dragon_tab[i].dragonid == dragonid) return dragon_tab + i;
		i++;								    
	}

	return NULL;
}

uint32_t Cuser_dragon::get_dragon_id_by_egg(uint32_t egg_id)
{
	int i = 0;
	int count = sizeof(dragon_tab) / sizeof(dragon_tab[0]);
	while(i < count ) {
		if(dragon_tab[i].eggid == egg_id) return dragon_tab[i].dragonid;
		i++;
	}

	return 0;
}

int Cuser_dragon::add(userid_t userid, uint32_t dragonid)
{
	int growth = 1;
	char name[NICK_LEN] = {0};
	uint32_t create_time = time(NULL);
	def_dragon_info *p = get_def_dragon_info(dragonid);
	if(p) {
		strncpy(name, p->dragon_name,NICK_LEN-1);
	} else {
		return NOT_FOUND_DRAGON_ERR;
	}

	char nick_mysql[mysql_str_len(NICK_LEN)];

	memset(nick_mysql,0,NICK_LEN);
	set_mysql_string(nick_mysql,name,NICK_LEN);
	
	if(NOT_DRAGON(dragonid)) growth = 0;
	
	this->log_dragon(userid,dragonid,1);
	sprintf(this->sqlstr, "insert into %s values(%u, %u, '%s', %d, 0, %u)",
	this->get_table_name(userid),userid,dragonid,nick_mysql,growth,create_time);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_EXISTED_ERR);
}
                 
int Cuser_dragon::del(userid_t userid, uint32_t dragonid)
{
	this->log_dragon(userid,dragonid,-1);
	sprintf(this->sqlstr, "delete from %s where userid=%u and dragonid=%u",
		this->get_table_name(userid),userid,dragonid);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon::get_dragon_count(userid_t userid,uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid=%u and (dragonid <= 1350010 or dragonid = 1350043 \
		or dragonid >=1350111)",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dragon::get_dragon_count_by_isvip(userid_t userid, uint32_t is_vip, uint32_t&count)
{
	if(is_vip >= 1){
		sprintf(this->sqlstr, "select count(*) from %s where userid=%u and (dragonid <= 1350010 or \
			dragonid = 1350043 or dragonid >=1350111)",
			this->get_table_name(userid),userid);
	}
	else{
		sprintf(this->sqlstr, "select count(*) from %s where userid=%u and state != 2 and (dragonid <= 1350010 \
			or dragonid = 1350043 or dragonid >=1350111)",
			this->get_table_name(userid),userid);
	}

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dragon::get_unlock_count(userid_t userid,uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid=%u "
		"and (dragonid <= 1350010 or dragonid = 1350043 or dragonid >=1350111) and state != 2",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();
}

int Cuser_dragon::clear_state_1(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set state=0 where userid=%u and state = 1",
        	this->get_table_name(userid),userid);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon::clear_state_2(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set state=0 where userid=%u and (dragonid <= 1350010 or \
		dragonid = 1350043 or dragonid >=1350111) and state = 2",
		this->get_table_name(userid),userid);
	STD_SET_RETURN_WITH_NO_AFFECTED(this->sqlstr);
}

int Cuser_dragon::lock_all_dragons(userid_t userid)
{
	sprintf(this->sqlstr, "update %s set state=2 where userid=%u and (dragonid <= 1350010 \
		or dragonid = 1350043 or dragonid >=1350111)",
		this->get_table_name(userid),userid);
	STD_SET_RETURN_WITH_NO_AFFECTED (this->sqlstr);
}

int Cuser_dragon::get_dragons(userid_t userid, get_dragon_list_out_item **p_out, uint32_t *p_count)
{
	uint32_t nouse;
	sprintf(this->sqlstr, "select dragonid,name,growth,state,"
		" if(dragonid > 1350010 && dragonid < 1350111 && dragonid != 1350043,2,if(state != 2,0,1)) as A"
		" from %s where userid=%u order by A, create_time",
		this->get_table_name(userid),userid);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out, p_count);
		INT_CPY_NEXT_FIELD((*p_out + i)->dragon_id);
		BIN_CPY_NEXT_FIELD((*p_out + i)->name,16);
		INT_CPY_NEXT_FIELD((*p_out + i)->growth);
		INT_CPY_NEXT_FIELD((*p_out + i)->state);
		INT_CPY_NEXT_FIELD(nouse);
	STD_QUERY_WHILE_END();	
}

int Cuser_dragon::get_dragon_info(userid_t userid, uint32_t dragonid, uint32_t &p_growth, uint32_t &p_state)
{
	sprintf(this->sqlstr, "select growth,state from %s where userid=%u and dragonid=%u",
		this->get_table_name(userid),userid,dragonid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_growth);
        	INT_CPY_NEXT_FIELD(p_state);
	STD_QUERY_ONE_END();
}

int Cuser_dragon::set_dragon_state(userid_t userid, uint32_t dragonid, uint32_t state)
{
	sprintf(this->sqlstr, "update %s set state=%u where userid=%u and dragonid=%u",
		this->get_table_name(userid),state,userid,dragonid);
	STD_SET_RETURN_EX (this->sqlstr,USER_ID_NOFIND_ERR);
}

int Cuser_dragon::add_current_growth(userid_t userid, uint32_t growth)
{
	sprintf(this->sqlstr, "update %s set growth=growth+%u where userid=%u "
		" and (dragonid <= 1350010 or dragonid = 1350043 or dragonid >=1350111) and state=1",
		this->get_table_name(userid),growth,userid);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon::add_dragon_growth(userid_t userid, uint32_t dragonid, uint32_t growth)
{
	if(dragonid > 1350010 && dragonid < 1350111 && dragonid != 1350043) {
		return SUCC;
	}

	sprintf(this->sqlstr, "update %s set growth=growth+%u where userid=%u and dragonid=%u",
		this->get_table_name(userid),growth,userid,dragonid);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon::set_name(userid_t userid, uint32_t dragonid, char *name)
{
	char nick_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(nick_mysql,name,NICK_LEN);
	sprintf(this->sqlstr, "update %s set name='%s' where userid=%u and dragonid=%u",
        	this->get_table_name(userid),nick_mysql,userid,dragonid);
	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}

int Cuser_dragon::get_current_dragon(userid_t userid, get_current_dragon_out *p_out)
{
	sprintf(this->sqlstr, "select dragonid,name,growth from %s "
		"where userid=%u and state = 1",
		this->get_table_name(userid),userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->dragon_id);
		BIN_CPY_NEXT_FIELD(p_out->name,16);
		INT_CPY_NEXT_FIELD(p_out->growth);
	STD_QUERY_ONE_END();
}

int Cuser_dragon::get_angel_dragon(userid_t userid, uint32_t angelid, uint32_t &count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid=%u and dragonid = %u",
		this->get_table_name(userid),
		userid,
		angelid
		);

	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(count);
	STD_QUERY_ONE_END();

}

int Cuser_dragon::set_dragonid(userid_t userid, uint32_t dragonid, uint32_t new_dragonid)
{
	sprintf(this->sqlstr, "update %s set dragonid = %u where userid=%u and dragonid=%u",
        	this->get_table_name(userid),new_dragonid,userid,dragonid);

	STD_SET_RETURN_EX (this->sqlstr, USER_ID_NOFIND_ERR);
}
