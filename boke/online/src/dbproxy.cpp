#include "dbproxy.h"
#include "./proto/pop_db_enum.h"
#include "./proto/main_login.h"
#include "./proto/main_login_enum.h"
#include "cli_proto.h"
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>

#include "timer_func.h"
#include "stat.h"
//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    void proto_name( Csprite* p, uint32_t id , Cmessage* c_in, uint32_t ret ) ;
#include "./proto/pop_db_bind_for_online.h"
#include "./proto/main_login_bind_for_check_session.h"
#include "./proto/main_login_bind_get_gameflag.h"


//-------------------------------------------------------------
//对应的结构体
#include "./proto/pop_db.h"
//-------------------------------------------------------------
//命令绑定
typedef   void (*P_DEALFUN_T)( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_out), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_proxy_cmd_list[]={
#include "./proto/pop_db_bind_for_online.h"
#include "./proto/main_login_bind_for_check_session.h"
#include "./proto/main_login_bind_get_gameflag.h"
};
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_proxy_cmd_map;

void init_db_handle_funs()
{
    g_proxy_cmd_map.initlist(g_proxy_cmd_list,sizeof(g_proxy_cmd_list)/sizeof(g_proxy_cmd_list[0]));
}


/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if (!dbpkg->seq) {
		return;
	}

	uint32_t waitcmd = dbpkg->seq & 0xFFFF;
	int      connfd  = dbpkg->seq >> 16;

	Csprite* p =g_sprite_map->get_sprite_by_fd(connfd);
	/*
	 * 检查
	 * 1、	找不到对应的用户
	 * 2、	根据fd找到的用户米米号与返回包不匹配（fd释放又分配）
	 * 3、	waitcmd不同（用户刚离线又上线，分配的fd相同，上次的db请求返回）
	 */
	if (!p || (p->id != dbpkg->id ) || (p->get_waitcmd() != waitcmd && p->get_waitcmd()!=0)) {
		if ( dbpkg->cmd != 0x741B) {
			KDEBUG_LOG(dbpkg->id, "DB R CLIENT CLOSED\t[pid=%u][cmd=%u %04X %u]", 
					((p) ? p->id : 0), ((p)?p->get_waitcmd(): 0), dbpkg->cmd, waitcmd);
		}
		return;
	}


    Ccmd< P_DEALFUN_T> * p_cmd_item =g_proxy_cmd_map.getitem( dbpkg->cmd );
    if (p_cmd_item ) {//
		//if (g_is_test_env)
		{
			char outbuf[13000];
			bin2hex(outbuf,(char*)dbpkg,pkglen  ,500 );
			KDEBUG_LOG(p->id, "SI\t[u=%u c=%u 0x%04X ret=%u][%s]", dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret, outbuf );
		}

	    Cmessage * msg = NULL;
	    if (dbpkg->ret==0){//成功
			bool unpack_ret=  p_cmd_item->proto_pri_msg->read_from_buf_ex( 
					 (char *)dbpkg->body, dbpkg->len - sizeof(*dbpkg) );
	        if (!unpack_ret){
	            KDEBUG_LOG(p->id, "err: pkglen error");
	            return;
	        }
	        msg=p_cmd_item->proto_pri_msg;
	    }
		p_cmd_item->func(p, dbpkg->id ,msg, dbpkg->ret);
    }else{
		KDEBUG_LOG(p->id, "SI\t[u=%u c=%u 0x%04X ret=%u]", dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret);
		if ( dbpkg->ret !=SUCC ) {
			if ( dbpkg->cmd!=0xA123 ){//TODO,测试使用
				KERROR_LOG(p->id, "SET USER OFFLINE  DBERR CMDID[%04X][%d]", dbpkg->cmd ,dbpkg->ret);
				close_client_conn(p->fdsess->fd);
			}
		}
	}
}

/*登入*/
void pop_login( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret )
{

	if (ret!=SUCC){
		p->send_err( cli_userid_nofind_err );
		KDEBUG_LOG(p->id, "SET USER OFFLINE\t[%d]", cli_userid_nofind_err);
		close_client_conn(p->fdsess->fd);
		return ;
	}
	// 添加登录统计项
	stat_log_login_ip(p);
	stat_log_login_on(p);

	//通知上线
	send_sw_report_user_onoff(p,true );

	pop_login_out * p_out = P_OUT;

	p->game_map_id=0;
	p->flag=p_out->user_base_info.flag;
	p->xiaomee=p_out->user_base_info.xiaomee;
	p->age=p_out->user_base_info.age;
	p->color=p_out->user_base_info.color;
	memcpy(p->nick,p_out->user_base_info.nick,sizeof(p_out->user_base_info.nick));
	//不能为0，因为判断是否登入时使用到它
	p->register_time=p_out->user_base_info.register_time 
		? p_out->user_base_info.register_time : 1;

	//修复bug.如果已经注册，但位置不对(0,0)，则设置到(0,2)
	if ((p->flag & user_flag_reg ) && p_out->user_base_info.last_islandid==0
		&& p_out->user_base_info.last_mapid == 0){
		p_out->user_base_info.last_islandid = 0;
		p_out->user_base_info.last_mapid=2;
	}

	// 抽样体验，设置最后一位为1的新用户作为体验号
	if (!(p->flag & user_flag_reg) 
			&& (p->id % 10 == 1 || p->id % 10 == 2 || p->id % 10 == 3)) {
		p->flag |= user_flag_sample;
	}

	p->last_login =p_out->user_base_info.last_login;
	p->online_time=p_out->user_base_info.online_time;
	p->login_time=time(NULL);

	p->task_list.init(p_out->task_list );
	p->item_list.init(p_out->item_list);
	p->spec_item_list.init(p, p_out->item_day_list);
	p->find_map_list_init(p_out->find_map_list);
	p->user_log_list.init(p_out->user_log_list);
	p->game_stat_map.init(p, p_out->game_info_list);
	
	//用户进入地图 
	cli_walk_in tmp_in;
	tmp_in.islandid=p_out->user_base_info.last_islandid;
	tmp_in.mapid=p_out->user_base_info.last_mapid;
	tmp_in.x=p_out->user_base_info.last_x;
	tmp_in.y=p_out->user_base_info.last_y;
	p->reset_level();

	g_sprite_map->cd_map(p->id, &tmp_in, true);

	stat_log_task_finish(p->id, p->task_list.finish_task_count);
	// 防沉迷定时器设置
	p->reset_online_timer();
	p->add_login_count();

	if (p->flag & user_flag_reg) {	// 如果已经注册，不再拉取
		p->send_succ_login_response();
	} else {				// 拉取该米米号的游戏激活信息
		KDEBUG_LOG(p->id, "GET USER GAMEID");
		userinfo_get_gameflag_in gameflag_in;
		gameflag_in.region = 0;
		send_msg_to_db(p, userinfo_get_gameflag_cmd, &gameflag_in);
	}
}


/*check session*/
void main_login_check_session( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC ){
		bool close_flag=true;
		if ( g_is_test_env && ret == 4331) {//是测试环境的密码验证失败可疑跳过
			close_flag=false;
		}
		if (close_flag ){
			p->send_db_err(ret);
			KDEBUG_LOG(p->id, "SET USER OFFLINE SESSION FAIL\t[%d]",ret );
			close_client_conn(p->fdsess->fd);
			return ;
		}
	}

	//登录
 	send_msg_to_db(p,pop_login_cmd);
}

void userinfo_get_gameflag( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret == SUCC) {
		userinfo_get_gameflag_out* p_out = P_OUT;	
		if (!p_out->dx_gameflag && !p_out->wt_gameflag) {
			p->flag |= user_flag_newid;
			KDEBUG_LOG(p->id, "NEW ID");
		}
	}
	p->send_succ_login_response();
}
// ./proto/pop_db_online_db_src.cpp
