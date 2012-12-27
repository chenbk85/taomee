#ifndef  __GF_CGF_NUMEN_H__
#define  __GF_CGF_NUMEN_H__
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"



class Cgf_numen:public CtableRoute{
	public:
		Cgf_numen(mysql_interface * db ); 

        int get_numen_list(userid_t userid, uint32_t usertm, uint32_t* p_count, gf_get_numen_list_out_item** pplist);

		int add_numen(userid_t userid, uint32_t usertm, uint32_t numen_type, char* name);

        int update_numen_nick(userid_t userid , uint32_t usertm, uint32_t numen_type, char* nick);

        int set_int_value(userid_t userid, uint32_t usertm, uint32_t mon_tm, const char* column_type, uint32_t value);

        int set_numen_status(userid_t userid, uint32_t usertm, uint32_t numen_type, uint32_t status);

        int set_role_numen_status(userid_t userid, uint32_t usertm, uint32_t status);

    private:

};
#endif   /* ----- #ifndef __GF_CGF_NUMEN_H__  ----- */
