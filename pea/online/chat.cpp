extern "C" {
#include <glib.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}

#include <time.h>
#include <libtaomee++/inet/pdumanip.hpp>

#include "fwd_decl.hpp" //脏词库头文件
#include "proto.hpp"    //协议头文件，chat.hpp已经包含
#include "cli_proto.hpp"//send_to_player相关
#include "pea_common.hpp"
#include "db_item.hpp"  //db_del_item相关
#include "map.hpp"      //查找地图人员相关
#include "player.hpp"   //玩家相关
#include "room.hpp"
//#include "union.hpp"  //工会相关
#include "battle.hpp"	//战队聊天转发信息
#include "battle_switch.hpp"
#include "dbproxy.hpp"
#include "utils.hpp"
#include "chat.hpp"

using namespace taomee;

enum chat_msg_type {
	CHAT_MSG_TYPE_COUNT		= 7,	//目前的聊天类型数
    CHAT_MSG_TYPE_BEGIN     = 0,    //聊天起始
    CHAT_MSG_TYPE_WORLD     = 1,    //世界消息(本服)
    CHAT_MSG_TYPE_UNION     = 2,    //工会消息
    CHAT_MSG_TYPE_MAP       = 3,    //本地图消息
    CHAT_MSG_TYPE_ITEM1     = 4,    //收费道具1消息
	CHAT_MSG_TYPE_ROOM		= 5,	//房间聊天
	CHAT_MSG_TYPE_TEAM		= 6,	//战斗中小队聊天
	CHAT_MSG_TYPE_ALL_IN_BTL= 7,	//战斗中和所有参战人聊天
    CHAT_MSG_TYPE_END       = 8,   	//收尾

	CHAT_MSG_MAX_PRIVATE_LEN	= 128,	//私聊消息最大长度
	CHAT_MSG_MAX_PUBLIC_LEN		= 64,	//公聊消息最大长度
};


//hard code for testing 聊天类型-道具对应关系表
static uint32_t chat_msg_itemid_type_table[CHAT_MSG_TYPE_COUNT] = {0, 0, 0, 10001, 0, 0, 0};

int chat_msg_world(player_t *p, cli_proto_chat_public_msg_out *p_out, int item_flag)
{
	uint64_t urkey = gen_key_by_uid_role(p->id, p->role_tm);
    GHashTableIter iter;
    gpointer key, val;
    g_hash_table_iter_init(&iter, all_players);
    while (g_hash_table_iter_next(&iter, &key, &val)) {
        player_t *p_dest = reinterpret_cast<player_t*>(val);
		if (p_dest != p) {//先不给用户自己转发
			if (item_flag == 1) {//喇叭消息 不管是否黑名单
				send_to_player(p_dest, p_out, p->waitcmd, 0);
			} else if(p_dest->blacklist.find(urkey) == p_dest->blacklist.end()) {//不在黑名单
        		send_to_player(p_dest, p_out, p->waitcmd, 0);
			}
		}
    }
    
	//用户自身也要知道消息是否发送成功
    return send_to_player(p, p_out, p->waitcmd, 1);
}

int chat_msg_union(player_t *p, cli_proto_chat_public_msg_out *p_out)
{
    /*TODO singku 工会聊天*/
    
    return send_to_player(p, p_out, p->waitcmd, 1);
}

int chat_msg_map(player_t *p, cli_proto_chat_public_msg_out *p_out)
{
    map_line_players *p_line =  p->get_cur_map_line();
    if (p_line == NULL) {/*几乎不可能的错误*/
        ERROR_TLOG("user: %u, chat msg to map, cur map is null", p->id);
        return send_error_to_player(p, ONLINE_ERR_CHAT_CUR_MAP_NULL);
    }
	
	uint64_t key = gen_key_by_uid_role(p->id, p->role_tm);
    list_head_t *l = NULL;
    list_for_each(l, &(p_line->map_line_playerlist)) {
        player_t *p_dest = list_entry(l, player_t, maphook);
		if (p_dest != p) {//先不给自己发
			if (p_dest->blacklist.find(key) == p_dest->blacklist.end())//不在黑名单才转发
        		send_to_player(p_dest, p_out, p->waitcmd, 0);
		}
    }
    return send_to_player(p, p_out, p->waitcmd, 1);;
}

//在房间中聊天,房间在battle_switch上面
int chat_msg_room(player_t *p, cli_proto_chat_public_msg_out *p_out)
{
	p->clear_waitcmd();
	return send_to_battle_switch(p, btlsw_chat_msg_transfer_cmd, p_out);
}

//战斗在战斗服务器中,发给所有参战玩家
int chat_msg_all_in_btl(player_t *p, cli_proto_chat_public_msg_out *p_out)
{
	p->clear_waitcmd();
	return send_to_battle_svr(p, p_out, online_proto_btl_chat_msg_transfer_cmd, p->battle_grp->fd_idx);
}

//战斗中聊天，发给本队玩家
int chat_msg_team(player_t *p, cli_proto_chat_public_msg_out *p_out)
{
    p->clear_waitcmd();
	return send_to_battle_svr(p, p_out, online_proto_btl_chat_msg_transfer_cmd, p->battle_grp->fd_idx);
}

int cli_proto_chat_private_msg(DEFAULT_ARG)
{
    cli_proto_chat_private_msg_in *p_in = P_IN;

	if (p_in->receiver.user_id == p->id) {
		return send_error_to_player(p, ONLINE_ERR_CHAT_SELF_CHAT_DENIED);
	}
	
	/*MYTEST singku如果加对方为黑名单，则不可与对方聊 暂时去掉这个限制*/
#if 0
	if (p->blacklist.find(p_in->receiver_id) != p->blacklist.end()) {
		//对方在自己的黑名单中,可能外挂
		return send_error_to_player(p, ONLINE_ERR_CHAT_BLACKLIST);
	}
#endif

    /* 脏词检测 */
	p_in->msg_content[p_in->_msg_content_len] = 0;//强制将最后一个字节置0
    int ret = tm_dirty_check(0, p_in->msg_content);
    if (ret > 0) {
        ERROR_TLOG("user: %u, chat msg to [%d] with dirty word[%s]", p->id, p_in->receiver.user_id, p_in->msg_content);
        return send_error_to_player(p, ONLINE_ERR_CHAT_DIRTY_MSG);
    } else if (ret < 0) {
        ERROR_TLOG("user: %u, tm_dirty_check: check error", p->id);
    }

	/* 信息内容长度检测 */
	if (p_in->_msg_content_len == 0 || p_in->_msg_content_len > CHAT_MSG_MAX_PRIVATE_LEN) {
		ERROR_TLOG("user: %u, chat msg len illegal[%u]", p->id, p_in->_msg_content_len);
		return send_error_to_player(p, ONLINE_ERR_CHAT_MSG_LEN_ILLEGAL);
	}

    cli_proto_chat_private_msg_out *p_out = P_OUT;
    p_out->msg_time = (uint32_t)time(NULL);
    p_out->sender.user_id = p->id;
	p_out->sender.role_tm = p->role_tm;
	p_out->receiver.user_id = p_in->receiver.user_id;
	p_out->receiver.role_tm = p_in->receiver.role_tm;
	p_out->_msg_content_len = p_in->_msg_content_len;
    memcpy(p_out->msg_content, p_in->msg_content, p_out->_msg_content_len);

	uint64_t key = gen_key_by_uid_role(p->id, p->role_tm);
    player_t *p_dest = get_player(p_in->receiver.user_id);
    if (p_dest != NULL && p_dest->role_tm == p_in->receiver.role_tm) {
		if (p_dest->blacklist.find(key) == p_dest->blacklist.end()) {
			//对方玩家角色存在且对方没有加你到黑名单，你的消息才能发到对方
        	send_to_player(p_dest, p_out, p->waitcmd, 0);
		} else {
			ERROR_TLOG("user: %u, chat msg to [%d], but [%d] is in you blacklist",
				p->id, p_in->receiver.user_id, p_in->receiver.user_id);
		}
    } else if (p_dest != NULL && p_dest->role_tm != p_in->receiver.role_tm){
		//玩家在线，但角色时间不对
		return send_error_to_player(p, ONLINE_ERR_CHAT_RECV_ID_NOT_EXIST);
	} else {
		//对方离线
		/*--TODO 发送离线消息--*/
		db_proto_send_offline_msg_in out;
		int idx = sizeof(cli_proto_t);
		pack(out.msg.msg, p_out->msg_time, idx);
		pack(out.msg.msg, p_out->sender.user_id, idx);
		pack(out.msg.msg, p_out->sender.role_tm, idx);
		pack(out.msg.msg, p_out->receiver.user_id, idx);
		pack(out.msg.msg, p_out->receiver.role_tm, idx);
		pack(out.msg.msg, p_out->_msg_content_len, idx);
		pack(out.msg.msg, p_out->msg_content, p_out->_msg_content_len, idx);
		player_t tmp_player;
		tmp_player.id = p_out->receiver.user_id;
		init_cli_proto_head(out.msg.msg, &tmp_player, cli_proto_chat_private_msg_cmd, idx);
		out.msg._msg_len = idx;
		out.db_user_id.user_id = p_out->receiver.user_id;
		out.db_user_id.role_tm = p_out->receiver.role_tm;
		out.db_user_id.server_id = p->server_id;
		send_to_db(NULL, p_out->receiver.user_id, p_out->receiver.role_tm, db_proto_send_offline_msg_cmd, &out);
    }
    return send_to_player(p, p_out, p->waitcmd, 1);
}

int cli_proto_chat_public_msg(DEFAULT_ARG)
{
    cli_proto_chat_public_msg_in *p_in = P_IN;

    /* 检查公聊类型是否正确 */
    if (p_in->msg_type <= CHAT_MSG_TYPE_BEGIN || p_in->msg_type >= CHAT_MSG_TYPE_END) {
        ERROR_TLOG("user: %u, chat msg type error[%u]", p->id, p_in->msg_type);
        return send_error_to_player(p, ONLINE_ERR_CHAT_TYPE_NOT_EXIST);
    }

	/* 信息内容长度检测 */
	if (p_in->_msg_content_len == 0 || p_in->_msg_content_len > CHAT_MSG_MAX_PUBLIC_LEN) {
		ERROR_TLOG("user: %u, chat msg len ilegal[%u]", p->id, p_in->_msg_content_len);
		return send_error_to_player(p, ONLINE_ERR_CHAT_MSG_LEN_ILLEGAL);
	}

    /* 检查是否使用道具 */
    uint32_t item_id = chat_msg_itemid_type_table[(p_in->msg_type - 1) % CHAT_MSG_TYPE_COUNT];
    if (item_id) {//使用道具
        if (false == item_data_mgr::get_instance()->is_item_data_exist(item_id)) {//道具ID不存在
            ERROR_TLOG("user: %u, chat msg item not exist[%u]", p->id, item_id);
            return send_error_to_player(p, ONLINE_ERR_CHAT_ITEM_ID_NOT_EXIST);
        }
        if (p->bag->get_item_count(item_id) <= 0) {//道具数量不够
            return send_error_to_player(p, ONLINE_ERR_CHAT_ITEM_LACK);
        }    
    }

	p_in->msg_content[p_in->_msg_content_len] = 0;//强制置0
    /* 脏词检测 */
    int ret = tm_dirty_check(0, p_in->msg_content);
    if (ret > 0) {
        ERROR_TLOG("user: %u, chat msg with dirty word[%s]", p->id, p_in->msg_content);
        return send_error_to_player(p, ONLINE_ERR_CHAT_DIRTY_MSG);
    } else if (ret < 0) {
        ERROR_TLOG("user: %u, tm_dirty_check: check error", p->id);
    }

    cli_proto_chat_public_msg_out *p_out = P_OUT;
    p_out->sender.user_id = p->id;
	p_out->sender.role_tm = p->role_tm;
    memcpy(p_out->sender_nick, p->nick, sizeof(p_out->sender_nick));
    p_out->msg_type = p_in->msg_type;
	p_out->_msg_content_len = p_in->_msg_content_len;
    memcpy(p_out->msg_content, p_in->msg_content, p_out->_msg_content_len);

    ret = 0;
    switch (p_in->msg_type) {
    case CHAT_MSG_TYPE_WORLD :
        ret = chat_msg_world(p, p_out, 0);
        break;
    case CHAT_MSG_TYPE_UNION :
        ret = chat_msg_union(p, p_out);
        break;
    case CHAT_MSG_TYPE_MAP :
        ret = chat_msg_map(p, p_out);
        break;
	case CHAT_MSG_TYPE_ITEM1 :
		ret = chat_msg_world(p, p_out, 1);
		break;
	case CHAT_MSG_TYPE_ROOM :
		ret = chat_msg_room(p, p_out);
		break;
	case CHAT_MSG_TYPE_TEAM :
		if (p->battle_grp == NULL) {
			return send_error_to_player(p, ONLINE_ERR_CHAT_NO_BATTLE);
		}
		ret = chat_msg_team(p, p_out);
		break;
	case CHAT_MSG_TYPE_ALL_IN_BTL :
		if (p->battle_grp == NULL) {
			return send_error_to_player(p, ONLINE_ERR_CHAT_NO_BATTLE);
		}
		ret = chat_msg_all_in_btl(p, p_out);
		break;
    default :   /*之前已经检查过TYPE*/
        break;        
    }
    
	if (ret == 0 && item_id && p_in->msg_type == CHAT_MSG_TYPE_ITEM1) {
        //用了道具且发送成功，则需扣除道(1枚)
        std::vector<db_del_item_request_t> rsp;
        ret = parse_db_del_item_request(p, item_id, 1, rsp);
        if(!ret) {
            ERROR_TLOG("user: %u, chat: reduce chat item failed!", p->id);
            return ret;
        }
        db_del_item(p, rsp);
    } 
    return ret;
}
