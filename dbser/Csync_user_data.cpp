/*
 * =====================================================================================
 *
 *       Filename:  Csync_user_data.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/24/2010 09:51:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */

#include <Csync_user_data.h>
#include <proto_header.h>

Csync_user_data::Csync_user_data(mysql_interface * db, char * db_old_ip,short db_old_port ):tcp(db_old_ip,db_old_port,1,10),user_table(db)
{
	this->db=db;
}


int  Csync_user_data::sync_data(const uint32_t userid,uint16_t get_sql_date_cmdid,
		const char * dbname_pre,const char * tablename_pre ,
		 enum_db_table_type db_table_type ,
		const char * useridname )
{
	bool is_existed;
	int ret;

	//检查用户是否己同步过
	
	switch ( db_table_type )
	{
		case type_100x10 :
			GEN_SQLSTR(this->sql_str,"select 1 from  %s_%02d.%s_%01d where %s=%u ",
			dbname_pre,userid%100, tablename_pre,(userid%10000)/1000,useridname ,userid);
		break;
		case type_100x100:
			GEN_SQLSTR(this->sql_str,"select 1 from  %s_%02d.%s_%02d where %s=%u ",
			dbname_pre,userid%100, tablename_pre,(userid%10000)/100,useridname ,userid);
	break;
		default :
		break;
	}

	ret=this->user_table.record_is_existed(this->sql_str,&is_existed );
	if (ret!=SUCC) return ret;
	if (is_existed){//记录存在
		DEBUG_LOG("record find :%s",this->sql_str );
		return SUCC;
	}

	//需要同步数据
	proto_header ph={};
	ph.proto_length=PROTO_HEADER_SIZE;
	ph.cmd_id=get_sql_date_cmdid;
	ph.id=userid;
	char *rcvbuf; int  rcvlen;
	
	ret=this->tcp.do_net_io((char*)&ph,PROTO_HEADER_SIZE,&rcvbuf,&rcvlen );	
	if(ret!=SUCC){
		return ret;
	}

	uint32_t pri_msglen= ((PROTO_HEADER * )rcvbuf)->proto_length-PROTO_HEADER_SIZE;
	if (pri_msglen<4) {
		return  PROTO_RETURN_LEN_ERR;   
	}

	stru_str_with_len *p_pri=(stru_str_with_len*)(rcvbuf+PROTO_HEADER_SIZE);
	if (pri_msglen!=p_pri->len+4 ){
		return  PROTO_RETURN_LEN_ERR;   
	}

	if (p_pri->len==0){//没有数据
		return  SUCC;   
	};

	//把最后一个字节设置'\0';防止越界，如果最后一个字节为";",设置为'\0',运行没有问题
	p_pri->str[p_pri->len-1]='\0';
	//导入数据
	int db_ret=0;

    mysql_set_server_option(&this->db->handle, MYSQL_OPTION_MULTI_STATEMENTS_ON);
    db_ret=mysql_query( &this->db->handle,p_pri->str);
    MYSQL_RES *result;
    do {
        if ((result= mysql_store_result(&this->db->handle ))!=NULL ){
            mysql_free_result(result);
        }
    } while (!mysql_next_result(&this->db->handle  ));
    mysql_set_server_option(&this->db->handle, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
	return SUCC;
}

