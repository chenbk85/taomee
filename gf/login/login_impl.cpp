#include <errno.h>

extern "C" {
#include <kf/player_attr.hpp>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include <async_serv/dll.h>
}

#include "login_impl.hpp"
#include "kf/vip_config_data_mgr.hpp"



using namespace taomee;

#define gf_dll_handle
#define gf_client_cmd
#define gf_switch
#define gf_del_server
#define gf_call_back
#define gf_utility

const char *version_str="2010-08-05 22:00";

//----------------------------------------------------------
//inline func
//----------------------------------------------------------
/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_child_lv_num(userid_t userid, uint32_t lv)
{
	statistic_msglog(gf_report_child_lv_num + lv, &userid, sizeof(userid));
    TRACE_LOG("stat log: [cmd=%x] [lv=%u] [buf=%u]",
        gf_report_child_lv_num , lv, userid);
}

inline void do_master_num(userid_t userid)
{
	statistic_msglog(0x09819105, &userid, sizeof(userid));
}

inline void do_enter_del_role()
{
	uint32_t buf[2] = {1 , 0};
	statistic_msglog(0x0901040D, &buf, sizeof(buf));
}

inline void do_leave_del_role()
{
	uint32_t buf[2] = {0 , 1};
	statistic_msglog(0x0901040D, &buf, sizeof(buf));
}


/**
 * @brief add here for platform of statstics
 */
inline void do_stat_log_interface(uint32_t cmd, userid_t userid, uint32_t role_type, uint32_t exp)
{
    uint32_t buf[2] = {0};
    buf[0] = userid;
    buf[1] = exp;
	statistic_msglog(cmd+role_type , buf, sizeof(buf));
    TRACE_LOG("stat log: [cmd=%x] [type=%u] [buf=%u %u]",
        gf_report_user_role_exp, role_type, buf[0], buf[1]);
}

#ifdef gf_dll_handle
//-----------------------------------------------------------
// dll_handles definations
//-----------------------------------------------------------

/**
 * @brief 副登录如果需要初始化额外的内容，比如说读取配置文件等等，可以通过实现这个接口来完成
 * @return 正常的话，返回true，如果出现任何初始化失败的状况，请返回false。
 */
bool KfLogin::init_service()
{
	INFO_LOG("ol cur  VERSION:%s", version_str);

	if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
		ERROR_LOG("Failed to load dirty word!");
		return -1;
	}

    init_timer();
    my_read_conf();
    multicast_init();
    //vip_config_data_mgr::getInstance()->init();
    mgr = new vip_config_data_mgr();
	mgr->init();
	return true;
}

/**
 * @brief 副登录如果需要清理额外的内容，比如说释放内存等等，可以通过实现这个接口来完成
 */
void KfLogin::fini_service()
{
    //vip_config_data_mgr::getInstance()->final();
	mgr->final();
	delete mgr;
	mgr = NULL;
	return;
}

/**
 * @brief 副登录如果需要解析其它服务器来包（比如说Switch）的长度，可以通过实现这个接口来完成。
 *		  一般来说，因为服务器来包包头都一样，可以直接在这里调用get_dbproxy_pkg_len。
 * @return 解析出来的数据包长度，或者如果失败的话，返回-1 
 */
int KfLogin::get_other_svrs_pkg_len(int fd, const void* avail_data, int)
{
	return get_dbproxy_pkg_len(avail_data);

}

/**
 * @brief 副登录如果需要处理其它的客户端协议命令，比如说拉去服务器列表/创建角色，可以通过实现这个接口来完成
 * @return 一切正常的话，返回0，如果客户端发包非法，或者处理过程中出现系统级的错误，请返回-1
 */
int KfLogin::proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
{
	TRACE_LOG("proc_other_cmds uid=%u, cmd=%u, len=%u", usr->uid, usr->waitcmd, bodylen);
    switch (usr->waitcmd) {
    case proto_get_recommend_svr_list:
        return get_recommeded_svr_list_cmd(usr, body, bodylen);
    case proto_get_ranged_svr_list:
        return get_ranged_svr_list_cmd(usr, body, bodylen);
		
    case proto_get_role_list:
        return get_role_list_cmd(usr, body, bodylen);
    case proto_create_role:
		return create_role_cmd(usr, body, bodylen);

    case proto_gray_delete_role:
        return gray_delete_role_cmd(usr, body, bodylen);
//	case proto_logic_delete_role:
//		return logic_delete_role_cmd(usr, body, bodylen);
	case proto_resume_gray_role:
		return resume_gray_role_cmd(usr, body, bodylen);
	case proto_get_version:
		return get_version_cmd(usr, body, bodylen);

/*
INVITE_VESION

	case proto_check_invite_code:
		return check_invite_code_cmd(usr, body, bodylen);
	case proto_create_role_with_inv_code:
		return create_role_with_invite_code_cmd(usr, body, bodylen);//create_role_cmd(usr, body, bodylen);

*/
		
    default:
        ERROR_LOG("invalid command=%u", usr->waitcmd);
    }
    return -1;
}

/**
 * @brief 副登录如果需要处理其它的dbproxy回报，比如说创建角色的返回包，可以通过实现这个接口来完成
 * @return 一切正常的话，返回0，如果DBproxy返回包非法，或者处理过程中出现系统级的错误，请返回-1
 */
int KfLogin::proc_other_dbproxy_return(usr_info_t* p, uint16_t cmd, const uint8_t* body, uint32_t bodylen, uint32_t ret)
{
	TRACE_LOG("proc_other_dbproxy_return cmd:%02X id:%u ret:%u", cmd, p->uid, ret);
	int err = -1;
    switch(cmd) {
        case db_gf_login:
            err = login_gf_callback(p, body, bodylen, ret);
            break;
        case db_gf_get_role_list:
            err = get_role_list_callback(p, body, bodylen, ret);
            break;
        case db_gf_register:
            err = gf_create_role_callback(p, body, bodylen, ret);
            break;
        /*case db_gf_add_game_flag:
		case db_gf_add_game_flag_ex:
            err = add_game_flag_callback(p, body, bodylen, ret);
            break;*/
		case db_gf_add_childid:
			err = add_childid_callback(p, body, bodylen, ret);
			break;
			
#ifdef INVITE_CODE_VESION
		case db_userinfo_get_mee_fans:
			err = get_mee_fans_callback(p, body, bodylen, ret);
			break;
		case db_other_check_invite_code:
			err = check_invite_code_callback(p, body, bodylen, ret);
			break;
		case db_other_check_user_invited:
			err = check_user_invited_callback(p, body, bodylen, ret);
			break;
#endif
		case db_sess_check_session:
			err = check_session_callback(p, body, bodylen, ret);
			break;
		case db_gf_get_amb_info:
			err = get_amb_info_callback(p, body, bodylen, ret);
			break;
		//delete role
		case db_gf_gray_delete_role:
			do_enter_del_role();
			err = gray_delete_role_callback(p, body, bodylen, ret);
			break;
//		case db_gf_logic_delete_role:
//			err = logic_delete_role_callback(p, body, bodylen, ret);
//			break;
		case db_gf_resume_gray_role:
			do_leave_del_role();
			err = resume_gray_role_callback(p, body, bodylen, ret);
			//err = delete_role_callback(p, body, bodylen, ret);
			break;
		case db_gf_verify_pay_passwd:
			err = verify_pay_passwd_callback(p, body, bodylen, ret);
			break;
        default:
            ERROR_LOG("err db cmd [%d %d]", cmd, p->uid);
    }
    if(err) {
		return -1;
    }

	return 0;
}

/**
 * @brief 副登录如果需要处理其它服务器的返回，比如说Switch的返回，可以通过实现这个接口来完成
 */
void KfLogin::proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
{
#if 1
	
	if (fd == m_switch_fd) {
		TRACE_LOG("m_switch_fd svrs return %u %u", fd, pkglen);
		handle_switch_return(reinterpret_cast<svr_proto_t*>(dbpkg), pkglen);
	} else if (fd == m_deluser_fd) {
		TRACE_LOG("m_deluser_fd svrs return %u %u", fd, pkglen);
		handle_deluser_return(reinterpret_cast<svr_proto_t*>(dbpkg), pkglen);
	}
#endif

}

/**
 * @brief 副登录如果需要处理其它服务器断开连接的情况，比如说Switch断开连接，可以通过实现这个接口来完成
 */
void KfLogin::on_other_svrs_fd_closed(int fd)
{
    if (fd == m_switch_fd) {
        DEBUG_LOG("SWITCH CONNECTION CLOSED\t[fd=%d]", m_switch_fd);
        m_switch_fd = -1;
    } else if (fd == m_timestamp_fd) { 
        DEBUG_LOG("CACHE SERVER CONNECTION CLOSED\t[fd=%d]", m_timestamp_fd);
        m_timestamp_fd = -1;
    } else if (fd == m_deluser_fd) {
        DEBUG_LOG("GF DELUSER SERVER CONNECTION CLOSED\t[fd=%d]", m_deluser_fd);
        m_deluser_fd = -1;
    }
}

#endif





#ifdef gf_client_cmd
//-----------------------------------------------------------
// client definations
//-----------------------------------------------------------

//--! recv

int KfLogin::check_invite_code_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, max_invi_code_len);
	DEBUG_LOG("check_invite_code:%u %s", p->uid, reinterpret_cast<char*>(body));
	return send_to_gf_db(p, 0, db_other_check_invite_code, body, max_invi_code_len);
}

int KfLogin::get_role_list_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, 16);
	memcpy(p->tmpinfo, body, bodylen);
	if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
		//varify the session
		login_session_t sess;
		(void)unpkg_auth(body, 16, &sess);
		if ( verify_session(p->uid, &sess) != gf_success) {
			return gf_close_conn;
		}
		TRACE_LOG("%u  %u", p->uid, bodylen);
		uint32_t role_tm = 0;
		return send_to_dbproxy(p, p->uid, db_gf_get_role_list, &role_tm, sizeof(role_tm));
	}
	
	return 0;
}

int KfLogin::create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, sizeof(role_info_t));

	role_info_t *role_info = (role_info_t*)body;
	role_info->parentid = ntohl(role_info->parentid); 
	role_info->channel  = ntohl(role_info->channel); 
	role_info->parent_role_tm  = ntohl(role_info->parent_role_tm); 
	memcpy(p->tmpinfo, body, bodylen);
	
	//DEBUG_LOG("%u puid: %u %u", p->uid, role_info->parentid, role_info->channel);
	DEBUG_LOG("%u puid: %u %u %u", p->uid, role_info->parentid, role_info->parent_role_tm, role_info->channel);
	if (db_check_session(p, gf_game_flag, (uint8_t*)(role_info->sess), gf_sess_len, 0) != 0) {
		char nick[gf_nick_len + 1];
		memcpy(nick, role_info->nick, gf_nick_len);
		nick[gf_nick_len] = '\0';
		if( check_dirty_word(p, nick)!=0 ) {
		    return 0;
		}
		role_info->role_type = ntohl(role_info->role_type);
		if( check_role_type(p, role_info->role_type) ) {
			return -1;
		}
		
		login_session_t sess;
		struct gf_register_in reg;
		memcpy(reg.nick, role_info->nick, gf_nick_len);
		reg.role_tm = time(NULL);
		reg.role_type = role_info->role_type;
		reg.level = 1;
		reg.hp = calc_hp[reg.role_type]( calc_body_quality[reg.role_type](reg.level) );
		reg.mp = calc_mp[reg.role_type]( calc_stamina[reg.role_type](reg.level) );
		reg.parent_role_tm = role_info->parent_role_tm;
		//varify the session
		(void)unpkg_auth((uint8_t*)role_info->sess, 16, &sess);
		if ( verify_session(p->uid, &sess) != gf_success) {
			return gf_close_conn;
		}
		
        if (reg.role_type == 4) {
            do_stat_log_interface(gf_report_dragon_exp, p->uid, 0, 0);
        } else {
            do_stat_log_interface(gf_report_user_role_exp, p->uid, reg.role_type, 0);
        }
		DEBUG_LOG("LOCAL SESSION ROLE REGISTER\t[uid=%u, role_type=%d, nick=%16s, level=%d, hp=%u, mp=%u]", 
				p->uid, reg.role_type, reg.nick, reg.level, reg.hp ,reg.mp);

#ifdef VERSION_KAIXIN
		uint32_t tmp_buf[4];
		tmp_buf[0] = 0;
		tmp_buf[1] = 0;
		tmp_buf[2] = 0;
		tmp_buf[3] = 0;
		tmp_buf[reg.role_type - 1] = 1;
		statistic_msglog(gf_role_type_distr, tmp_buf, sizeof(tmp_buf));		
		DEBUG_LOG("regrole\t[uid=%u, role_type=%d]", 
				p->uid, reg.role_type);

#endif
		if (p->create_role_flag) {
			return send_to_dbproxy(p, p->uid, db_gf_register, &reg, sizeof(gf_register_in));
		} else {
			return do_get_amb_info(p);
		}
	} else {
		return 0;
	}
}

int KfLogin::create_role_with_invite_code_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, sizeof(role_info_t));

	memcpy(p->tmpinfo, body, bodylen);
	if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
			
		role_info_t *role_info = (role_info_t*)body;
		//varify the session
		login_session_t sess;
		(void)unpkg_auth((uint8_t*)role_info->sess, 16, &sess);
		if ( verify_session(p->uid, &sess) != gf_success) {
			return gf_close_conn;
		}
		
		DEBUG_LOG("LOCAL SESSION ROLE REGISTER\t[uid=%u]", 
				p->uid);
#ifdef INVITE_CODE_VESION
		return do_check_user_invited(p);
#else
		return do_add_childid(p, 0, 0, 0);
#endif
		//return send_to_dbproxy(p, p->uid, db_userinfo_get_mee_fans, 0, 0);
		//return send_to_gf_db(p, 0, db_other_check_user_invited, 0, 0);
	}
	return 0;
}

int KfLogin::gray_delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen) 
{
    CHECK_VAL(bodylen, 52);

	memcpy(p->tmpinfo, body, bodylen);

	gf_gay_del_role_in* p_in = (gf_gay_del_role_in*)(p->tmpinfo);
	p_in->role_tm = ntohl(p_in->role_tm);

	if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
		return gf_close_conn;
	}
	return 0;
}

int KfLogin::logic_delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen) 
{
    CHECK_VAL(bodylen, 20);

	memcpy(p->tmpinfo, body, bodylen);
	if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
		return gf_close_conn;
	}
	return 0;
}

int KfLogin::resume_gray_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen) 
{
    CHECK_VAL(bodylen, 20);

	memcpy(p->tmpinfo, body, bodylen);
	if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
		return gf_close_conn;
	}
	return 0;
}
int KfLogin::get_version_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen) 
{
	m_clipkg->init(p->waitcmd, p->uid, gf_success);
    m_clipkg->pack(version_str, sizeof(version_str));
    return send_to_user(p);
}



int KfLogin::get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
    CHECK_VAL(bodylen, 28);
    char *sessbody = (char*)body;
    p->channel = ntohl(*(uint32_t*)(body + gf_sess_len));
	uint32_t role_tm = ntohl(*(uint32_t*)(body + gf_sess_len + 4));
	uint32_t role_lv = ntohl(*(uint32_t*)(body + gf_sess_len + 8));

    gf_login_out *user_login_info = reinterpret_cast<gf_login_out*>(p->tmpinfo);

    user_login_info->vip_flag    = 0;
    user_login_info->friendcount = 0;
    DEBUG_LOG("RECOMMD SVR CHECK SESS\t[uid=%d,role_tm=%u channel=%u]", p->uid, role_tm, p->channel);
    if (p->uid != 0) {
		if (db_check_session(p, gf_game_flag, body, gf_sess_len, 0) != 0) {
	        login_session_t sess;
	        //varify the session
	        unpkg_auth((uint8_t*)sessbody, 16, &sess);
	        if ( verify_session(p->uid, &sess) != gf_success ) {
	            return gf_close_conn;
	        }
			return send_to_dbproxy(p, p->uid, db_gf_login, &role_tm, sizeof(role_tm));
		}
		memcpy(p->tmpinfo, body, bodylen);
		return 0;
    } else {
        return get_recommended_svr_list(p, role_lv);
    }
}


int KfLogin::get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	TRACE_LOG("enter get_ranged_svr_list_cmd");
	//check protocol defined data, to prevent invalid access.
	CHECK_VAL_GE(bodylen, 12); 

	//get the client's request package body
	//uint32_t friendcount;
	uint32_t startid;
	uint32_t endid;
	int idx = 0;
	
	taomee::unpack(body, startid, idx);
	taomee::unpack(body, endid, idx);
	//if id is invalid, then make notification to the flash client.
	if ((startid < gf_min_valid_svr) || (endid > gf_max_valid_svr)
            || (startid > endid) || ((endid - startid) > 100)) {
        INFO_LOG("INVALID SVR ID\t[startid=%u endid=%u uid=%u]", startid, endid, p->uid);
        m_clipkg->init(p->waitcmd, p->uid, gf_success);
        m_clipkg->pack(static_cast<uint32_t>(0));
        return send_to_user(p);
    }
	DEBUG_LOG("RANGED SVR\t[uid=%u startid=%u endid=%u]", p->uid, startid, endid);

	//make a request to switch server, and get the response data.
	return get_ranged_svr_list(p, startid, endid);

}


//--! send
int KfLogin::send_head_to_user(usr_info_t* p,uint32_t cmd, uint32_t err_no)
{
	m_clipkg->init(cmd, p->uid, err_no);
	TRACE_LOG("send head to user %u %u %u", cmd, p->waitcmd, err_no);
	return send_to_user(p);
}

int KfLogin::send_error_to_user(usr_info_t* p, uint32_t err_no)
{
	return send_head_to_user(p, p->waitcmd, err_no);
}

int KfLogin::send_to_gf_db(usr_info_t* p, uint32_t role_tm, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	*(uint32_t*)m_gf_db_sendbuf = role_tm;
	memcpy(m_gf_db_sendbuf + sizeof(role_tm), dbpkgbuf, body_len);
	return send_to_dbproxy(p, p->uid, cmd, m_gf_db_sendbuf, body_len + sizeof(role_tm));
}

int KfLogin::send_to_gf_db(usr_info_t* p, uint32_t userid, uint32_t role_tm, uint16_t cmd, const void* dbpkgbuf, uint32_t body_len)
{
	*(uint32_t*)m_gf_db_sendbuf = role_tm;
	memcpy(m_gf_db_sendbuf + sizeof(role_tm), dbpkgbuf, body_len);
	return send_to_dbproxy(p, userid, cmd, m_gf_db_sendbuf, body_len + sizeof(role_tm));
}


int KfLogin::send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id)
{
	m_clipkg->init(proto_get_recommend_svr_list, p->uid, gf_success);
	m_clipkg->pack(static_cast<uint32_t>(max_online_id));
	m_clipkg->pack(*(uint32_t*)(p->tmpinfo));
	m_clipkg->pack(online_cnt);
    TRACE_LOG("max_online=%u online_cnt=%u",max_online_id,online_cnt);
    for (uint32_t i = 0; i != online_cnt; ++i) {
		m_clipkg->pack(svrs[i].id);
		m_clipkg->pack(svrs[i].users);
		m_clipkg->pack(svrs[i].ip, 16);
		m_clipkg->pack(svrs[i].port);
		m_clipkg->pack(svrs[i].domain);
		m_clipkg->pack(svrs[i].friends);
    	TRACE_LOG("online=[%u %u %16s %u %u]",svrs[i].id,svrs[i].users,svrs[i].ip,svrs[i].port,svrs[i].friends);
    }

    // pkg the frd id and timestamp
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t blackcount = *(uint32_t*)(p->tmpinfo + 8);
    uint32_t* friend_id = (uint32_t*)(p->tmpinfo + 12);
    uint32_t* black_id = (uint32_t*)(p->tmpinfo + 12 + sizeof(userid_t) * friendcount);

	m_clipkg->pack(friendcount);
    TRACE_LOG("friend_cnt=%u",friendcount);
    for (uint32_t i = 0; i < friendcount; i++ ) {
		m_clipkg->pack(friend_id[i]);
    	TRACE_LOG("friend_id=%u",friend_id[i]);
    }
	m_clipkg->pack(blackcount);
    TRACE_LOG("black_cnt=%u",blackcount);
    for (uint32_t i = 0; i < blackcount; i++ ) {
		m_clipkg->pack(black_id[i]);
    	TRACE_LOG("black_id=%u",black_id[i]);
    }
    
    statistic_info_t info;
    uint32_t msglog_type;
    info.user_id = p->uid;
    msglog_type = gf_login_offset + p->channel;
    statistic_msglog(msglog_type, (void *)&info, sizeof info);
	TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u p->channel:%u", stat_file, msglog_type, p->uid, p->channel);
	return send_to_user(p);
}

int KfLogin::send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt)
{
	//make a response package to the flash client
	m_clipkg->init(proto_get_ranged_svr_list, p->uid, gf_success);
	m_clipkg->pack(online_cnt);
	for (uint32_t i = 0; i != online_cnt; ++i) {
		m_clipkg->pack(svrs[i].id);
		m_clipkg->pack(svrs[i].users);
		m_clipkg->pack(svrs[i].ip, 16);
		m_clipkg->pack(svrs[i].port);
		m_clipkg->pack(svrs[i].domain);
		m_clipkg->pack(svrs[i].friends);
		
    	TRACE_LOG("ranged online=[%u %u %16s %u %u]",svrs[i].id, svrs[i].users, svrs[i].ip, svrs[i].port, svrs[i].friends);
	}
    return send_to_user(p);
}



#endif

#ifdef gf_db_proxy
//-----------------------------------------------------------
// db_proxy definations
//-----------------------------------------------------------


#endif


#ifdef gf_switch
//-----------------------------------------------------------
// switch definations
//-----------------------------------------------------------

int switch_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(owner);
	KfLogin* plogin = reinterpret_cast<KfLogin*>(data);
    TRACE_LOG("SWITCH TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
        plogin->get_backup_svr_list(p);
    }
	//send_warning(p, warning_switch, p->waitcmd);
	uint32_t nip = get_remote_ip(plogin->m_switch_fd);
    char     ip[INET_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &nip, ip, INET_ADDRSTRLEN);
	send_warning("switch", p->uid, p->waitcmd, 0, ip);
    return 0;
}

int KfLogin::send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (m_switch_fd == -1) {
        m_switch_fd = connect_to_service(m_my_opt.master_ser, 0, 65535, 1);
		TRACE_LOG("connect to switch %s fd=%d", m_my_opt.master_ser, m_switch_fd);
    }
    if (m_switch_fd == -1 && p) {
		TRACE_LOG("connect to switch failed %s", m_my_opt.master_ser);
        return get_backup_svr_list(p);
    }
    if(p) {
        ADD_TIMER_EVENT(p, switch_timeout, this, get_now_tv()->tv_sec + 10); 
    }
    return net_send(m_switch_fd , buf, length);
}

/**
  * @brierf Send a request package to switch server, then get the respond, but do not parse it.
  * @param  uid. user id.
  * @param    friendcount. haw many friends.
  * @param    allfriendid. store the ids in an array
  * @return     int. 0 on success. less than 0 on error.
  */
int KfLogin::get_ranged_svr_list(usr_info_t* p, int start_id, int end_id)
{
    TRACE_LOG("enter get_ranged_svr_list");
    // pack up the request data
	int friendcount = 0;
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(m_login_switch_sendbuf, start_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, end_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, friendcount, idx);
    init_switch_head(p, m_login_switch_sendbuf, idx, gf_switch_get_ranged_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, idx);
}

int KfLogin::get_backup_svr_list(usr_info_t* p)
{
	TRACE_LOG("enter get backup svr list");
    switch(p->waitcmd) {
        case proto_get_recommend_svr_list:
            return send_recommeded_svr_list(p, m_backup_svrlist, m_backup_svrlist_cnt, m_backup_svrlist_cnt);
        case proto_get_ranged_svr_list:
            return send_ranged_svrlist(p, m_backup_svrlist, m_backup_svrlist_cnt);
    default:
        ERROR_LOG("invalid command=%u", p->waitcmd);
    }
    return 0;
}

/**
  * @brierf Send a request package to switch server, then get the respond, but do not parse it.
  * @param  uid. user id.
  * @param    friendcount. haw many friends.
  * @param    allfriendid. store the ids in an array
  * @return     int. 0 on success. less than 0 on error.
  */
int KfLogin::get_recommended_svr_list(usr_info_t* p, uint32_t lv)
{
    TRACE_LOG("enter get_recommended_svr_list %u %u", p->uid, lv);
    // pack up the request data
    int idx = sizeof(svr_proto_t);
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 4);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint16_t>(m_my_opt.dx_or_wt), idx);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint8_t>(1), idx);
	taomee::pack_h(m_login_switch_sendbuf, lv, idx);
    taomee::pack_h(m_login_switch_sendbuf, friendcount, idx);
    taomee::pack(m_login_switch_sendbuf, (void*)(p->tmpinfo + 12), friendcount * sizeof(userid_t), idx);
    init_switch_head(p, m_login_switch_sendbuf, idx, gf_switch_get_recommend_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, idx);
}

void KfLogin::init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);

    pkg->len = len;
    pkg->seq = (p->session->fd << 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
}

int update_backup_svrlist(void* owner, void* data)
{
	KfLogin* plogin = reinterpret_cast<KfLogin*>(data);
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(plogin->m_login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, gf_min_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, gf_max_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, static_cast<uint32_t>(0), idx);
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(plogin->m_login_switch_sendbuf);

    pkg->len = idx;
    if(plogin->m_switch_fd == -1) {
		plogin->m_switch_fd = connect_to_service(plogin->m_my_opt.master_ser, 0, 65535, 1);
    }
    if(plogin->m_switch_fd == -1) {
        ADD_TIMER_EVENT(&(plogin->m_get_backup_tmr), update_backup_svrlist, plogin, get_now_tv()->tv_sec + 60);
        return 0;
    }
    pkg->seq = (0xFFFF<< 16) | 0;
    pkg->cmd = gf_switch_get_ranged_svr_list;
    pkg->ret = 0;
    pkg->id  = 0;
    plogin->send_to_switch(NULL, plogin->m_login_switch_sendbuf, idx);
    //TRACE_LOG("update backup svrlist");
    ADD_TIMER_EVENT(&(plogin->m_get_backup_tmr), update_backup_svrlist, plogin, get_now_tv()->tv_sec + 60);
    return 0;
}


#endif

#ifdef gf_del_server
//-----------------------------------------------------------
// del_server definations
//-----------------------------------------------------------

int mydeluser_timeout(void* owner, void* data)
{
    usr_info_t* p = reinterpret_cast<usr_info_t*>(owner);
	KfLogin* plogin = reinterpret_cast<KfLogin*>(data);
    ERROR_LOG("DELUSER TIMEOUT [%d %d]", p->uid, p->waitcmd);
    if(p->waitcmd) {
		uint32_t nip = get_remote_ip(plogin->m_deluser_fd);
	    char     ip[INET_ADDRSTRLEN] = "";
	    inet_ntop(AF_INET, &nip, ip, INET_ADDRSTRLEN);
		send_warning("delser", p->uid, p->waitcmd, 0, ip);
       // send_warning(p, warning_db, p->waitcmd);
		plogin->send_error_to_user(p, clierr_system_error);
       // send_to_self_error(p, clierr_system_error);
    }
    return 0;
}

int KfLogin::send_request_to_deluser(usr_info_t* p, userid_t uid, uint16_t cmd, const void* pkgbuf, uint32_t body_len)
{
    static uint8_t buf[1024];

    if (m_deluser_fd == -1) {
        m_deluser_fd = connect_to_svr(m_my_opt.gf_deluser_ip,
                                        m_my_opt.gf_deluser_port, 65535, 1);
    }

    if ((m_deluser_fd == -1) ||  (body_len > (sizeof(buf) - sizeof(svr_proto_t)))) {
        ERROR_LOG("send to deluser failed: fd=%d len=%d", m_deluser_fd, body_len);
        uint32_t nip = get_remote_ip(m_deluser_fd);
	    char     ip[INET_ADDRSTRLEN] = "";
	    inet_ntop(AF_INET, &nip, ip, INET_ADDRSTRLEN);
		send_warning("delser", uid, p ? p->waitcmd : 0, 0, ip);
        return -1;
    }

    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);
    pkg->len = sizeof(svr_proto_t) + body_len;
    pkg->seq = p ? ((p->session->fd<< 16) | p->counter) : 0;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = uid;
    //*(reinterpret_cast<uint32_t*>(pkg->body)) = 0;//role_tm
    memcpy((pkg->body), pkgbuf, body_len);

    DEBUG_LOG("DELUSER SEND\t[uid=%u cmd=0x%X]", uid ,cmd);
    if (p) {
        ADD_TIMER_EVENT(p, mydeluser_timeout, p, get_now_tv()->tv_sec + 10); 
    }
    return net_send(m_deluser_fd, buf, pkg->len);
}



#endif


#ifdef gf_call_back
//-----------------------------------------------------------
// call back definations
//-----------------------------------------------------------

int KfLogin::check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	
	if ((ret && ret !=  dberr_session_check && verify_login_session(p->uid, p->tmpinfo) == -1) ||
		(ret == dberr_session_check)) {
		
		ERROR_LOG("local:invalid session: uid=%u ret=%u", p->uid, ret);
		//send_header_to_player(p, p->waitcmd, cli_err_invalid_session, 1);
		return -1;
	}

	switch (p->waitcmd) {
	 case proto_get_role_list:
		return do_get_role_list(p);
	 case proto_create_role:
	 case proto_create_role_with_inv_code:
		{
			const role_info_t *role_info = (role_info_t*)(p->tmpinfo);
			if (p->create_role_flag || !(role_info->parentid) || !(role_info->parent_role_tm)) {
				return do_create_role(p, 0);
			} else {
				return do_get_amb_info(p);
			}
		}
		break;
	 case proto_get_recommend_svr_list:
		return do_get_recommend_svr_list(p);

	 case proto_gray_delete_role:
	 	return verify_pay_passwd(p);

	 case proto_resume_gray_role:
		return do_resume_gray_role(p);
	 default:
		break;
	}
	return -1;
}

int KfLogin::get_amb_info_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	role_info_t *role_info = (role_info_t*)p->tmpinfo;
	if (ret) {
		//if (p->create_role_flag || !(role_info->parentid)) {
			return do_create_role(p, 0);
		//} else {
		//	return do_add_childid(p, role_info->parentid);
		//}
	} else {
		//uint32_t parentid = *(uint32_t*)(body + 4);
		//role_info->parentid = parentid;
		DEBUG_LOG("child create: %u  parentid %u %u", p->uid, role_info->parentid, role_info->parent_role_tm);
		return do_create_role(p, 1);
		//return do_add_childid(p, role_info->parentid);
	}
	return -1;
}

int KfLogin::check_invite_code_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	if (ret)
	{
	    TRACE_LOG("CHECK INVITE FAILED\t[err=%d uid=%u]", ret, p->uid);
        switch (ret) {
			case gf_invalid_invite_code_err:
				DEBUG_LOG("invite invalid:%u", p->uid);
				send_error_to_user(p, gf_invalid_invite_code);
				break;
			case gf_invite_code_used_err:
				DEBUG_LOG("invite used:%u", p->uid);
				send_error_to_user(p, gf_invite_code_used);
				break;
       		default:
	            send_error_to_user(p, clierr_system_error);
	            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
	            break;
        }
        return gf_success;
	}
	return send_error_to_user(p, gf_success);
}

int KfLogin::get_mee_fans_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	if (ret)
	{
	    TRACE_LOG("CHECK USER INFO ERR\t[err=%d uid=%u] uid may not exist!", ret, p->uid);
        return -1;
	}
	uint32_t mee_fans = *(uint32_t*)body;
	if (p->waitcmd == proto_get_role_list) {
		if (mee_fans) {
			m_clipkg->init(proto_get_role_list, p->uid, gf_success);
			m_clipkg->pack(0);
			return send_to_user(p);
		} else {
			return do_check_user_invited(p);
		}
	} else if (p->waitcmd == proto_create_role_with_inv_code || p->waitcmd == proto_create_role) {
		if (mee_fans) {
			return do_add_childid(p, 0, 0, 0);
		} else {
			return do_check_user_invited(p);
		}
	}
	return gf_success;
}

int KfLogin::check_user_invited_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	if (ret)
	{
	    TRACE_LOG("CHECK USER INVITED FAILED\t[err=%d uid=%u]", ret, p->uid);

		send_error_to_user(p, gf_user_never_input_invite_code);

        return gf_success;
	}
	switch(p->waitcmd) {
		case proto_get_role_list:
			m_clipkg->init(proto_get_role_list, p->uid, gf_success);
			m_clipkg->pack(0);
			return send_to_user(p);
			break;
		case proto_create_role:
		case proto_create_role_with_inv_code:
			return do_add_childid(p, 0, 0, 0);
			break;
		default:
			return -1;
			break;
	}
	return gf_success;
	
}

int KfLogin::update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //chk if the pkg len is valid
    CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t));     
    m_backup_svrlist_cnt = svrlist->online_cnt;
    memcpy(m_backup_svrlist, svrlist->svrs, svrlist->online_cnt * sizeof(svr_info_t));
    //for(uint32_t i= 0; i< svrlist->online_cnt; i++) {
    //    DEBUG_LOG("BACKUP svr ID [%d]", backup_svrlist[i].id);
    //    DEBUG_LOG("BACKUP svr usr [%d]", backup_svrlist[i].users);
    //    DEBUG_LOG("BACKUP svr port[%d]", backup_svrlist[i].port);
    //}
    return 0;
}


int KfLogin::login_gf_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    if ( ret) {
        DEBUG_LOG("LOGIN FAILED\t[err=%d uid=%u]", ret, p->uid);
        switch (ret) {
        case gf_user_id_nofind_err:
		case gf_role_id_nofind_err:
            send_error_to_user(p, gf_role_not_find);
            break;
        default:
            send_error_to_user(p, clierr_system_error);
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            break;
        }
        return gf_success;
        //return gf_close_conn;
    }
    CHECK_VAL_GE(bodylen, 16);

    const gf_login_out* user_login_info = reinterpret_cast<const gf_login_out*>(body);
	uint32_t friendcount = user_login_info->friendcount;
	uint32_t blackcount = user_login_info->blackcount;

    CHECK_VAL(bodylen, 16 + 4 * friendcount + 4 * blackcount);
    TRACE_LOG("uid=%u, frdcnt=%u, blkcnt=%u",p->uid,friendcount,blackcount);

	if (friendcount > gf_max_friend_num) {
        ERROR_LOG("severe db error[friendcount]: uid=%u frdcnt=%u", p->uid, friendcount);
        friendcount = gf_max_friend_num;
    }
    if (blackcount > gf_max_friend_num) {
        ERROR_LOG("severe db error[blackcount]: uid=%u frdcnt=%u", p->uid, blackcount);
        blackcount = gf_max_friend_num;
    }
	
    *(uint32_t*)p->tmpinfo = user_login_info->vip_flag;
    *(uint32_t*)(p->tmpinfo + 4) =friendcount;
    *(uint32_t*)(p->tmpinfo + 8) = blackcount;
	TRACE_LOG("friend: %u  black: %u ", friendcount, blackcount);
    memcpy(p->tmpinfo + 12, user_login_info->frienditem, 4 * (friendcount + blackcount));
	UserSession_t * p_sess = (UserSession_t*)(p->tmpinfo);
    return get_recommended_svr_list(p, p_sess->level);
}

void KfLogin::translate_users_num_to_type(uint32_t online_cnt, svr_info_t* p_svrs)
{
	uint32_t max_svr_users = 300;
	for (uint32_t i = 0; i < online_cnt; i++) {
		uint32_t type = p_svrs[i].users * 7 / max_svr_users;
		
		DEBUG_LOG("tran: %u %u %u : %u", i, p_svrs[i].id, p_svrs[i].users, type ? type : 1);
		p_svrs[i].users = type ? type : 1;
		
	}
}

int KfLogin::get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    TRACE_LOG("get recommend callback");
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

	uint32_t stat_tmp = 1;
	uint32_t msglog_type = get_server_id() + 0x09010700;
	statistic_msglog(msglog_type, &stat_tmp, sizeof(stat_tmp));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
    CHECK_VAL(bodylen, sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
    uint32_t max_online_id = svrlist->max_online_id;
	translate_users_num_to_type(svrlist->online_cnt, svrlist->svrs);
    return send_recommeded_svr_list(p, svrlist->svrs, svrlist->online_cnt, max_online_id);
}

int KfLogin::get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	//varify the respond data
    TRACE_LOG("get ranged callback");
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //DEBUG_LOG("svrlist [%d %d]", svrlist->domain_id, svrlist->online_cnt);
	//chk if the pkg len is valid
	CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t)); 

	translate_users_num_to_type(svrlist->online_cnt, svrlist->svrs);
    return send_ranged_svrlist(p, svrlist->svrs, svrlist->online_cnt);
}

int KfLogin::get_role_list_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(role_list_header_t));
    const role_list_header_t* role_list_header = reinterpret_cast<const role_list_header_t*>(body);
#ifdef INVITE_CODE_VESION
    if (!role_list_header->role_count) {
        //check user invited
        return do_check_user_invited(p);
        //return send_to_dbproxy(p, p->uid, db_userinfo_get_mee_fans, 0, 0);
        //return send_to_dbproxy(p, db_other_check_user_invited, &tmp_role_tm, sizeof(tmp_role_tm));
    }
#endif
	int off_pos = 0;
	m_clipkg->init(proto_get_role_list, p->uid, gf_success);
	m_clipkg->pack(role_list_header->forbit_flag);
	m_clipkg->pack(role_list_header->deadline);
	m_clipkg->pack(role_list_header->vip_is);
	m_clipkg->pack(role_list_header->x_value);
    uint32_t left_val = 0;
    uint32_t x_lv = 0;
    if (role_list_header->vip_is != 0) {
        do_stat_log_interface(gf_report_user_vip_xvalue, p->uid, 0, role_list_header->x_value);
        x_lv = calc_player_vip_level(mgr, role_list_header->x_value, &left_val);
    }
	m_clipkg->pack(x_lv);
	m_clipkg->pack(role_list_header->vip_end_tm);
	m_clipkg->pack(role_list_header->role_count);
	TRACE_LOG("header [%u %u %u %u %u %u %u]",role_list_header->forbit_flag,role_list_header->deadline,
        role_list_header->vip_is, role_list_header->x_value,
        x_lv, role_list_header->vip_end_tm, role_list_header->role_count);

	for ( uint32_t i = 0; i < role_list_header->role_count; i++) {
		const role_list_info_t* role_info = reinterpret_cast<const role_list_info_t*>(body + sizeof(role_list_header_t) + off_pos);
		m_clipkg->pack(role_info->role_tm);
		m_clipkg->pack(role_info->role_type);

		uint32_t del_tm = 0;
		if (role_info->del_tm) {
			del_tm = get_now_tv()->tv_sec <= role_info->del_tm ? 1 : get_now_tv()->tv_sec - role_info->del_tm;
		} 
		m_clipkg->pack(del_tm);
		m_clipkg->pack(role_info->Ol_last);
		m_clipkg->pack(role_info->nick, gf_nick_len);
		m_clipkg->pack(role_info->level);
		m_clipkg->pack(role_info->clothes_count);
        if (role_info->role_type == 4) {
            do_stat_log_interface(gf_report_dragon_exp, p->uid, 0, role_info->exp);
        } else {
            do_stat_log_interface(gf_report_user_role_exp, p->uid, role_info->role_type, role_info->exp);
        }

		TRACE_LOG("[%u %u %16s %u %u %u %u]",role_info->role_tm, role_info->role_type,
				role_info->nick, role_info->level, role_info->clothes_count,
				role_info->Ol_last, role_info->del_tm);
        

		for ( uint32_t j = 0; j < role_info->clothes_count; j++) {
			const clothes_info_t* info = &(role_info->clothes_item[j]);

			m_clipkg->pack(info->clothes_id);
			m_clipkg->pack(info->get_time);
			m_clipkg->pack(info->duration);
			m_clipkg->pack(info->level);
			
			TRACE_LOG("clothes_info:[%u %u %u %u]",info->clothes_id, info->get_time, info->duration, info->level);
		}
		off_pos += sizeof(role_list_info_t) + role_info->clothes_count * sizeof(clothes_info_t);
	}
	
	return send_to_user(p);
}

int KfLogin::gf_create_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	uint32_t msglog_type;
	//login_tmpinfo_t* login_tmp = reinterpret_cast<login_tmpinfo_t*>(p->tmpinfo);
	role_info_t * login_tmp = reinterpret_cast<role_info_t*>(p->tmpinfo);
	uint32_t role_tm = *(reinterpret_cast<const uint32_t*>(body));
	uint32_t user_existed = *(reinterpret_cast<const uint32_t*>(body + 8));
	uint32_t parent_uid = *(reinterpret_cast<const uint32_t*>(body + 12));
	uint32_t parent_role_tm = *(reinterpret_cast<const uint32_t*>(body + 16));
    switch (ret) {
        case gf_success:		
            {
			msglog_type = gf_register_channel_offset;
#ifdef VERSION_TAIWAN
            msglog_type = gf_register_channel_offset_tw + login_tmp->channel;
#endif
			if (!user_existed) {
				uint32_t stat_tmp = 1;
				statistic_msglog(msglog_type, &stat_tmp, sizeof(stat_tmp));
				TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u channel:%u", stat_file, msglog_type, p->uid, login_tmp->channel);

				if ((240273282 <= p->uid && p->uid <= 240323464) ||
					(240223021 <= p->uid && p->uid <= 240273281) ||
					(240001000 <= p->uid && p->uid <= 240101443) ||
					(240101444 <= p->uid && p->uid <= 240223020) ||
					(241000100 <= p->uid && p->uid <= 241201966) ||
					(241400010 <= p->uid && p->uid <= 241601554) ||
					(240323465 <= p->uid && p->uid <= 240976689)) {
					uint32_t tmp_buf[2];
					tmp_buf[0] = p->uid;
					tmp_buf[1] = 1;
					statistic_msglog(0x0A000005, tmp_buf, sizeof(tmp_buf));
				}
				if (parent_uid) {
					db_add_record(parent_uid, 1);
				}
				
			}
			db_add_record(p->uid, 1);
            int version_flag = 0;

#ifdef VERSION_KAIXIN
            version_flag = 1;
#endif

#ifdef VERSION_TAIWAN
            version_flag = 2;
#endif

            if (version_flag) {
                uint32_t tmp_buf[4];
                tmp_buf[0] = 1;
                tmp_buf[1] = p->uid;
                tmp_buf[2] = p->uid;
                tmp_buf[3] = get_now_tv()->tv_sec;
                msglog_type = gf_register_mimi_offset;
                statistic_msglog(msglog_type, tmp_buf, sizeof(tmp_buf));
                TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u ", stat_file, msglog_type, p->uid);			
            } else {
                uint32_t tmp_buf[3];
                tmp_buf[0] = p->uid;
                tmp_buf[1] = p->uid;
                tmp_buf[2] = get_now_tv()->tv_sec;
                msglog_type = gf_register_mimi_offset;
                statistic_msglog(msglog_type, tmp_buf, sizeof(tmp_buf));
                TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u ", stat_file, msglog_type, p->uid);
            }

			uint32_t flag[2];	      	
			flag[0] = 1;	      	
			flag[1] = 0;	      	
			msglog_type = gf_report_lv_offset;
			statistic_msglog(msglog_type, &flag, sizeof(flag));
			TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u ", stat_file, msglog_type, p->uid);
            }
            break;
        case gf_user_role_existed_err:
            ERROR_LOG("ROLE CREATED ERR\t[uid=%u]", p->uid);
            return send_error_to_user(p, gf_role_existed_err);
            break;
        case gf_user_role_max_err:
            //ERROR_LOG("ROLE CREATED ERR\t[uid=%u]", p->uid);
            return send_error_to_user(p, gf_role_over_max_err);
            break;
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_error_to_user(p, clierr_system_error);
    }

	CHECK_VAL(bodylen, 20);
	/*check user role num*/
	uint32_t role_num = *(reinterpret_cast<const uint32_t*>(body + 4));
    DEBUG_LOG("PLAYER ROLE NUM\t[uid=%u, role_num=%u]", p->uid, role_num);
	if ( role_num >= gf_max_role_num ) {
		//uint32_t role_tm = 0;
		//send_request_to_deluser(p, deluser_gf_phy_del_role, &role_tm, sizeof(role_tm));
	}

	/*to add game flag*/
    //return add_game_flag(p);
#ifdef VERSION_KAIXIN
    return send_error_to_user(p, gf_success);
#else
	if (parent_uid && parent_role_tm) {
		do_add_childid(p, role_tm, parent_uid, parent_role_tm);
	}
	// update the gameflag
	DEBUG_LOG("ADD GAME FLAG\t[uid=%u] type=%u channel=%u", p->uid, login_tmp->role_type, login_tmp->channel);
	return db_add_game_flag(p, 6, login_tmp->channel);
#endif
}

/*int KfLogin::add_game_flag(usr_info_t* p) 
{
	int idx = 0;
	uint32_t gf_flag = 6;
	static char tmp_buf[sizeof(m_idc_zone) + sizeof(gf_flag)];
	
	//get verify md5
	idx = 0;
	pack_h(tmp_buf, m_idc_zone, idx);
	pack_h(tmp_buf, gf_flag, idx);
	const char* verify_code = req_verify_md5(tmp_buf, idx);

	//send buf
	idx = 0;
	pack_h(m_gf_db_sendbuf, m_channel_id, idx);
	pack(m_gf_db_sendbuf, verify_code, 32, idx);
	pack_h(m_gf_db_sendbuf, m_idc_zone, idx);
	pack_h(m_gf_db_sendbuf, gf_flag, idx);

	TRACE_LOG("%u %u %u", p->uid, m_idc_zone, gf_flag);
	return send_to_dbproxy(p, p->uid, db_gf_add_game_flag_ex, m_gf_db_sendbuf, idx);
}

int KfLogin::add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case gf_success:
            send_error_to_user(p, gf_success);
            break;
        case gf_user_id_nofind_err:
            ERROR_LOG("USER NOT REGISTED\t[uid=%u]", p->uid);
            send_error_to_user(p, gf_role_not_find);
            break;
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            add_game_flag(p);
            m_clipkg->init(p->waitcmd, p->uid, gf_success);
			send_to_user(p);
            break;
    }
    return gf_success;
}*/

int KfLogin::add_childid_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	return gf_success;
    if (ret == gf_success) {
		CHECK_VAL(bodylen, 8);
		uint32_t lv = *(uint32_t*)(body + 4);
		do_stat_log_child_lv_num(p->uid, lv);
    	do_create_role(p, 1);
    } else {
    	do_create_role(p, 0);
    }
    return gf_success;
}

int KfLogin::gray_delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
		case gf_success:
            m_clipkg->init(p->waitcmd, p->uid, gf_success);
			m_clipkg->pack(0);
			return send_to_user(p);   	
            break;
        default:
            DEBUG_LOG("DELETE ROLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_error_to_user(p, clierr_system_error);
	}
	return gf_success;
}

int KfLogin::logic_delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	uint32_t lv = *(uint32_t*)body;
	uint32_t flag[2];
	uint32_t msglog_type = 0;
	uint32_t role_tm = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len));
    switch (ret) {
		case gf_success:
            send_error_to_user(p, gf_success);	 

			DEBUG_LOG("SEND TO DEL SER ROLE\t[uid=%d,role_tm=%u]", p->uid, role_tm);
			send_request_to_deluser(0, p->uid, deluser_gf_add_del_role, &role_tm, sizeof(role_tm));

			flag[0] = 0;	      	
			flag[1] = 1;

			if (lv < 31) {
	            uint32_t msg_id = gf_report_lv_offset + lv - 1;
				statistic_msglog(msg_id, &flag, sizeof(flag));

	        } else if (lv > 30 && lv < 100 && !((lv - 1) % 5)){
	            uint32_t msg_id = gf_report_lv_offset + 30 + (lv - 30 - 1)/5;
				statistic_msglog(msg_id, &flag, sizeof(flag));

	        } else if (lv == 100) {
             	uint32_t msg_id = gf_report_lv_offset + 30 + (lv - 30)/5;
              	statistic_msglog(msg_id, &flag, sizeof(flag));
         	} 
			if (lv == 100) {
				msglog_type = gf_report_lv_offset + 10;
			} else {
				msglog_type = gf_report_lv_offset + (lv - 1) / 10;
			}
			statistic_msglog(msglog_type, &flag, sizeof(flag));
			TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u ", stat_file, msglog_type, p->uid);
            break;
        default:
            DEBUG_LOG("DELETE ROLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_error_to_user(p, clierr_system_error);
	}
	return gf_success;

}

int KfLogin::resume_gray_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
		case gf_success:
            send_error_to_user(p, gf_success);	      	
            break;
        default:
            DEBUG_LOG("DELETE ROLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_error_to_user(p, clierr_system_error);
	}
	return gf_success;
}

int KfLogin::verify_pay_passwd_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	if (ret) {
		DEBUG_LOG("VERIFY PAY PASSWD ERR\t[err=%d uid=%u]", ret, p->uid);
        m_clipkg->init(p->waitcmd, p->uid, gf_success);
		m_clipkg->pack(1);
		return send_to_user(p);
	}
	return do_gray_del_role(p);
}

/*
int KfLogin::delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	uint32_t lv = *(uint32_t*)body;
	uint32_t flag[2];
	uint32_t msglog_type = 0;
    switch (ret) {
		case gf_success:
            send_error_to_user(p, gf_success);	      	
			flag[0] = 0;	      	
			flag[1] = 1;

			if (lv < 31) {
	            uint32_t msg_id = gf_report_lv_offset + lv - 1;
				statistic_msglog(msg_id, &flag, sizeof(flag));

	        } else if (lv > 30 && lv < 100 && !((lv - 1) % 5)){
	            uint32_t msg_id = gf_report_lv_offset + 30 + (lv - 30 - 1)/5;
				statistic_msglog(msg_id, &flag, sizeof(flag));

	        } else if (lv == 100) {
             	uint32_t msg_id = gf_report_lv_offset + 30 + (lv - 30)/5;
              	statistic_msglog(msg_id, &flag, sizeof(flag));
         	} 
			if (lv == 100) {
				msglog_type = gf_report_lv_offset + 10;
			} else {
				msglog_type = gf_report_lv_offset + (lv - 1) / 10;
			}
			statistic_msglog(msglog_type, &flag, sizeof(flag));
			TRACE_LOG("statictic msglog :%s:msgid:%x  uid:%u ", stat_file, msglog_type, p->uid);
            break;
        default:
            DEBUG_LOG("DELETE ROLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_error_to_user(p, clierr_system_error);
	}
	return gf_success;
}
*/

void KfLogin::handle_deluser_return(svr_proto_t* pkg, uint32_t pkglen)
{
    if (!pkg->seq) {
        ERROR_LOG("deluser return error");
        return;
    }
    uint32_t counter = pkg->seq & 0xFFFF;
    uint32_t connfd  = pkg->seq >> 16;
	
    usr_info_t* p = user_mng.get_user(connfd);

	if(!p || (counter != p->counter)) {
        ERROR_LOG("connection closed: waitcmd=%u fd=%u uid=%u pkg->id=%u",
				p->waitcmd, connfd, p->uid, pkg->id);
        return;
    }

    DEBUG_LOG("DELUSER RETURN\t[uid=%u %u fd=%d cmd=0x%X ret=%u]",
                p->uid, pkg->id, connfd, pkg->cmd, pkg->ret);
    REMOVE_TIMERS(p);
	TRACE_LOG("psession fd %u %u %ld", p->session->fd, connfd ,p);
    int err = -1;
//    uint32_t bodylen = pkg->len - sizeof(svr_proto_t);
	/*
    switch(pkg->cmd) {
		
        case deluser_gf_delete_role:
            err = delete_role_callback(p, pkg->body, bodylen, pkg->ret);
            break;
		case deluser_gf_phy_del_role:
			if (pkg->ret) {
				ERROR_LOG("phy_del_role failed: waitcmd=%u fd=%u uid=%u pkg->id=%u",
				p->waitcmd, connfd, p->uid, pkg->id);
			}
			err = 0;
			break;
			
        default:
            ERROR_LOG("err deluser cmd [%d %d]", pkg->cmd, pkg->id);
    }*/
    if(err) {
        close_client_conn(connfd);
    }
}

void KfLogin::handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        ERROR_LOG("Switch return error");
        return;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    int connfd  = dbpkg->seq >> 16;
    uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t); 

    if(connfd == 0xFFFF) {
		TRACE_LOG("connfd == 0xFFFF  update_backup_svrlist_callback");
        update_backup_svrlist_callback(dbpkg->body, bodylen, dbpkg->ret);
        return;
    }

    usr_info_t* p = user_mng.get_user(connfd);
    if(!p || !p->waitcmd || p->uid != dbpkg->id) {
        ERROR_LOG("connection closed: fd=%u uid=%u %u",connfd, p ? p->uid : 0, dbpkg->id);
        return;
    }
    if(counter != p->counter) {
        ERROR_LOG("counter error [%d %d %d %d]", p->uid, p->waitcmd, counter, p->counter);
        return;
    }

    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u len=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret, dbpkg->len);
    int err = -1;
    REMOVE_TIMERS(p);
    switch(dbpkg->cmd) {
        case gf_switch_get_recommend_svr_list:
            err = get_recommeded_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case gf_switch_get_ranged_svr_list:
            err = get_ranged_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            DEBUG_LOG("err switch cmd [%d %d]", dbpkg->cmd, dbpkg->id);
    }
    if(err) {
        close_client_conn(connfd);
    }
}

#endif


#ifdef gf_utility
//-----------------------------------------------------------
// utility definations
//-----------------------------------------------------------

int KfLogin::init_timer()
{
    INIT_LIST_HEAD(&m_get_backup_tmr.timer_list);
    ADD_TIMER_EVENT(&m_get_backup_tmr, update_backup_svrlist, this, get_now_tv()->tv_sec + 60);
    return 0;
}

int KfLogin::my_read_conf()
{
    /*read all configuration param into global variable*/
    strcpy(m_my_opt.multicast_interface, config_get_strval("multicast_interface"));
    strcpy(m_my_opt.multicast_ip, config_get_strval("multicast_ip"));
    m_my_opt.multicast_port = config_get_intval("multicast_port", 0);


    strcpy(m_my_opt.gf_deluser_ip, config_get_strval("gf_deluser_ip"));
    m_my_opt.gf_deluser_port = config_get_intval("gf_deluser_port", 0);

    //strcpy(m_my_opt.master_ip, config_get_strval("master_ip"));
    //m_my_opt.master_port = config_get_intval("master_port", 0);
	strcpy(m_my_opt.master_ser, config_get_strval("master_ip"));
	
	m_my_opt.dx_or_wt = config_get_intval("domain", 0);

    return 0;
}

/**
  * @brierf initialize the multicast fd.
  * @param  void.
  * @return     int. 0 on success. -1 on database error.
  */
int KfLogin::multicast_init()
{
    //DEBUG_LOG("enter multicast_init");

    m_multicast_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (m_multicast_fd == -1) {
        ERROR_LOG("MCAST FD INIT ERROR");
        return -1;
    }

    m_multicast_addr.sin_family = AF_INET;
    inet_pton(AF_INET, m_my_opt.multicast_ip, &(m_multicast_addr.sin_addr));
    m_multicast_addr.sin_port = htons(m_my_opt.multicast_port);

    // Set Default Interface For Outgoing Multicasts
    in_addr_t ipaddr;
    if (inet_pton(AF_INET, m_my_opt.multicast_interface, &ipaddr)!=1)
       ERROR_LOG("inet_pton<=0 [%s]",m_my_opt.multicast_interface );

    if (setsockopt(m_multicast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
        ERROR_LOG("MCAST SET OPTION ERROR:%d,%m",errno );
        return -1;
    }
    return 0;
}

int KfLogin::unpkg_auth(const uint8_t body[], int len, login_session_t* sess)
{
    char outbuf[32];
    if ( len != 16 )
        return -1;

    //session: ip + time + userid + time
    des_decrypt(LOGIN_DES_KEY, (char*)(body), outbuf);
    des_decrypt(LOGIN_DES_KEY, (char*)(body + 8), outbuf + 8);
    sess->ip  = *(uint32_t *)outbuf;
    sess->uid = *(uint32_t *)(outbuf + 8);
    sess->tm1 = *(uint32_t*)(outbuf + 4);
    sess->tm2 = *(uint32_t*)(outbuf + 12);

    return 0;
}

/** 
 * @brief verify if the DESed session from the user is correct
 * @param const sprite_t*, the user that is currently logging in
 * @param const login_session_t*, UnDESed session
 *
 * @return int, 0 on success, otherwise failure
 */
int KfLogin::verify_session(const userid_t uid, const login_session_t* sess)
{
    time_t now_time = time(0);

    if (sess->uid != uid) {
        ERROR_RETURN(("verify userid failed: id=%u %u", uid, sess->uid), -1);
    }
    if (sess->tm1 != sess->tm2) {
        ERROR_RETURN(("verify time failed: tm=%u %u", sess->tm1, sess->tm2), -1);
    }
    int diff = now_time - sess->tm1;
    if ( (diff > 3600) || (diff < -3600) ) {
        ERROR_RETURN(("verify time failed: tm=%u %ld tmdiff=%d uid=%u", sess->tm1, now_time, diff, uid), -1);
    }
    return 0;
}

/**  
 * @brief check session
 * @param p   player_t*
 * @param gameid		项目类型:
 * @param session		计算得到的session值
 * @param session_len     session长度
 * @param del_flag		验证后是否要删除session(0:不是,1:是)
 * @return 0 on success, -1 on error
**/
int KfLogin::db_check_session(usr_info_t* p, uint32_t gameid, uint8_t* session, 
	uint32_t session_len, uint32_t del_flag)
{
	int idx = 0;
	pack_h(m_gf_db_sendbuf, gameid, idx);
	pack(m_gf_db_sendbuf, session, session_len, idx);
	pack_h(m_gf_db_sendbuf, del_flag, idx);

	return send_to_dbproxy(p, p->uid, db_sess_check_session, m_gf_db_sendbuf, idx);
}

int KfLogin::check_role_type(usr_info_t *p, uint32_t role_type)
{
	if( role_type < (uint32_t)gf_role_type_min || role_type > (uint32_t)gf_role_type_max ) {
		ERROR_LOG("ROLE_TYPE ERROR\t[uid=%u type=%d]",p->uid, role_type);
		return 1;
	} else {
		return 0;
	}
}

int KfLogin::do_get_mee_fans(usr_info_t *p)
{
	return send_to_dbproxy(p, p->uid, db_userinfo_get_mee_fans, 0, 0);
}

int KfLogin::do_check_user_invited(usr_info_t *p)
{
	DEBUG_LOG("check_invite_code:%u ", p->uid);
	return send_to_gf_db(p, 0, db_other_check_user_invited, 0, 0);
}


int KfLogin::do_gray_del_role(usr_info_t *p)
{
	gf_gay_del_role_in* p_in = (gf_gay_del_role_in*)(p->tmpinfo);

    DEBUG_LOG("DELETE ROLE\t[uid=%d,role_tm=%u]", p->uid, p_in->role_tm);

    //return send_request_to_db(p, DB_DELETE_ROLE, &role_tm, sizeof(role_tm));
	return send_to_gf_db(p, p_in->role_tm, db_gf_gray_delete_role, 0, 0);
}

int KfLogin::verify_pay_passwd(usr_info_t *p)
{
	gf_gay_del_role_in* p_in = (gf_gay_del_role_in*)(p->tmpinfo);
	char passwd_buf[128] = "";
	str2hex(p_in->paypasswd, 32, passwd_buf);
	printf("%s", p_in->paypasswd);
	return send_to_dbproxy(p, p->uid, db_gf_verify_pay_passwd, passwd_buf, 16);
}

int KfLogin::do_logic_del_role(usr_info_t *p)
{
	uint32_t role_tm = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len));

    DEBUG_LOG("LOGIC DELETE ROLE\t[uid=%d,role_tm=%u]", p->uid, role_tm);

    //return send_request_to_db(p, DB_DELETE_ROLE, &role_tm, sizeof(role_tm));
	return send_to_gf_db(p, role_tm, db_gf_logic_delete_role, 0, 0);

}

int KfLogin::do_resume_gray_role(usr_info_t *p)
{
	uint32_t role_tm = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len));

    DEBUG_LOG("RESUME ROLE\t[uid=%d,role_tm=%u]", p->uid, role_tm);

    //return send_request_to_db(p, DB_DELETE_ROLE, &role_tm, sizeof(role_tm));
	return send_to_gf_db(p, role_tm, db_gf_resume_gray_role, 0, 0);
}


int KfLogin::do_get_role_list(usr_info_t *p)
{
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(m_login_switch_sendbuf, p->uid, idx);
    init_switch_head(p, m_login_switch_sendbuf, idx, gf_switch_kick_user_offline);
    send_to_switch(0, m_login_switch_sendbuf, idx);

	uint32_t role_tm = 0;
	return send_to_dbproxy(p, p->uid, db_gf_get_role_list, &role_tm, sizeof(role_tm));
}
/*
INVITE_VESION
int KfLogin::do_create_role_with_inv_code(usr_info_t *p)
{
	return do_create_role(p, 0);
}
*/
int KfLogin::do_create_role(usr_info_t *p, uint32_t use_amb)
{
	role_info_t *role_info = (role_info_t*)p->tmpinfo;
	char nick[gf_nick_len + 1];
	memcpy(nick, role_info->nick, gf_nick_len);
	nick[gf_nick_len] = '\0';
	if( check_dirty_word(p, nick)!=0 ) {
		return 0;
	}
	role_info->role_type = ntohl(role_info->role_type);
	if( check_role_type(p, role_info->role_type) ) {
		return -1;
	}
	
	struct gf_register_in reg = {0};
	memcpy(reg.nick, role_info->nick, gf_nick_len);
	reg.role_tm = time(NULL);
	reg.role_type = role_info->role_type;
	reg.level = 1;
	reg.hp = calc_hp[reg.role_type]( calc_body_quality[reg.role_type](reg.level) );
	reg.mp = calc_mp[reg.role_type]( calc_stamina[reg.role_type](reg.level) );
	if (use_amb) {
		reg.parentid = role_info->parentid;
		reg.parent_role_tm = role_info->parent_role_tm;
	}
#ifdef VERSION_KAIXIN
	uint32_t tmp_buf[4];
	tmp_buf[0] = 0;
	tmp_buf[1] = 0;
	tmp_buf[2] = 0;
	tmp_buf[3] = 0;
	tmp_buf[reg.role_type - 1] = 1;
	statistic_msglog(gf_role_type_distr, tmp_buf, sizeof(tmp_buf)); 	
	DEBUG_LOG("regrole\t[uid=%u, role_type=%d]", 
			p->uid, reg.role_type);
			
#endif

	DEBUG_LOG("ROLE REGISTER\t[uid=%u, role_type=%d, nick=%16s, level=%d, hp=%u, mp=%u channel=%u %u %u]", 
			p->uid, reg.role_type, reg.nick, reg.level, reg.hp ,reg.mp, role_info->channel,
			reg.parentid, reg.parent_role_tm);
	return send_to_dbproxy(p, p->uid, db_gf_register, &reg, sizeof(gf_register_in));	
}

int KfLogin::do_add_childid(usr_info_t *p, uint32_t child_role_tm, userid_t parentid, uint32_t parent_role_tm)
{
	do_master_num(p->uid);
	DEBUG_LOG("ROLE REGISTER\t[uid=%u, puid=%u %u", p->uid, parentid, parent_role_tm);
	int idx = 0;
	pack_h(m_gf_db_sendbuf, parent_role_tm, idx);
	pack_h(m_gf_db_sendbuf, p->uid, idx);
	pack_h(m_gf_db_sendbuf, child_role_tm, idx);
	return send_to_dbproxy(0, parentid, db_gf_add_childid, m_gf_db_sendbuf, idx);	
}
int KfLogin::db_add_record(uint32_t userid, uint32_t value)
{
	DEBUG_LOG("ADD RECORD\t[uid=%u, value=%u", userid, value);
	int idx = 0;
	uint8_t tmp_buf[16] = { 0 };
	pack_h(tmp_buf, 1, idx);
	pack_h(tmp_buf, userid, idx);
	pack_h(tmp_buf, value, idx);

	return send_to_gf_db(0, userid, 0, db_gf_insert_tmp_info, tmp_buf, idx);	
}


int KfLogin::do_get_amb_info(usr_info_t *p)
{
	const role_info_t *role_info = (const role_info_t*)p->tmpinfo;
	DEBUG_LOG("ROLE REGISTER\t[uid=%u parent %u %u]", p->uid, role_info->parentid, role_info->parent_role_tm);
	int idx = 0;
	pack_h(m_gf_db_sendbuf, role_info->parent_role_tm, idx);
	return send_to_dbproxy(p, role_info->parentid, db_gf_get_amb_info, m_gf_db_sendbuf, idx);	
}

int KfLogin::do_get_recommend_svr_list(usr_info_t* p)
{
    p->channel = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len));
	uint32_t role_tm = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len + 4));
	uint32_t role_lv = ntohl(*(uint32_t*)(p->tmpinfo + gf_sess_len + 8));
	UserSession_t * p_sess = (UserSession_t*)(p->tmpinfo);
	p_sess->level = role_lv;

    gf_login_out *user_login_info = reinterpret_cast<gf_login_out*>(p->tmpinfo);
    user_login_info->vip_flag    = 0;
    user_login_info->friendcount = 0;
	
    DEBUG_LOG("RECOMMD SVR\t[uid=%d,role_tm=%u channel=%u]", p->uid, role_tm, p->channel);
    if (p->uid != 0) {
		return send_to_dbproxy(p, p->uid, db_gf_login, &role_tm, sizeof(role_tm));
    } else {
    	return -1;
    }
}

int KfLogin::check_dirty_word(usr_info_t* p, char* msg)
{
	CHECK_DIRTYWORD(p, msg);
	return 0;
}

/**
  * @brierf handle the multicast message to multicast group members.
  * @param  body_buf. Package body buffer.
  * @param    body_len. Body buffer's len.
  * @return     int. 0 on success. -1 on database error.
  */
int  KfLogin::multicast_player_login(uint8_t* body_buf, int body_len)
{
    TRACE_LOG("enter multicast_player_login");

    static char multicast_buf[4096];

    int len = sizeof(login_online_pk_header_t) + body_len;

    if ( len > 4096 ) {
        ERROR_LOG("multicast send len is too big");
        return -1;
    }

    // make the package header
    login_online_pk_header_t* pHeader =(login_online_pk_header_t*)multicast_buf;
    pHeader->len        = len;
    pHeader->online_id  = 0;
    pHeader->cmd        = gf_multicast_cmd;
    pHeader->ret        = 0;
    pHeader->id         = 0;
    pHeader->mapid      = 0;
    pHeader->opid       = 1100; //online cmd, notify friend login
    memcpy(multicast_buf + sizeof(login_online_pk_header_t), body_buf, body_len);

    sendto(m_multicast_fd, multicast_buf, len, 0,
            reinterpret_cast<sockaddr*>(&m_multicast_addr), sizeof(m_multicast_addr));

    return 0;
}

#endif

KfLogin login;


