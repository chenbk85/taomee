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

#ifndef  CFUNC_ROUTE_cmd_v2_INC
#define  CFUNC_ROUTE_cmd_v2_INC

#include  <assert.h>
extern "C" {
#include  <libtaomee/log.h>
}
#include  "proto_header.h"
#include  "libtaomee++/proto/proto_base.h"
#include  "db_error_base.h"
#include  <set>
#define  BIND_PROTO_CMD(cmdid,cmd_name,struct_in,struct_out,md5_tag,bind_bitmap ) \
	{cmdid, new struct_in(),new struct_out (), md5_tag,&Croute_func::cmd_name },	
#define  P_OUT dynamic_cast<typeof p_out>(c_out);
#define  P_IN dynamic_cast<typeof p_in>(c_in);
#define  DEAL_FUNC_ARG  userid_t userid, Cmessage * c_in, Cmessage * c_out 


//------------------定义与Ccmdmap相关的私有结构---begin
class Croute_func;
//定义调用函数的指针类型
typedef   int(Croute_func::*P_DEALFUN_T)(DEAL_FUNC_ARG);
typedef   struct stru_cmd_item{
	uint16_t cmdid;
	Cmessage * msg_in; 
	Cmessage * msg_out; 
	uint32_t  md5_tag;
	P_DEALFUN_T p_deal_func; 
} stru_cmd_item_t;

class collect_cmd_item_t{
	public:
	uint16_t cmdid;
	int32_t result;
	Cmessage * msg; 
};

class collect_proto_t{
	public:
	uint32_t collect_seq;//原样返回
	int32_t result; //错误码
	uint16_t err_cmdid;//出错的命令号
	std::vector <collect_cmd_item_t>	 collect_cmd_list;
};

class collect_proto_header_t{
	public:
	uint32_t collect_seq;//原样返回
	int32_t result; //错误码
	uint16_t err_cmdid;//出错的命令号
} __attribute__((packed)) ;






/*
 * =====================================================================================
 *        Class:  Cfunc_route_base
 *  Description:  
 * =====================================================================================
 */

class Cfunc_route_cmd_v2
{
private:
	stru_cmd_item_t *p_cmd_list;//命令行列表
protected: 
    bool is_check_proto_md5;
    int ret;/*用于保存操作返回值，只是为了方便 */
	byte_array_t ba_in;
	byte_array_t ba_out;
 	collect_proto_t collect_proto;

	//哪些命令是聚合命令
	std::set<uint16_t> collect_cmd_set;        
	

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
	inline void add_collect_cmd(uint16_t cmdid){
		this->collect_cmd_set.insert(cmdid);
	}

public:
	inline Cfunc_route_cmd_v2() {
		//初始化命令列表
		 uint32_t malloc_size=sizeof(stru_cmd_item_t )*64*1024;
 		 this->is_check_proto_md5=false;
		 this->p_cmd_list=(stru_cmd_item_t* ) malloc (malloc_size );
		 memset(this->p_cmd_list,0, malloc_size);
	}

	inline void set_is_check_proto_md5(bool value )	{
 		 this->is_check_proto_md5=value;
	}

	int do_collect_cmd_list (char *recvbuf, int rcvlen, char **sendbuf, int *sndlen  )
	{
		/*
	uint32_t collect_seq;//原样返回
	int32_t result; //错误码
	uint16_t err_cmdid;//出错的命令号
	std::vector <collect_cmd_item_t>	 collect_cmd_list;
	*/	
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;

		static byte_array_t ba;

		//私有报文处理
		this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS,
					RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE );
	
		if( ! this->ba_in.read_uint32(this->collect_proto.collect_seq ) ) return PROTO_LEN_ERR; 
		if( ! this->ba_in.read_int32(this->collect_proto.result) ) return PROTO_LEN_ERR; 
		if( ! this->ba_in.read_uint16(this->collect_proto.err_cmdid) ) return PROTO_LEN_ERR; 

		uint32_t cmd_count;

		stru_cmd_item_t * p_cmd_item;
		collect_cmd_item_t item;
		if( ! this->ba_in.read_uint32(cmd_count) ) return PROTO_LEN_ERR; 

		this->ba_out.init_postion();
		if( ! this->ba_out.write_uint32(this->collect_proto.collect_seq ) ) return SYS_ERR;
		if( ! this->ba_out.write_int32(0) ) return SYS_ERR; 
		if( ! this->ba_out.write_uint16(0) ) return SYS_ERR; 
		if( ! this->ba_out.write_uint32(cmd_count) ) return SYS_ERR; 

		for(uint32_t i=0 ; i<cmd_count ; i++ ){
			if( ! this->ba_in.read_uint16(item.cmdid) ) return PROTO_LEN_ERR; 
			if( ! this->ba_in.read_int32(item.result) ) return PROTO_LEN_ERR; 
			
			if((p_cmd_item =this->get_cmd_item(item.cmdid ))!=NULL){
				KDEBUG_LOG ( userid ,"I:%04X",  item.cmdid );
				if (this->is_check_proto_md5 ){
					//检查md5值,md5_tags 放在上送报文的result中
					uint32_t md5_value=((PROTO_HEADER*)recvbuf)->result;
					if (md5_value!=0 ){//==0,表示不用md5_tags检查
						if (p_cmd_item->md5_tag!=md5_value){
							KDEBUG_LOG(userid,"CMD_MD5_TAG_ERR:DB:%u,client:%u",
									p_cmd_item->md5_tag ,md5_value);
							return  CMD_MD5_TAG_ERR;
						}
					}
				}	


				//私有报文处理
				this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS,
							RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE );
			
				p_cmd_item->msg_in->init();
				p_cmd_item->msg_out->init();
				//还原对象失败
				if (!p_cmd_item->msg_in->read_from_buf( this->ba_in)) {
					DEBUG_LOG("还原对象失败");
					return PROTO_LEN_ERR;	
				}
				//请求报文还原完毕,调用相关函数。

				this->ret=this->deal_func(cmdid,userid,
						p_cmd_item->msg_in,p_cmd_item->msg_out ,
						p_cmd_item->p_deal_func,false );

				if (this->ret==SUCC){//需要构造返回报文

					if (!this->ba_out.write_uint16(item.cmdid)) return SYS_ERR;
					if (!this->ba_out.write_int32(0)) return SYS_ERR;

					if (!p_cmd_item->msg_out->write_to_buf(this->ba_out)) {
						DEBUG_LOG("p_cmd_item->msg_out->write_to_buf :SYS_ERR" );
						return SYS_ERR;
					}
				}else{//返回错误
						
					this->do_rollback();
					collect_proto_header_t	 cph;
					cph.collect_seq=this->collect_proto.collect_seq;
					cph.err_cmdid=item.cmdid;
					cph.result=this->ret;

					if (!(set_std_return (sendbuf,sndlen,  
									(PROTO_HEADER*)recvbuf, SUCC, 
									sizeof(cph)+ PROTO_HEADER_SIZE ))){ 
							return SYS_ERR;
					}
					memcpy(SNDBUF_PRIVATE_POS, (char*)(&cph) , (sizeof(cph) )  ); 
					return SUCC;
				}
			}else{
				DEBUG_LOG("cmd no define  cmdid[%04X]",cmdid );
				return  CMDID_NODEFINE_ERR;
			}

		}//for

		//数据没有取完
		if (!this->ba_in.is_end()) {
			DEBUG_LOG("client 过多报文");
			return  PROTO_LEN_ERR;
		}
		//处理完毕

		if (!set_std_return( sendbuf,  sndlen,(PROTO_HEADER*)recvbuf , 
					SUCC,this->ba_out.get_postion() )) 
		{
			DEBUG_LOG("set_std_return:SYS_ERR: %u",
					this->ba_out.get_postion() );
			return SYS_ERR; 
		}
		memcpy(SNDBUF_PRIVATE_POS,this->ba_out.get_buf(),
				this->ba_out.get_postion()  );
		return SUCC;

		
	}
	
	virtual int deal_func(uint16_t cmdid, userid_t userid, Cmessage * c_in, Cmessage * c_out ,P_DEALFUN_T p_func ,bool is_commit )
	{
		//调用相关处理函数
		this->ret=9999;
		return (((Croute_func*)this)->*(p_func))(
				userid,c_in ,c_out);	
	}
	virtual int do_rollback( )
	{
		return 0;
	}



	int deal(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		if(this->collect_cmd_set.find(cmdid)!=this->collect_cmd_set.end()){
			//是聚合命令
			return do_collect_cmd_list(recvbuf, rcvlen, sendbuf, sndlen );

		}else{
			return deal_one_cmd(recvbuf, rcvlen, sendbuf, sndlen );
		}
	

	}


	int deal_one_cmd(char *recvbuf, int rcvlen, char **sendbuf, int *sndlen )
	{
		stru_cmd_item_t * p_cmd_item;
		uint16_t cmdid=((PROTO_HEADER*)recvbuf)->cmd_id;
		userid_t userid=((PROTO_HEADER*)recvbuf)->id;

		if((p_cmd_item =this->get_cmd_item(cmdid))!=NULL){
			KDEBUG_LOG ( userid ,"I:%04X", cmdid );
			if (this->is_check_proto_md5) {
				//检查md5值,md5_tags 放在上送报文的result中
				uint32_t md5_value=((PROTO_HEADER*)recvbuf)->result;
				if (md5_value!=0 ){//==0,表示不用md5_tags检查
					if (p_cmd_item->md5_tag!=md5_value){
						KDEBUG_LOG(userid,"CMD_MD5_TAG_ERR:DB:%u,client:%u",
								p_cmd_item->md5_tag ,md5_value);
						return  CMD_MD5_TAG_ERR;
					}
				}
			}


			//私有报文处理
			this->ba_in.init_read_mode(RCVBUF_PRIVATE_POS,
						RECVLEN_FROM_RECVBUF-PROTO_HEADER_SIZE );
		
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
					p_cmd_item->p_deal_func, true);

			if (this->ret==SUCC){//需要构造返回报文
				this->ba_out.init_postion();
				if (!p_cmd_item->msg_out->write_to_buf(this->ba_out)) {
					DEBUG_LOG("p_cmd_item->msg_out->write_to_buf :SYS_ERR" );
					return SYS_ERR;
				}

				if (!set_std_return( sendbuf,  sndlen,(PROTO_HEADER*)recvbuf , 
							SUCC,this->ba_out.get_postion() )) 
				{
					DEBUG_LOG("set_std_return:SYS_ERR: %u",
							this->ba_out.get_postion() );
					return SYS_ERR; 
				}
				memcpy(SNDBUF_PRIVATE_POS,this->ba_out.get_buf(),
						this->ba_out.get_postion()  );
				return SUCC;
			}else{//返回错误
				return ret;
			}
		}else{
			DEBUG_LOG("cmd no define  cmdid[%04X]",cmdid );
			return  CMDID_NODEFINE_ERR;
		}
	}
	virtual ~Cfunc_route_cmd_v2 (){ }
	
}; /* -----  end of class  Cfunc_route_base  ----- */

#endif   /* ----- #ifndef CFUNC_ROUTE_cmd_v2_INC  ----- */

