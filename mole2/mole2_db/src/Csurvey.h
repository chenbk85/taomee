
/*
 * =====================================================================================
 * 
 *       Filename:  Cpet.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分58秒 CST
 *       Revision:  none
 *       Compiler:  g++
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CSURVEY_INC
#define  CSURVEY_INC

#include "Ctable.h"
#include "proto.h"
#include "benchapi.h"
#include <time.h>
#include "benchapi.h"
#include "db_error.h"
#include "Ccommon.h"

class Csurvey : public Ctable
{
public:
	Csurvey(mysql_interface * db);

	int update(uint32_t surveyid,stru_survey_reply &reply);
	int get_survey(uint32_t surveyid,std::vector<stru_survey_reply_new> &reply);
};

#endif /* ----- #ifndef CSURVEY_INC  ----- */

