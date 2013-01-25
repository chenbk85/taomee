#include    "Crank.h"
#define		STR_USERID	"userid"
#define		STR_VAL     "val"

Crank::Crank(mysql_interface * db)
    :Ctable(db , "RAND_ITEM" , "t_rank")
{
}
int Crank::max(uint32_t *uid,uint32_t *val){
	GEN_SQLSTR(this->sqlstr,"select userid,val from %s as a where userid=a.userid and val=(select MAX(val)from %s)",
             this->get_table_name(),this->get_table_name());
    STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(*uid);
        INT_CPY_NEXT_FIELD(*val);
    STD_QUERY_ONE_END();
}

int Crank::update_rank(uint32_t userid, mole2_user_update_rank_in *p_in)
{
	uint32_t max=0,uid=0;
	this->max(&uid,&max);
	if(max>p_in->val){
		char nick_mysql[mysql_str_len(sizeof(p_in->nick))];
		set_mysql_string(nick_mysql, (char *)(p_in->nick), sizeof(p_in->nick));
		GEN_SQLSTR(this->sqlstr,"update %s set userid=%u,val=%u,nick='%s' where userid=%u",
						this->get_table_name(),userid,p_in->val,nick_mysql,uid);
		uint32_t ret=this->exec_update_sql(this->sqlstr,MOLE2_KEY_EXIST_ERR );
		if( ret != SUCC ){
			GEN_SQLSTR(this->sqlstr,"update %s set userid=%u,val=%u,nick='%s' where userid=%u  and val>%u ",
							this->get_table_name(),userid,p_in->val,nick_mysql,userid,p_in->val);
			return this->exec_update_sql(this->sqlstr,MOLE2_KEY_EXIST_ERR );
		}
	}
	return 0;
}

int Crank::get_top_ten(std::vector<stru_rank_val> &ranks)
{
	GEN_SQLSTR(this->sqlstr,"select *  from %s order by val asc",
			 this->get_table_name());
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, ranks);
        INT_CPY_NEXT_FIELD(item.id);
		if(item.id<5000)
			continue;
        INT_CPY_NEXT_FIELD(item.val);
		BIN_CPY_NEXT_FIELD(item.nick,sizeof(item.nick));
    STD_QUERY_WHILE_END_NEW();
}
