/*
 * =====================================================================================
 *
 *       Filename:  Cbig_cache.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月08日 13时01分38秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */


#ifndef  CBIG_CACHE_INC
#define  CBIG_CACHE_INC
#include <libtaomee++/utils/tcpip.h>
#include <libtaomee++/utils/strings.hpp>
#include "proto_header.h" 

#define BIG_CACHE_VERSION     0x01
#define BIG_CACHE_NO_CACHE_ERR   3  
#define BIG_CACHE_CLEAN_BY_TRANSACTION  500  
inline int get_db_err(int err ){
	if ( err>0 && err<100 ){
		DEBUG_LOG("cache return err:%d",err );
	   	return DB_ERR;
	} else return err;
}

#define PH_LEN( _buf ) (((proto_head_t*)_buf )->len)
#define PH_RET( _buf ) (((proto_head_t*)_buf )->err )
#define PH_ROW( _buf ) (((proto_head_t*)_buf )->row)
struct proto_head_t {
    uint32_t len;
    uint16_t ver;
    uint16_t cmd; //0:统计，1：普通sql 
    uint32_t key;
    uint32_t err;
    uint32_t row;
}__attribute__((packed));
class  Cbig_cache:public Ctcp{
	private :
		char  buf[1024*32+100];
		bool use_cache;
		int execsql( const uint32_t key, const char * sql, char ** pp_result_buf  ){ 
			int len;
			proto_head_t  *ph= (proto_head_t*) buf;
			int sql_len=strlen(sql);
			DEBUG_LOG("sql:%s;",sql );

			ph->len=sql_len+sizeof(proto_head_t );
			if (g_db_cache_state.transaction_open_flag){
				ph->cmd=2;//在事务中，请求big_cache清空数据
			}else{
				ph->cmd=1;
			}
			ph->key=key;
			ph->ver=BIG_CACHE_VERSION;
			ph->row=0;
			ph->err=0;
		

			memcpy(buf+sizeof(proto_head_t ),sql,sql_len+1 );
		    //DEBUG_LOG("I[%s]",bin2hex(NULL,(char*)buf,ph->len));
	
			int ret=this->do_net_io(buf,ph->len,pp_result_buf, &len);
			if (ret==SUCC){
			}else{
				this->reconnect();
		    	DEBUG_LOG("O ERROR %d",ret );
			}
			return ret;
		}	

	public:

		Cbig_cache(const char * host_port ):Ctcp(host_port,(uint32_t)5 ){ 
			//当传进来的host 空时，则说明不使用cache
			this->use_cache=true;
			if (host_port==NULL){
				this->use_cache=false;		
			}else{
				if (host_port[0]=='\0'){
					this->use_cache=false;		
				}
			}	
			if (this->use_cache){
				DEBUG_LOG("Cbig_cache:  USE_CACHE:%s",host_port );
			}else{
				DEBUG_LOG("Cbig_cache: NO  USE CACHE,%s", host_port);
			}
		}
		inline bool is_use_cache(){
			 return this->use_cache;
		}

		int exec_update_sql(const uint32_t key, const char * sql,int *p_affected_rows ){ 
			char * p_result_buf; 
			int ret=this->execsql(key, sql, &p_result_buf);
			if (ret!=SUCC)	 return ret;


			if (!(PH_LEN(p_result_buf)==sizeof(proto_head_t) ||
					//insert返回长度 ,header+4
					PH_LEN(p_result_buf)==sizeof(proto_head_t)+4 
					)) {
				ERROR_LOG("big cache pkglen err:sql[%s] %d,[%s]\n",sql, PH_LEN(p_result_buf),
					   	bin2hex(NULL,p_result_buf ,PH_LEN(p_result_buf) ) );
				return DB_ERR;
			}

			if (PH_RET(p_result_buf)!=SUCC) return  PH_RET(p_result_buf);   
			//成功了
			//看看是不是事务处理
			if (g_db_cache_state.transaction_open_flag){
				return    BIG_CACHE_CLEAN_BY_TRANSACTION;
			}
			
			*p_affected_rows=PH_ROW(p_result_buf);
		 	INFO_LOG("C[%d][%s;]",*p_affected_rows , sql);
			return  SUCC;
		}	
		int exec_insert_sql_get_auto_increment_id(const uint32_t key, const char * sql,int *p_affected_rows, uint32_t *p_increment_id ){ 
			char * p_result_buf; 
			int ret=this->execsql(key, sql, &p_result_buf);
			if (ret!=SUCC)	 return ret;
			switch ( PH_LEN(p_result_buf) )
			{
				case sizeof(proto_head_t) :
					if (g_db_cache_state.transaction_open_flag){
						return    BIG_CACHE_CLEAN_BY_TRANSACTION;
					}
					return PH_RET(p_result_buf);
					break;
				case sizeof(proto_head_t)+4 :
					*p_affected_rows=PH_ROW(p_result_buf);
					*p_increment_id=*(uint32_t*)(p_result_buf+sizeof(proto_head_t));
		 			INFO_LOG("C[%d][%s;]",*p_affected_rows , sql);
					return  SUCC;
					break;
				default :
					ERROR_LOG("big cache pkglen err: %d\n",
							PH_LEN(p_result_buf) );
					return DB_ERR;
					break;
			}

		}	
		inline int exec_query_sql ( const uint32_t key, const char * sql, char ** pp_reslut  )
		{
			return this->execsql(key, sql,  pp_reslut);
		}

};
#endif   /* ----- #ifndef CBIG_CACHE_INC  ----- */

