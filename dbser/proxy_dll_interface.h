/*
 * =====================================================================================
 *
 *       Filename:  proxy_dll_interface.h
 *
 *    Description: proxy dll 接口  
 *
 *        Version:  1.0
 *        Created:  03/08/2010 10:38:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  jim (xcwen), jim@taomee.com
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef  proxy_dll_interface_incl
#define  proxy_dll_interface_incl
extern "C" {
#include <async_serv/net_if.h>
}
#define PROXY_PAGE_SIZE 16384 

//最近一次客户端发上来的g_client_proto_id;
extern uint32_t g_client_proto_id;
#include "proto_header.h"

struct stru_pri_buf{
	uint32_t state; //状态,
	char  buf[20];//可以自由使用的空间，如果不够用的话，就可使用p_ex_buf 
	char * p_ex_buf;//如果需要的话,自行分配空间，但是，也同样要自己释放,不然就会造成内存泄漏
};
struct stru_client_info{
	//接口实现时, 不用修改，也不要修改
		//用于最后向客户端返回应答包
		fdsession_t* p_client;//连接信息,用于给客服端发送应答信息
		uint32_t client_protoid;//客服端报文头部的protoid
		uint16_t client_cmdid; //客服端报文头部的命令ID
		uint32_t client_userid;//客服端报文头部的id 
	
		uint32_t db_userid;//用于检验DB返回的报文是不是合法的,

	//协议私有数据
		stru_pri_buf pri_buf;//用于保存每个协议自己需要保存的数据,由每个协议自己处理
};




//将报文向哪个方向发
enum  enum_send_pkg_flag{
	FLAG_NULL=0,				//没有定义
	FLAG_SEND_TO_CLIENT=1,		//发给client,如 online
	FLAG_SEND_TO_SERVER=2		//发给server,如 db 
};

//将结构在接口(proxy_process_client_cmd,proxy_process_serv_return)返回0 时才起作用。
//结构的的功能是确定发哪里发什么报文
//当send_pkg_flag
struct stru_proxy_interface_out{
	enum_send_pkg_flag send_pkg_flag;//将报文向哪个方向发
	//以下三个变量,当send_pkg_flag=FLAG_SEND_TO_CLIENT时,将会使用,
		uint32_t send_client_result;//返回错误码，将send_client_result=0时，下面两个才会使用
		//私有报文长度
		uint32_t send_client_private_buf_len;//
		//私有报文的内容
		char send_client_private_buf[PROXY_PAGE_SIZE];

		//当send_pkg_flag=FLAG_SEND_TO_SERVER 时将会使用,指向完整的报文
		//报文长度在协议头部所以不要指定
		char send_server_buf[PROXY_PAGE_SIZE];

	//设置发给客户端的报文 非buf的部分的值
	void set_send_to_client_without_buf( uint32_t send_client_result , 
			uint32_t send_client_private_buf_len=0)
	{
		this->send_pkg_flag=FLAG_SEND_TO_CLIENT;
		this->send_client_result=send_client_result ;
		this->send_client_private_buf_len=send_client_private_buf_len;
	};

	//设置发给server端的报文非buf的部分的值 
	void set_send_to_server_without_buf( uint16_t cmdid ,uint32_t userid, uint32_t private_size )
	{
		this->send_pkg_flag=FLAG_SEND_TO_SERVER;
		PROTO_HEADER *ph=(PROTO_HEADER*)(this->send_server_buf);
		memset(ph,0,PROTO_HEADER_SIZE);
		ph->proto_length=PROTO_HEADER_SIZE+private_size ;
		ph->cmd_id=cmdid ;
		ph->id=userid;
	};
};





struct  stru_lib_info{
	char lib_name[32];//如lib_mole
	char version[32];//如20080901
};

//接口定义-----------------------------------------------------------------------

//用于返回lib 的标识信息
extern "C" void proxy_get_lib_info(stru_lib_info *p_out );
//程序开始启动时调用
extern "C" int proxy_init_service ();
//程序终止时调用
extern "C" int proxy_fini_service ();

//收到客户端报文时调用
//返回－1：表示该报文这需要proxy正常继续往下传。
//0:已经处理完毕，proxy不再处理recvbuf,
// p_out 部分请查看它的说明
extern "C" int proxy_process_client_cmd(char* recvbuf,int recvlen, stru_client_info * p_client_info , 
		stru_proxy_interface_out *p_out);

//收到后端报文（一般是DBSER）时调用
//返回－1：表示该报文这需要proxy正常继续往下传。
//0:已经处理完毕，proxy不再处理recvbuf,
// p_out 部分请查看它的说明
extern "C" int proxy_process_serv_return(char* recvbuf,int recvlen,stru_pri_buf * p_pri_buf , 
	   stru_proxy_interface_out *p_out	); 

#endif

