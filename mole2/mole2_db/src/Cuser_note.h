/*
 * =====================================================================================
 *
 *       Filename:  Cuser_note.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010-07-21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */



#ifndef  CUSER_NOTE_INC
#define  CUSER_NOTE_INC

#include "CtableRoute100x10.h"
#include "proto.h"
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

#include <time.h>
#include <algorithm>




class Cuser_note : public CtableRoute100x10
{

public:
	Cuser_note (mysql_interface * db);

	int insert(uint32_t userid, char* job_dream, char* brave_dream);

	int brave_dream_set(uint32_t userid, char* job_dream, char* brave_dream);



protected:


};


#endif
