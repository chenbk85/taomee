#include <libtaomee++/bitmanip/bitmanip.hpp>
#include <libtaomee/project/utilities.h>
#include <libtaomee++/utils/md5.h>

extern "C" {
#include <async_serv/mcast.h>
}
using namespace taomee;

#include "utils.hpp"
#include "message.hpp"
#include "player.hpp"
#include "cli_proto.hpp"
#include "switch.hpp"
//#include "login.hpp"
#include "global_data.hpp"
//#include "mcast_proto.hpp"

#include "master_prentice.hpp"
//#include "common_op.hpp"
//#include "active_config.hpp"
#include "stat_log.hpp"

//-----------------------------------------------------------
// static function declarations
//-----------------------------------------------------------
//--------------------------------------------------------------------
bool is_player_canbe_master(player_t* p)
{
    if (p->power_user == 1) {
		
        if (p->p_prentice->size() < (10 + (is_vip_player(p) ? p->vip_level * 5 : 0))) {
            return true;
        }
    }
    return false;
}

bool is_player_have_master(player_t* p)
{
    if (is_valid_uid(p->p_master->master_id)) return true;
    return false;
}

bool is_player_can_kick_prentice(player_t* p, uint32_t prentice_id, uint32_t prentice_tm)
{
    std::vector<mp_prentice_t>::iterator it = p->p_prentice->begin();
    for (; it != p->p_prentice->end(); ++it) {
        if ( it->uid == prentice_id && it->roletm == prentice_tm) {
            if (it->tm + 3600 < get_now_tv()->tv_sec) {
                return true;
            }
            break;
        }
    }
    return false;
}

bool is_player_can_kick_master(player_t* p, uint32_t master_id, uint32_t master_tm)
{
    if ( p->p_master->master_id == master_id && p->p_master->master_tm == master_tm) {
        if (p->p_master->join_tm + 3600 < get_now_tv()->tv_sec) {
            return true;
        }
    }
    return false;
}

//----------------------------------------------------------
// @param is_or_operate (1-- |; 0-- &)
//----------------------------------------------------------
void set_prentice_grade(player_t* p, uint32_t uid, uint32_t roletm, uint32_t grade, int is_or_operate = 1)
{
    std::vector<mp_prentice_t>::iterator it = p->p_prentice->begin();
    for (; it != p->p_prentice->end(); ++it) {
        if ( it->uid == uid && it->roletm == roletm) {
            //it->grade = is_or_operate ? (it->grade | grade) : (it->grade & grade);
            it->grade = is_or_operate ? (it->grade | grade) : grade;
            break;
        }
    }
}

uint32_t get_prentice_grade(player_t* p, uint32_t uid, uint32_t roletm)
{
    std::vector<mp_prentice_t>::iterator it = p->p_prentice->begin();
    for (; it != p->p_prentice->end(); ++it) {
        if ( it->uid == uid && it->roletm == roletm) {
            return it->grade;
        }
    }
    return 0;
}

void master_del_prentice(player_t* p, uint32_t uid, uint32_t roletm)
{
    std::vector<mp_prentice_t>::iterator it = p->p_prentice->begin();
    for (; it != p->p_prentice->end(); ++it) {
        if ( it->uid == uid && it->roletm == roletm) {
            it = p->p_prentice->erase(it);
            break;
        }
    }
}

void prentice_del_master(player_t* p, uint32_t uid, uint32_t roletm)
{
    p->p_master->master_id = 0;
	p->p_master->master_tm = 0;
	p->p_master->join_tm = 0;
}


//--------------------------------------
//  邀请以及应答
//--------------------------------------
int pack_rltm_invite_prentice_pkg(uint8_t* buf, uint32_t cli_cmd, player_t* inviter) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, inviter->id, idx);
	pack(buf, inviter->role_tm, idx);
	pack(buf, inviter->nick, sizeof(inviter->nick), idx);
	
	init_cli_proto_head(buf, inviter, cli_cmd, idx);
	return idx;
}

/**
  * @brief 
  */
int invite_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t invitee_id = 0, invitee_tm;
	unpack(body, invitee_id, idx);
	unpack(body, invitee_tm, idx);

	if (!is_player_canbe_master(p)) {
		return send_header_to_player(p, p->waitcmd, cli_err_master_prentice_err, 1);
	}
	
	player_t* to = get_player(invitee_id);
    if (to) {
        if (is_player_have_master(to)) {
            return send_header_to_player(p, p->waitcmd, cli_err_player_have_master, 1);
        }
    }
	int off_len = sizeof(userid_t);
	
	(*(uint32_t*)pkgbuf) = invitee_id;
	int pkglen = pack_rltm_invite_prentice_pkg(pkgbuf + off_len, cli_proto_send_invite_prentice, p);

	TRACE_LOG("%u -> %u ", p->id, invitee_id);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
	}
	return send_header_to_player(p, p->waitcmd, 0, 1);
}

int pack_rltm_reply_invite_prentice_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t accept, player_t* p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, p->id, idx);
	pack(buf, p->role_tm, idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
	pack(buf, accept, idx);

	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}

void send_reply_invite_prentice(player_t* p, uint32_t inviter, uint32_t accept)
{
	player_t* to = get_player(inviter);
	int off_len = sizeof(userid_t);
	
	(*(uint32_t*)pkgbuf) = inviter;
	int pkglen = pack_rltm_reply_invite_prentice_pkg(pkgbuf + off_len, 
		cli_proto_send_reply_invite_prentice, accept, p);

	KDEBUG_LOG(p->id, "SEND REPLY TO INVITER[%u %u %u]", p->id, inviter, accept);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(p, pkgbuf,  pkglen + off_len);
	}
}

/**
  * @brief 
  */
int reply_invite_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int i = 0;
	uint32_t inviter = 0, role_tm = 0, accept = 0;
	unpack(body, inviter, i);
	unpack(body, role_tm, i);
	unpack(body, accept, i);

	if (is_player_have_master(p) || p->lv >= 30) {
		return send_header_to_player(p, p->waitcmd, cli_err_player_have_master, 1);
	}
	
    if (accept) {
        //加入师门
        return db_master_add_prentice(p, inviter, role_tm);
    } else {

        send_reply_invite_prentice(p, inviter, accept);

        int idx = sizeof(cli_proto_t);
        pack(pkgbuf, 0, idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }
}

//-------------------------------------------------------------
// 师徒邮件
//  8-action--| 1--导师收徒
//            | 2--徒弟拜师
//            | 3--导师逐出
//            | 4--徒弟被逐出
//            | 5--导师收到徒弟进阶通知
//            | 6--徒弟进阶
//            | 7--导师收到徒弟出师通知
//            | 8--徒弟出师
//-------------------------------------------------------------
void master_prentice_mail(player_t* p, const char *nick, uint32_t action, uint32_t uid = 0, uint32_t roletm = 0, uint32_t itemid = 0)
{
    char title[MAX_MAIL_TITLE_LEN + 1] = {0};
    char content[MAX_MAIL_CONTENT_LEN + 1] = {0};
    std::vector<mail_item_enclosure> mail_item;

    memcpy(title, char_content[16].content, MAX_MAIL_TITLE_LEN);
    snprintf(content, MAX_MAIL_CONTENT_LEN, " %s %s", nick, char_content[16 + action].content);

    TRACE_LOG("title[%s] [%u %u]", title, action, itemid);
    if (itemid) {
        mail_item.push_back(mail_item_enclosure(itemid, 1));
    }

    if (p) {
        db_send_system_mail(p, title, sizeof(title), content, sizeof(content), 999, 0, mail_item.size() ? &mail_item : 0, 0);
    }else {
        db_send_system_mail(p, title, sizeof(title), content, sizeof(content), 999, 0, mail_item.size() ? &mail_item : 0, 0, uid, roletm);
    }
    mail_item.clear();
}

//-----------------------------------------------------
//  @brief send master notice
//  @param action 1-- 加入师门 2-- 逐出师门
//-----------------------------------------------------
int pack_master_notice_pkg(uint8_t* buf, uint32_t cli_cmd, uint32_t action, player_t* p) 
{
	int idx = sizeof(cli_proto_t);
	pack(buf, action, idx);
	pack(buf, p->id, idx);
	pack(buf, p->role_tm, idx);
	pack(buf, p->nick, sizeof(p->nick), idx);
    //server id

	init_cli_proto_head(buf, p, cli_cmd, idx);
	return idx;
}

void send_master_notice_to_prentice(player_t* master, uint32_t prentice_id, uint32_t action) 
{
    player_t* to = get_player(prentice_id);
	int off_len = sizeof(userid_t);
	
    if (action == 1) {
        to->p_master->master_id = master->id;
        to->p_master->master_tm = master->role_tm;
    } else if (action == 2) {
        to->p_master->master_id = 0;
        to->p_master->master_tm = 0;
    }

	(*(uint32_t*)pkgbuf) = prentice_id;
	int pkglen = pack_master_notice_pkg(pkgbuf + off_len, cli_proto_master_notice, action, master);

	KDEBUG_LOG(master->id, "SEND MASTER NOTICE[%u %u %u]", master->id, prentice_id, action);
	
	if (to) {
		send_to_player(to, pkgbuf + off_len, pkglen, 0);
	} else {
		chat_across_svr_2(master, pkgbuf,  pkglen + off_len);
	}
}

//--------------------------------------------------
//   @brief 徒弟进阶通知
//   @param action 1--进阶 2--出师
//--------------------------------------------------
void send_prentice_notice_to_master()
{
}

//--------------------------------------------------
//   @brief 加入师门
//   @param action 1--master action 2--prentice action
//   @desc  prentice ---> master
//--------------------------------------------------
int pack_master_add_prentice_pkg(uint8_t* buf, uint32_t action, uint32_t id, uint32_t role_tm, uint32_t lv) 
{
	int idx = 0;//sizeof(cli_proto_t);
	pack_h(buf, action, idx);
	pack_h(buf, id, idx);
	pack_h(buf, role_tm, idx);
    pack_h(buf, static_cast<uint32_t>(get_now_tv()->tv_sec), idx);
    pack_h(buf, lv, idx);
    uint32_t grade = 0;
    //if (action == 2 && p->lv >= 20) {
    //    grade = 1;//set_bit_on(action, 1);
    //}
    pack_h(dbpkgbuf, grade, idx);

	return idx;
}

int db_master_add_prentice(player_t* prentice, uint32_t inviter, uint32_t role_tm)
{
    int idx = 0;
    pack_h(prentice->tmp_session, inviter, idx);
    pack_h(prentice->tmp_session, role_tm, idx);

    idx = 0;
    //master add prentice
    idx = pack_master_add_prentice_pkg(dbpkgbuf, 1, prentice->id, prentice->role_tm, prentice->lv);
    TRACE_LOG("master [%u] add prentice\t[%u %u]", inviter, prentice->id, prentice->role_tm);
    return send_request_to_db(prentice, inviter, role_tm, dbproto_master_add_prentice, dbpkgbuf, idx);
}

int db_master_add_prentice_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
    CHECK_DBERR(p, ret);

    uint32_t inviter = 0, role_tm = 0;
    int i = 0;
    unpack_h(p->tmp_session, inviter, i);
    unpack_h(p->tmp_session, role_tm, i);

    int idx = 0;
    int out_idx = sizeof(cli_proto_t);
    db_master_add_prentice_t *rsp = reinterpret_cast<db_master_add_prentice_t *>(body);
    if (rsp->action != 0) {
                //prentice add master
        idx = pack_master_add_prentice_pkg(dbpkgbuf, 2, inviter, role_tm, 0);
        KDEBUG_LOG(p->id, "prentice add master\t[%u %u]", inviter, role_tm);
        send_request_to_db(0, p->id, p->role_tm, dbproto_master_add_prentice, dbpkgbuf, idx);	
        //update prentice online date
        p->p_master->master_id = inviter;
        p->p_master->master_tm = role_tm;
        p->p_master->join_tm = get_now_tv()->tv_sec;
        //send prentice mail
        char nick[16] = {0};
        sprintf(nick, "%u", inviter);
        master_prentice_mail(p, nick, 2);

        player_t* master = get_player(inviter);
        if (master && master->role_tm == role_tm) {
            //update master online date
            mp_prentice_t p_info = {0};
            p_info.uid    = p->id;
            p_info.roletm = p->role_tm;
            p_info.lv     = p->lv;
            p_info.tm     = get_now_tv()->tv_sec;
            p_info.grade  = 0;
            master->p_prentice->push_back(p_info);
            //send master mail
            master_prentice_mail(master, p->nick, 1);

            do_stat_log_universal_interface_1(0x09819101, 0, inviter);
            do_stat_log_universal_interface_1(0x09819102, 0, p->id);
        }else {
            master_prentice_mail(0, p->nick, 1, inviter, role_tm);
        }
        send_reply_invite_prentice(p, inviter, 1); //success
        pack(pkgbuf, static_cast<uint32_t>(1), out_idx);
    } else {
        send_reply_invite_prentice(p, inviter, 2); //full
        pack(pkgbuf, static_cast<uint32_t>(0), out_idx);
    }

    init_cli_proto_head(pkgbuf, p, p->waitcmd, out_idx);
    return send_to_player(p, pkgbuf, out_idx, 1);
}

int db_prentice_add_master(player_t* prentice, uint32_t inviter, uint32_t role_tm)
{
    int idx = 0;
    //prentice add master
    idx = pack_master_add_prentice_pkg(dbpkgbuf, 2, inviter, role_tm, 0);
    KDEBUG_LOG(prentice->id, "prentice add master\t[%u %u]", inviter, role_tm);
    send_request_to_db(0, prentice->id, prentice->role_tm, dbproto_master_add_prentice, dbpkgbuf, idx);	
    //update prentice online date
    prentice->p_master->master_id = inviter;
    prentice->p_master->master_tm = role_tm;
    prentice->p_master->join_tm = get_now_tv()->tv_sec;
    //send prentice mail
    master_prentice_mail(prentice, "xx", 2);

    //master add prentice
    idx = pack_master_add_prentice_pkg(dbpkgbuf, 1, prentice->id, prentice->role_tm, prentice->lv);
    KDEBUG_LOG(inviter, "master add prentice\t[%u %u]", prentice->id, prentice->role_tm);
    send_request_to_db(0, inviter, role_tm, dbproto_master_add_prentice, dbpkgbuf, idx);

    player_t* master = get_player(inviter);
    if (master && master->role_tm == role_tm) {
        //update master online date
        mp_prentice_t p_info = {0};
        p_info.uid    = prentice->id;
        p_info.roletm = prentice->role_tm;
        p_info.lv     = prentice->lv;
        p_info.tm     = get_now_tv()->tv_sec;
        p_info.grade  = 0;
        master->p_prentice->push_back(p_info);
        //send master mail
        master_prentice_mail(master, prentice->nick, 1);
    }else {
        master_prentice_mail(0, prentice->nick, 1, inviter, role_tm);
    }
    return 0;
}
//--------------------------------------------------
//   @brief 逐出师门
//   @param action 1--master action 2--prentice action
//   @desc  master ------> prentice
//--------------------------------------------------
int pack_master_del_prentice_pkg(uint8_t* buf, uint32_t action, uint32_t id, uint32_t role_tm) 
{
	int idx = 0;//sizeof(cli_proto_t);
	pack_h(buf, action, idx);
	pack_h(buf, id, idx);
	pack_h(buf, role_tm, idx);
	return idx;
}

int db_master_del_prentice(player_t* master, uint32_t prentice_id, uint32_t prentice_tm)
{
    int idx = 0;

    //master del prentice
    idx = pack_master_del_prentice_pkg(dbpkgbuf, 1, prentice_id, prentice_tm);
    KDEBUG_LOG(master->id, "master del prentice\t[%u %u]", prentice_id, prentice_tm);
    send_request_to_db(0, master->id, master->role_tm, dbproto_master_del_prentice, dbpkgbuf, idx);
    //update master online date
    master_del_prentice(master, prentice_id, prentice_tm);
    //send master mail
    char nick[16] = {0};
    sprintf(nick, "%u", prentice_id);
    master_prentice_mail(master, nick, 3);

    //prentice del master
    idx = pack_master_del_prentice_pkg(dbpkgbuf, 2, master->id, master->role_tm);;
    KDEBUG_LOG(prentice_id, "prentice del master\t[%u %u]", master->id, master->role_tm);
    send_request_to_db(0, prentice_id, prentice_tm, dbproto_master_del_prentice, dbpkgbuf, idx);	

    player_t* prentice = get_player(prentice_id);
    if (prentice && prentice->role_tm == prentice_tm) { //徒弟在线
        //update prentice online data
        prentice->p_master->master_id = 0;
        prentice->p_master->master_tm = 0;
        //send prentice mail
        master_prentice_mail(prentice, master->nick, 4);
    } else {
        //send prentice mail
        master_prentice_mail(0, master->nick, 4, prentice_id, prentice_tm);
    }

    idx = sizeof(cli_proto_t);
	pack(pkgbuf, prentice_id, idx);
	pack(pkgbuf, prentice_tm, idx);
	init_cli_proto_head(pkgbuf, master, master->waitcmd, idx);
    send_to_player(master, pkgbuf, idx, 1);

    do_stat_log_universal_interface_1(0x09819103, 0, master->id);
    do_stat_log_universal_interface_1(0x09819103, 0, prentice_id);
    return 0;
}

int db_prentice_del_master(player_t* prentice, uint32_t master_id, uint32_t master_tm)
{
    int idx = 0;
    player_t* master = get_player(master_id);

    //prentice del master
    idx = pack_master_del_prentice_pkg(dbpkgbuf, 2, master_id, master_tm);
    KDEBUG_LOG(prentice->id, "prentice del master\t[%u %u]", master_id, master_tm);
    send_request_to_db(0, prentice->id, prentice->role_tm, dbproto_master_del_prentice, dbpkgbuf, idx);
    //update prentice online date
    prentice_del_master(prentice, master_id, master_tm);
    //send prentice mail
    //master_prentice_mail(prentice, "", 4);

    //master del prentice
    idx = pack_master_del_prentice_pkg(dbpkgbuf, 1, prentice->id, prentice->role_tm);;
    KDEBUG_LOG(prentice->id, "master del prentice\t[%u %u]", prentice->id, prentice->role_tm);
    send_request_to_db(0, master_id, master_tm, dbproto_master_del_prentice, dbpkgbuf, idx);	

	if (master && master->role_tm == master_tm) { //徒弟在线
        //update master online data
        master_del_prentice(master, prentice->id, prentice->role_tm);
		
        //send master mail
        master_prentice_mail(master, prentice->nick, 3);
    } else {
        //send master mail
        master_prentice_mail(0, prentice->nick, 3, master_id, master_tm);
    }

    do_stat_log_universal_interface_1(0x09819103, 0, prentice->id);
    do_stat_log_universal_interface_1(0x09819103, 0, master_id);
    return 0;
}


int kick_prentice_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	int idx = 0;
	uint32_t kick_id = 0, kick_tm = 0;
	unpack(body, kick_id, idx);
	unpack(body, kick_tm, idx);

	if (p->p_master->master_id == kick_id && p->p_master->master_tm == kick_tm) {
	//徒弟剔出师傅	
		if (!is_player_can_kick_master(p, kick_id, kick_tm)) {
			return send_header_to_player(p, p->waitcmd, cli_err_master_prentice_err2, 1);
		}
		db_prentice_del_master(p, kick_id, kick_tm);
		
	} else {
	//师傅剔除徒弟
		if (!is_player_can_kick_prentice(p, kick_id, kick_tm)) {
			return send_header_to_player(p, p->waitcmd, cli_err_master_prentice_err2, 1);
		}
		db_master_del_prentice(p, kick_id, kick_tm);
	}
	
	idx = sizeof(cli_proto_t);
	pack(pkgbuf, kick_id, idx);
	pack(pkgbuf, kick_tm, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}

//----------------------------------------------------------------
// @brief 徒弟自己离开师门
//----------------------------------------------------------------

//------------------------------------------------------------------
//   @brief 徒弟进阶时调用的接口
//   @desc  prentice -----> master
//------------------------------------------------------------------
int db_set_prentice_grade(player_t* prentice)
{
    if (!is_valid_uid(prentice->p_master->master_id)) return 0;

    //tell master
    uint32_t grade = 0;
    switch (prentice->lv) {
    case 30:
        grade = set_bit_on(grade, 1);
        break;
    case 40:
        grade = set_bit_on(grade, 2);
        break;
    }
    //set master db grade
    int idx = 0;
    pack_h(dbpkgbuf, prentice->id, idx);
    pack_h(dbpkgbuf, prentice->role_tm, idx);
    pack_h(dbpkgbuf, grade, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(1), idx); 
    KDEBUG_LOG(prentice->p_master->master_id, "STE PRENTICE GRADE\t[%u %u %u]", prentice->id, prentice->role_tm, grade);
    send_request_to_db(0, prentice->p_master->master_id, prentice->p_master->master_tm, dbproto_set_prentice_grade, dbpkgbuf, idx);

    player_t* master = get_player(prentice->p_master->master_id);
    if (master && master->role_tm == prentice->p_master->master_tm) {
        //update online date
        set_prentice_grade(master, prentice->id, prentice->role_tm, grade);        
        //send master mail
        master_prentice_mail(master, prentice->nick, 5);
    } else {
        //send master mail
        master_prentice_mail(0, prentice->nick, 5, prentice->p_master->master_id, prentice->p_master->master_tm);
    }

    //send self mail
    char nick[16] = {0};
    sprintf(nick, "%u", prentice->p_master->master_id);
    master_prentice_mail(prentice, nick, 6);
    return 0;
}

//----------------------------------------------------------
//  @brief 记录领奖标志 (当师傅领奖的时候,标示对应徒弟的信息)
//  @desc master action
//----------------------------------------------------------
bool is_can_get_prentice_reward(player_t* master, uint32_t prentice_id, uint32_t prentice_tm, uint32_t reward_id)
{
    uint32_t grade = get_prentice_grade(master, prentice_id, prentice_tm);
    if (reward_id == 1217) {
        return test_bit_on(grade, 1);
    } else if (reward_id == 1218) {
        return test_bit_on(grade, 2);
    }
    return false;
}

void db_master_set_prentice_grade(player_t* master, uint32_t prentice_id, uint32_t prentice_tm, uint32_t reward_id)
{
    uint32_t grade = get_prentice_grade(master, prentice_id, prentice_tm);
    if (reward_id == 1217) {
        //grade = 0xfffffffe;
        grade = set_bit_off(grade, 1);
    } else if (reward_id == 1218) {
        //grade = 0xfffffffd;
        grade = set_bit_off(grade, 2);
    }

    set_prentice_grade(master, prentice_id, prentice_tm, grade, 0);
    //set master db grade
    int idx = 0;
    pack_h(dbpkgbuf, prentice_id, idx);
    pack_h(dbpkgbuf, prentice_tm, idx);
    pack_h(dbpkgbuf, grade, idx);
    pack_h(dbpkgbuf, static_cast<uint32_t>(0), idx);

    KDEBUG_LOG(master->id, "STE PRENTICE GRADE\t[%u %u %u]", prentice_id, prentice_tm, grade);
    send_request_to_db(0, master->id, master->role_tm, dbproto_set_prentice_grade, dbpkgbuf, idx);
}

//------------------------------------------------------------
//  @brief 出师(侠士进阶时调用的接口)
//  @desc  prentice -----> master
//------------------------------------------------------------
int db_prentice_graduate(player_t* prentice)
{
    if (!is_valid_uid(prentice->p_master->master_id)) return 0;
    //master del prentice
    int idx = pack_master_del_prentice_pkg(dbpkgbuf, 1, prentice->id, prentice->role_tm);
    KDEBUG_LOG(prentice->p_master->master_id, "master del prentice\t[%u %u]", prentice->id, prentice->role_tm);
    send_request_to_db(0, prentice->p_master->master_id, prentice->p_master->master_tm, dbproto_master_del_prentice, dbpkgbuf, idx);

    player_t* master = get_player(prentice->p_master->master_id);
    if (master && master->role_tm == prentice->p_master->master_tm) {
        //update prentice online data
        master_del_prentice(master, prentice->id, prentice->role_tm);
        //send master mail
        master_prentice_mail(master, prentice->nick, 7, 0, 0, 2500007);
		do_swap_vip_qualify(master, master->id, 3);
    } else {
        master_prentice_mail(0, prentice->nick, 7, prentice->p_master->master_id, prentice->p_master->master_tm, 2500007);
		do_swap_vip_qualify(0, prentice->p_master->master_id, 3);
    }
	
    //prentice del master
    idx = pack_master_del_prentice_pkg(dbpkgbuf, 2, prentice->p_master->master_id, prentice->p_master->master_tm);
    KDEBUG_LOG(prentice->id, "prentice del master\t[%u %u]", prentice->p_master->master_id, prentice->p_master->master_tm);
    send_request_to_db(0, prentice->id, prentice->role_tm, dbproto_master_del_prentice, dbpkgbuf, idx);	
    //send prentice mail
    char nick[16] = {0};
    sprintf(nick, "%u", prentice->p_master->master_id);
    master_prentice_mail(prentice, nick, 8, 0, 0, 2500006);
	do_swap_vip_qualify(prentice, prentice->id, 3);
    //update prentice online data
    prentice->p_master->master_id = 0;
    prentice->p_master->master_tm = 0;

    do_stat_log_universal_interface_1(0x09819104, 0, prentice->id);
    return 0;
}
/**
 * @brief get master information
 *
 */
int get_master_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;
	uint32_t cur_time = get_now_tv()->tv_sec;
	if (cur_time - p->master_prentice_tm < 5) {
		if (is_valid_uid(p->p_master->master_id)) {
			idx = sizeof(cli_proto_t);
			pack(pkgbuf, (uint32_t)(1), idx);
			pack(pkgbuf, p->p_master->master_id, idx);
			pack(pkgbuf, p->p_master->master_tm, idx);

			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, pkgbuf, idx, 1);
		}
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, 0, idx);
		pack(pkgbuf, 0, idx);
		pack(pkgbuf, 0, idx);

		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
	p->master_prentice_tm = cur_time;
    
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_master_info, 0, 0);	
}

int db_get_master_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	//CHECK_DBERR(p, ret);
    int idx = 0;
    if (ret != 0) {
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, (uint32_t)(0), idx);
        pack(pkgbuf, (uint32_t)(0), idx);
        pack(pkgbuf, (uint32_t)(0), idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }

    db_get_master_t *rsp = reinterpret_cast<db_get_master_t *>(body);
    KDEBUG_LOG(p->id, "MASTER INFO\t cnt = [%u]", rsp->cnt);
    idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->cnt, idx); // 0 or 1
    for (uint32_t i = 0; i < rsp->cnt; i++) {
        TRACE_LOG("master info [%u] [%u %u]", p->id, rsp->master[i].master_id, rsp->master[i].master_tm);
        pack(pkgbuf, rsp->master[i].master_id, idx);
        pack(pkgbuf, rsp->master[i].master_tm, idx);
    }

	if (rsp->cnt) {
	    p->p_master->master_id = rsp->master[0].master_id;
	    p->p_master->master_tm = rsp->master[0].master_tm;
		p->p_master->join_tm = rsp->master[0].join_tm;
	}

	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


/**
 * @brief get prentice information
 */
int get_prentice_info_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
    int idx = 0;

	uint32_t cur_time = get_now_tv()->tv_sec;
	if (cur_time - p->master_prentice_tm < 5) {
		if ( !(p->p_prentice->empty()) ) {
			idx = sizeof(cli_proto_t);
			pack(pkgbuf, (uint32_t)(p->p_prentice->size()), idx);
			std::vector<mp_prentice_t>::iterator it = p->p_prentice->begin();
			for (; it != p->p_prentice->end(); ++it) {
			    pack(pkgbuf, it->uid, idx);
			    pack(pkgbuf, it->roletm, idx);
			    pack(pkgbuf, it->grade, idx);
				TRACE_LOG("%zd %u %u %u %u", p->p_prentice->size(), p->id, it->uid, it->roletm, it->grade);
			}

			init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
			return send_to_player(p, pkgbuf, idx, 1);
		}
		idx = sizeof(cli_proto_t);
		pack(pkgbuf, 0, idx);
		init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
		return send_to_player(p, pkgbuf, idx, 1);
	}
   
	p->master_prentice_tm = get_now_tv()->tv_sec;
	return send_request_to_db(p, p->id, p->role_tm, dbproto_get_prentice_info, 0, 0);	
}

int db_get_prentice_info_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret) 
{
	//CHECK_DBERR(p, ret);
	p->p_prentice->clear();
    int idx = 0;
    if (ret != 0) {
        idx = sizeof(cli_proto_t);
        pack(pkgbuf, (uint32_t)(0), idx);
        init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
        return send_to_player(p, pkgbuf, idx, 1);
    }

    db_get_prentice_t *rsp = reinterpret_cast<db_get_prentice_t *>(body);
    KDEBUG_LOG(p->id, "PRENTICE INFO\t cnt = [%u]", rsp->cnt);
    idx = sizeof(cli_proto_t);
    pack(pkgbuf, rsp->cnt, idx);

    mp_prentice_t *prentice = rsp->prentice;
    for (uint32_t i = 0; i < rsp->cnt; i++) {
        TRACE_LOG("prentice info [%u] [%u %u]", p->id, rsp->prentice[i].uid, rsp->prentice[i].roletm);
        pack(pkgbuf, prentice[i].uid, idx);
        pack(pkgbuf, prentice[i].roletm, idx);
        pack(pkgbuf, prentice[i].grade, idx);

        p->p_prentice->push_back(prentice[i]);
    }
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
    return send_to_player(p, pkgbuf, idx, 1);
}


