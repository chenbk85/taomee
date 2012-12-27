/*
 * =====================================================================================
 * 
 *       Filename:  func_route_base.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2007年11月02日 16时40分57秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 * 
 * =====================================================================================
 */

#ifndef  CFUNC_ROUTE_CHECKLEN_INC
#define  CFUNC_ROUTE_CHECKLEN_INC

#define  DEAL_FUN_ARG char *recvbuf, char **sendbuf, int *sndlen 

#include "Cfunc_route_base.h"
#include "Ccmdmap_private_checklen.h"

//template <class _Tp > 
//typedef   int(_Tp::*P_DEALFUN_T)(DEAL_FUN_ARG );

template <class _Tp > 
	struct pri_stru_check_len< int(_Tp::*)(DEAL_FUN_ARG ) > ;

template <class _Tp ,class _deal_Tp > 
class Cfunc_route_checklen :public Cfunc_route_base<_Tp> 
{
 	int	deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen ){
	    _Tp * p_pri_stru;
	    uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
	    //int ret;
	    if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
	        DEBUG_LOG("--[%X][%s][%d]",
	            cmdid,p_pri_stru->desc_msg ,((PROTO_HEADER*)recvbuf)->id);
	
	        //检查报文长度
	        if (! p_pri_stru->check_proto_size(rcvlen - PROTO_HEADER_SIZE) ){
	            DEBUG_LOG("len err pre [%d] send [%d]",
	            p_pri_stru->predefine_len+PROTO_HEADER_SIZE ,rcvlen );
	            return PROTO_LEN_ERR;
	        }
			return ( this->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );
		}


	}

}

#endif   /* ----- #ifndef CFUNC_ROUTE_CHECKLEN_INC  ----- */

