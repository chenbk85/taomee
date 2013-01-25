
#include "Ccli_buff.h"

Ccli_buff::Ccli_buff(mysql_interface* db):
	CtableRoute(db, "MOLE2_USER", "t_cli_buff", "userid")
{
}


int Ccli_buff::set(uint32_t userid, mole2_set_user_cli_buff_in *p_in)
{
	char buff_mysql[mysql_str_len(sizeof(p_in->buff))];
	set_mysql_string(buff_mysql, (char *)(p_in->buff), sizeof(p_in->buff));

	GEN_SQLSTR(this->sqlstr, "update %s set buff='%s' where userid=%u and type=%u",
        	this->get_table_name(userid),buff_mysql,userid,p_in->type);
	int ret = this->exec_update_sql(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);

	if(ret != SUCC) {
		GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,'%s')",
				this->get_table_name(userid),userid,p_in->type,buff_mysql);
		ret = this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
	}

	return ret;
}

int Ccli_buff::get_range(uint32_t userid,uint32_t min,uint32_t max,mole2_get_user_cli_buff_out * p_out )
{
	GEN_SQLSTR(this->sqlstr, "select type,buff from %s where userid=%u and type >= %u and type <= %u",
        	this->get_table_name(userid),userid,min,max);
	STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr,p_out->item_list );
		INT_CPY_NEXT_FIELD(item.type);
		BIN_CPY_NEXT_FIELD(item.buff,sizeof(item.buff));
	STD_QUERY_WHILE_END_NEW();
}

