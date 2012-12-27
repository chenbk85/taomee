#ifndef  __GF_CGF_CARD_H__
#define  __GF_CGF_CARD_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"

class Cgf_card:public CtableRoute{
	public:
		Cgf_card(mysql_interface * db ); 

		int get_card_list(userid_t userid,
			  			  uint32_t usertm, 
						  gf_get_card_list_out_item ** plist,
						  uint32_t *count);

		int insert_card(userid_t userid,
						uint32_t usertm,
						uint32_t card_type,
						uint32_t card_set,
						uint32_t *card_id);

		int erase_card(userid_t userid,
					   uint32_t usertm,
					   uint32_t card_id);

		int get_card_cnt(userid_t userid, uint32_t role_regtime, gf_get_player_community_info_out* p_out);
		int clear_role_info(userid_t userid,uint32_t role_regtime);
	private:

};
#endif   /* ----- #ifndef __GF_CGF_CARD_H__  ----- */
