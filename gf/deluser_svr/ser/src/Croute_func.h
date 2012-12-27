/*
 * =====================================================================================
 * 
 *       Filename:  dbdeal.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *        PRIu64
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CROUTE_FUNC_INC
#define  CROUTE_FUNC_INC

#include "Croute_func_def.h"
#include "Ctable.h"
#include "Cgf_role_deleted.h"
#include "Cgf_dbproxy.h"

#define __GF_DEBUG__

/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */




class Croute_func:public Cfunc_route_base < CMD_MAP >
{
  private:
	int ret;/* */
	mysql_interface *db;
	Cgf_role_deleted gf_role_deleted;
	Cgf_dbproxy gf_dbproxy;

  public:
	Croute_func (mysql_interface * db); 
	
	int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen );

	int gf_add_del_role(DEAL_FUN_ARG);

	int gf_del_allrole_overload(DEAL_FUN_ARG);
	
	int gf_del_role(DEAL_FUN_ARG);

//	int gf_del_allrole_overload(DEAL_FUN_ARG);



	/* Use compiler-generated copy constructor, assignment operator and destructor */
}; /* -----  end of class  Croute_func  ----- */
#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

