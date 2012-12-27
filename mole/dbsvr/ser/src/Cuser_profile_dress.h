/*
 * =====================================================================================
 *
 *       Filename:  Cuser_profile_dress.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/01/2011 11:19:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef CUSER_PROFILE_DRESS_INC
#define CUSER_PROFILE_DRESS_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include <map> 

class Cuser_profile_dress:public CtableRoute100x10
{
	private:
	
	public:
	Cuser_profile_dress(mysql_interface *db);
	int insert(userid_t userid, profile_dress_t *profile);
	int get_profiles(userid_t userid, profiles_t **profiles, uint32_t *p_count);
	int get_one_profile(userid_t userid, uint32_t index, profile_dress_t *profile);
	int update_profile(userid_t userid, uint32_t index, profile_dress_t *profile);
	int get_count(userid_t userid, uint32_t *count);
	int remove(userid_t userid, uint32_t index);
};
#endif
