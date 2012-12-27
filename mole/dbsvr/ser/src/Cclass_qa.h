/*
 * =====================================================================================
 * 
 *       Filename:  Ccup.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  Cclass_qa_INCL
#define  Cclass_qa_INCL
#include "CtableRoute10x10.h"
#include "proto.h"
#include "benchapi.h"
#include "func_comm.h"
#define  STR_ROOM_PUGLIST "room_puglist"

struct  stru_class_qa{
	uint32_t score;
	uint32_t count;
	uint32_t logdate;
	stru_member_score_list member_list;
	stru_member_score_list member_score_list;
}__attribute__((packed));


class Cclass_qa : public CtableRoute10x10{
	protected:
	int init(userid_t classid);
	int get_db(userid_t classid, stru_class_qa *p_out);
	public:
	Cclass_qa(mysql_interface * db );
	int get(userid_t classid, stru_class_qa *p_out);
	int update(userid_t classid, stru_class_qa *p_list);

	int check_add(userid_t classid, roominfo_class_qa_check_add_in *p_in,
			roominfo_class_qa_check_add_out *p_out	);

	int get_score_info(userid_t classid, stru_class_score_info *p_out);

	int add_score(userid_t classid, roominfo_class_qa_add_score_in *p_in,roominfo_class_qa_add_score_out *p_out );
	
	int class_get_flag(uint32_t userid, stru_class_medal_flag *p_in);

	int class_update_flag(uint32_t userid, stru_class_medal_flag *p_in);

	int set_class_flag(userid_t userid, uint32_t classid, uint32_t index);
	};

#endif   /* ----- #ifndef Cclass_qa_INCL  ----- */

