#include <algorithm>
#include "db_error.h"
#include "Cgf_summon_dragon_list.h"

Cgf_summon_dragon_list::Cgf_summon_dragon_list(mysql_interface * db )
		:Ctable(db, "GF_OTHER", "t_gf_summon_dragon_list")
{

}

int Cgf_summon_dragon_list::get_summon_dragon_list(uint32_t top_limit, 
	gf_get_summon_dragon_list_out_item** pdata, uint32_t* count)
{
	GEN_SQLSTR(sqlstr, "select userid,nick,nimbus,tm from %s order by tm desc limit %u", 
		get_table_name(), top_limit);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pdata, count);
        INT_CPY_NEXT_FIELD( (*pdata+i)->uid );
		BIN_CPY_NEXT_FIELD( (*pdata+i)->nick, NICK_LEN);
		INT_CPY_NEXT_FIELD( (*pdata+i)->nimbus );
		INT_CPY_NEXT_FIELD( (*pdata+i)->tm );
	STD_QUERY_WHILE_END();
}


int Cgf_summon_dragon_list::add_summon_dragon(gf_add_summon_dragon_in* p_in)
{
    char nick_mysql[mysql_str_len(NICK_LEN)];
    set_mysql_string(nick_mysql, p_in->nick, NICK_LEN);
	GEN_SQLSTR( this->sqlstr, "insert into %s (userid,nick,nimbus,tm) values (%u,'%s', %u, %u);" ,
		this->get_table_name(), p_in->uid, nick_mysql, p_in->nimbus, p_in->tm);
	return this->exec_insert_sql (this->sqlstr, GF_SUMMON_EXISTED_ERR);	
}

