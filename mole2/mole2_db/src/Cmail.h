#ifndef  CMAIL_INC
#define  CMAIL_INC

#include "CtableRoute.h"
#include "proto.h"
#include "db_error.h"
#include "benchapi.h"


#define MAIL_COUNT_MAX	 500

typedef struct {
	uint32_t itmid;
	uint16_t count;
} __attribute__((packed)) mail_item_t;

typedef struct {
	uint32_t count;
	mail_item_t item[0];
} __attribute__((packed)) item_list_t;

class Cmail : public CtableRoute
{
public:
	Cmail(mysql_interface * db);

	int insert(userid_t userid, uint32_t mailid, stru_mole2_mail_info* p_in);

	int del(userid_t userid, uint32_t mailid);

	int del_all(userid_t userid);

	int count_get(userid_t userid, uint32_t* p_count);
	int get_theme_count(userid_t userid, uint32_t themeid,uint32_t* p_count);

	int simple_list_get(userid_t userid, user_mail_simple_list_get_out_item** pp_out_item, uint32_t* p_count);

	int content_get(userid_t userid, uint32_t mailid, char* p_msg, char* p_item_list, uint32_t* p_flag, char* p_id_list);

	int flag_set(userid_t userid, uint32_t mailid, uint32_t flag);

	int get_mail_item_list(userid_t userid, uint32_t mailid, item_list_t* p_list);
	int set_mail_item_list(userid_t userid, uint32_t mailid, item_list_t* p_list);

private:

};


#endif

