/*
 * =====================================================================================
 *
 *       Filename:  Cuser_together_photo.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/25/2012 05:34:47 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_TOGETHER_PHPTO_INCL
#define CUSER_TOGETHER_PHPTO_INCL

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"

class Cuser_together_photo : public CtableRoute100x10
{
	private:

	public:
		Cuser_together_photo(mysql_interface *db);
		int insert(userid_t userid, uint32_t type, user_photo_t * photo);
		int get_info(userid_t userid, uint32_t type, user_photo_t* photo);
		
};
#endif
