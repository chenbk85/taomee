#ifndef  GF_CGF_INVITE_CODE_H
#define    GF_CGF_INVITE_CODE_H
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cgf_invite_code:public Ctable{
	public:
		Cgf_invite_code(mysql_interface * db ); 

		int check_invite_code(const char* invite_code, uint32_t* p_used);

//		int use_invite_code(const char* invite_code);

		int check_user_invited(uint32_t userid, uint32_t* p_invited);

		int update_invite_code(uint32_t userid, const char* invite_code);

		int get_unused_code(uint32_t num, uint32_t* p_count, gf_get_invit_code_out_item** pp_item);
};
#endif 

