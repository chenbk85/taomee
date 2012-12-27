/*
 * =====================================================================================
 *
 *
 *	   Filename:  route_func.cpp
 *
 *	Description:
 *
 *		Version:  1.0
 *		Created:  2009 
 *	   Revision:  none
 *	   Compiler:  gcc
 *
 *		 Author:  jim(jim), jim@taomee.com
 *		Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include "Croute_func.h"
#include "benchapi.h"
#include "proto.h"
//#include "db_error.h"
//#include "gf_common.h"
#include <limits.h>
#include <libtaomee/time/time.h>
#include <libtaomee/bitmanip/bitmanip.h>
#include <libtaomee++/inet/pdumanip.hpp>



using namespace taomee ;

CMD_MAP g_cmdmap[]={
	FILL_CMD_WITHOUT_PRI_IN (gf_mail_head_list),  
	FILL_CMD_WITH_PRI_IN    (gf_mail_body),
	FILL_CMD_WITH_PRI_IN    (gf_delete_mail),
	FILL_CMD_WITH_PRI_IN_GE (gf_take_mail_enclosure),
	FILL_CMD_WITH_PRI_IN    (gf_send_mail),
	FILL_CMD_WITH_PRI_IN    (gf_send_system_mail)
};

#define DEALFUN_COUNT  (int(sizeof(g_cmdmap )/sizeof(g_cmdmap[0])))





/*
 *--------------------------------------------------------------------------------------
 *	   Class:  Croute_func
 *	  Method:  Croute_func
 * Description:  constructor
 *--------------------------------------------------------------------------------------
 */


Croute_func::Croute_func (mysql_interface * db) : Cfunc_route_base(db),
gf_mail(db)
{
	this->db=db;
	initlist(g_cmdmap,DEALFUN_COUNT );	
	uint32_t malloc_size=sizeof(stru_cmd_item_t )*64*1024;
	this->p_cmd_list=(stru_cmd_item_t* ) malloc (malloc_size );
	memset(this->p_cmd_list,0, malloc_size);
}  

Croute_func::~Croute_func()
{

}

int Croute_func::do_sync_data(uint32_t userid, uint16_t cmdid )
{
       return SUCC;
}

int Croute_func::gf_mail_head_list(DEAL_FUN_ARG)
{
	gf_mail_head_list_out_header out_header = { 0 };
	gf_mail_head_list_out_element* p_out_element = NULL;

	ret = gf_mail.get_mail_head_list(USERID_ROLETM,  &p_out_element, &out_header.count);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT_LIST(ret, out_header, p_out_element, out_header.count);
}

int Croute_func::gf_mail_body(DEAL_FUN_ARG)
{
	gf_mail_body_in* p_in = PRI_IN_POS;
	gf_mail_body_out out = {0};
	ret = gf_mail.get_mail_body(RECVBUF_USERID,  p_in->mail_id, &out);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	ret = gf_mail.set_mail_read_flag(RECVBUF_USERID, p_in->mail_id, 1);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}


int Croute_func::gf_delete_mail(DEAL_FUN_ARG)
{
	gf_delete_mail_in* p_in = PRI_IN_POS;
	gf_delete_mail_out out = {0};

	ret = gf_mail.delete_mail(RECVBUF_USERID,  p_in->mail_id);
	if(ret != SUCC)
	{
		STD_ROLLBACK();
		return ret;
	}
	out.mail_id = p_in->mail_id;
	STD_RETURN_WITH_STRUCT(ret, out);
}


int Croute_func::gf_take_mail_enclosure(DEAL_FUN_ARG)
{
	char buf[PROTO_MAX_SIZE] = {0};
	int idx = 0;

	gf_take_mail_enclosure_in_header* p_in = PRI_IN_POS;
	ret = gf_mail.clear_mail_enclosures(RECVBUF_USERID,  p_in->mail_id);	
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	return 0;
}


int  Croute_func::gf_send_mail(DEAL_FUN_ARG)
{
	gf_send_mail_in* p_in = PRI_IN_POS;
	gf_send_mail_out out = {0};

	uint32_t cur_count = 0;
	ret = gf_mail.get_mail_count(RECVBUF_USERID, p_in->receive_id, &cur_count);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	if(cur_count >= p_in->max_mail_limit){
		STD_ROLLBACK();
		return GF_MAIL_FULL;
	}
	ret = gf_mail.insert_user_mail(RECVBUF_USERID, p_in, &out);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}


int Croute_func::gf_send_system_mail(DEAL_FUN_ARG)
{
	gf_send_system_mail_in* p_in = PRI_IN_POS;
	gf_send_mail_out out = {0};
	
	uint32_t cur_count = 0;
	ret = gf_mail.get_mail_count(RECVBUF_USERID, p_in->receive_id, &cur_count);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	if(cur_count >= p_in->max_mail_limit){
		STD_ROLLBACK();
		return GF_MAIL_FULL;
	}
	ret = gf_mail.insert_system_mail(USERID_ROLETM, p_in, &out);
	if(ret != SUCC){
		STD_ROLLBACK();
		return ret;
	}
	STD_RETURN_WITH_STRUCT(ret,out);
}



