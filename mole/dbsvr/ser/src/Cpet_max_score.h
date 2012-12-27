/*
 * =====================================================================================
 *
 *       Filename:  Cpet_max_score.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月23日 13时06分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  CPET_MAX_SCORE_INCL
#define  CPET_MAX_SCORE_INCL

#include "Ctable.h"
#include "proto.h"
class Cpet_max_score: public Ctable{
	protected:

		int set_score_db( uint32_t type  , uint32_t score , 
		 uint32_t groupid, char * user_nick,  char * pet_nick );
	public:
		int set_score( uint32_t type  , uint32_t score , 
		 uint32_t groupid, char * user_nick,  char * pet_nick );

		int get_score( uint32_t type, temp_pet_sports_get_max_score_out * p_out );
		int get_score( uint32_t type, uint32_t * p_score);
		Cpet_max_score(mysql_interface * db );
};
#endif
