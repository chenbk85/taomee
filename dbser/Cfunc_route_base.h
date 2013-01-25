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

#ifndef  CFUNC_ROUTE_BASE_INC
#define  CFUNC_ROUTE_BASE_INC
#include "mysql_iface.h"
#include "Csync_user_data.h"
#include "Cfunc_route_cmd.h"
/*
 * =====================================================================================
 *        Class:  Cfunc_route_base
 *  Description:  
 * =====================================================================================
 */

class Cfunc_route_base:public Cfunc_route_cmd
{
protected: 
    bool sync_user_data_flag;/*当前是否处于同步数据状态 */
    int ret;/*用于保存操作返回值，只是为了方便 */
	//db 连接
	mysql_interface *db;
	//在线同步
	Csync_user_data sync_user_data;
public:
	inline Cfunc_route_base(mysql_interface * db ):
		db(db), sync_user_data(db,
			config_get_strval("SYNC_USER_DATA_DB_OLD_IP"),
			config_get_intval("SYNC_USER_DATA_DB_OLD_PORT",0) )
	{
		this->db=db;
		this->sync_user_data_flag=config_get_intval("SYNC_USER_DATA_FLAG",0);
	}


	inline  virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
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
				uint32_t (p_pri_stru->predefine_len+PROTO_HEADER_SIZE ),rcvlen );
				return PROTO_LEN_ERR;
			}
	
			//处理在线分裂数据
			if (this->sync_user_data_flag ){//是否打开同步标志
				ret=this->do_sync_data(userid,cmdid);
				if (ret!=SUCC) return ret;
			}
	
			this->ret=9999;
			//调用相关DB处理函数
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	
			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
	
			return this->ret;
		}else{
			DEBUG_LOG("DB CMD no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}

	virtual int  do_sync_data(uint32_t userid, uint16_t cmdid) { return SUCC; }
	virtual ~Cfunc_route_base (){ }
	
}; /* -----  end of class  Cfunc_route_base  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_BASE_INC  ----- */

