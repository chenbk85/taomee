#ifndef  GF_CROUTE_FUNC_H
#define  GF_CROUTE_FUNC_H

#include <libtaomee++/proto/proto_base.h>
#include "kf/vip_config_data_mgr.hpp"
#include "Citem_change_log.h"


#include "Cfunc_route_base.h"
#include "Ctable.h"
#include "Cgf_mail.h"

#define  BIND_PROTO_CMD(cmdid,cmd_name,struct_in,struct_out,md5_tag,bind_bitmap ) \
	{cmdid, new struct_in(),new struct_out (), md5_tag,&Croute_func::cmd_name },	
#define  P_OUT dynamic_cast<typeof p_out>(c_out);
#define  P_IN dynamic_cast<typeof p_in>(c_in);
#define  DEAL_FUNC_ARG  userid_t userid, Cmessage * c_in, Cmessage * c_out 

class Croute_func;
//定义调用函数的指针类型

typedef   int(Croute_func::*P_DEALFUN_NEW_T)(DEAL_FUNC_ARG);

typedef   struct stru_cmd_item{
	uint16_t cmdid;
	Cmessage * msg_in; 
	Cmessage * msg_out; 
	uint32_t  md5_tag;
	P_DEALFUN_NEW_T p_deal_func; 
} stru_cmd_item_t;







/*
 * =====================================================================================
 *        Class:  Croute_func
 *  Description:  
 * =====================================================================================
 */




class Croute_func:public Cfunc_route_base 
{
private:
	stru_cmd_item_t *p_cmd_list;//命令行列表
	byte_array_t ba_in;
	byte_array_t ba_out;
	/**
	 * 功能:从deal_fun 统一初始化
	 * count:数组中元素的个数.
	 */
	inline void set_cmd_list(stru_cmd_item_t * p_list, int count ){
		for (int i=0;i<count;i++ ){
			uint16_t cmdid=p_list[i].cmdid;
			assert( this->p_cmd_list[cmdid].cmdid==0  );
			this->p_cmd_list[cmdid]=p_list[i];
		}
	}
	inline stru_cmd_item_t * get_cmd_item(uint16_t cmdid){
		if (this->p_cmd_list[cmdid].cmdid!=0 ) return & (this->p_cmd_list[cmdid]);
		else return NULL;
	}


private:
	int do_sync_data(uint32_t userid, uint16_t cmdid );
public:
	Croute_func (mysql_interface * db); 
    ~Croute_func ();

	virtual int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
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
			if (! p_pri_stru->check_proto_size(rcvlen - (PROTO_HEADER_SIZE)) ){
				DEBUG_LOG("len err pre [%lu] send [%d]",
				p_pri_stru->predefine_len+(PROTO_HEADER_SIZE) ,rcvlen );
				return PROTO_LEN_ERR;
			}
	
			//处理在线分裂数据
			if (this->sync_user_data_flag ){//是否打开同步标志
				ret=this->do_sync_data(userid,cmdid);
				if (ret!=SUCC) return ret;
			}
	
			this->ret=9999;
			//调用相关DB处理函数
			*sendbuf=NULL;
			*sndlen=0;
			this->ret=(((Croute_func*)this)->*(p_pri_stru->p_deal_fun))(recvbuf, sendbuf, sndlen );	

			//提交数据
			if (mysql_commit(&(this->db->handle))!=DB_SUCC){
				this->db->show_error_log("db:COMMIT:err");
			}
	
			return this->ret;
		}else{
			//调用新的分
			return  this->deal_new(recvbuf, rcvlen, sendbuf, sndlen );
		}
	}

	virtual int deal_func(uint16_t cmdid, userid_t userid, Cmessage * c_in, Cmessage * c_out ,P_DEALFUN_NEW_T p_func  )
	{
		//处理在线分裂数据
		if (this->sync_user_data_flag ){//是否打开同步标志
			ret=this->do_sync_data(userid,cmdid);
			if (ret!=SUCC) return ret;
		}

		//调用相关DB处理函数
		this->ret=9999;
		this->ret=(((Croute_func*)this)->*(p_func))(
				userid,c_in ,c_out);	

		//提交数据
		if (mysql_commit(&(this->db->handle))!=DB_SUCC){
			this->db->show_error_log("db:COMMIT:err");
		}
		return this->ret;


	}



	int deal_new(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		stru_cmd_item_t * p_cmd_item;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;

		if((p_cmd_item =this->get_cmd_item(cmdid))!=NULL){
			KDEBUG_LOG ( userid ,"I:%04X", cmdid );
			//检查md5值,md5_tags 放在上送报文的result中
			uint32_t md5_value=((PROTO_HEADER*)recvbuf)->result;
			if (md5_value!=0 ){//==0,表示不用md5_tags检查
				if (p_cmd_item->md5_tag!=md5_value){
					KDEBUG_LOG(userid,"CMD_MD5_TAG_ERR:DB:%u,client:%u",
							p_cmd_item->md5_tag ,md5_value);
					return  CMD_MD5_TAG_ERR;
				}
			}


			//私有报文处理
			this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS-4,
						RECVLEN_FROM_RECVBUF-(PROTO_HEADER_SIZE-4) );
            
		
			p_cmd_item->msg_in->init();
			p_cmd_item->msg_out->init();
			//还原对象失败
			if (!p_cmd_item->msg_in->read_from_buf( this->ba_in)) {
				DEBUG_LOG("还原对象失败");
				return PROTO_LEN_ERR;	
			}
			//数据没有取完
			if (!this->ba_in.is_end()) {
				DEBUG_LOG("client 过多报文");
				return  PROTO_LEN_ERR;
			}

			//请求报文还原完毕,调用相关函数。

			this->ret=this->deal_func(cmdid,userid,
					p_cmd_item->msg_in,p_cmd_item->msg_out ,
					p_cmd_item->p_deal_func );

			if (this->ret==SUCC){//需要构造返回报文
				this->ba_out.init_postion();
				if (!p_cmd_item->msg_out->write_to_buf(this->ba_out)) {
					DEBUG_LOG("p_cmd_item->msg_out->write_to_buf :SYS_ERR" );
					return SYS_ERR;
				}

				if (!set_std_return( sendbuf,  sndlen,(PROTO_HEADER*)recvbuf , 
							SUCC,this->ba_out.get_postion() , PROTO_HEADER_SIZE-4 )) 
				{
					DEBUG_LOG("set_std_return:SYS_ERR: %u",
							this->ba_out.get_postion() );
					return SYS_ERR; 
				}
				memcpy(SNDBUF_PRIVATE_POS-4,this->ba_out.get_buf(),
						this->ba_out.get_postion()  );
				return SUCC;
			}else{//返回错误
				return this->ret;
			}
		}else{
			DEBUG_LOG("cmd no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}
	
public:
	Cgf_mail      gf_mail;

public:	
	int  gf_mail_head_list(DEAL_FUN_ARG);
	int  gf_mail_body(DEAL_FUN_ARG);
	int  gf_delete_mail(DEAL_FUN_ARG);
	int  gf_take_mail_enclosure(DEAL_FUN_ARG);
	int  gf_send_mail(DEAL_FUN_ARG);
	int  gf_reduce_money(DEAL_FUN_ARG);
	int  gf_send_system_mail(DEAL_FUN_ARG);

}; /* -----  end of class  Croute_func  ----- */
#endif   /* ----- #ifndef CROUTE_FUNC_INC  ----- */

