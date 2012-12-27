/*
 * =====================================================================================
 *
 *       Filename:  decorate.cpp
 *
 *    Description:  :wq
 *
 *
 *        Version:  1.0
 *        Created:  12/05/2011 15:14:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Saga (), saga@taomee.com
 *        Company:  TaoMee, Inc. ShangHai CN.
 *
 * =====================================================================================
 */
#include "decorate.hpp"
#include "player.hpp"
#include "home.hpp"
#include "home_impl.hpp"
#include "utils.hpp"


HomeDecorate::HomeDecorate()
{
    this->db_update_flag = false;
}

HomeDecorate::~HomeDecorate()
{
}

void HomeDecorate::add_decorate_to_map(decorate_t *pdata)
{
    if (pdata == NULL) return;

    std::map<uint32_t, decorate_t>::iterator it = decorate_map_.find(pdata->id);
    if (it != decorate_map_.end()) return;

    decorate_map_[pdata->id] = *pdata;
}

int HomeDecorate::pack_all_decorate(void *buf)
{
    int idx = 0;
    pack(buf, (uint32_t)(decorate_map_.size()), idx);
    TRACE_LOG("decorate cnt [%u]", (uint32_t)(decorate_map_.size()));
    std::map<uint32_t, decorate_t>::iterator it = decorate_map_.begin();
    for (; it != decorate_map_.end(); ++it) {
        pack(buf, it->second.id, idx);
        pack(buf, it->second.tm, idx);
        pack(buf, it->second.lv, idx);
        pack(buf, it->second.status, idx);
        TRACE_LOG("decorate [%u %u %u %u]", it->second.id, it->second.tm, it->second.lv, it->second.status);
    }

    return idx;
}

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//gobal func
int get_decorate_list_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	if (p->CurHome && p->CurHome->IsSummerHome() && p->CurHome->GetHomeDetail()) {
		if (p->CurHome->GetHomeDetail()->p_decorate->is_db_update()) {
			int idx = sizeof(home_proto_t);
	     	idx += sizeof(cli_proto_t);
            pack(pkgbuf, p->CurHome->OwnerUserId, idx);
            pack(pkgbuf, p->CurHome->OwnerRoleTime, idx);
            idx += p->CurHome->GetHomeDetail()->p_decorate->pack_all_decorate(pkgbuf + idx);
	    	init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
	    	init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_get_decorate_list, 0, idx-sizeof(home_proto_t));
			send_to_player(p, pkgbuf, idx, 1);
			return 0;
		} else {
			db_get_decorate_list(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime);
			return 0;
		}
	}
	return 0;
}

int db_get_decorate_list(Player* p, uint32_t uid, uint32_t utm)
{
	return send_request_to_db(p, uid, utm, dbproto_get_decorate_list, 0, 0);
}

int db_get_decorate_list_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	if (!(p->CurHome) || !(p->CurHome->GetHomeDetail())) {
		p->waitcmd = 0;
		ERROR_LOG("%u get_decorate_list", p->id);
		return 0;
	}
	
	uint32_t cnt = 0;
    int idx = 0;
    unpack_h(body, cnt, idx);
    CHECK_VAL_EQ(bodylen, cnt * sizeof(decorate_t) + 4);

    p->CurHome->GetHomeDetail()->p_decorate->set_db_update_flag();

    TRACE_LOG("decorate list [%u %u %u]", p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, cnt);

    idx = sizeof(home_proto_t);
    idx += sizeof(cli_proto_t);
    pack(pkgbuf, p->CurHome->OwnerUserId, idx);
    pack(pkgbuf, p->CurHome->OwnerRoleTime, idx);
    pack(pkgbuf, cnt, idx);
    decorate_t *decorate = reinterpret_cast<decorate_t*>((uint8_t*)(body) + 4);
    for (uint32_t i = 0; i < cnt; i++) {
        decorate_t pdata;
        pdata.id    =   decorate[i].id;
        pdata.lv    =   decorate[i].lv;
        pdata.tm    =   decorate[i].tm;
        pdata.status=   decorate[i].status;
        p->CurHome->GetHomeDetail()->p_decorate->add_decorate_to_map(&pdata);

        pack(pkgbuf, decorate[i].id, idx);
        pack(pkgbuf, decorate[i].tm, idx);
        pack(pkgbuf, decorate[i].lv, idx);
        pack(pkgbuf, decorate[i].status, idx);
        TRACE_LOG("decorate info [%u %u %u %u]", pdata.id, pdata.lv, pdata.tm, pdata.status);
    }

    init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_get_decorate_list, 0, idx-sizeof(home_proto_t));
    send_to_player(p, pkgbuf, idx, 1);

	return 0;
}

int set_decorate_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	if (p->CurHome && p->CurHome->IsSummerHome() && p->CurHome->GetHomeDetail()) {
        uint32_t decorate_id = 0;
        int idx = 0;
        unpack_h(body, decorate_id, idx);

        db_set_decorate(p, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, decorate_id);
	}
	return 0;
}

int db_set_decorate(Player* p, uint32_t uid, uint32_t utm, uint32_t decorate_id)
{
    int idx = 0;
    pack_h(dbpkgbuf, decorate_id, idx);
	return send_request_to_db(p, uid, utm, dbproto_set_decorate, dbpkgbuf, idx);
}

int db_set_decorate_callback(Player* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret)
{
	CHECK_DBERR(p, ret);
	if (!(p->CurHome) || !(p->CurHome->GetHomeDetail())) {
		p->waitcmd = 0;
		ERROR_LOG("%u set_decorate", p->id);
		return 0;
	}

    decorate_t decorate;
    int idx = 0;
    unpack_h(body, decorate.id, idx);
    unpack_h(body, decorate.lv, idx);
    unpack_h(body, decorate.tm, idx);

    p->CurHome->GetHomeDetail()->p_decorate->add_decorate_to_map(&decorate);

    idx = sizeof(home_proto_t);
    idx += sizeof(cli_proto_t);
    pack(pkgbuf, decorate.id, idx);
    pack(pkgbuf, decorate.tm, idx);
    pack(pkgbuf, decorate.lv, idx);
    pack(pkgbuf, static_cast<uint32_t>(0), idx);
    init_home_proto_head_full(pkgbuf, home_transmit_only, idx, 0);
    init_cli_proto_head_full(pkgbuf + sizeof(home_proto_t), p->id, cli_proto_set_decorate, 0, idx-sizeof(home_proto_t));
    send_to_player(p, pkgbuf, idx, 1);
    return 0;
}

int add_decorate_lv_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	if (p->CurHome && p->CurHome->IsSummerHome() && p->CurHome->GetHomeDetail()) {
        uint32_t decorate_id = 0, flag = 0, itemcnt = 0;
        int idx = 0;
        unpack_h(body, decorate_id, idx);
        unpack_h(body, flag, idx);
        unpack_h(body, itemcnt, idx);

        TRACE_LOG("add decorate lv in [%u %u %u]", decorate_id, flag, itemcnt);

        CHECK_VAL_EQ(bodylen, itemcnt * sizeof(uint32_t) + 12);

        uint32_t del_item_arr[4] = {0}; // make sure array is enough
        for (uint32_t i = 0; i < itemcnt; i++) {
            unpack_h(body, del_item_arr[i], idx);
        }

        uint32_t fillingin = 0;
        // 1--->增加进度 2--->完成装饰
        if (flag == 1) {

            uint32_t cur_lv = p->CurHome->GetHomeDetail()->p_decorate->get_decorate_lv(decorate_id);
            if (cur_lv ==0 || cur_lv >= 100) {
                //error_code
                return send_header_to_player(p, p->waitcmd, cli_err_cannot_heap, 1);
            }

            if (!(p->CurHome->IsOwner(p)) && p->lv < 30) {
                return send_header_to_player(p, p->waitcmd, cli_err_cannot_heap, 1);
            }

            uint32_t add_lv = 2;
            if (p->is_vip_player()) {
                add_lv = 4;
            }

            p->CurHome->GetHomeDetail()->p_decorate->add_decorate_lv(decorate_id, add_lv);

            fillingin = add_lv;
        } else if (flag == 2) {
            if (!(p->CurHome->IsOwner(p)) || 
                p->CurHome->GetHomeDetail()->p_decorate->get_decorate_lv(decorate_id) < 100) {
                //error_code
                return send_header_to_player(p, p->waitcmd, cli_err_cannot_heap, 1);
            }

            p->CurHome->GetHomeDetail()->p_decorate->set_decorate_status(decorate_id, 1);

            fillingin = 1;
        } else {
            return 0;
        }
        db_add_decorate_lv(p, decorate_id, flag, fillingin, itemcnt, del_item_arr);

        idx = sizeof(home_proto_t);
        pack_h(pkgbuf, flag, idx);
        pack_h(pkgbuf, decorate_id, idx);
        pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
        uint32_t lv = p->CurHome->GetHomeDetail()->p_decorate->get_decorate_lv(decorate_id);
        pack_h(pkgbuf, lv, idx);
        uint32_t status = p->CurHome->GetHomeDetail()->p_decorate->get_decorate_status(decorate_id);
        pack_h(pkgbuf, status, idx);
        pack_h(pkgbuf, itemcnt, idx);
        for (uint32_t i = 0; i < itemcnt; i++) {
            pack_h(pkgbuf, del_item_arr[i], idx);
        }

        init_home_proto_head(pkgbuf, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
	}
	return 0;
}

int db_add_decorate_lv(Player* p, uint32_t decorate_id, uint32_t flag, uint32_t fillingin, uint32_t itemcnt, uint32_t *del_item_arr)
{
    int idx = 0;
    pack_h(dbpkgbuf, flag, idx);
    pack_h(dbpkgbuf, decorate_id, idx);
    pack_h(dbpkgbuf, fillingin, idx);
    pack_h(dbpkgbuf, itemcnt, idx);
    for (uint32_t i = 0; i < itemcnt; i++) {
        pack_h(dbpkgbuf, del_item_arr[i], idx);
    }
    TRACE_LOG("decorate add to db [%u %u]", p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime);
    return send_request_to_db(0, p->CurHome->OwnerUserId, p->CurHome->OwnerRoleTime, dbproto_add_decorate_lv, dbpkgbuf, idx);
}

int get_decorate_reward_cmd(Player * p, uint8_t * body, uint32_t bodylen)
{
	if (p->CurHome && p->CurHome->GetHomeDetail()) {
        uint32_t decorate_id = 0;
        int idx = 0;
        unpack_h(body, decorate_id, idx);

        uint32_t status = p->CurHome->GetHomeDetail()->p_decorate->get_decorate_status(decorate_id);
        if (!(p->CurHome->IsOwner(p)) || status != 1) {
            return send_header_to_player(p, p->waitcmd, cli_err_system_error, 1);
        }

        status = 2;
        p->CurHome->GetHomeDetail()->p_decorate->set_decorate_status(decorate_id, status);

        db_add_decorate_lv(p, decorate_id, 2, status, 0, NULL);

        idx = sizeof(home_proto_t);
        pack_h(pkgbuf, decorate_id, idx);
        pack_h(pkgbuf, static_cast<uint32_t>(0), idx);
        uint32_t lv = p->CurHome->GetHomeDetail()->p_decorate->get_decorate_lv(decorate_id);
        pack_h(pkgbuf, lv, idx);
        pack_h(pkgbuf, status, idx);

        init_home_proto_head(pkgbuf, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
	}
	return 0;
}

