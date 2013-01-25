#include "Csurvey.h"

Csurvey::Csurvey(mysql_interface * db)
	:Ctable(db , "RAND_ITEM" , "t_survey")
{

}

int Csurvey::update(uint32_t surveyid,stru_survey_reply &reply)
{
	GEN_SQLSTR(this->sqlstr, "insert into %s values(%u,%u,%u,1) "
		"ON DUPLICATE KEY UPDATE total = total + 1",
		this->get_table_name(),surveyid,reply.option_id,reply.value_id);
	return this->exec_insert_sql(this->sqlstr, SUCC);
}

int Csurvey::get_survey(uint32_t surveyid, std::vector<stru_survey_reply_new> &reply)
{
	GEN_SQLSTR(this->sqlstr,"select optionid, answerid, total from %s where surveyid = %u"
			 ,this->get_table_name(), surveyid);
    STD_QUERY_WHILE_BEGIN_NEW(this->sqlstr, reply);
        INT_CPY_NEXT_FIELD(item.option_id);
        INT_CPY_NEXT_FIELD(item.answer_id);
        INT_CPY_NEXT_FIELD(item.total);
    STD_QUERY_WHILE_END_NEW();
}
