/*
 * =====================================================================================
 *
 *       Filename:  Croominfo_sports_pet_info.h
 *
 *    Description:  deal in table roominfo_0.t_sports_pet_info_0
 *
 *        Version:  1.0
 *        Created:  05/10/2010 07:08:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

       
#ifndef  CROOMINFO_SPORTS_PET_INFO_INCL
#define  CROOMINFO_SPORTS_PET_INFO_INCL
#include <algorithm>
#include <cstring>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"
#include "CtableRoute10x10.h"
#include "proto.h"
#include "logproto.h"
#include "benchapi.h"

class Croominfo_sports_pet_info : CtableRoute10x10 
{
	protected:
		
	public:
		Croominfo_sports_pet_info(mysql_interface * db);

		int reward_flag_select(uint32_t userid, uint32_t petid, uint32_t *reward_flag);
		
		int reward_flag_update(uint32_t userid, uint32_t petid);
		
		int reward_flag_insert(uint32_t userid, uint32_t petid);
};


#endif

