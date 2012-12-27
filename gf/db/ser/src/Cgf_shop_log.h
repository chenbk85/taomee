
#ifndef  GF_CGF_SHOP_LOG_H
#define  GF_CGF_SHOP_LOG_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"
using namespace std;

class Cgf_shop_log :public CtableRoute{
	public:
 		Cgf_shop_log(mysql_interface * db);
		int add_shop_log(userid_t userid, uint32_t role_tm, gf_add_shop_log_in* p_in);
		int get_shop_last_log_list(userid_t userid, uint32_t role_tm, uint32_t* p_count, gf_get_shop_last_log_out_item** pp_list);
		int add_shop_log_indb(userid_t userid, uint32_t role_tm, gf_sell_goods_in* p_in);
		int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:
		int ret;
	public:
};


#endif   /* ----- #ifndef GF_CGF_FRIEND_H  ----- */

