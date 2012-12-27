/*
 * =====================================================================================
 * 
 *       Filename:  Cqa.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2009年02月09日 11时36分18秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CQA_INC
#define  CQA_INC
#include "Ctable.h"
#include "proto.h"
//问题和答案
class Cqa: public Ctable{
	protected:
	public:
		Cqa(mysql_interface * db ); 
		int getmsglist( uint32_t start, uint32_t count, 
				uint32_t *p_count, qa_msg_item ** pp_list  );
		int get_count(uint32_t *p_count);


		int getmsglist_ex( uint32_t start, uint32_t count, 
				uint32_t *p_count, msgboard_qa_get_list_ex_out_item ** pp_list  );
		int insert( qa_msg_item *p_item );
		int del(uint32_t msgid );
};

#endif   /* ----- #ifndef CQA_INC  ----- */
