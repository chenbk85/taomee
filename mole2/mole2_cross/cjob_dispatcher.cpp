#include <cerrno>
#include <cstdlib>
#include <cstring>

#include <vector>

using namespace std;
#include <libtaomee++/inet/pdumanip.hpp>

using namespace taomee;

extern "C" {
#include <fcntl.h>
#include <libtaomee/list.h>
#include <libtaomee/log.h>
#include <libtaomee/timer.h>

#include <async_serv/dll.h>
#include <async_serv/net_if.h>
#include <libtaomee/conf_parser/config.h>
}


#include "utils.hpp"
#include "proto.hpp"
#include <map>
#include <set>
#include <string.h>

#include "cross_comm.hpp"
#include "proto.hpp"
#include "cjob_dispatcher.hpp"
BEGIN_NAME_SPACE(CROSS_DISPATCHER)
CJobDispatcher::CJobDispatcher()
{
	m_max_online_id = 0;
	int i = 0;
	for (i = 0; i != online_num_max; ++i) {
		memset(&m_online_info[i], 0x00, sizeof(m_online_info[i]) - sizeof(list_head_t));
        INIT_LIST_HEAD(&m_online_info[i].online_head);
    }

	for (i = 0; i != online_num_max; ++i) {
		m_keepalive_tmr[i].tmr = NULL;
        INIT_LIST_HEAD(&m_keepalive_tmr[i].timer_list);
    }


	memset(m_pkg, 0x00, sizeof(m_pkg));
	
	for (int loop = 0; loop < pklv_phase_max; loop ++) {
		memset(&m_single_pk_[loop], 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&m_single_pk_[loop].timer_list);

		for (int l = 0; l < pk_team_cnt_max; l ++) {
			memset(&m_team_pk_[l][loop], 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&m_team_pk_[l][loop].timer_list);
		}
	}
}

CJobDispatcher::~CJobDispatcher()
{
}

/*!
  * @Function:Release
  * @Description: release all source that hold by object
  * @Param:none
  * @return:void
  */
void CJobDispatcher::Release()
{
}

/*!
  * @Function:GetObjId
  * @Description: get object's id
  * @Param:none
  * @return:const char*
  */
const char* CJobDispatcher::GetObjId() const
{
	return "";
}

int CJobDispatcher::make_online_dead(void* owner, void* data)
{
	online_info_t* on_info = reinterpret_cast<online_info_t*>(data);
	if (on_info){
		DEBUG_LOG("ONLINE DEAD\t[fd=%d id=%u ip=%s port=%d]", on_info->fdsess->fd, on_info->online_id, on_info->online_ip, on_info->online_port);
		close_client_conn(on_info->fdsess->fd);
	}
	return 0;
}

int CJobDispatcher::online_keepalive(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	int idx = m_fd_map_online[fdsess->fd] - 1;
	CHECK_VAL_GE(idx, 0);
	mod_expire_time(m_keepalive_tmr[idx].tmr, get_now_tv()->tv_sec + 70);
	return 0;
}

int CJobDispatcher::report_online_info(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	DEBUG_LOG("REPORT ONLINE INFO");
	// define online information 
	struct online_info_req_t {
		uint16_t	domain_id;
		uint32_t	online_id;
		uint8_t		online_name[16];
		uint8_t		online_ip[16];
		in_port_t	online_port;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(online_info_req_t));

	online_info_req_t* req = reinterpret_cast<online_info_req_t*>(pkg->body);
	
	if ((req->online_id > online_num_max) || (req->online_id == 0)) {
		ERROR_LOG("invalid onlineid=%u from fd=%d", req->online_id, fdsess->fd);
		return -1;
	}

	int idx = req->online_id - 1;
	// this can happen on a very special case
	if (m_online_info[idx].online_id == req->online_id) {
		DEBUG_LOG("DUPLICATE ONLINE ID\t[id=%u]", req->online_id);
		// close previous fd
		close_client_conn(m_online_info[idx].fdsess->fd);
	}
	
	m_online_info[idx].domain_id   = req->domain_id;
	m_online_info[idx].online_id   = req->online_id;
	memcpy(m_online_info[idx].online_ip, req->online_ip, sizeof(m_online_info[0].online_ip));
	m_online_info[idx].online_port = req->online_port;
	m_online_info[idx].fdsess      = fdsess;

	// make an online connection dead if no package is received in 70 secs
	INIT_LIST_HEAD(&(m_keepalive_tmr[idx].timer_list));
	
    //add new online server info, init the list head 
    INIT_LIST_HEAD(&m_online_info[idx].online_head);
	
	m_keepalive_tmr[idx].tmr = ADD_TIMER_EVENT_EX(&m_keepalive_tmr[idx], n_make_online_dead, &m_online_info[idx], get_now_tv()->tv_sec + 70);
	if (!m_keepalive_tmr[idx].tmr) {
		ERROR_RETURN(("add timer fail"), -1);
	}

	m_fd_map_online[fdsess->fd] = req->online_id;

	// update m_max_online_id
	if (m_max_online_id < req->online_id) {
		m_max_online_id = req->online_id;
	}

	DEBUG_LOG("ONLINE INFO\t[max=%u fd=%d olid=%u olip=%.16s olport=%d]", m_max_online_id, fdsess->fd, req->online_id, req->online_ip, req->online_port);

	return 0;
}

int CJobDispatcher::try_match_again(void* owner, void* data)
{
    pkwait_info_t* ppi = (pkwait_info_t *)owner;
	DEBUG_LOG("TIME OUT TRY MATCH AGAIN\t[%u %u %u %u]", \
		ppi->teamid, ppi->type, ppi->level, ppi->count);
	uint32_t online_id = ppi->online_id;
	if (!online_id) {
		ERROR_LOG("user offline\t[%u]", ppi->teamid);
		m_pkwait.erase(ppi->teamid);
		ppi->teamid = 0;
		ppi->count = 0;
		ppi->level = 0;
		return 0;
	}

	pkwait_info_t* ppi_ret = NULL;
	if (ppi->type) {
		ppi_ret = match_again_team(ppi);
	} else {
		ppi_ret = match_again_single(ppi);
	}

	if (ppi_ret) {
		REMOVE_TIMERS(ppi_ret);
		int idx = sizeof(mole2cross_proto_t);
		pack_h(m_pkg, ppi_ret->teamid, idx);
		init_proto_head(m_pkg, ppi->teamid, idx, 0, CROSS_COMMID_ONLI_AUTO_CHALLENGE, 0);
		send_pkg_to_online(online_id, m_pkg, idx);
		m_start.insert(pair<userid_t, pkwait_info_t>(ppi->teamid, *ppi_ret));
		pkaccept_info_t pi = {ppi->teamid, 0};
		m_accept.insert(pair<userid_t, pkaccept_info_t>(ppi_ret->teamid, pi));
		memset(ppi_ret, 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&ppi_ret->timer_list);
		memset(ppi, 0, sizeof(pkwait_info_t));
		INIT_LIST_HEAD(&ppi->timer_list);
	}

	return 0;
}

pkwait_info_t* CJobDispatcher::match_again_single(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	int lvdiff = 100;
	if (lvidx < (pklv_phase_max - 1) && m_single_pk_[lvidx + 1].teamid) {
		lvdiff = m_single_pk_[lvidx + 1].level - ppi->level;
		ppi_ret = &m_single_pk_[lvidx + 1];
	}

	if (lvidx > 0 && m_single_pk_[lvidx - 1].teamid) {
		int lvd = ppi->level - m_single_pk_[lvidx - 1].level;
		if (lvd < lvdiff) {
			ppi_ret = &m_single_pk_[lvidx - 1];
			lvdiff = lvd;
		}
	}

	if (!ppi_ret) {
		if (lvidx < (pklv_phase_max - 2) && m_single_pk_[lvidx + 2].teamid) {
			lvdiff = m_single_pk_[lvidx + 2].level - ppi->level;
			ppi_ret = lvdiff <= 10 ? &m_single_pk_[lvidx + 2] : ppi_ret;
		}

		if (lvidx > 1 && m_single_pk_[lvidx - 2].teamid) {
			int lvd = ppi->level - m_single_pk_[lvidx - 2].level;
			if (lvd < lvdiff && lvd <= 10) {
				ppi_ret = &m_single_pk_[lvidx - 2];
				lvdiff = lvd;
			}
		}
	}
	return ppi_ret;
}

pkwait_info_t* CJobDispatcher::match_again_team(pkwait_info_t* ppi)
{
	pkwait_info_t* ppi_ret = NULL;
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	DEBUG_LOG("MATCH AGAIN TEAM\t[%u %u %u]", ppi->teamid, lvidx, ppi->count);

	for (uint32_t cnt = 0; cnt < 3; cnt ++) {
		if (!cnt) {
			DEBUG_LOG("MATCH SAME CNT");
			pkwait_info_t* ppit = (lvidx < pklv_phase_max - 1) ? &m_team_pk_[ppi->count - 1][lvidx + 1] : NULL;
			ppi_ret = ppit && ppit->teamid ? ppit : NULL;
			uint32_t lvdiff = ppi_ret ? (ppi_ret->level - ppi->level) : 100;
			ppit = (lvidx > 0) ? &m_team_pk_[ppi->count - 1][lvidx - 1] : NULL;
			if (ppit && ppit->teamid) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			ppit = (lvidx < pklv_phase_max - 2) ? &m_team_pk_[ppi->count - 1][lvidx + 2] : NULL;
			if (ppit && ppit->teamid && (ppit->level - ppi->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppit->level - ppi->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppit->level - ppi->level) : lvdiff;
			}
			ppit = (lvidx > 1) ? &m_team_pk_[ppi->count - 1][lvidx - 2] : NULL;
			if (ppit && ppit->teamid && (ppi->level - ppit->level) <= 10) {
				ppi_ret = (!ppi_ret || (ppi_ret && lvdiff > ppi->level - ppit->level)) ? ppit : ppi_ret;
				lvdiff = (ppi_ret == ppit) ? (ppi->level - ppit->level) : lvdiff;
			}
			if (ppi_ret) return ppi_ret;
		}

		if (cnt && ppi->count + cnt <= pk_team_cnt_max) {
			DEBUG_LOG("MATCH CNT INCR\t[%u]", cnt);
			ppi_ret = lvidx > 0 ? &m_team_pk_[ppi->count + cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &m_team_pk_[ppi->count + cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 1) ? &m_team_pk_[ppi->count + cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 1 ? &m_team_pk_[ppi->count + cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &m_team_pk_[ppi->count + cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
		}

		if (cnt && ppi->count > cnt) {
			DEBUG_LOG("MATCH CNT DECR\t[%u]", cnt);
			ppi_ret = (lvidx < pklv_phase_max - 1) ? &m_team_pk_[ppi->count - cnt - 1][lvidx + 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = &m_team_pk_[ppi->count - cnt - 1][lvidx];
			if (ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx > 0 ? &m_team_pk_[ppi->count - cnt - 1][lvidx - 1] : NULL;
			if (ppi_ret && ppi_ret->teamid) return ppi_ret;
			ppi_ret = lvidx < (pklv_phase_max - 2) ? &m_team_pk_[ppi->count - cnt - 1][lvidx + 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi_ret->level - ppi->level) <= 10) return ppi_ret;
			ppi_ret = lvidx > 1 ? &m_team_pk_[ppi->count - cnt - 1][lvidx - 2] : NULL;
			if (ppi_ret && ppi_ret->teamid && (ppi->level - ppi_ret->level) <= 10) return ppi_ret;
		}
	}
	return NULL;
}

int CJobDispatcher::challenge_battle(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	DEBUG_LOG("CHALLENGE OTHER ONLINE's WARRIOR");
	struct battle_req_t {
		battle_id_t	btid;
		userid_t	challengee;
		uint16_t    lineid;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(battle_req_t));

	battle_req_t* req = reinterpret_cast<battle_req_t*>(pkg->body);
	int      idx        = sizeof(mole2cross_proto_t);
	uint32_t online_id  = req->lineid;
	
	if (online_id <= 0 || online_id >= online_num_max){
		ERROR_LOG("cannot find challengee\t[uid=%u]", req->challengee);
		init_proto_head(m_pkg, pkg->id, idx, pkg->seq, pkg->opid, 200003);
		send_pkg_to_client(fdsess, m_pkg, idx);
		return 0;
	}
	
	{
		init_proto_head(m_pkg, pkg->id, idx, pkg->seq, pkg->opid, 0);
		send_pkg_to_client(fdsess, m_pkg, idx);

		pack_h(m_pkg, (req->btid >> 32), idx);
		pack_h(m_pkg, req->btid, idx);
		init_proto_head(m_pkg, req->challengee, idx, 0, proto_challenge_battle_onli, 0);
		DEBUG_LOG("CHALLENGE UID\t[uid=%u olid=%u]", req->challengee, online_id);
		send_pkg_to_online(online_id, m_pkg, idx);
	}
	return 0;
}

int CJobDispatcher::send_pkg_to_online(uint16_t online_id, const void* pkg, int pkglen)
{
	const online_info_t* olinfo = get_online_info(online_id);
	if (olinfo) {
		return send_pkg_to_client(olinfo->fdsess, m_pkg, pkglen);// with details in net_if.h
	} else {
		ERROR_LOG("Online Svr Info Not Found: olid=%d", online_id);
		return -1;
	}
}

const online_info_t* CJobDispatcher::get_online_info(uint16_t online_id) const
{
	int idx = online_id - 1;
	if ((online_id > 0) && (online_id <= online_num_max)
			&& m_online_info[idx].online_id == online_id) {
		return &(m_online_info[idx]);
	}
	return 0;
}

int CJobDispatcher::onli_apply_pk(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct pk_req_t {
		uint32_t	type;
		uint32_t	level;
		uint32_t	count;
	} __attribute__((packed));
	CHECK_VAL(bodylen, sizeof(pk_req_t));

	pk_req_t* req = reinterpret_cast<pk_req_t*>(pkg->body);
	int idx = sizeof(mole2cross_proto_t);
	if (m_pkwait.find(pkg->id) != m_pkwait.end() \
		|| !req->level || req->level > pklv_per_phase * pklv_phase_max \
		|| !req->count || req->count > pk_team_cnt_max) {
		ERROR_LOG("apply pk fail\t[%u %u %u %u]", pkg->id, req->type, req->level, req->count);
		pack_h(m_pkg, (uint32_t)apply_fail, idx);
		init_proto_head(m_pkg, pkg->id, idx, 0, pkg->opid, 0);
		send_pkg_to_client(fdsess, m_pkg, idx);
	} else {
		uint32_t match;
		pkwait_info_t pkreq;
		pkreq.teamid = pkg->id;
		pkreq.type = req->type;
		pkreq.level = req->level;
		pkreq.count = req->count;
		pkreq.online_id = m_fd_map_online[fdsess->fd];
		DEBUG_LOG("APPLY PK SUCC\t[%u %u %u %u %u]", pkg->id, req->type, req->level, req->count, pkreq.online_id);

		// memory useris & online id
		reset_onlineid_by_usrid(pkg->id, pkreq.online_id);

		INIT_LIST_HEAD(&pkreq.timer_list);
		
		pkwait_info_t* ppi = try_to_match(&pkreq, match);

		pack_h(m_pkg, match, idx);
		init_proto_head(m_pkg, pkg->id, idx, 0, pkg->opid, 0);
		send_pkg_to_client(fdsess, m_pkg, idx);

		if (match == match_succ) {
			idx = sizeof(mole2cross_proto_t);
			pack_h(m_pkg, ppi->teamid, idx);
			init_proto_head(m_pkg, pkg->id, idx, 0, CROSS_COMMID_ONLI_AUTO_CHALLENGE, 0);
			send_pkg_to_client(fdsess, m_pkg, idx);
			
			m_start.insert(pair<userid_t, pkwait_info_t>(pkg->id, *ppi));
			pkaccept_info_t pi = {pkg->id, 0};
			m_accept.insert(pair<userid_t, pkaccept_info_t>(ppi->teamid, pi));
			ppi->teamid = 0;
			ppi->level = 0;
			REMOVE_TIMERS(ppi);
			INIT_LIST_HEAD(&ppi->timer_list);
		}
	}

	return 0;
}

pkwait_info_t* CJobDispatcher::try_to_match(pkwait_info_t* ppi, uint32_t& match)
{
	int lvidx = (ppi->level - 1) / pklv_per_phase;
	pkwait_info_t* ppret = ppi->type ? &m_team_pk_[ppi->count - 1][lvidx] : &m_single_pk_[lvidx];
	
	pkwait_info_t* ppi_next = (lvidx < (pklv_per_phase - 1)) \
		? (ppi->type ? &m_team_pk_[ppi->count - 1][lvidx + 1] : &m_single_pk_[lvidx + 1]) : NULL;
	
	pkwait_info_t* ppi_pre = lvidx > 0 ? (ppi->type ? &m_team_pk_[ppi->count-1][lvidx-1] : &m_single_pk_[lvidx-1]) : NULL;
	if (ppret->teamid) {
		match = match_succ;
	} else if (ppi_next && ppi_next->teamid && (ppi_next->level - ppi->level) < 5) {
		match = match_succ;
		ppret = ppi_next;
	} else if (ppi_pre && ppi_pre->teamid && (ppi->level - ppi_pre->level) < 5) {
		match = match_succ;
		ppret = ppi_pre;
	} else {
		match = match_fail;
		ppret->teamid = ppi->teamid;
		ppret->level = ppi->level;
		ppret->count = ppi->count;
		ppret->type = ppi->type;
		ppret->online_id = ppi->online_id;
		DEBUG_LOG("ADD TIMER FOM MATCH AGAIN\t[%u %u %u %u]", ppi->teamid, ppi->level, ppi->count, ppi->type);
		ADD_TIMER_EVENT_EX(ppret, n_try_match_again, NULL, get_now_tv()->tv_sec + 30);
		m_pkwait.insert(ppret->teamid);
	}

	return ppret;
}

int CJobDispatcher::onli_auto_accept(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	struct battle_req_t {
		battle_id_t	btid;
		userid_t	challengee;
	} __attribute__((packed));

	CHECK_VAL(bodylen, sizeof(battle_req_t));

	battle_req_t* req 	= reinterpret_cast<battle_req_t*>(pkg->body);
	int      idx        = sizeof (mole2cross_proto_t);
	uint32_t online_id  = get_onlineid_by_usrid(req->challengee);
	
	/* The follwing two statements for  building the response protocol package to online server */
	if (online_id) {
		std::map<userid_t, pkaccept_info_t>::iterator it = m_accept.find(req->challengee);
		if (it != m_accept.end()) {
			it->second.btid = req->btid;
		} else {
			ERROR_LOG("can't find accept info\t[%u %u]", pkg->id, req->challengee);
			pkaccept_info_t pi = {pkg->id, req->btid};
			m_accept.insert(pair<userid_t, pkaccept_info_t>(req->challengee, pi));
		}

		pack_h(m_pkg, req->btid, idx);
		init_proto_head(m_pkg, req->challengee, idx, 0, CROSS_COMMID_AUTO_FIGHT, 0);
		DEBUG_LOG("CHALLENGE UID\t[uid=%u olid=%u]", req->challengee, online_id);
		send_pkg_to_online(online_id, m_pkg, idx);
		m_start.erase(pkg->id);
	}
	return 0;
}

int CJobDispatcher::onli_auto_fight(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 4);
	userid_t uid = *(userid_t*)pkg->body;
	m_accept.erase(pkg->id);
	m_pkwait.erase(pkg->id);
	m_pkwait.erase(uid);
	return 0;
}

pkwait_info_t* CJobDispatcher::get_pkwait_info(uint32_t uid)
{
	for (int loop = 0; loop < pklv_phase_max; loop ++) {
		if (m_single_pk_[loop].teamid == uid) {
			return &m_single_pk_[loop];
		}

		for (int l = 0; l < pk_team_cnt_max; l ++) {
			if (m_team_pk_[l][loop].teamid == uid) {
				return &m_team_pk_[l][loop];
			}
		}
	}
	return NULL;
}

int CJobDispatcher::onli_cancel_pk_apply(mole2cross_proto_t* pkg, uint32_t bodylen, fdsession_t* fdsess)
{
	CHECK_VAL(bodylen, 0);

	int ret = 0;
	DEBUG_LOG("CANCEL APPLY\t[%u ]", pkg->id);

	if (m_pkwait.find(pkg->id) == m_pkwait.end()) {
		ret = 200072;
	} else if (m_start.find(pkg->id) != m_start.end() \
		|| m_accept.find(pkg->id) != m_accept.end()) {
		ret = 200073;
	}

	if (!ret) {
		pkwait_info_t* ppi = get_pkwait_info(pkg->id);
		if (ppi) {
			REMOVE_TIMERS(ppi);
			memset(ppi, 0, sizeof(pkwait_info_t));
			INIT_LIST_HEAD(&ppi->timer_list);			
			m_pkwait.erase(pkg->id);
		} else {
			ret = 200000;
		}
	}

	int idx = sizeof(mole2cross_proto_t);
	init_proto_head(m_pkg, pkg->id, idx, 0, pkg->opid, ret);
	send_pkg_to_client(fdsess, m_pkg, idx);

	return 0;
}

int CJobDispatcher::clear_online_info(void* lparam, void* wparam)
{
	if (!lparam){
		return -1;
	}
	
	int fd = -1;
	CONVERT_PTR_TO_PTR(lparam, fd);
	uint16_t online_id = m_fd_map_online[fd];
	if (online_id) {
		int idx = online_id - 1;

		DEBUG_LOG("ONLINE CLOSE CONN\t[id=%u %u ip=%.16s]",
					online_id, m_online_info[idx].online_id, m_online_info[idx].online_ip);

		m_fd_map_online.erase(fd);

		REMOVE_TIMERS(&(m_keepalive_tmr[idx]));
		m_keepalive_tmr[idx].tmr = 0;
#if 0
        usr_info_t* p;
        //set all usr at this online server offline
        list_for_each_entry (p, &m_online_info[idx].online_head, online_node) {
            p->online_id = 0;
        }
#endif
		memset(&(m_online_info[idx]), 0, sizeof(m_online_info[0]));

		// reset max online id
		if (online_id == m_max_online_id) {
			uint32_t i = m_max_online_id - 1;
			while (i && (m_online_info[i - 1].online_id == 0)) {
				--i;
			}
			m_max_online_id = i;
		}
	}

	return 0;
}

// 
uint16_t CJobDispatcher::get_onlineid_by_usrid(userid_t uid)
{
	std::map<userid_t, uint16_t>::const_iterator item = m_usrid_2_onlineid.find(uid);
	if (item == m_usrid_2_onlineid.end()){
		return 0;
	}

	return item->second;
}

void  CJobDispatcher::reset_onlineid_by_usrid(userid_t uid, uint16_t onlineid)
{
	std::map<userid_t, uint16_t>::iterator item = m_usrid_2_onlineid.find(uid);
	if (item == m_usrid_2_onlineid.end()){
		m_usrid_2_onlineid.insert(std::pair<userid_t, uint16_t>(uid, onlineid));
	}else{
		item->second = onlineid;
	}
}

END_NAME_SPACE()
