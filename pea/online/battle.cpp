#include <cstdio>
#include <libtaomee++/inet/pdumanip.hpp>
#include <libtaomee++/conf_parser/xmlparser.hpp>

extern "C" 
{
#include <glib.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <async_serv/service.h>
#include <async_serv/net_if.h>
}
#include <string>


#include "utils.hpp"
#include "battle.hpp"
#include "player.hpp"
#include "room.hpp"
#include "battle_switch.hpp"
#include "db_pet.hpp"

using namespace taomee;
using namespace std;

#define         max_battle_svr_num   100

typedef struct battle_server_t
{
    battle_server_t()
    {
        memset(this, 0, sizeof(battle_server_t));
    }
    char 	svr_name[16];	
    int16_t id;
}battle_server_t;

typedef int(*btlsvr_hdlr_t)(player_t* p, btl_proto_t* pkg);

struct btlsvr_handle_t
{
    btlsvr_handle_t(btlsvr_hdlr_t h = 0, uint32_t l = 0, uint8_t cmp = 0)
    {
        hdlr = h;
        len  = l;
        cmp_method = cmp;
    }
    btlsvr_hdlr_t   hdlr;
    uint32_t        len;	
    uint8_t         cmp_method;
};

#define SET_BTL_HANDLE(op_, len_, cmp_) \
    do { \
        if (btl_handles[online_proto_ ## op_ ## _cmd - btl_cmd_start].hdlr != 0) { \
            ERROR_TLOG("duplicate cmd=%u name=%s", online_proto_ ## op_ ## _cmd , #op_); \
            return false; \
        } \
        btl_handles[online_proto_ ## op_ ## _cmd - btl_cmd_start] = btlsvr_handle_t(btlsvr_ ## op_ ## _callback, len_, cmp_); \
    } while (0)


static btlsvr_handle_t btl_handles[10000];





battle_server_t battle_svrs[max_battle_svr_num];

int             battle_svr_cnt = 0;

int             battle_fds[max_battle_svr_num] = {-1};

static uint32_t btlpkgbuf[btl_proto_max_len];

///////////////////////////////////////////////////////////////////////////////////
bool is_alloc_player_cmd(uint32_t cmd)
{
    return ( (cmd >= online_proto_create_btl_cmd) && (cmd <= online_proto_syn_player_info_cmd));	
}


battle_grp_t* alloc_battle_grp()
{
    return reinterpret_cast<battle_grp_t*>(g_slice_alloc0( sizeof(battle_grp_t)));	
}


void free_battle_grp(battle_grp_t* grp)
{
    g_slice_free1(sizeof(battle_grp_t), grp);
}


void init_btl_proto_head(const player_t* p, userid_t id, void* header, int len, uint32_t cmd)
{
    btl_proto_t* pkg = reinterpret_cast<btl_proto_t*>(header);

    pkg->len = len;
    pkg->seq = ((p && p->battle_grp) ? p->battle_grp->id : 0);
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = id;
}

int send_to_battle_svr(player_t* p, const void* btlpkgbuf, uint32_t len, int index)
{
	const btl_proto_t* pkg = reinterpret_cast<const btl_proto_t*>(btlpkgbuf);
    if(!(index >=0 && index < max_battle_svr_num))
    {
        ERROR_TLOG("invalid battle fd index = %d", index);
        goto ERROR;
    }
    if(len > btl_proto_max_len)
    {
        ERROR_TLOG("invalid btlpkgbuf len = %u", len);
        goto ERROR;
    }
    if((battle_fds[index] == -1)) 
    {
        battle_fds[index] = connect_to_service(battle_svrs[index].svr_name, battle_svrs[index].id, 65535, 1);
    }
    if(battle_fds[index] == -1)
    {
        ERROR_TLOG("connect_to_service failed: index = %d", index);
        goto ERROR;
    }
	
	KDEBUG_LOG(p->id, "---------->>>>>SEND 2 BTL\t[cmd %u]", pkg->cmd);
    return net_send_ex(battle_fds[index],  btlpkgbuf, len);

ERROR:
    free_battle_grp(p->battle_grp);
    p->battle_grp = NULL;
    return 0;
}

int send_to_battle_svr(player_t* p, Cmessage *p_out, uint16_t cmd, int index)
{
	if(!(index >=0 && index < max_battle_svr_num)) {
        ERROR_TLOG("invalid battle fd index = %d", index);
        goto ERROR;
    }
    if((battle_fds[index] == -1)) {
        battle_fds[index] = connect_to_service(battle_svrs[index].svr_name, battle_svrs[index].id, 65535, 1);
    }
    if(battle_fds[index] == -1) {
        ERROR_TLOG("connect_to_service failed: index = %d", index);
        goto ERROR;
    }

	btl_proto_t head;
	head.len = sizeof(btl_proto_t);
    head.seq = ((p && p->battle_grp) ? p->battle_grp->id : 0);
    head.cmd = cmd;
    head.ret = 0;
    head.id  = p->id;
	if (p) {
		KDEBUG_LOG(p->id, "---------->>>>>SEND 2 BTL\t[cmd=%u, waitcmd=%u, fd=%u]", cmd, p->waitcmd, battle_fds[index]);
	} else {
		KDEBUG_LOG(p->id, "---------->>>>>SEND 2 BTL\t[cmd %u]", cmd);
	}
	
    return net_send_msg(battle_fds[index], (char *)&head, p_out);

ERROR:
	if (p && p->battle_grp) {
    	free_battle_grp(p->battle_grp);
    	p->battle_grp = NULL;
	}
    return 0;
}

int  get_btlsvr_fd_idx(int fd) 
{
    for (int i = 0; i < battle_svr_cnt; ++i) 
    {   
        if (fd == battle_fds[i]) 
        {   
            return i;
        }   
    }   
    return -1; 
}

void clear_players_battle(void* key, void* player, void* userdata)
{
    struct clear_players_battle_in_t
    {
        int btl_fd;	
    };
    clear_players_battle_in_t* p_btlinfo = reinterpret_cast<clear_players_battle_in_t*>(userdata);
    player_t* p = reinterpret_cast<player_t*>(player);
    if (p->battle_grp)
    {
        if (p->battle_grp->fd_idx < max_battle_svr_num)
        {
            if (battle_fds[p->battle_grp->fd_idx] == p_btlinfo->btl_fd) 
            {
                p->clear_waitcmd();
                free_battle_grp(p->battle_grp);
                p->battle_grp = NULL;
            }

        }

    }
}

void do_while_battle_svr_crashed(int btl_fd)
{
    g_hash_table_foreach(all_players, clear_players_battle, &btl_fd);	
}

bool init_battle_server_config(const char* xml)
{
    battle_svr_cnt = 0;
    memset(battle_svrs, 0, sizeof(battle_svrs));

    if(xml == NULL){
        return false;	
    }
    char err_msg[4096] = {0};
    xmlDocPtr 	doc = NULL;
    xmlNodePtr 	root = NULL;
    xmlNodePtr  server_node = NULL;

    doc = xmlParseFile(xml);
    if(doc == NULL)
    {
        sprintf(err_msg, "xml file %s is not exist", xml);
        throw XmlParseError(err_msg);
        return false;
    }

    root = xmlDocGetRootElement(doc);
    if(root == NULL)
    {
        xmlFreeDoc(doc);
        sprintf(err_msg, "xml file %s content is empty", xml);
        throw XmlParseError(err_msg);
        return false;
    }

    server_node = root->xmlChildrenNode;
    while(server_node)
    {
        if( xmlStrcmp(server_node->name, reinterpret_cast<const xmlChar*>("Server")) == 0)
        {
            if(battle_svr_cnt >= max_battle_svr_num)
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "too many battle servers: cnt= %u", battle_svr_cnt);
                throw XmlParseError(err_msg);
                return false;
            }
            string svr_name;
            get_xml_prop(svr_name, server_node, "SvrName");
            if(svr_name.length() >= 16)
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "invalid server name: %s idx = %d", svr_name.c_str(), battle_svr_cnt);	
                throw XmlParseError(err_msg);
                return false;
            }
            int16_t   svr_id = 0;
            get_xml_prop(svr_id, server_node, "SvrId");
            if(svr_id == 0)
            {
                xmlFreeDoc(doc);
                sprintf(err_msg, "invalid svr_id: %d idx = %d", svr_id, battle_svr_cnt);
                throw XmlParseError(err_msg);
                return false;
            }
            strncpy(battle_svrs[battle_svr_cnt].svr_name, svr_name.c_str(), sizeof(battle_svrs[battle_svr_cnt].svr_name));
            battle_svrs[battle_svr_cnt].id = svr_id;
            battle_svr_cnt++;
        }
        server_node = server_node->next;
    }
    xmlCleanupParser();
    xmlFreeDoc(doc);
    return true;	
}

bool final_battle_server_config()
{
    battle_svr_cnt = 0;
    memset(battle_svrs, 0, sizeof(battle_svrs));
    return true;	
}


bool init_btl_proto_handles()
{
    memset(btl_handles, 0, sizeof(btl_handles));	
    SET_BTL_HANDLE(create_btl, 4, cmp_must_ge);	
    SET_BTL_HANDLE(battle_end, 8, cmp_must_ge);
    SET_BTL_HANDLE(transmit_only, sizeof(cli_proto_t), cmp_must_ge);	

    return true;
}

bool final_btl_proto_handles()
{
    memset(btl_handles, 0, sizeof(btl_handles));
    return true;
}

void handle_battle_svr_return(btl_proto_t* data, uint32_t len, int bs_fd)
{
    player_t* p = get_player(data->id);

    if( !(data->cmd > btl_cmd_start && data->cmd < btl_cmd_max)){
        ERROR_TLOG("invalid btl_cmd userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);	
        return;
    }	
    if(p == NULL){
        ERROR_TLOG("can not find player in btl handle return userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);			
        return;
    }
    if(p->battle_grp == NULL){
        ERROR_TLOG("btl ended in  btl handle return userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);
        return;
    }

    if (p->battle_grp->fd_idx >= max_battle_svr_num)
    {
        ERROR_TLOG("btl ended in  btl handle return userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);
        return;
    }

    if(battle_fds[p->battle_grp->fd_idx] != bs_fd){
        ERROR_TLOG("invalid btl svr fd in  btl handle return userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);	
        return;	
    }
    if(p->battle_grp->id != data->seq &&  !is_alloc_player_cmd(data->cmd) ){
        ERROR_TLOG("invalid data seq in  btl handle return userid = %u, cmd = %u, seq = %u, ret = %u",
                data->id, data->cmd, data->seq, data->ret);	
        return;
    }
    if( data->ret)
    {
        send_error_to_player(p, data->ret);	
        return;
    }
    int err = -1;
    int cmd = data->cmd  - btl_cmd_start;
    uint32_t bodylen = data->len - sizeof(btl_proto_t);
    if( !(cmd >= 0 && cmd < (int)(sizeof(btl_handles)/sizeof(btl_handles[0])) ))
    {
        ERROR_TLOG("invalid cmd index of btl_handles cmd = %u", data->cmd);
        return ;
    }


    if ( ((btl_handles[cmd].cmp_method == cmp_must_eq) && (bodylen != btl_handles[cmd].len))
            || ((btl_handles[cmd].cmp_method == cmp_must_ge) && (bodylen < btl_handles[cmd].len)) ) 
    {
        if ((bodylen != 0) || (data->ret == 0)) 
        {
            ERROR_TLOG("invalid package pack_len=%u hope_len=%u cmpmethod=%d ret=%u uid=%u cmd =%u",
                    bodylen, btl_handles[cmd].len,
                    btl_handles[cmd].cmp_method, data->ret, data->id, cmd);
            goto ret; 
        }    
    }    

	KDEBUG_LOG(p->id, "<<<<<----------BTL RET\t[cmd %u]", cmd);
    err = btl_handles[cmd].hdlr(p, data);

ret:
    if (err) {
        close_client_conn(p->fd);
    }   
}


bool init_player_btl_grp(player_t* p)
{
    battle_grp_t* grp = p->battle_grp;
    if(grp == NULL)
    {
        p->battle_grp = alloc_battle_grp();
        p->battle_grp->fd_idx = p->id % battle_svr_cnt;
        return true;
    }
    return false;
}

bool final_player_btl_grp(player_t* p)
{
    battle_grp_t* grp = p->battle_grp;
    if(grp)
    {
        free_battle_grp(grp);
        grp = NULL;
        return true;
    }
    return false;	
}

int btlsvr_init_battle(player_t* p, uint32_t stage, uint32_t player_count, uint32_t cmd)
{
    int idx = sizeof(btl_proto_t);
    pack_h(btlpkgbuf, get_server_id(), idx);
    pack_h(btlpkgbuf, stage, idx);
    pack_h(btlpkgbuf, player_count, idx);
    pack_h(btlpkgbuf, p->id, idx);
    init_btl_proto_head(p, p->id, btlpkgbuf, idx, cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_create_btl_callback(player_t* p, btl_proto_t* bpkg)
{
    struct create_btl_rsp_t {
        uint32_t btl_id;
    }__attribute__((packed));

    create_btl_rsp_t* rsp = reinterpret_cast<create_btl_rsp_t*>(bpkg->body); 

    p->battle_grp->id = rsp->btl_id;
    p->clear_waitcmd();
    KDEBUG_LOG(p->id, "CREATED BTL\t[btlid=%u], rsp->btl_id");
    return pvp_room_tell_players_start_battle(p, p->battle_grp->id, p->battle_grp->fd_idx);
}

int btlsvr_transmit_only_callback(player_t* p, btl_proto_t* pkg)
{
	cli_proto_t* proto = reinterpret_cast<cli_proto_t*>(pkg->body);
	uint16_t cmd = proto->cmd;
	cmd = taomee::bswap(cmd);//解出转输的命令号
	if (cmd != cli_proto_chat_public_msg_cmd)
		return send_to_player(p, (char*)pkg->body, pkg->len - sizeof(btl_proto_t), 0);

	uint32_t sender_id;
	uint32_t sender_role_tm;
	int idx = 0;
	//如果是传递聊天消息
	unpack(pkg->body + sizeof(cli_proto_t), sender_id, idx);//解出发送者ID
	unpack(pkg->body + sizeof(cli_proto_t), sender_role_tm, idx);
	uint64_t key = gen_key_by_uid_role(sender_id, sender_role_tm);
	if (p->blacklist.find(key) != p->blacklist.end()) {
		//如果发送者在我的黑名单中，则我不接收
		return 0;
	}
	return send_to_player(p, (char*)pkg->body, pkg->len - sizeof(btl_proto_t), 0);
}

int btlsvr_battle_end_callback(player_t* p, btl_proto_t* bpkg)
{
    struct statistical_info_t
    {
        uint32_t uid;
        uint32_t team;
        uint8_t  nick[MAX_NICK_SIZE];
        uint32_t hit_rate;
        uint32_t damage;
        uint32_t exp;
        uint32_t extern_exp;
    }__attribute__((packed));

    struct battle_end_rsp_t {
        uint32_t btl_id;
        uint32_t player_win_flag;
        uint32_t count;
        statistical_info_t infos[0];
    }__attribute__((packed));	

    battle_end_rsp_t* rsp = reinterpret_cast<battle_end_rsp_t*>(bpkg->body);

    if(p->battle_grp && p->battle_grp->id == rsp->btl_id)
    {
        btlsw_pvp_end_battle_in out;
        out.online_id = get_server_id(); // TODO(zog): 应该去掉 (哪个online发的sw知道)
        out.room_id = p->room.room_id;
        send_to_battle_switch(p, btlsw_pvp_end_battle_cmd, &out);

        free_battle_grp(p->battle_grp);
        p->battle_grp = NULL;

        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, rsp->btl_id, idx);
        pack(pkgbuf, rsp->player_win_flag, idx);
        pack(pkgbuf, rsp->count, idx);
        for(uint32_t i =0 ; i< rsp->count; i++)
        {
            statistical_info_t* info = &rsp->infos[i];	
            pack(pkgbuf, info->uid, idx);
            pack(pkgbuf, info->team, idx);
            pack(pkgbuf, info->nick, sizeof(info->nick), idx);
            pack(pkgbuf, info->hit_rate, idx);
            pack(pkgbuf, info->damage, idx);
            pack(pkgbuf, info->exp, idx);
            pack(pkgbuf, info->extern_exp, idx);
        }


        init_cli_proto_head(pkgbuf, p, cli_proto_battle_end_cmd, idx);
        send_to_player(p, (char*)pkgbuf, idx, 0);
    }
    return 0;
}	

int btlsvr_syn_player_info(player_t* p, uint32_t battle_id, uint32_t fd_idx)
{

	/* TODO(zog): 没有宠物应该用错误码返回,
	 * 并且要放在进入房间的时候就检查,
	 * 而不要在发起对战的时候再检查, 而就算在这个函数做, 也要放在最开始, 避免无谓的pack_h */

    if (NULL == p->fight_pet)
    {
    	ERROR_TLOG("p->fight_pet = NULL, p = %u", p->id);
		return -1;
    }

    p->battle_grp = alloc_battle_grp();
    p->battle_grp->id = battle_id;
    p->battle_grp->fd_idx = fd_idx;

    int idx = sizeof(btl_proto_t);
    pack_h(btlpkgbuf, get_server_id(), idx);
    pack_h(btlpkgbuf, battle_id, idx);
    pack_h(btlpkgbuf, p->id, idx);
    pack(btlpkgbuf, p->nick, sizeof(p->nick), idx);
	pack_h(btlpkgbuf, (uint32_t)(p->room.team_id), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_HP), idx);
    pack_h(btlpkgbuf, p->get_player_attr_value(OBJ_ATTR_EXP), idx);
    pack_h(btlpkgbuf, p->get_player_attr_value(OBJ_ATTR_LEVEL), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_MAGIC), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_AGILITY), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_LUCK), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_ANGER), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_ATK), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_DEF), idx);
	uint32_t strength = p->get_merge_attr_value(OBJ_ATTR_STRENGTH) == 0 ? 100 :  p->get_merge_attr_value(OBJ_ATTR_STRENGTH);
    pack_h(btlpkgbuf, strength, idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_HAPPY), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_SOUL), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_INTENSITY), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_CRIT), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_DOUBLE), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_SPEED), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_CRIT_DAMAGE), idx);
    pack_h(btlpkgbuf, p->get_merge_attr_value(OBJ_ATTR_TENACITY), idx);
    pack_h(btlpkgbuf, p->eye_model, idx);
    pack_h(btlpkgbuf, p->resource_id, idx);

	
	btl_pet_info_t pet_info;
	p->fight_pet->export_info(&pet_info);
    byte_array_t ba;
    pet_info.write_to_buf_ex(ba);
    pack(btlpkgbuf, ba.get_buf(), ba.get_postion(), idx);

	uint32_t skill_count = 4;

	pack_h(btlpkgbuf, skill_count, idx);

	pack_h(btlpkgbuf, p->fight_pet->m_skill.m_skill_1, idx);
	pack_h(btlpkgbuf, (uint32_t)1, idx);

	pack_h(btlpkgbuf, p->fight_pet->m_skill.m_skill_2, idx);
	pack_h(btlpkgbuf, (uint32_t)1, idx);

	pack_h(btlpkgbuf, p->fight_pet->m_skill.m_skill_3, idx);
	pack_h(btlpkgbuf, (uint32_t)1, idx);

	pack_h(btlpkgbuf, p->fight_pet->m_skill.m_uni_skill, idx);
	pack_h(btlpkgbuf, (uint32_t)1, idx);

    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_syn_player_info_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_load_resource(player_t* p)
{
    int idx = sizeof(btl_proto_t);
    pack_h(btlpkgbuf, p->battle_grp->id, idx);
    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_load_resource_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_get_btl_user_list(player_t* p)
{
    int idx = sizeof(btl_proto_t);	
    pack_h(btlpkgbuf, p->battle_grp->id, idx);
    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_get_btl_user_list_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_move(player_t* p, uint32_t x, uint32_t y, uint32_t dir, uint32_t angle, uint32_t gun_angle)
{
    int idx = sizeof(btl_proto_t);
    pack_h(btlpkgbuf, x, idx);
    pack_h(btlpkgbuf, y, idx);
    pack_h(btlpkgbuf, dir, idx);
    pack_h(btlpkgbuf, angle, idx);
    pack_h(btlpkgbuf, gun_angle, idx);

    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_move_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_attack(player_t* p, uint32_t gun_angle, uint32_t muzzle_velocity)
{
    int idx = sizeof(btl_proto_t);	

    pack_h(btlpkgbuf, gun_angle, idx);
    pack_h(btlpkgbuf, muzzle_velocity, idx);

    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_attack_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_read_attack(player_t* p)
{
    int idx = sizeof(btl_proto_t);
    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_ready_attack_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}


int btlsvr_player_cancel_battle(player_t* p)
{
    int idx = sizeof(btl_proto_t);

    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_cancel_battle_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_progress_bar(player_t* p,  uint32_t progress)
{
    int idx = sizeof(btl_proto_t);

    pack_h(btlpkgbuf, p->id, idx);
    pack_h(btlpkgbuf, progress, idx);

    init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_progress_bar_cmd);
    return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_cancel_turn(player_t* p)
{
	int idx = sizeof(btl_proto_t);
	
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_cancel_turn_cmd);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_player_finish_turn(player_t* p)
{
	int idx = sizeof(btl_proto_t);

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_player_finish_turn_cmd);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}

int btlsvr_select_skill(player_t* p, uint32_t skill_id, uint32_t skill_lv)
{
	int idx = sizeof(btl_proto_t);

	pack_h(btlpkgbuf, skill_id, idx);
	pack_h(btlpkgbuf, skill_lv, idx);

	init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_select_skill_cmd);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);	
}

int btlsvr_get_player_skill_list(player_t* p)
{
	int idx = sizeof(btl_proto_t);	
	
	init_btl_proto_head(p, p->id, btlpkgbuf, idx, online_proto_get_btl_user_list_cmd);
	return send_to_battle_svr(p, btlpkgbuf, idx, p->battle_grp->fd_idx);
}
