#include "dispatcher.h"

#include "version.h"

#include "onlinehome.h"

#include "home.h"
#include "sprite.h"

//新的命令分发
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>
#include <libtaomee++/utils/strings.hpp>
#include "./proto/mole2_home.h" 
#include "./proto/mole2_home_enum.h" 



extern int g_log_send_buf_hex_flag;
//-------------------------------------------------------
//函数定义
//

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name( home_t* p_home, sprite_ol_t* p , Cmessage* c_in ) ;
#include "./proto/mole2_home_bind_for_cli_request.h"


//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_home.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)( home_t* p_home, sprite_ol_t* p, Cmessage* p_in );

//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_cli_cmd_map;
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_in), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_cli_cmd_list[]={
#include "./proto/mole2_home_bind_for_cli_request.h"
};

void init_cli_handle_funs()
{
    g_cli_cmd_map.initlist(g_cli_cmd_list, sizeof(g_cli_cmd_list)/sizeof(g_cli_cmd_list[0]));
}

int dispatcher(void* data, fdsession_t* fdsess)
{
	char version[256] = {0};
	home_proto_t* proto = (home_proto_t*) data;

 	if ( g_log_send_buf_hex_flag==1)
	{
	        char outbuf[13000];
	        bin2hex(outbuf,(char*)data,proto->len,2000);
	        DEBUG_LOG("CI[%u][%u][%u][%lu][%s]", proto->len, proto->opid,proto->id, (uint64_t)proto->homeid, outbuf  );
	}

	if(proto->cmd == proto_get_version) {
		proto = (home_proto_t*)version;
		proto->len = 256;
		strncpy((char*)proto->body,g_version,200);
		return send_pkg_to_client(fdsess, proto, 256);
	}

	if (proto->onlineid > MAX_ONLINE_NUM) {
		KERROR_LOG(proto->id,"invaild onlineid=%u", proto->onlineid);
		return -1;
	}

	if ((proto->opid <= proto_begin) || (proto->opid >= proto_max)) {
		KERROR_LOG(proto->id, "invalid opid [len=%u cmd=%u onineid=%u homeid=%u opid=%u]",proto->len,proto->cmd,proto->onlineid,(uint32_t)proto->homeid,proto->opid);
		return -1;
	}

	all_fds[proto->onlineid] = fdsess;
	DEBUG_LOG("set fds id=%u, fdsess=%p",proto->onlineid,fdsess  );

	bool use_home_flag=true;
	switch(proto->opid) {
		case proto_enter_home:
			return proto_enter_home_op(proto);
			break;
		case home_game_get_map_id_cmd:
			use_home_flag=false;
			break;
		case home_trade_get_mapid_cmd:
			use_home_flag=false;
			break;
	}
	home_t* p_home=NULL;	
	sprite_ol_t* p =NULL;
	sprite_ol_t tmp_sprite ;

	DEBUG_LOG("XXXX: use_home_flag=%u ",uint32_t(use_home_flag) );
	if (use_home_flag){
		 p_home	= (home_t*) g_hash_table_lookup(all_home, &proto->homeid);
		if (!p_home) {
			KDEBUG_LOG(proto->id,"MAP NOT EXSIT[onlineid=%u mapid=(%x,%u) opid=%u]",proto->onlineid,HI32(proto->homeid),LO32(proto->homeid),proto->opid);
			return 0;
		}
		 p =(sprite_ol_t*) g_hash_table_lookup(p_home->sprites, &proto->id);
		if(!p) {
			KDEBUG_LOG(proto->id,"PLAYER NOT EXSIT[onlineid=%u mapid=(%x,%u) opid=%u]",proto->onlineid,HI32(proto->homeid),LO32(proto->homeid),proto->opid);
			return 0;
		}
	}else{//两个协议需要userid
		tmp_sprite.id=proto->id;
		tmp_sprite.onlineid=proto->onlineid;
		p=&tmp_sprite;
	}
 	Ccmd< P_DEALFUN_T> * p_cmd_item =g_cli_cmd_map.getitem(proto->opid);
	DEBUG_LOG("cmdinfo :%u 0x%p ",proto->opid,p_cmd_item );
    if (p_cmd_item ) {//是新的命令
		DEBUG_LOG("cli dispatch cmd %u", proto->opid);
		//还原对象
		p_cmd_item->proto_pri_msg->init( );

		byte_array_t in_ba ( ((char*)data)+sizeof(*proto),
			proto->len -sizeof(*proto) );
		//失败
		if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
			DEBUG_LOG("client recover object fail");
			return -1;
		}

		//客户端多上传报文
		if (!in_ba.is_end()) {
			DEBUG_LOG("client !in_ba.is_end()");
			return  -1;
		}
	    return p_cmd_item->func( p_home, p,  p_cmd_item->proto_pri_msg);
	}else{
		return handle_online_op(p_home, p, proto);
	}
	

}

int home_ex_xxx( home_t* p_home,  sprite_ol_t* p , Cmessage* c_in ) 
{
	return 0;
}

int home_game_get_map_id(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{

	home_game_get_map_id_in * p_in = P_IN;
	DEBUG_LOG("home_game_get_map_id() called [type=%u]",p_in->type);
	if(p_in->type >1){
		DEBUG_LOG("parse error");
		return -1; 
	}
	g_pvp[p_in->type].get_map_id(p->id, p_in->nick, p_in->level, p->onlineid,p_in->type);

	return 0;   
}

int home_game_del_user(home_t* p_home, sprite_ol_t* p , Cmessage* c_in)
{
	DEBUG_LOG("home_game_del_user() called");

	home_game_del_user_in* p_in = P_IN;

	g_pvp[0].del_user(p->homeid, p->id, p_in->other_userid);
	g_pvp[1].del_user(p->homeid, p->id, p_in->other_userid);
	//if(g_pvp[0].is_in(p->id)){
		//DEBUG_LOG("pvp1:del user=%u",p->id);
		//g_pvp[0].del_user(p->homeid, p->id, p_in->other_userid);
	//}else{
		//DEBUG_LOG("pvp1:del user=%u",p->id);
		//g_pvp[1].del_user(p->homeid, p->id, p_in->other_userid);
	//}
	return 0;
}


// ./proto/mole2_home_online_src.cpp

