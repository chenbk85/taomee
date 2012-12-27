/*
 * =====================================================================================
 * 
 *       Filename:  Cuser_pic_book.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年12月22日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  zheng, zheng@taomee.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CUSER_PIC_BOOK_INCL
#define  CUSER_PIC_BOOK_INCL
#include "CtableRoute.h"
#include "proto.h"
#include "benchapi.h"


class Cuser_pic_book: public CtableRoute {
public:
	Cuser_pic_book(mysql_interface * db); 

	int insert(userid_t userid, uint32_t attireid); 

	int get(userid_t userid, user_pic_book_get_out_item **pp_list, uint32_t *p_count);

};

#endif   /* ----- #ifndef CUSER_CARD_INCL  ----- */

