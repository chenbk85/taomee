
#include <arpa/inet.h>
#ifdef __cplusplus
extern "C"
{
#endif
#include <libtaomee/conf_parser/config.h>
#include <libtaomee/inet/tcp.h>
#include <async_serv/async_serv.h>

#ifdef __cplusplus
}
#endif


#include "dbproxy.h"
#include "cli_proto.h"
#include "cli_login.h"
#include "pet.h"
#include "sns.h"
#include "items.h"
#include "skill.h"
#include "task_new.h"
#include "honor.h"
#include "home.h"
#include "mail.h"
#include "task_new.h"
#include "beast.h"
#include "activity.h"
#include "viponly.h"
#include <libtaomee++/utils/tcpip.h>
#include <libtaomee++/proto/Ccmd_map.h>
#include <libtaomee++/proto/proto_base.h>
#include <libtaomee++/proto/proto_util.h>

//-------------------------------------------------------
//函数定义
#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    int proto_name( sprite_t* p, uint32_t id,  Cmessage* c_in, uint32_t ret ) ;
#include "./proto/mole2_db_bind_for_online.h"
#include "./proto/bus_server_bind_for_online.h"


//-------------------------------------------------------------
//对应的结构体
#include "./proto/mole2_db.h"
#include "./proto/bus_server.h"
//-------------------------------------------------------------
//命令绑定
typedef   int(*P_DEALFUN_T)( sprite_t* p, uint32_t id,  Cmessage* c_in, uint32_t ret );

#undef  BIND_PROTO_CMD
#define BIND_PROTO_CMD(cmdid,proto_name,c_in,c_out,md5_tag,bind_bitmap )\
    {cmdid, new (c_out), md5_tag,bind_bitmap ,proto_name },

Ccmd< P_DEALFUN_T> g_proxy_cmd_list[]={
#include "./proto/mole2_db_bind_for_online.h"
#include "./proto/bus_server_bind_for_online.h"
};
//命令map
Ccmd_map< Ccmd< P_DEALFUN_T> >  g_proxy_cmd_map;

void init_db_handle_funs()
{
    g_proxy_cmd_map.initlist(g_proxy_cmd_list,sizeof(g_proxy_cmd_list)/sizeof(g_proxy_cmd_list[0]));
}

/*! udp socket address for db server */
//static struct sockaddr_in dbsvr_udp_sa;

/*! udp socket address for login report */
//static struct sockaddr_in login_report_udp_sa;
/*! udp socket address for talk report */
//static struct sockaddr_in talk_report_udp_sa;


/*! dbsvr udp socket fd */
//int dbsvr_udp_fd = -1;

/*! login report udp socket fd */
//int login_report_udp_fd = -1;

/*! talk report udp socket fd */
//int talk_report_udp_fd = -1;


//-----------------------------------------------------------------
/**
  * @brief set dbproxy protocol handle  
  */
#define SET_DB_HANDLE(op_, func_, len_, cmp_) \
		do { \
			if (rstart == 0 && db_handles[op_].func != 0) { \
				ERROR_RETURN(("duplicate cmd=%u", op_), -1); \
			} \
			db_handles[op_].func = func_; \
			db_handles[op_].len = len_; \
			db_handles[op_].cmp_method = cmp_; \
		} while (0)


/**
  * @brief handle package return from dbproxy
  * @param dbpkg package from dbproxy
  * @param pkglen length of dbpkg
  */
void handle_db_return(db_proto_t* dbpkg, uint32_t pkglen)
{
	if(dbpkg->cmd == proto_db_try_vip){
	}
	if (!dbpkg->seq) {
		return;
	}

	uint32_t waitcmd = dbpkg->seq & 0xFFFF;
	int      connfd  = dbpkg->seq >> 16;

	sprite_t* p = get_sprite_by_fd(connfd);
	if (!p || (p->waitcmd != waitcmd) || (p->waitcmd == proto_cli_login && p->id != dbpkg->id)) {
		KERROR_LOG(dbpkg->id, "connection has been closed: pwcmd=%u cmd=0x%04X %u seq=%X",
					((p)?p->waitcmd : 0), dbpkg->cmd, waitcmd, dbpkg->seq);
		return;
	}

	KDEBUG_LOG(p->id, "DB\t[u=%u c=%u 0x%04X ret=%u len=%u]", dbpkg->id, waitcmd, dbpkg->cmd, dbpkg->ret, dbpkg->len);

	int err = -1;
	uint32_t bodylen = dbpkg->len - sizeof(db_proto_t);
    Ccmd< P_DEALFUN_T> * p_cmd_item =g_proxy_cmd_map.getitem( dbpkg->cmd );
    if (p_cmd_item ) {//
	    int dbret=dbpkg->ret;
	    Cmessage * msg;
	    if (dbret==0){//成功
	        //还原对象
	        p_cmd_item->proto_pri_msg->init( );
	
	        byte_array_t in_ba ( (char *)dbpkg->body, dbpkg->len - sizeof(*dbpkg));
	
	        //失败
	        if (!p_cmd_item->proto_pri_msg->read_from_buf(in_ba)) {
	            KDEBUG_LOG(0, "DB还原对象失败");
	            return;
	        }
	
	        //客户端多上传报文
	        if (!in_ba.is_end()) {
	            KDEBUG_LOG(0, "client 过多报文");
	            return;
	        }
	        msg=p_cmd_item->proto_pri_msg;
	    } else {
	        msg= NULL;
	    }
	    p_cmd_item->func(p, dbpkg->id ,msg, dbpkg->ret);


    }else{//处理原有的命令
		if (!db_handles[dbpkg->cmd].func) {
			KERROR_LOG(0, "db_handle is NULL: cmd=%d", dbpkg->cmd);
			return;
		}

		if ( ((db_handles[dbpkg->cmd].cmp_method == cmp_must_eq) && (bodylen != db_handles[dbpkg->cmd].len))
				|| ((db_handles[dbpkg->cmd].cmp_method == cmp_must_ge) && (bodylen < db_handles[dbpkg->cmd].len)) ) {
			if ((bodylen != 0) || (dbpkg->ret == 0)) {
				KERROR_LOG(0, "invalid package len=%u needlen=%u cmd=0x%X cmpmethod=%d ret=%u",
							bodylen, db_handles[dbpkg->cmd].len, dbpkg->cmd, 
							db_handles[dbpkg->cmd].cmp_method, dbpkg->ret);
				goto ret;
			}
		}

		err = db_handles[dbpkg->cmd].func(p, dbpkg->id, dbpkg->body, bodylen, dbpkg->ret);

	ret:
		if (err) {
			KDEBUG_LOG(p->id, "DB RET ERR KICK USER OFFLINE\t[%u]", err);
			close_client_conn(p->fd);
		}

	}
}

int no_body_return(sprite_t* p, userid_t id, uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	response_proto_head(p, p->waitcmd, 1, 0);
	return 0;
}

/**
  * @brief init handles to handle protocol packages from dbproxy
  * @return true if all handles are inited successfully, false otherwise
  */
int init_db_proto_handles(int rstart)
{
	init_db_handle_funs();
	/* operation, bodylen, compare method */
	SET_DB_HANDLE(proto_db_get_sprite, db_get_sprite_callback, sizeof(get_sprite_rsp_t) + 212, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_other_user_info, get_other_user_info_callback, sizeof(get_sprite_rsp_t) + 36, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_offline_msg, get_offline_msg_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_update_sprite, update_sprite_callback, sizeof(get_sprite_rsp_t) + 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_set_user_nick, set_user_nick_callback, USER_NICK_LEN, cmp_must_eq);
	SET_DB_HANDLE(proto_db_set_user_color, set_user_color_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_base_attr, add_base_attr_callback, 12, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_hp_mp, add_hp_mp_callback, 12, cmp_must_eq);
	SET_DB_HANDLE(proto_db_cure_all, cure_all_callback, 12, cmp_must_ge);
	SET_DB_HANDLE(proto_db_pet_get_info, get_pet_info_callback, sizeof(pet_db_t), cmp_must_ge);
	SET_DB_HANDLE(proto_db_set_pet_nick, set_pet_nick_callback, 4 + USER_NICK_LEN, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_cure, cure_pet_callback, 20, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_cure_injury, cure_pet_injury_callback, 12, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_set_state, set_pet_state_callback, sizeof(pet_db_t), cmp_must_ge);
	SET_DB_HANDLE(proto_db_pet_follow_set, set_pet_follow_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_allocate_exp, allocate_exp_callback, 28, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_xiaomee, add_xiaomee_callback, 8, cmp_must_eq);
	SET_DB_HANDLE(proto_db_set_energy, set_energy_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_user_set_injury, set_injury_callback, 0, cmp_must_ge);

	SET_DB_HANDLE(proto_db_add_item_feed_count, add_item_feed_count_callback, 12, cmp_must_eq);
    SET_DB_HANDLE(proto_db_get_item_feed_count, get_item_feed_count_callback, 12, cmp_must_eq);
	
	SET_DB_HANDLE(proto_db_set_position, set_position_callback, 1, cmp_must_eq);
	SET_DB_HANDLE(proto_db_get_pet_list, get_pet_list_callback, 4, cmp_must_ge);
	
	SET_DB_HANDLE(proto_db_add_friend, add_friend_callback, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_del_friend, no_body_return, 0, cmp_must_eq);

	SET_DB_HANDLE(proto_db_bklist_add, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_bklist_del, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_get_bklist, get_bklist_callback, 4, cmp_must_ge);
	
	SET_DB_HANDLE(proto_db_get_pakt_clothes, get_pakt_clothes_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_add_cloth, buy_cloth_callback, sizeof(body_cloth_t) + 8, cmp_must_eq);
	SET_DB_HANDLE(proto_db_del_cloth, del_cloth_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_set_clothes_where, set_clothes_where_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_sell_cloths, sell_cloths_callback, 12, cmp_must_ge);
	SET_DB_HANDLE(proto_db_repair_cloth_list, repair_cloths_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_house_cloth_list, get_house_cloth_list_callback, 12, cmp_must_ge);
	SET_DB_HANDLE(proto_db_cloth_in_out, cloth_in_out_callback, sizeof(body_cloth_t), cmp_must_eq);

	SET_DB_HANDLE(proto_db_buy_skill, buy_skill_callback, sizeof(skill_t) + 8, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_buy_skill, buy_pet_skill_callback, sizeof(skill_t) + 7, cmp_must_eq);
	SET_DB_HANDLE(proto_db_del_skill, del_skill_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_del_skill, del_pet_skill_callback, 9, cmp_must_eq);
	SET_DB_HANDLE(proto_db_pet_add_base_attr, add_pet_base_attr_callback, 16, cmp_must_eq);
	SET_DB_HANDLE(proto_db_get_pet_inhouse, get_pet_inhouse_callback, 12, cmp_must_ge);
	SET_DB_HANDLE(proto_db_update_pet_location, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_exchange_pet, exchange_pet_callback, 8, cmp_must_ge);

	SET_DB_HANDLE(proto_db_get_task_detail, get_task_detail_callback, 8 + TASK_DIARY_BUF_LEN, cmp_must_ge);
	SET_DB_HANDLE(proto_db_write_task_diary, write_task_diary_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_task_all, get_task_all_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_task_db, get_task_db_callback, sizeof(task_db_t), cmp_must_eq);
	SET_DB_HANDLE(proto_db_set_task_db, set_task_db_callback, sizeof(task_db_t), cmp_must_eq);
	SET_DB_HANDLE(proto_db_task_reward, add_task_reward_callback, sizeof(task_reward_t), cmp_must_ge);

	SET_DB_HANDLE(proto_db_update_user_honor, use_honor_callback, 4, cmp_must_eq);

	SET_DB_HANDLE(proto_db_add_normal_item, add_normal_item_callback, 12, cmp_must_eq);
	SET_DB_HANDLE(proto_db_buy_normal_item, buy_normal_item_callback, 16, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_item_list, add_item_list_callback, 12, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_house_item_list, get_house_item_list_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_item_in_out, item_in_out_callback, 8, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_house_item, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_get_item_day, get_item_day_callback, 4, cmp_must_ge);

	SET_DB_HANDLE(proto_db_set_skill_lv, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_set_prof, set_prof_callback, 1, cmp_must_eq);

	SET_DB_HANDLE(proto_db_check_session, check_session_callback, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_add_session, add_session_callback, 16, cmp_must_eq);

	SET_DB_HANDLE(proto_db_rand_info_check, rand_info_check_callback, 12, cmp_must_eq);
	SET_DB_HANDLE(proto_db_rand_info_update, rand_info_update_callback, 0, cmp_must_eq);

	SET_DB_HANDLE(proto_db_register_bird_map, register_bird_map_callback, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_get_bird_maps, get_maps_state_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_set_map_info, no_body_return, 0, cmp_must_eq);
	SET_DB_HANDLE(proto_db_register_boss_map, register_boss_map_callback, 0, cmp_must_eq);

	SET_DB_HANDLE(proto_db_get_home_info, get_home_info_callback, sizeof(home_t), cmp_must_eq);
	SET_DB_HANDLE(proto_db_update_home_flag, no_body_return, 4, cmp_must_eq);

	SET_DB_HANDLE(proto_db_get_handbook_info, get_handbook_info_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_use_item_hb_item, use_handbook_item_callback, 4, cmp_must_eq);
	SET_DB_HANDLE(proto_db_use_beast_hb_item, use_handbook_item_callback, 4, cmp_must_eq);

	SET_DB_HANDLE(proto_db_try_vip, try_vip_callback, 0, cmp_must_ge);//try vip
/*! mail */
	SET_DB_HANDLE(proto_db_get_mail_simple, get_mail_simple_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_mail_content, get_mail_content_callback, 356, cmp_must_eq);
	SET_DB_HANDLE(proto_db_del_mail, del_mail_callback, 4, cmp_must_ge);
	SET_DB_HANDLE(proto_db_mail_set_flag, mail_set_flag_callback, 8, cmp_must_ge);
	SET_DB_HANDLE(proto_db_send_mail, mail_send_callback, 0, cmp_must_ge);
	SET_DB_HANDLE(proto_db_get_invited, get_invited_callback, 8, cmp_must_eq);
	SET_DB_HANDLE(proto_db_mail_items_get, mail_items_get_callback, MAIL_ITEM_LIST_LEN, cmp_must_eq);

	SET_DB_HANDLE(proto_db_get_monster_hb_range,get_monster_handbook_info_callback,4,cmp_must_ge);
	SET_DB_HANDLE(proto_db_set_cli_buff,set_cli_buff_callback,0,cmp_must_eq);
	return 0;
}

