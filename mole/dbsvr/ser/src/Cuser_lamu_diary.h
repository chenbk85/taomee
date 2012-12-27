#ifndef CUSER_LAMU_DIARY
#define CUSER_LAMU_DIARY

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_lamu_diary : public CtableRoute100x10 {
public:
    Cuser_lamu_diary(mysql_interface * db);
	int insert(userid_t userid, user_lamu_diary_edit_in *p_in, uint32_t *p_id);

	int get_catalog_list(userid_t userid, uint32_t start, uint32_t end,
		user_lamu_catalog_get_out_item **pp_out, uint32_t *p_count);

	int delete_diary(userid_t userid, uint32_t id);

	int update(userid_t userid, user_lamu_diary_edit_in *p_in);

	int update_state(userid_t userid, uint32_t id, uint32_t state);

	int update_flower(userid_t userid, uint32_t id); 

	int get_content(userid_t userid, uint32_t id, user_lamu_diary_content_get_out *p_out, uint32_t *p_state);

	int get_count(userid_t userid, uint32_t *p_count);

	int get_diary_simple_list(userid_t userid,user_get_lamu_diary_list_out_item **p_out, uint32_t *p_count);

private:
	const uint32_t title_len;

	const uint32_t lamu_len;

	const uint32_t whisper_len;

	const uint32_t star_len;

	const uint32_t abc_len;

	const uint32_t content_len;
};



#endif // _CUSER_DIARY_H_20081110_
/**********************************EOF*****************************************/


