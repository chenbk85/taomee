#include "switch.h"
#include "./proto/pop_switch_enum.h"
#include "./proto/pop_online_enum.h"
#include <libtaomee++/proto/Ccmd_map.h>
#include "./proto/pop_online.h"

//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    void proto_name( Csprite* p, uint32_t id , Cmessage* c_in, uint32_t ret ) ;
#include "./proto/pop_switch_bind_for_online.h"


//-------------------------------------------------------------
//对应的结构体
#include "./proto/pop_switch.h"
//-------------------------------------------------------------
//命令绑定
typedef   void (*P_DEALFUN_T)( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid, proto_name, c_in, c_out, md5_tag, bind_bitmap)\
    {cmdid, new (c_out), md5_tag,bind_bitmap, proto_name },

Ccmd< P_DEALFUN_T> g_switch_cmd_list[]={
#include "./proto/pop_switch_bind_for_online.h"
};
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_switch_cmd_map;

void init_switch_handle_funs()
{
    g_switch_cmd_map.initlist(g_switch_cmd_list,sizeof(g_switch_cmd_list)/sizeof(g_switch_cmd_list[0]));
}


/**
  * @brief handle package return from switchswitch
  * @param switchpkg package from switchswitch
  * @param pkglen length of switchpkg
  */
void handle_switch_return(db_proto_t* switchpkg, uint32_t pkglen)
{

	//p 有可能为NULL
	Csprite* p =g_sprite_map->get_sprite(switchpkg->id);

	KDEBUG_LOG( switchpkg->id, "SWITCH\t[ cmdid=%d ret=%u,pkglen=%u]", switchpkg->cmd, switchpkg->ret,pkglen);

    Ccmd< P_DEALFUN_T> * p_cmd_item =g_switch_cmd_map.getitem( switchpkg->cmd );
    if (p_cmd_item ) {//
	    int switchret=switchpkg->ret;
	    Cmessage * msg = NULL;
	    if (switchret==0){//成功
			bool unpack_ret=  p_cmd_item->proto_pri_msg->read_from_buf_ex( (char *)switchpkg->body, switchpkg->len - sizeof(*switchpkg) );
	        if (!unpack_ret){
	            KDEBUG_LOG(0, "err: pkglen error");
	            return;
	        }
	        msg=p_cmd_item->proto_pri_msg;
	    }
	    p_cmd_item->func(p, switchpkg->id ,msg, switchpkg->ret);

    }else{//
		KDEBUG_LOG(0, "CMDID NOFIND  ERR [%04X]",   switchpkg->cmd);
	}
}

//---------------------------------
void sw_set_user_offline( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if(p){
		p->send_err_server_noti(0,cli_login_other_online_err);
		close_client_conn(p->fdsess->fd);
		KDEBUG_LOG(p->id, "SET USER OFFLINE SWCMD");
	}
}

void sw_noti_across_svr( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	sw_noti_across_svr_out* p_out = P_OUT;
	//byte_array_t ba;
	//p_out->write_to_buf(ba);
	switch (p_out->cmdid) {
	case cli_noti_island_complete_cmd:
		{
			cli_noti_island_complete_out cli_out;
			cli_out.userid = *(uint32_t *)p_out->buflist;
			memcpy(cli_out.nick, p_out->buflist + 4, sizeof(cli_out.nick));
			cli_out.islandid = *(uint32_t *)(p_out->buflist + 20);
			//cli_out.read_from_buf(ba);
			g_sprite_map->noti_all(cli_noti_island_complete_cmd, &cli_out);
		}
		break;
	default:
		break;
	}
}
