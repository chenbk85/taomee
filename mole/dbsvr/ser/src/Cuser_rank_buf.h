/*
 * =====================================================================================
 *
 *       Filename:  Cuser_rank_buf.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/06/2010 04:37:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  benjamin (zhangbiao), benjamin@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_RANK_BUF_INCL
#define CUSER_RANK_BUF_INCL
    
#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <algorithm>


inline int
cmp_date_record( const date_id &p1, const date_id &p2)
{    
	return p1.date > p2.date;   
}       

class Cuser_rank_buf: public CtableRoute100x10 {
public:
    Cuser_rank_buf(mysql_interface * db);
    int insert(userid_t userid,uint32_t type, users_rank_buf_in *p_in);
	int update_rank_buf(userid_t userid,uint32_t type, userid_t other);
	int get_rank_buf(userid_t userid ,uint32_t type, user_rank_buf_out* p_out);
	int clear_buf(userid_t userid, uint32_t type);
	int get_date_record(userid_t userid, uint32_t type,uint32_t *date);
};
#endif


 
