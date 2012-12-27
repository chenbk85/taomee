/*
 * =====================================================================================
 * 
 *       Filename:  Croute_func_def.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年06月19日 10时38分56秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 * 
 *         Author:  xcwen (xcwen), xcwenn@gmail.com
 *        Company:  TAOMEE
 *
 *      ------------------------------------------------------------
 *      view configure:
 *          VIM:  set tabstop=4  
 * 
 * =====================================================================================
 */


#ifndef  CROUTE_FUNC_DEF_INC
#define  CROUTE_FUNC_DEF_INC

#include "Ccmdmap_private_checklen.h"
#include "Cfunc_route_base.h"

#define FILL_CMD_WITH_PRI_IN( TYPE , type  )  \
        { TYPE##_CMD,{MUST_EQ,sizeof( type##_in),#type ,&Croute_func::type }}   

#define FILL_CMD_WITHOUT_PRI_IN( TYPE,type  )  \
        { TYPE##_CMD,{MUST_EQ,0,#type,&Croute_func::type }} 

#define FILL_CMD_WITH_PRI_IN_GE( TYPE ,type )  \
        { TYPE##_CMD,{MUST_GE ,sizeof( type##_in_header) ,#type,&Croute_func::type }}   
/*
#define FILL_CMD_WITH_PRI_IN( TYPE )  \
		{ TYPE##_CMD,{MUST_EQ,sizeof( TYPE##_IN),#TYPE ,&Croute_func::f_##TYPE }}	

#define FILL_CMD_WITHOUT_PRI_IN( TYPE  )  \
		{ TYPE##_CMD,{MUST_EQ,0,#TYPE,&Croute_func::f_##TYPE }}	

#define FILL_CMD_WITH_PRI_IN_GE( TYPE )  \
		{ TYPE##_CMD,{MUST_GE ,sizeof( TYPE##_IN_HEADER) ,#TYPE,&Croute_func::f_##TYPE }}	
*/


//------------------定义与Ccmdmap相关的私有结构---begin
#define  DEAL_FUN_ARG char *recvbuf, char **sendbuf, int *sndlen 
class Croute_func;
//定义调用函数的指针类型
typedef   int(Croute_func::*P_DEALFUN_T)(DEAL_FUN_ARG );
typedef   struct pri_stru_check_len<P_DEALFUN_T > PRI_STRU  ;
typedef   Ccmdmap < PRI_STRU > 		CMD_MAP ;
//------------------定义与Ccmdmap相关的私有结构---end




#endif   /* ----- #ifndef CROUTE_FUNC_DEF_INC  ----- */

