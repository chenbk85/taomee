
extern "C" {
#include <stdint.h>
#include <async_serv/mcast.h>
}
#include <libtaomee++/inet/pdumanip.hpp>


#include "limit.hpp"
#include "mcast_proto.hpp"
#include "global_data.hpp"

using namespace taomee;


void add_red_banner(player_t *p, uint32_t cnt)
{
	g_limit_data_mrg.add_item_cnt(red_banner_id, cnt, true, 4);
}

void add_blue_banner(player_t *p, uint32_t cnt)
{
	g_limit_data_mrg.add_item_cnt(blue_banner_id, cnt, true, 4);
}

uint32_t get_red_banner_cnt()
{
	return g_limit_data_mrg.get_item_cnt(red_banner_id);
}

uint32_t get_blue_banner_cnt()
{
	return g_limit_data_mrg.get_item_cnt(blue_banner_id);
}

bool limit_data_mrg::init()
{
	m_datas.clear();
	return true;
}

bool limit_data_mrg::final()
{
	m_datas.clear();
	return true;
}

bool limit_data_mrg::add_limit_data(global_limit_data_t& data)
{
	m_datas.insert(limit_data_map_t::value_type(data.item_id, data));
	return true;
}

global_limit_data_t* limit_data_mrg::get_limit_data(uint32_t item_id)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	return &(it->second);
}

uint32_t limit_data_mrg::reduce_item_cnt(uint32_t item_id,uint32_t cnt, bool broad_cast)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	global_limit_data_t* p_data = &(it->second);
	p_data->left_cnt = p_data->left_cnt > cnt ? p_data->left_cnt - cnt : 0;

	if (broad_cast) {
		consume_global_data(item_id, cnt);
	}
	return p_data->left_cnt;
}

uint32_t limit_data_mrg::reduce_item_cnt_with_order(uint32_t item_id,uint32_t cnt, uint32_t order, bool broad_cast)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	global_limit_data_t* p_data = &(it->second);
	if (p_data->order > order) {
		return 0;
	}
	p_data->left_cnt = p_data->left_cnt > cnt ? p_data->left_cnt - cnt : 0;

	if (broad_cast) {
		consume_global_data(item_id, cnt);
	}
	return cnt;
}

void limit_data_mrg::order_datas()
{
	limit_data_map_t::iterator it = m_datas.begin();
	for (; it != m_datas.end(); ++it) {
		global_limit_data_t* p_data1 = &(it->second);

		uint32_t order = 1;

		TRACE_LOG("s %u %u %u", p_data1->item_id, p_data1->type, p_data1->left_cnt);
		limit_data_map_t::iterator it_2 = m_datas.begin();
		for (; it_2 != m_datas.end(); ++it_2) {
			global_limit_data_t* p_data2 = &(it_2->second);
			if (p_data2->type == p_data1->type && p_data2 != p_data1) {
				if (p_data2->left_cnt > p_data1->left_cnt) {
					order ++;
					TRACE_LOG("p %u %u %u", p_data2->item_id, p_data2->type, p_data2->left_cnt);
				}
			}
		}
		p_data1->order = order;
		TRACE_LOG("d %u %u", p_data1->item_id, p_data1->order);
	}
}


uint32_t limit_data_mrg::add_item_cnt(uint32_t item_id,uint32_t cnt, bool broad_cast, uint32_t reset_type)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		global_limit_data_t data;
		data.type = reset_type;
		data.item_id = item_id;
		data.left_cnt = cnt;
		TRACE_LOG("%u %u ", item_id, cnt);
		g_limit_data_mrg.add_limit_data(data);
	} else {
		global_limit_data_t* p_data = &(it->second);
		p_data->left_cnt += cnt;
	}
	if (broad_cast) {
		consume_global_data(item_id, cnt, 1, reset_type);
	}
	return 0;
}

int limit_data_mrg::reset_global_limit_data(uint32_t type)
{
    limit_data_map_t::iterator it = m_datas.begin();
	for (; it != m_datas.end(); ++it) {
		if (it->second.type == type) {
            reset_global_data(type, 7);
            break;
		}
	}
    return 0;
}

uint32_t limit_data_mrg::update_item_cnt(uint32_t item_id,uint32_t cnt)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		global_limit_data_t data;
		data.type = 2;
		data.item_id = item_id;
		data.left_cnt = cnt;
		add_limit_data(data);
		return 0;
	}
	global_limit_data_t* p_data = &(it->second);
	p_data->left_cnt = cnt;
	return p_data->left_cnt;
}

uint32_t limit_data_mrg::get_item_cnt(uint32_t item_id)
{
	limit_data_map_t::iterator it = m_datas.find(item_id);
	if (it == m_datas.end()) {
		return 0;
	}
	global_limit_data_t* p_data = &(it->second);
	return p_data->left_cnt;
}

int limit_data_mrg::pack_limit_data(uint8_t* buf, uint32_t type)
{
	int idx = 0;
	uint32_t cnt = 0;

	pack(buf, type, idx);
	int cnt_idx = idx;
	idx += 4;
	
	limit_data_map_t::iterator it = m_datas.begin();
	for (; it != m_datas.end(); ++it) {
		if (it->second.type == type || !type) {
			//pack(buf, it->second.type, idx);
			pack(buf, it->second.item_id, idx);
			pack(buf, it->second.left_cnt, idx);
			TRACE_LOG("%u %u", it->second.item_id, it->second.left_cnt);
			cnt ++;
		}
	}
	pack(buf, cnt, cnt_idx);
	return idx;
}

void limit_data_mrg::broad_limit_data(global_limit_data_t* p_data)
{
struct limit_data_mcast_header {
	uint32_t type;//0 reset; 1 update
	uint32_t cnt;
};

struct limit_data_mcast_item {
	uint32_t item_id;
	uint32_t item_cnt;
};
	
	int idx = sizeof(mcast_pkg_t);

	if (p_data) {
		pack(pkgbuf, 1, idx);
		pack(pkgbuf, p_data->item_id, idx);
		pack(pkgbuf, p_data->left_cnt, idx);
	} else {
		uint32_t cnt = m_datas.size();
		pack(pkgbuf, cnt, idx);

		limit_data_map_t::iterator it = m_datas.begin();
		for (; it != m_datas.end(); ++it) {
			pack(pkgbuf, it->second.item_id, idx);
			pack(pkgbuf, it->second.left_cnt, idx);
		}
	}
    init_mcast_pkg_head(pkgbuf, 60006, 0);
    send_mcast_pkg(pkgbuf, idx);
}

/**
  * @brief  get global limit info
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int get_global_limit_data_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t type = 0;

	unpack(body, type, idx);

	idx = sizeof(cli_proto_t);
	idx += g_limit_data_mrg.pack_limit_data(pkgbuf + idx, type);
	init_cli_proto_head_full(pkgbuf, p->id, p->seqno, p->waitcmd, idx, 0);
	return send_to_player(p, pkgbuf, idx, 1);
}

