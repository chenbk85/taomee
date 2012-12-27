/*
 * =====================================================================================
 * 
 *       Filename:  Cusermsg.h
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

#ifndef  CUSERMSG_INCL
#define  CUSERMSG_INCL
#include "Ctable.h"
#include "proto.h"
class Cusermsg : public Ctable{
	protected:
		int get_accomplishment_by_medals(uint32_t medals, uint32_t &accomplishment);
	public:
		Cusermsg(mysql_interface * db ); 
		int insert(  uint32_t userid, uint32_t objuserid, uint32_t reason , uint32_t medal);

		int get_by_date(userid_t objuserid ,
			usermsg_get_report_by_date_in *p_in, uint32_t * p_count,  
			usermsg_get_report_by_date_out_item ** pp_item ) ;
		int get_by_date_userid( userid_t objuserid ,
			usermsg_get_report_by_date_userid_in *p_in, uint32_t * p_count,  
			usermsg_get_report_by_date_userid_out_item ** pp_item ) ;

		int get_reportid_by_date_userid(userid_t objuserid , 
		usermsg_clear_by_date_userid_in *p_in, uint32_t * p_count,  
			stru_id ** pp_item ) ;
		int clear_by_date_userid(userid_t objuserid , usermsg_clear_by_date_userid_in *p_in );
		int get_tip_offs_by_date(su_get_usermsg_mushroom_guide_in *p_in, su_get_usermsg_mushroom_guide_out_item **pp_list,
			   uint32_t	*p_count);
		int get_count1(su_get_usermsg_mushroom_guide_in *p_in, uint32_t* count);
		int remove(usermsg_mushroom_guide_del_in *p_in );
		int drop_one_record(su_user_drop_super_mushroom_record_in *p_in);
		int get_del_count(usermsg_mushroom_guide_del_in *p_in, uint32_t *count);
};
#endif   /* ----- #ifndef CUSERMSG_INCL  ----- */

