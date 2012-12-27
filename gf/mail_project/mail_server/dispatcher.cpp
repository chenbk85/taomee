
extern "C" 
{
#include <stdint.h>
#include <libtaomee/log.h>
#include <async_serv/dll.h>
}

#include "online.hpp"
#include "mail_server.hpp"
#include "dispatcher.hpp"

typedef int (*func_t)(svr_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess);

func_t funcs[5500];

/**
  * @brief initialize processing functions
  */
void init_funcs()
{
	final_funcs();
	funcs[ proto_online_svrinfo - 60001 ] = Online::report_online_info; 
	funcs[ proto_online_mail_head_list - 60001] = Online::get_mail_head_list;
	funcs[ proto_online_mail_body - 60001 ] = Online::get_mail_body;
} 

void final_funcs()
{
	for(uint32_t i =0; i< sizeof(funcs)/sizeof(funcs[0]); i++)
	{
		funcs[i] = NULL;
	}
}
/**
  * @brief dispatch package to the corresponding processing functions
  * @param data package data
  * @param fdsess fd related information of sender
  * @return 0 on success, -1 on error
  */
int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
	uint32_t cmd = pkg->cmd - 60001;

	if ((cmd < 0) || (cmd > 5499) || (funcs[cmd] == 0)) 
	{
		ERROR_LOG("invalid cmd=%u from fd=%d", pkg->cmd, fdsess->fd);
		return -1;
	}
	
	int ret =  funcs[cmd](pkg, pkg->len - sizeof(svr_proto_t), fdsess);
	return ret;
}

