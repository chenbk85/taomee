/*
 * =====================================================================================
 *
 *       Filename:  Csync_user.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2008年01月14日 16时54分15秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 		
 * 		------------------------------------------------------------
 * 		view configure:
 * 			VIM:  set tabstop=4 
 * 		
 * =====================================================================================
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "Csync_user.h"
#include "common.h"
#include "benchapi.h"
#include <errno.h> 

Csync_user::Csync_user (int msg_list_key)
{
	this->qid = msgget( msg_list_key , IPC_CREAT | 0660 );
}  /* -----  end of method Csync_user::Csync_user  (constructor)  ----- */


int Csync_user::send(char  * buf ,int len  )
{
	int ret;
	if((ret = msgsnd( qid,  buf , len-sizeof(long) ,IPC_NOWAIT )) == -1)
	{
		DEBUG_LOG("  send  msg  err %d  [%d:%m] "  ,ret,errno );
		return FAIL;
	}
	return SUCC;

}
/*
int Csync_user::revf( TUSER_FLAG* uf )
{
	int ret;
	if((ret = msgrcv( qid,uf, sizeof (TUSER_FLAG) ,0 , IPC_NOWAIT )) == -1)
	{
		return FAIL; 
	}
	return SUCC;
}
*/
