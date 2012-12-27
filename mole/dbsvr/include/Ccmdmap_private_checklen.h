/*
 * =====================================================================================
 * 
 *       Filename:  Ccmdmap_checklen.h
 * 
 *    Description:  
 * 
 *        Version:  1.0
 *        Created:  2008年01月10日 17时59分46秒 CST
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

#ifndef  CCMDMAP_CHECKLEN_INC
#define  CCMDMAP_CHECKLEN_INC

#include <stdint.h>
//说明:
//用于Ccmdmap 结构的私有数据结构.
// _Tp:推荐定义为函数指针
// 提供比较数值的能力.
//检查报文长度方式
enum check_len_type  {MUST_EQ=0/*==*/,MUST_GE=1/*>=*/};

template <class _Tp >
struct pri_stru_check_len {
	//比较方式  
	enum 		check_len_type check_type;
	//预定义长度      
	uint32_t	predefine_len;	
	char		desc_msg[65];	
	//调用的函数指针 
	_Tp p_deal_fun;	
	//检查报文长度是否合法
	inline bool check_proto_size( uint32_t len ){
		if (check_type==MUST_EQ ) {
			if(len==predefine_len )	 return true;
			else return false; 
		} else if (check_type==MUST_GE ) {
			if(len>=predefine_len)	 return true;
			else return false; 
		}else {//类型未定义
			return false;
		}
	}
};



#endif   /* ----- #ifndef CCMDMAP_CHECKLEN_INC  ----- */

