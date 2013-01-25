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

#ifndef  CFUNC_ROUTE_cmd_INC
#define  CFUNC_ROUTE_cmd_INC
#include "Ccmdmaplist.h"
#include  <assert.h>
#include "Ccmdmap_private_checklen.h"
#include "proto_header.h"
#include "db_error_base.h"

//------------------定义与Ccmdmap相关的私有结构---begin
#define  DEAL_FUN_ARG char *recvbuf, char **sendbuf, int *sndlen 
class Croute_func;
//定义调用函数的指针类型
typedef   int(Croute_func::*P_DEALFUN_T)(DEAL_FUN_ARG );
typedef   struct pri_stru_check_len<P_DEALFUN_T > PRI_STRU  ;
typedef   Ccmdmap < PRI_STRU > 		CMD_MAP ;

//填充命令
#define FILL_CMD_WITH_PRI_IN_EX(  type , cmd_max_limit )  \
		{ type##_cmd,{MUST_EQ,sizeof( type##_in),&Croute_func::type,{0,0,cmd_max_limit } }}	

#define FILL_CMD_WITHOUT_PRI_IN_EX( type, cmd_max_limit  )  \
		{ type##_cmd,{MUST_EQ,0,&Croute_func::type,{0,0,cmd_max_limit } }}	

#define FILL_CMD_WITH_PRI_IN_GE_EX( type,cmd_max_limit )  \
		{ type##_cmd,{MUST_GE ,sizeof( type##_in_header), &Croute_func::type,{0,0,cmd_max_limit } }}	

//填充命令
#define FILL_CMD_WITH_PRI_IN(  type  )   FILL_CMD_WITH_PRI_IN_EX(type, 0)

#define FILL_CMD_WITHOUT_PRI_IN( type  )    FILL_CMD_WITHOUT_PRI_IN_EX (type,0)

#define FILL_CMD_WITH_PRI_IN_GE( type )    FILL_CMD_WITH_PRI_IN_GE_EX(type,0)


#define SET_SNDBUF(private_len)  \
	if (!(set_std_return (sendbuf,sndlen,  (PROTO_HEADER*)recvbuf, SUCC, (private_len)))){ \
		DEBUG_LOG("set_std_return:private size err [%d]",int(private_len));\
		return SYS_ERR;\
	}

//没有私有返回数据时，可用 在Croute_func 的 相关处理函数中使用
#define STD_RETURN(ret) {\
	if (ret==SUCC){  \
		SET_SNDBUF(0);\
	}\
	return ret;\
}

//用于输入长度为变长的情况下，长度的验正
#define CHECK_PRI_IN_LEN_WITHOUT_HEADER(pri_len) {\
	if ( PRI_IN_LEN != (sizeof(*p_in)+(pri_len)) ){\
		DEBUG_LOG("check size err [%u][%u]", uint32_t (PRI_IN_LEN) ,\
				uint32_t (sizeof(*p_in)+(pri_len))	);\
		return PROTO_LEN_ERR; \
	}	\
}

//用于输入长度为变长的情况下，长度的验正
#define CHECK_PRI_IN_LEN_WITHOUT_HEADER_EX(pri_len, max_len){\
	if ( PRI_IN_LEN>(sizeof(*p_in)+max_len) || PRI_IN_LEN != (sizeof(*p_in)+(pri_len))   ){\
		DEBUG_LOG("check size err  in[%u] need [%u] max [%d] ", uint32_t (PRI_IN_LEN) ,\
				uint32_t ((sizeof(*p_in)+(pri_len))) , max_len	);\
		return PROTO_LEN_ERR; \
	}	\
}



#define STD_RETURN_WITH_BUF(ret,_buf,_count ) { \
	if (ret==SUCC){ \
		SET_SNDBUF( (_count) ); \
		memcpy(SNDBUF_PRIVATE_POS, (char*)(_buf) , (_count)  ); \
	}\
	return ret; \
}

#define STD_RETURN_WITH_TWO_BUF(ret,_buf1,_buf1_len,_buf2,_buf2_len) { \
	if (ret==SUCC){ \
		SET_SNDBUF( (_buf1_len)+(_buf2_len) ); \
		memcpy(SNDBUF_PRIVATE_POS, (char*)(_buf1) , (_buf1_len)  ); \
		memcpy(SNDBUF_PRIVATE_POS+_buf1_len, (char*)(_buf2) , (_buf2_len)  ); \
	}\
	return ret; \
}


//在有私有数据时采用， stru 为 返回的私有结构体。
#define STD_RETURN_WITH_STRUCT_AND_BUF(ret,stru,_buf,_buf_len ) STD_RETURN_WITH_TWO_BUF (ret,((char*)&stru), sizeof(stru),_buf,_buf_len  )



//在有私有数据时采用， stru 为 返回的私有结构体。
#define STD_RETURN_WITH_STRUCT(ret,stru ) STD_RETURN_WITH_BUF (ret,((char*)&stru), sizeof(stru)  )



//如果列表malloc
#ifndef DB_GET_LIST_NO_MALLOC  
//在有返回列表 时采用， 
//ret:返回值  
//stru_header:结构体头部
//pstru_item:列表起始指针
//itemcount: 个数
//注意事项：pstru_item所指向的空间应是molloc得到的,:free(pstru_item);
#define STD_RETURN_WITH_STRUCT_LIST(ret,stru_header,pstru_item ,itemcount) \
	if (ret==DB_SUCC){ \
		uint32_t stru_header_size=sizeof(stru_header );\
		uint32_t out_list_size= sizeof(*pstru_item ) * (itemcount);\
		SET_SNDBUF(stru_header_size + out_list_size);\
		memcpy(SNDBUF_PRIVATE_POS,&stru_header,stru_header_size);\
		memcpy(SNDBUF_PRIVATE_POS+stru_header_size,pstru_item, out_list_size );\
		free(pstru_item);\
		return SUCC;\
	} else {\
		return ret;\
	} 


#else

#define STD_RETURN_WITH_STRUCT_LIST(ret,stru_header,pstru_item ,itemcount) \
	if (ret==DB_SUCC){ \
		uint32_t stru_header_size=sizeof(stru_header );\
		uint32_t out_list_size= sizeof(*pstru_item ) * (itemcount);\
		SET_SNDBUF(stru_header_size + out_list_size);\
		memcpy(SNDBUF_PRIVATE_POS,&stru_header,stru_header_size);\
		memcpy(SNDBUF_PRIVATE_POS+stru_header_size,pstru_item, out_list_size );\
		return SUCC;\
	} else {\
		return ret;\
	} 
#endif



/*
 * =====================================================================================
 *        Class:  Cfunc_route_base
 *  Description:  
 * =====================================================================================
 */

class Cfunc_route_cmd
{
protected: 
    int ret;/*用于保存操作返回值，只是为了方便 */

	Ccmdmaplist<CMD_MAP>  cmdmaplist;		
	/**
	 * 功能:从deal_fun 统一初始化
	 * count:数组中元素的个数.
	 */
	inline void initlist(CMD_MAP * deal_fun, int count ){
		for (int i=0;i<count;i++ ){
			assert( this->cmdmaplist.v_cmdmap[ (*(deal_fun+i)).cmd_id].cmd_id==0 );
			this->cmdmaplist.v_cmdmap[ (*(deal_fun+i)).cmd_id]=*(deal_fun+i);
		}
	}
public:
	inline Cfunc_route_cmd() {
	}

	inline bool set_cmd_max_limit_per_minute(uint16_t cmdid ,uint32_t max_limit )
	{
		PRI_STRU * p_pri_stru;
		if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
			p_pri_stru->exec_cmd_limit.limit_max_count_per_min=max_limit;
			return true;
		}else{
			return false;
		}
	}

	virtual  int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		PRI_STRU * p_pri_stru;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;
		if((p_pri_stru =this->cmdmaplist.getitem(cmdid))!=NULL){
			DEBUG_LOG("I:%04X:%d", cmdid, userid );
			//检查协议频率
			if (! p_pri_stru->exec_cmd_limit.add_count() ){
				DEBUG_LOG("cmd max err:cmdid %u, max_count:%u ",
					cmdid,p_pri_stru->exec_cmd_limit.limit_max_count_per_min);
			
				return CMD_EXEC_MAX_PER_MINUTE_ERR;
			}

			//检查报文长度
			if (! p_pri_stru->check_proto_size(rcvlen - PROTO_HEADER_SIZE) ){
				DEBUG_LOG("len err pre [%u] send [%d]",
				uint32_t (p_pri_stru->predefine_len+PROTO_HEADER_SIZE) ,rcvlen );
				return PROTO_LEN_ERR;
			}
	
			this->ret=9999;
			//调用相关DB处理函数
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
			return this->ret;
		}else{
			DEBUG_LOG("cmd no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}
	virtual ~Cfunc_route_cmd (){ }
	
}; /* -----  end of class  Cfunc_route_base  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_cmd_INC  ----- */

