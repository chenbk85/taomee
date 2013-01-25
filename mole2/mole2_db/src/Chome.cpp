#include "Chome.h"


#define STR_USERID		"userid"
#define STR_FLAG		"flag"
#define STR_TYPE		"type"
#define STR_LEVEL		"level"
#define STR_EXP			"exp"
#define STR_NAME		"name"
#define	STR_PHOTO		"photo"
#define STR_BLACKBOARD	"blackboard"
#define STR_BOOKSHELF	"bookshelf"
#define STR_HONORBOX	"honorbox"
#define STR_PETFUN		"petfun"
#define STR_EXPBOX		"expbox"
#define STR_EFFIGY		"effigy"
#define	STR_POSTBOX		"postbox"
#define	STR_ITEMBOX		"itembox"
#define STR_PETBOX		"petbox"
#define STR_COMPOSE1	"compose1"
#define STR_COMPOSE2	"compose2"
#define STR_COMPOSE3	"compose3"
#define STR_COMPOSE4	"compose4"
#define STR_WALL		"wall"
#define STR_FLOOR		"floor"
#define STR_BED			"bed"
#define STR_DESK		"desk"
#define	STR_LADDER		"ladder"
#define STR_PETBOX_CNT	"petbox_cnt"




Chome::Chome(mysql_interface * db ) 
	 :CtableRoute100x10(db , "MOLE2_USER" , "t_home" , "userid")
{

}

int Chome::insert(userid_t userid, stru_mole2_home_info_t* p_in)
{
	char name_mysql[mysql_str_len(NICK_LEN)];
	set_mysql_string(name_mysql, p_in->name, NICK_LEN);
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s, %s, %s, %s, %s, %s) \
			values(%u, %u, %u, %u, %u, '%s')",
			this->get_table_name(userid),
			STR_USERID,
			STR_FLAG,
			STR_TYPE,
			STR_LEVEL,
			STR_EXP,
			STR_NAME,
			userid,
			p_in->flag,
			p_in->type,
			p_in->level,
			p_in->exp,
			name_mysql);
	return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Chome::insert(userid_t userid)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s(%s) values(%u)",
			this->get_table_name(userid),
			STR_USERID,
			userid);
	return this->exec_insert_sql(this->sqlstr, USER_ID_EXISTED_ERR);
}

int Chome::info_get(uint32_t	userid, stru_mole2_home_info_t* p_out)
{
	GEN_SQLSTR(this->sqlstr, 
		"select %s,%s,%s,%s,%s,%s,%s,%s,%s,%s,	\
				%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,	\
				%s,%s,%s,%s,%s,%s		\
		from %s where %s=%u",
			STR_USERID,
			STR_FLAG,
			STR_TYPE,
			STR_LEVEL,
			STR_EXP,
			STR_NAME,
			STR_PHOTO,
			STR_BLACKBOARD,
			STR_BOOKSHELF,
			STR_HONORBOX,
			STR_PETFUN,
			STR_EXPBOX,
			STR_EFFIGY,
			STR_POSTBOX,
			STR_ITEMBOX,
			STR_PETBOX,
			STR_COMPOSE1,
			STR_COMPOSE2,
			STR_COMPOSE3,
			STR_COMPOSE4,
			STR_WALL,
			STR_FLOOR,
			STR_BED,
			STR_DESK,
			STR_LADDER,
			STR_PETBOX_CNT,
			this->get_table_name(userid),
			STR_USERID,		userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr, USER_ID_NOFIND_ERR);
		INT_CPY_NEXT_FIELD(p_out->userid);
		INT_CPY_NEXT_FIELD(p_out->flag);
		INT_CPY_NEXT_FIELD(p_out->type);
		INT_CPY_NEXT_FIELD(p_out->level);
		INT_CPY_NEXT_FIELD(p_out->exp);
		BIN_CPY_NEXT_FIELD(p_out->name, NICK_LEN);
		INT_CPY_NEXT_FIELD(p_out->photo);
		INT_CPY_NEXT_FIELD(p_out->blackboard);
		INT_CPY_NEXT_FIELD(p_out->bookshelf);
		INT_CPY_NEXT_FIELD(p_out->honorbox);
		INT_CPY_NEXT_FIELD(p_out->petfun);
		INT_CPY_NEXT_FIELD(p_out->expbox);
		INT_CPY_NEXT_FIELD(p_out->effigy);
		INT_CPY_NEXT_FIELD(p_out->postbox);
		INT_CPY_NEXT_FIELD(p_out->compose1);
		INT_CPY_NEXT_FIELD(p_out->compose2);
		INT_CPY_NEXT_FIELD(p_out->compose3);
		INT_CPY_NEXT_FIELD(p_out->compose4);
		INT_CPY_NEXT_FIELD(p_out->wall);
		INT_CPY_NEXT_FIELD(p_out->floor);
		INT_CPY_NEXT_FIELD(p_out->bed);
		INT_CPY_NEXT_FIELD(p_out->desk);
		INT_CPY_NEXT_FIELD(p_out->ladder);
		INT_CPY_NEXT_FIELD(p_out->petbox_cnt);
	STD_QUERY_ONE_END();
}

int Chome::flag_set(uint32_t userid, uint32_t index,uint32_t flag)
{
	uint32_t mask = ~(1 << index);
	GEN_SQLSTR(this->sqlstr, "update %s set %s=(%s & %u)|%u where %s=%u",
		this->get_table_name(userid),
		STR_FLAG,		STR_FLAG,	mask, flag,
		STR_USERID,		userid);
	return this->exec_update_sql(this->sqlstr, USER_ID_NOFIND_ERR);
}

