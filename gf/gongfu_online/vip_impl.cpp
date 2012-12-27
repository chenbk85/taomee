#include <libtaomee/project/utilities.h>
#include <libtaomee++/utils/md5.h>
#include "player.hpp"
#include "fwd_decl.hpp"
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "cli_proto.hpp"
#include "item.hpp"
#include "warehouse.hpp"
#include "dbproxy.hpp"
#include "vip_impl.hpp"
#include "utils.hpp"
#include "global_data.hpp"

using namespace taomee;

//------------------------------------------------------------------
// helper functions
//------------------------------------------------------------------
int send_request_to_vipsvr_2(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len)
{
	static uint8_t vipbuf[vipproto_max_len];

	if (vipsvr_fd == -1) {
		/* TODO: 支持 connect_to_service() */
		vipsvr_fd = connect_to_svr(config_get_strval("vipsvr_ip"), config_get_intval("vipsvr_port", 0), 65535, 1);
	}

	if ((vipsvr_fd == -1) || (body_len > (sizeof(vipbuf) - sizeof(vip_proto_t)))) {
		ERROR_LOG("send to vipsvr failed: fd=%d len=%d", vipsvr_fd, body_len);
		if (p) {
			if (p->waitcmd == cli_proto_login
					|| p->waitcmd == cli_proto_enter_own_home || p->waitcmd == cli_proto_enter_other_home ) {
				return -1;
			}
			return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
		}
		return 0;
	}

	vip_proto_t* pkg = reinterpret_cast<vip_proto_t*>(vipbuf);
	pkg->len = sizeof(vip_proto_t) + body_len;
	pkg->seq = (p ? ((p->fd << 16) | p->waitcmd) : 0);
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->id  = id;
	memcpy(pkg->body, body, body_len);
    KDEBUG_LOG(p ? p->id : 0, "SEND to STORE SERVER uid=[%u] pkglen=[%u]\t", id, pkg->len);
	return net_send(vipsvr_fd, vipbuf, pkg->len);
}

void handle_vipsvr_return(vip_proto_t* pkg, uint32_t pkglen)
{
	uint32_t waitcmd = pkg->seq & 0xFFFF;
	int      connfd  = pkg->seq >> 16;

	player_t* p = get_player_by_fd(connfd);
	if (!p) {
		ERROR_LOG("connection has been closed: uid=%u cmd=%u %u fd=%d",
				pkg->id, pkg->cmd, waitcmd, connfd);
		return;
	}

#define PROC_STORE_PKG(proto, func) \
    case proto: func(p, pkg->body, pkglen - sizeof(store_proto_t), pkg->ret); break

    switch (pkg->cmd) {
	PROC_STORE_PKG(vip_proto_swap_vip_qualify, vipsvr_swap_vip_qualify_callback);

#undef PROC_STORE_PKG 
	default:
		ERROR_LOG("unsupported vipsvr cmd=%u id=%u waitcmd=%d",
				pkg->cmd, p->id, p->waitcmd);
		break;
    }
}

/**
 * @brief swap vip qualify 
 */
int do_swap_vip_qualify(player_t* p, userid_t id, uint32_t day)
{
    const int verify_len = 32;
    char verify_code[verify_len] = {0}; 
    //get md5 by send_body
	static utils::MD5 md5;
	const uint32_t verify_buf_len = 1024;
	static char verify_buf[verify_buf_len] = {0};
	
	uint32_t s_len = 0;
	static uint16_t m_channel_id = config_get_intval("channel_id_vip", 90);
	static char *m_security_code = config_get_strval("security_code_vip");//, "12345678");
	memset(verify_buf, 0x00, verify_buf_len);
	sprintf(verify_buf, "channelId=%d&securityCode=%s&data=", m_channel_id, m_security_code);
	s_len = strlen(verify_buf);

	int idx = 0;
	pack_h(verify_buf + s_len, day, idx);
	md5.reset();
	md5.update(verify_buf, s_len + idx);
	memcpy(verify_code, md5.toString().c_str(), verify_len);

	KDEBUG_LOG(p ? p->id : 0, "SWAP VIP QUALITY\t[channel_id:%d security_code:%s day:%u ip:%04x]", m_channel_id, m_security_code, day, p ? p->fdsess->remote_ip : 0);
	//send pack
	idx = 0;
	pack_h(vippkgbuf_2, m_channel_id, idx);
	pack(vippkgbuf_2, verify_code, verify_len, idx);
	pack_h(vippkgbuf_2, day, idx);

	return send_request_to_vipsvr_2(p, id, vip_proto_swap_vip_qualify, vippkgbuf_2, idx);
}

/**
 * @brief swap vip qualify  callback
 */
int vipsvr_swap_vip_qualify_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret)
{
    if (ret) {
        ERROR_LOG("VIP QUALITY RESULT [uid:%u ret:%u]", p->id, ret);
    }
	CHECK_VIP_SVR_ERR(p, ret);

	return 0; //send_header_to_player(p, p->waitcmd, 0, 0);
}


//------------------------------------------------------------
//
//------------------------------------------------------------
bool      init_player_vip(player_t* p)
{
	p->vip_level = 0;
	p->vip_base_point = 0;
    p->vip_sword_value = 0;
	p->vip_total_point = 0;
	p->vip_current_show_point = 0;
	p->extern_warehouse_grid_count = 0;
	p->extern_item_bag_grid_count = 0;
	p->vip_begin_time = 0;
	p->vip_end_time = 0;
	return true;
}

bool      final_player_vip(player_t* p)
{
	p->vip_level = 0;
    p->vip_base_point = 0;
    p->vip_sword_value = 0;
	p->vip_total_point = 0;
	p->vip_current_show_point = 0;
    p->extern_warehouse_grid_count = 0;
    p->extern_item_bag_grid_count = 0;
    p->vip_end_time = 0;
	p->vip_begin_time = 0;
	return true;	
}

bool      send_player_vip_info(player_t* p)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf,  p->vip, idx);
	pack(pkgbuf,  p->vip_level, idx);
	pack(pkgbuf,  p->vip_end_time, idx);
	pack(pkgbuf,  p->vip_total_point, idx);

	init_cli_proto_head(pkgbuf, p,  cli_proto_user_vip_info, idx);
	return send_to_player(p, pkgbuf, idx, 0);
}

uint32_t  add_player_vip_level(player_t* p, uint32_t level)
{
	if( p->vip_level + level <= MAX_VIP_LEVEL)
	{
		p->vip_level += level;
		//p->extern_warehouse_grid_count =  vip_config_data_mgr::getInstance()->get_add_warehouse_count_by_level(p->vip_level);
		p->extern_warehouse_grid_count =  get_vip_config_data_mgr()->get_add_warehouse_count_by_level(p->vip_level);
		//p->extern_item_bag_grid_count  =  vip_config_data_mgr::getInstance()->get_add_item_bag_count_by_level(p->vip_level);
		p->extern_item_bag_grid_count  =  get_vip_config_data_mgr()->get_add_item_bag_count_by_level(p->vip_level);
	}
	return p->vip_level;
}

uint32_t  get_player_vip_point(player_t* p)
{
	return p->vip_total_point;
}
uint32_t  get_player_vip_level(player_t* p)
{
	return p->vip_level;
}

uint32_t  get_player_extern_warehouse_gird_count(player_t* p)
{
	return p->extern_warehouse_grid_count;
}
uint32_t  get_player_extern_item_bag_grid_count(player_t* p)
{
	return p->extern_item_bag_grid_count;
}

uint32_t  get_player_total_item_bag_grid_count(player_t* p)
{
	if(is_vip_player(p))
	{
		return max_pack_items_cnt + get_player_extern_item_bag_grid_count(p);
	}
	return max_pack_items_cnt;
}

void set_player_vip_info(player_t *p, uint32_t is_vip, uint32_t month_cnt, uint32_t vip_begin_time, uint32_t vip_end_time)
{
    if (!is_vip_year_player(p) && taomee::test_bit_on(is_vip, year_vip)) {
         p->vip_base_point += 100;
    }
	p->vip = is_vip;
	p->vip_begin_time = vip_begin_time;
	p->vip_end_time = vip_end_time;
	
    /// ------------- (乐翻翻) vip充值活动下架需要注释 ---------------------
    if (month_cnt - p->vip_months_cnt > 0 && (is_in_active_time_section(33) == 0)) {
        p->act_record[act_record_pos] += month_cnt - p->vip_months_cnt;
    }
    p->vip_months_cnt = month_cnt;
	calc_player_vip_info(p);
}

void      init_player_vip_info(player_t *p ,  get_player_rsp_t* rsp)
{
	p->vip   =  rsp->vip;
	p->vip_months_cnt   =  rsp->vip_month_cnt;
	p->vip_begin_time = rsp->vip_begin_tm;
	p->vip_end_time =   rsp->vip_end_tm;
	p->vip_base_point =  rsp->vip_point;
    p->vip_sword_value = rsp->sword_value;
	calc_player_vip_info(p, false);
}

void      add_player_vip_point(player_t *p,  uint32_t add_vip_point)
{
	p->vip_base_point += add_vip_point;
	calc_player_vip_info(p);
}

void  set_player_vip_point(player_t *p, uint32_t vip_point)
{
	p->vip_base_point = vip_point;

    if (is_vip_year_player(p)) {
        p->vip_base_point += 100;
    }
	calc_player_vip_info(p);
}

void      calc_player_vip_info(player_t *p, bool notify_client)
{
	uint32_t active_days = 0;
	if(is_vip_player(p)){
		time_t  now = time(NULL);
		if(now > p->vip_begin_time){
			active_days = (now - p->vip_begin_time) / (60*60*24);
		}
        if (is_vip_year_player(p)) {
            active_days = active_days * 3;
        }
	}
	p->vip_total_point = p->vip_base_point + p->vip_sword_value + active_days;
	//如果没有开通过VIP侠士
	if( !has_player_opened_vip(p) ){
		p->vip_level = 0;
	}
	else
	{
		p->vip_level = calc_player_vip_level( get_vip_config_data_mgr(),  p->vip_total_point, &p->vip_current_show_point);
	}
	//p->extern_warehouse_grid_count = vip_config_data_mgr::getInstance()->get_add_warehouse_count_by_level(p->vip_level); 
	p->extern_warehouse_grid_count = get_vip_config_data_mgr()->get_add_warehouse_count_by_level(p->vip_level); 
	//p->extern_item_bag_grid_count  = vip_config_data_mgr::getInstance()->get_add_item_bag_count_by_level(p->vip_level);
	p->extern_item_bag_grid_count = get_vip_config_data_mgr()->get_add_item_bag_count_by_level(p->vip_level); 

	if(notify_client){
		send_player_vip_info(p);
	}					

    if (p->vip_level > 0) {
        do_stat_log_universal_interface_2(stat_log_vip_level_distribution, 0, p->id, p->vip_level);
    }
}

int db_set_extern_grid_count_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	return 0;
}
