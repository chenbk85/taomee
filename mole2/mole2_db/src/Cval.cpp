#include    "Cval.h"
#define		STR_USERID	"userid"
#define		STR_VAL     "val"
#define     STR_TODAY   "today"

Cval::Cval(mysql_interface * db)
    :Ctable(db , "RAND_ITEM" , "t_val")
{

}

int Cval::check_val_day(uint32_t userid, uint32_t &val, uint32_t & day)
{
	GEN_SQLSTR(this->sqlstr,"select %s ,%s from %s where userid=%u for update",
			STR_VAL, STR_TODAY, this->get_table_name(), userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr,MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(val);
        INT_CPY_NEXT_FIELD(day);
    STD_QUERY_ONE_END();

}

int Cval::update_val(uint32_t userid, uint32_t limit, uint32_t &rt)
{
	uint32_t date=today(),day=0;
	this->check_val_day(userid, rt, day);
	if( day !=date ){
		GEN_SQLSTR(this->sqlstr,"insert into %s values(%u, 1, %u) ON DUPLICATE KEY UPDATE %s=1 ",
				this->get_table_name(), userid, date, STR_VAL);
		if(this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR) !=0)
			return MOLE2_KEY_EXIST_ERR;
		rt=1;
		GEN_SQLSTR(this->sqlstr,"update %s set %s= %u where %s =%u",
                this->get_table_name(), STR_TODAY, date, STR_USERID ,userid);	
		return this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR);
	
	}
	else
	{
		if(limit>rt){
			GEN_SQLSTR(this->sqlstr,"update %s set %s = %s +1 where userid = %u",
					this->get_table_name(), STR_VAL, STR_VAL, userid);
			if(this->exec_insert_sql(this->sqlstr, MOLE2_KEY_EXIST_ERR) !=0)
				return MOLE2_KEY_EXIST_ERR;
			rt+=1;
		} else {
			return MOLE2_KEY_EXIST_ERR;
		}
	}
	return 0;
}


int Cval::get_val(std::vector<stru_usr_val> &vals)
{
	int date=today(), day=0;
	GEN_SQLSTR(this->sqlstr,"select %s, %s, %s from %s",
			STR_USERID, STR_VAL, STR_TODAY, this->get_table_name());
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, vals);
        INT_CPY_NEXT_FIELD(item.userid);
        INT_CPY_NEXT_FIELD(item.val);
        INT_CPY_NEXT_FIELD(day);
		if(date !=day)
			item.val=0;
    STD_QUERY_WHILE_END_NEW();
}

int Cval::get_val(uint32_t userid, uint32_t &count)
{
    int date=today(), day=0;
    GEN_SQLSTR(this->sqlstr,"select %s, %s from %s where %s = %u",
            STR_VAL, STR_TODAY, this->get_table_name(), STR_USERID, userid);
    STD_QUERY_ONE_BEGIN(this->sqlstr, MOLE2_KEY_NOT_EXIST_ERR);
        INT_CPY_NEXT_FIELD(count);
        INT_CPY_NEXT_FIELD(day);
        if(date !=day)
            count=0;
    STD_QUERY_ONE_END();
}
