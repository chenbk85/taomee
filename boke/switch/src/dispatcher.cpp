extern "C" {
#include <stdint.h>
#include <libtaomee/log.h>
}
#include <libtaomee++/utils/strings.hpp>
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>


#include "online.hpp"
#include "proto.hpp"

#include "dispatcher.hpp"



#include "./proto/pop_switch_enum.h"

//-------------------------------------------------------------
//对应的结构体
#include "./proto/pop_switch.h"
//-------------------------------------------------------------

//命令绑定
typedef int (*P_DEALFUN_T )(svr_proto_t* pkg, Cmessage* p_in,  fdsession_t* fdsess);


//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,Online::proto_name },

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
	#include "./proto/pop_switch_bind_for_cli.h"
};

void init_cli_handle_funs()
{
    g_cli_cmd_map.initlist(g_cli_cmd_list,
			sizeof(g_cli_cmd_list)/sizeof(g_cli_cmd_list[0]));
}

int dispatch(void* data, fdsession_t* fdsess)
{
	svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(data);
 // if (log_hex_data_flag==1)
	{
        char outbuf[13000];
        bin2hex(outbuf,(char*)data,pkg->len  ,2000);
        KDEBUG_LOG( pkg->id, "CI[%d][%u][%u][%u][%u][%s]",fdsess->fd,  pkg->len, pkg->cmd,pkg->ret, pkg->seq,outbuf  );
    }
  	//返回版本信息
    if(pkg->cmd== 1) {
        std::vector<std::string> file_name_list;
        const char * version=get_version_send_buf("SWITCH-0805",file_name_list );
        return send_pkg_to_client(fdsess, version, 256);
    }
	
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(pkg->cmd );
    if (p_cmd_item ) {
		DEBUG_LOG("cli dispatch cmd %u", pkg->cmd);
		//还原对象
		p_cmd_item->proto_pri_msg->init( );

		byte_array_t in_ba ( ((char*)data)+sizeof(*pkg),
			pkg->len -sizeof(*pkg));
		//失败
		if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
			DEBUG_LOG("还原对象失败");
			return -1;
		}

		//客户端多上传报文
		if (!in_ba.is_end()) {
			DEBUG_LOG("client 过多报文");
			return  -1;
		}
	    return p_cmd_item->func(pkg, p_cmd_item->proto_pri_msg,  fdsess);
    }else{
		DEBUG_LOG("ERR:comdid  nofind err ");
		return  -1;
	}

	return 0;
}

