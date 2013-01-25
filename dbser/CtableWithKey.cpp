/*
 * =====================================================================================
 *
 *       Filename:  CtableWithKey.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2007年11月06日 19时52分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */
#include "CtableWithKey.h"


CtableWithKey::CtableWithKey (mysql_interface * db,const char * dbname, 
	const	char * tablename,const char* id_name   ) 
	:Ctable(db,dbname,tablename )
{  
	memset(this->id_name,0 ,sizeof(this->id_name));			
	strncpy(this->id_name,id_name,sizeof(this->id_name)-1);
}

int CtableWithKey::id_is_existed(uint32_t id, bool* existed)
{
	sprintf (this->sqlstr,"select  1 from %s where %s=%d ", \
	this->id_name, this->id_name,id);
		*existed=false;
	STD_QUERY_ONE_BEGIN(this->sqlstr, SUCC);
		*existed=true;
	STD_QUERY_ONE_END();
}

