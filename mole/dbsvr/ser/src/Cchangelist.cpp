/*
 * =====================================================================================
 *
 *       Filename:  Cchangelist.cpp
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
#include "Cchangelist.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "proto.h"
#include "benchapi.h"
Cchangelist::Cchangelist(mysql_interface * db,Cclientproto *cp  ) :
	Ctable( db, "SU_CHANGE_DB","t_changelist" )
{ 
	this->cp=cp;
}

int Cchangelist::insert( su_changelist_add_in  *p_in ) 
{
	char date_str[30];
	mysql_date( date_str, time(NULL),sizeof(date_str));
	sprintf(this->sqlstr, "insert into %s values ('%s',%u,%u,%u,%u,%d,%d,%d )", 
			this->get_table_name(), date_str,
				p_in->type,
				p_in->cmdid ,
				p_in->adminid,
				p_in->userid,
				p_in->v1,
				p_in->v2,
				p_in->v3
			);
	STD_INSERT_RETURN(this->sqlstr, EMAIL_EXISTED_ERR);	
}

int Cchangelist::change_user_value(  su_change_in *p_in )
{

	user_change_user_value_in user_value;
	memset(&user_value,0,sizeof(user_value ) );
	switch (p_in->attrid){
		case ATTR_EXP :
			user_value.addexp=p_in->changvalue;
			break;
		case ATTR_STRONG:
			user_value.addstrong=p_in->changvalue;
			break;
		case ATTR_IQ:
			user_value.addiq=p_in->changvalue;
			break;
		case ATTR_CHARM:
			user_value.addcharm=p_in->changvalue;
			break;
		case ATTR_XIAOMEE:
			user_value.addxiaomee=p_in->changvalue;
			if (user_value.addxiaomee>0)
				user_value.reason=XIAOMEE_USE_ADD_SU;
			else 
				user_value.reason=XIAOMEE_USE_DEL_SU;
			break;
		default :
			return  ENUM_OUT_OF_RANGE_ERR;
		break;
	}
 	return this->cp->f_user_change_user_value(p_in->userid, &user_value   );
}

int Cchangelist::changevalue( userid_t adminid, su_change_in *p_in )
{
	int ret;
	uint32_t attrid=p_in->attrid; 	
	if (attrid/1000==1){
			ret=this->change_user_value( p_in);
	}else{
		return  ENUM_OUT_OF_RANGE_ERR;
	}
	return ret;
}
