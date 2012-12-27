/* room_msg 模块*/ 

#include "Cuser_lamu_diary.h"

Cuser_lamu_diary::Cuser_lamu_diary(mysql_interface * db ) : 
	CtableRoute100x10( db, "USER", "t_user_lamu_diary", "userid"),
	title_len(31), lamu_len(61), whisper_len(31), star_len(61), abc_len(145), content_len(601)
{

}



int Cuser_lamu_diary::insert(userid_t userid, user_lamu_diary_edit_in *p_in, uint32_t *p_id)
{
	char title_mysql[mysql_str_len(this->title_len)];
	set_mysql_string(title_mysql, p_in->title, this->title_len);

	char lamu_mysql[mysql_str_len(this->lamu_len)];
	set_mysql_string(lamu_mysql, p_in->lamu, this->lamu_len);

	char whisper_mysql[mysql_str_len(this->whisper_len)];
	set_mysql_string(whisper_mysql, p_in->whisper, this->whisper_len);

	char star_mysql[mysql_str_len(this->star_len)];
	set_mysql_string(star_mysql, p_in->star, this->star_len);
	
	char abc_mysql[mysql_str_len(this->abc_len)];
	set_mysql_string(abc_mysql, p_in->abc, this->abc_len);

	char content_mysql[mysql_str_len(this->content_len)];
	set_mysql_string(content_mysql, p_in->content, this->content_len);
	
	sprintf( this->sqlstr, "insert into %s values (NULL, %u, %u, 2, %u, 0, '%s', '%s', '%s', '%s', '%s', '%s')",
			 this->get_table_name(userid),
			 userid,
			 get_date(time(NULL)),
			 p_in->mood,
			 title_mysql,
			 lamu_mysql,
			 whisper_mysql,
			 star_mysql,
			 abc_mysql,
			 content_mysql
     		);
	STD_INSERT_GET_ID(this->sqlstr, DB_ERR, *p_id);
}

int Cuser_lamu_diary::update(userid_t userid, user_lamu_diary_edit_in *p_in)
{

	char title_mysql[mysql_str_len(this->title_len)];
	set_mysql_string(title_mysql, p_in->title, this->title_len);

	char lamu_mysql[mysql_str_len(this->lamu_len)];
	set_mysql_string(lamu_mysql, p_in->lamu, this->lamu_len);

	char whisper_mysql[mysql_str_len(this->whisper_len)];
	set_mysql_string(whisper_mysql, p_in->whisper, this->whisper_len);

	char star_mysql[mysql_str_len(this->star_len)];
	set_mysql_string(star_mysql, p_in->star, this->star_len);
	
	char abc_mysql[mysql_str_len(this->abc_len)];
	set_mysql_string(abc_mysql, p_in->abc, this->abc_len);

	char content_mysql[mysql_str_len(this->content_len)];
	set_mysql_string(content_mysql, p_in->content, this->content_len);
	
	
	sprintf( this->sqlstr, "update %s set mood = %u, state = %u, title = '%s', lamu = '%s',\
			whisper = '%s', star = '%s', abc = '%s', content = '%s' where id = %d",
			 this->get_table_name(userid),
			 p_in->mood,
			 p_in->state,
			 title_mysql,
			 lamu_mysql,
			 whisper_mysql,
			 star_mysql,
			 abc_mysql,
			 content_mysql,
			 p_in->id
     		);
	STD_SET_RETURN_EX(this->sqlstr, USER_ID_NOFIND_ERR);
}


int Cuser_lamu_diary::get_catalog_list(userid_t userid, uint32_t start, uint32_t end, 
	                  user_lamu_catalog_get_out_item	**pp_out, uint32_t *p_count)
{
	if ((start > end) || (end - start > 20)) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	uint32_t diff = end - start + 1;
	sprintf(this->sqlstr, "select id, date, state, title from %s\
			where userid = %u order by id desc limit %u, %u",
			this->get_table_name(userid),
			userid,
			start, 
			diff
			);
	STD_QUERY_WHILE_BEGIN(this->sqlstr, pp_out, p_count);
		INT_CPY_NEXT_FIELD((*pp_out + i)->id);
		INT_CPY_NEXT_FIELD((*pp_out + i)->date);
		INT_CPY_NEXT_FIELD((*pp_out + i)->state);
		BIN_CPY_NEXT_FIELD(&((*pp_out + i)->title), this->title_len);
	STD_QUERY_WHILE_END();
}

int Cuser_lamu_diary::delete_diary(userid_t userid, uint32_t id) 
{
	sprintf(this->sqlstr, "delete from %s where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Cuser_lamu_diary::update_state(userid_t userid, uint32_t id, uint32_t state) 
{
	if (state > 2 || state < 1) {
		return VALUE_OUT_OF_RANGE_ERR;
	}
	sprintf(this->sqlstr, "update %s set state = %u where id = %u",
			this->get_table_name(userid),
			state,
			id
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Cuser_lamu_diary::update_flower(userid_t userid, uint32_t id) 
{
	sprintf(this->sqlstr, "update %s set flower = flower + 1 where id = %u",
			this->get_table_name(userid),
			id
			);
	STD_SET_RETURN(this->sqlstr,userid,USER_ID_NOFIND_ERR);
}

int Cuser_lamu_diary::get_content(userid_t userid, uint32_t id, user_lamu_diary_content_get_out *p_out, uint32_t *p_state)
{
	sprintf(this->sqlstr, "select state, mood, flower, lamu, whisper, star, abc, content from %s\
			where id = %u",
			this->get_table_name(userid),
			id
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DIARY_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_state);
		INT_CPY_NEXT_FIELD(p_out->mood);
		INT_CPY_NEXT_FIELD(p_out->flower);
		BIN_CPY_NEXT_FIELD(&(p_out->lamu), this->lamu_len);
		BIN_CPY_NEXT_FIELD(&(p_out->whisper), this->whisper_len);
		BIN_CPY_NEXT_FIELD(&(p_out->star), this->star_len);
		BIN_CPY_NEXT_FIELD(&(p_out->abc), this->abc_len);
		BIN_CPY_NEXT_FIELD(&(p_out->content), this->content_len);
	STD_QUERY_ONE_END();
}

int Cuser_lamu_diary::get_count(userid_t userid, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select count(*) from %s where userid = %u",
			this->get_table_name(userid),
			userid
			);

	STD_QUERY_ONE_BEGIN(this->sqlstr, DIARY_NOT_EXIST_ERR);
		INT_CPY_NEXT_FIELD(*p_count);
	STD_QUERY_ONE_END();
}

int Cuser_lamu_diary::get_diary_simple_list(userid_t userid,user_get_lamu_diary_list_out_item **p_out, uint32_t *p_count)
{
	sprintf(this->sqlstr, "select title,content from %s where userid = %u",
			this->get_table_name(userid),userid);

	STD_QUERY_WHILE_BEGIN(this->sqlstr, p_out, p_count);
		BIN_CPY_NEXT_FIELD(&((*p_out + i)->title), this->title_len);
		BIN_CPY_NEXT_FIELD(&((*p_out + i)->content), this->content_len);
	STD_QUERY_WHILE_END();
}

