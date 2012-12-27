#include "db_error.h"
#include "Cgf_invite_code.h"

Cgf_invite_code::Cgf_invite_code(mysql_interface * db) : Ctable(db, "GF_OTHER", "t_gf_invite_code")
{
}

int Cgf_invite_code::check_invite_code(const char* invite_code, uint32_t* p_used)
{
	char code_mysql[mysql_str_len(GF_MAX_INVI_CODE_LEN)];
	int len = strlen(invite_code);
	set_mysql_string(code_mysql, invite_code, len > GF_MAX_INVI_CODE_LEN ? GF_MAX_INVI_CODE_LEN : len);
	sprintf(this->sqlstr, "select usedflg from %s where code='%s' ",
		this->get_table_name(), code_mysql);
	*p_used = 0;
	STD_QUERY_ONE_BEGIN(this->sqlstr , GF_INVALID_INVITE_CODE);
	  INT_CPY_NEXT_FIELD( *p_used );
	STD_QUERY_ONE_END();
}

int Cgf_invite_code::get_unused_code(uint32_t num, uint32_t* p_count, gf_get_invit_code_out_item** pp_item)
{
	GEN_SQLSTR(this->sqlstr,"select code from %s \
		where usedflg = 0 limit %u ;",
		this->get_table_name(), num);
	
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_item, p_count);
		BIN_CPY_NEXT_FIELD( (*pp_item+i)->code, 16);
	STD_QUERY_WHILE_END();
}
#if 0
int Cgf_invite_code::use_invite_code(const char* invite_code)
{
	char code_mysql[mysql_str_len(GF_MAX_INVI_CODE_LEN)];
	int len = strlen(invite_code);
	set_mysql_string(code_mysql, invite_code, len > GF_MAX_INVI_CODE_LEN ? GF_MAX_INVI_CODE_LEN:len);
	sprintf(this->sqlstr, "update %s set usedflg=1 where code=%s and usedflg=0;",
		this->get_table_name(), code_mysql);
	return this->exec_update_sql(this->sqlstr, GF_INVITE_CODE_USED);
}
#endif
int Cgf_invite_code::update_invite_code(uint32_t userid, const char* invite_code)
{
	char code_mysql[mysql_str_len(GF_MAX_INVI_CODE_LEN)];
	int len = strlen(invite_code);
	set_mysql_string(code_mysql, invite_code, len > GF_MAX_INVI_CODE_LEN ? GF_MAX_INVI_CODE_LEN : len);
	sprintf(this->sqlstr, "update %s set usedflg=1,userid=%u where code='%s' and usedflg=0;",
		this->get_table_name(), userid, code_mysql);
	return this->exec_update_sql(this->sqlstr, GF_INVITE_CODE_USED);
}

int Cgf_invite_code::check_user_invited(uint32_t userid, uint32_t* p_invited)
{
	sprintf(this->sqlstr, "select count(code) from %s where userid = %u;",
		this->get_table_name(), userid);
	STD_QUERY_ONE_BEGIN(this->sqlstr , GF_INVALID_INVITE_CODE);
	  INT_CPY_NEXT_FIELD( *p_invited );
	STD_QUERY_ONE_END();
}


