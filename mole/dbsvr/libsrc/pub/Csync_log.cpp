/*
 * =====================================================================================
 *
 *       Filename:  Csync_log.cpp
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
#include "Csync_log.h"
#include "common.h"
#include "benchapi.h"



Csync_log::Csync_log (int msg_list_key)
{
	this->qid = msgget( msg_list_key , IPC_CREAT | 0777);
}  /* -----  end of method Csync_log::Csync_log  (constructor)  ----- */

int Csync_log::send(char  * buf ,int len  )
{
	int ret;
	if((ret = msgsnd( qid,  buf , len, IPC_NOWAIT)) == -1)
	{
		return FAIL;
	}
	return SUCC;
}

//
//int Csync_log::revf(char * uf )
//{
//	int ret;
//	if((ret = msgrcv( qid,uf, sizeof (TUSER_FLAG) ,0 , IPC_NOWAIT )) == -1)
//	{
//		return FAIL; 
//	}
//	return SUCC;
//}
