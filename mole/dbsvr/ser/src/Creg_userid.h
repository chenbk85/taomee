/*
 * =====================================================================================
 *
 *       Filename:  Creg_userid.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/17/2010 11:06:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#ifndef  Creg_userid_INCL
#define  Creg_userid_INCL

/*
 * =====================================================================================
 *        Class:  Creg_userid
 *  Description:  
 * =====================================================================================
 */
#include "Ctable.h" 
class Creg_userid: public Ctable
{
	public:
		/* ====================  LIFECYCLE     ======================================= */
		Creg_userid(mysql_interface * db ); 
		int set_noused(uint32_t userid);
		int get(uint32_t *p_userid);

	protected:
		int get_db(uint32_t *p_userid);
		int set_used(uint32_t userid);

	private:

}; /* -----  end of class Creg_userid  ----- */

#endif
