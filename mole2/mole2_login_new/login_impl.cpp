#include "login_impl.hpp"

using namespace taomee;

static char version[200] = __DATE__" "__TIME__;

int switch_timeout(void* owner, void* data)
{
    usr_info_t*		p = reinterpret_cast<usr_info_t*>(owner);
	mole2_login*	plogin = reinterpret_cast<mole2_login*>(data);
    KDEBUG_LOG(p->uid, "SWITCH TIMEOUT CMD=%d", p->waitcmd);
    if (p->waitcmd) {
        plogin->get_backup_svr_list(p);
    }
    return 0;
}

int update_backup_svrlist(void* owner, void* data)
{
	mole2_login* plogin = reinterpret_cast<mole2_login*>(data);
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(plogin->m_login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, mole2_min_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, mole2_max_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, static_cast<uint32_t>(0), idx);
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(plogin->m_login_switch_sendbuf);

    pkg->len = idx;
    pkg->seq = (0xFFFF<< 16) | 0; // backup fd
    pkg->cmd = mole2_switch_get_ranged_svr_list;
    pkg->ret = 0;
    pkg->id  = 0;
    plogin->send_to_switch(NULL, plogin->m_login_switch_sendbuf, idx);

    ADD_TIMER_EVENT(&(plogin->m_get_backup_tmr), update_backup_svrlist, plogin, get_now_tv()->tv_sec + 60);
    return 0;
}

//====================================

bool mole2_login::init_service()
{
	if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
			ERROR_LOG("Failed to load dirty word!");
			return -1;
	}

	init_timer();
    my_read_conf();

	return true;
}


void mole2_login::fini_service()
{
	    return;
}

int mole2_login::get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len)
{
    return get_dbproxy_pkg_len(avail_data);	
}

int mole2_login::proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
{
	KDEBUG_LOG(usr->uid,"CLIENT CMD=%u",usr->waitcmd);
	switch (usr->waitcmd) {
		case proto_get_recommend_svr_list:
			return get_recommeded_svr_list_cmd(usr, body, bodylen);
		case proto_get_ranged_svr_list:
        	return get_ranged_svr_list_cmd(usr, body, bodylen);				
#ifdef mole2_key
		case proto_activation:
			return activation_cmd(usr, body, bodylen);
		case proto_create_role:
			return check_mfan(usr, body, bodylen);
#else
		case proto_create_role:
        	return create_role_cmd(usr, body, bodylen);
#endif
		case proto_version:
			m_clipkg->init(usr->waitcmd, usr->uid, mole2_success);
			m_clipkg->pack(version, 200);
			send_to_user(usr);
		default:
        	KERROR_LOG(usr->uid, "INVALID CLIENT CMD=%u",usr->waitcmd);
			break;
	}
	return -1;
}

int mole2_login::proc_other_dbproxy_return(usr_info_t* p,
		uint16_t cmd,
		const uint8_t* body,
		uint32_t bodylen,
		uint32_t ret)
{
    int err = -1;
    switch(cmd) {
        case db_mole2_login:
            err = login_mole2_callback(p, body, bodylen, ret);
            break;
        case db_mole2_role_register:
            err = mole2_register_callback(p, body, bodylen, ret);
            break;
		case db_base_session_check:
			err = check_session_callback(p, body, bodylen, ret);
			break;
#ifdef mole2_key
		case db_base_get_create_able_flags:
			err = check_mfan_callback(p, body, bodylen, ret);
			break;
		case db_mole2_activation:
			err = activation_callback(p, body, bodylen, ret);
			break;
#endif
        default:
            KERROR_LOG(p->uid,"INVALID CMD RETURN [0x%X]", cmd);
			break;
    }
    if(err) {
        return -1;
    }

    return 0;
}

void mole2_login::proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
{
	if (fd == m_switch_fd) {
		TRACE_LOG("m_switch_fd svrs return %u %u", fd, pkglen);
		handle_switch_return(reinterpret_cast<svr_proto_t*>(dbpkg), pkglen);
	}
}

void mole2_login::on_other_svrs_fd_closed(int fd)
{
	if (fd == m_switch_fd) {
		TRACE_LOG("SWITCH CONNECTION CLOSED\t[fd=%d]", m_switch_fd);
		m_switch_fd = -1;
	}
}

//=============================
int mole2_login::get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, 20);

	p->channel = ntohl(*(uint32_t*)(body + mole2_sess_len));
 	KDEBUG_LOG(p->uid, "RECOMMD SVR\t[channel=%u]", p->channel);
 	if (send_to_dbproxy_check_session(p, mole2_game_flag, (uint8_t*)body, mole2_sess_len, mole2_del_flag) != mole2_success) {
		if (verify_login_session(p->uid, body) == -1)
			return mole2_close_conn;
		return send_to_dbproxy(p, p->uid, db_mole2_login, NULL, 0);
	}
	memcpy(p->tmpinfo, body, bodylen);
	return 0;
}

int mole2_login::get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL_GE(bodylen, 12);

	int idx = 0;
	uint32_t endid;
	uint32_t startid;
	
	KDEBUG_LOG(p->uid,"LIST SVR");
	taomee::unpack(body, startid, idx);
	taomee::unpack(body, endid, idx);
	if ((startid < mole2_min_valid_svr) || (endid > mole2_max_valid_svr)
            || (startid > endid) || ((endid - startid) > 100)) {
        KERROR_LOG(p->uid,"INVALID SVR RANGE\t[startid=%u endid=%u]", startid, endid);
        m_clipkg->init(p->waitcmd, p->uid, mole2_success);
        m_clipkg->pack(static_cast<uint32_t>(0));
        return send_to_user(p);
    }
	return get_ranged_svr_list(p, startid, endid);
}

int mole2_login::create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL_GE(bodylen, mole2_sess_len);
	CHECK_VAL_LE(bodylen, mole2_sess_len + mole2_sess_len);

	KDEBUG_LOG(p->uid, "create_role_cmd bodylen=%d",bodylen);

	if(bodylen == mole2_sess_len + mole2_sess_len) {
		char hex[32] = {0};
		char buff[8] = {0};
		memcpy(hex,body + mole2_sess_len,16);
		str2hex(body + mole2_sess_len,16,buff);
		des_decrypt(UID_DESKEY,buff,p->tmpinfo + 256);
		des_data_t *dat = reinterpret_cast<des_data_t*>(p->tmpinfo + 256);
		KDEBUG_LOG(p->uid, "hex=%s,parent=%u,tag=%c%c%c%c",hex,dat->parent,dat->magic[0],dat->magic[1],dat->magic[2],dat->magic[3]);
	} else {
		memset(p->tmpinfo + 256,0,sizeof(des_data_t));
	}

//verify the session
	if (send_to_dbproxy_check_session(p, mole2_game_flag, body, mole2_sess_len, mole2_del_flag) != mole2_success) {
		if (verify_login_session(p->uid, body) == -1) {
			return mole2_close_conn;
		}

		return do_create_role(p);
	}	
	return mole2_success;
}

#ifdef mole2_key
int mole2_login::check_mfan(usr_info_t* p, const uint8_t* body, int len)
{
	uint32_t idc_zone = 0;
	CHECK_VAL(len, mole2_sess_len);
	memcpy(p->tmpinfo, body, mole2_sess_len);
	return send_to_dbproxy(p, p->uid, db_base_get_create_able_flags, &idc_zone, sizeof(idc_zone));
}

int mole2_login::activation_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, mole2_activation_len + mole2_sess_len);
	char	sz_code[mole2_activation_len];
	memcpy(p->tmpinfo, body, mole2_sess_len);
	memcpy(sz_code, body + mole2_sess_len, mole2_activation_len);	
	KDEBUG_LOG(p->uid,"ACTIVATION CMD:[%s]", sz_code);
	return send_to_dbproxy(p, p->uid, db_mole2_activation, sz_code, mole2_activation_len);
}

#endif

int mole2_login::get_backup_svr_list(usr_info_t* p)
{
    switch (p->waitcmd) {
        case proto_get_recommend_svr_list:
            return send_recommeded_svr_list(p, m_backup_svrlist, m_backup_svrlist_cnt, m_backup_svrlist_cnt);
        case proto_get_ranged_svr_list:
            return send_ranged_svrlist(p, m_backup_svrlist, m_backup_svrlist_cnt);
    default:
        KERROR_LOG(p->uid, "INVALID CMD=%u", p->waitcmd);
    }
    return 0;
}

int mole2_login::get_recommended_svr_list(usr_info_t* p)
{
    KDEBUG_LOG(p->uid, "RECOMMEND SVR");
    // pack up the request data
    int idx = sizeof(svr_proto_t);
    uint32_t last_online = *(uint32_t*)(p->tmpinfo + 4);
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 8);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint16_t>(m_my_opt.dx_or_wt), idx);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint8_t>(1), idx);
    taomee::pack_h(m_login_switch_sendbuf, last_online, idx);
    taomee::pack_h(m_login_switch_sendbuf, friendcount, idx);
    taomee::pack(m_login_switch_sendbuf, (void*)(p->tmpinfo + 16), friendcount * sizeof(userid_t), idx);

    init_switch_head(p, m_login_switch_sendbuf, idx, mole2_switch_get_recommend_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, idx);
}

int mole2_login::get_ranged_svr_list(usr_info_t* p, int start_id, int end_id)
{
	KDEBUG_LOG(p->uid, "RANGED SVR\t[startid=%u endid=%u]",start_id, end_id);

	int friendcount = 0;
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(m_login_switch_sendbuf, static_cast<uint16_t>(0), idx);
    taomee::pack_h(m_login_switch_sendbuf, start_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, end_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, friendcount, idx);
    init_switch_head(p, m_login_switch_sendbuf, idx, mole2_switch_get_ranged_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, idx);
}


void mole2_login::init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);

    pkg->len = len;
    pkg->seq = (p->session->fd << 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
}

int mole2_login::login_mole2_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    if ( ret) {
        KDEBUG_LOG(p->uid,"LOGIN FAILED ERR=%d", ret);
        switch (ret) {
            case mole2_user_id_nofind_err:
                send_error_to_user(p, mole2_user_nofind);
                break;
            case mole2_user_noused_err:
                send_error_to_user(p, mole2_user_be_ban);
                break;
            case mole2_user_offlin24_err:
                send_error_to_user(p, mole2_user_be_24_ban);
                break;
            case mole2_user_not_enabled_err:
                send_error_to_user(p, mole2_user_no_actived);
                break;
            case mole2_user_offline_seven_day_err:
                send_error_to_user(p, mole2_user_be_7_day_ban);
                break;
            case mole2_user_offline_fourteen_day_err:
                send_error_to_user(p, mole2_user_be_14_day_ban);
				break;
			case mole2_user_anxi_service_out_of_err:
                send_error_to_user(p, mole2_user_out_of_anxi);
                break;
            default:
                send_error_to_user(p, mole2_system_err);
                KERROR_LOG(p->uid, "login_mole2 UNRESOLVABLE DB ERR=%d", ret);
                break;
        }
        return mole2_success;
        //return mole2_close_conn;
    }
    CHECK_VAL_GE(bodylen, sizeof(mole2_login_out));
    const mole2_login_out* user_login_info = reinterpret_cast<const mole2_login_out*>(body);
    uint32_t friendcount = user_login_info->friendcount;
    uint32_t blackcount = user_login_info->blackcount;

	KDEBUG_LOG(p->uid,"vip=%d,online=%d,friends=%d blacks=%d", 
		user_login_info->vip_flag,
		user_login_info->last_online,
		friendcount,blackcount);

    CHECK_VAL(bodylen, sizeof(mole2_login_out) + 4 * friendcount + 4 * blackcount);

    if (friendcount > mole2_max_friend_num) {
        KERROR_LOG(p->uid, "DATA ERR friendcount=%u", friendcount);
        friendcount = mole2_max_friend_num;
    }
    if (blackcount > mole2_max_friend_num) {
        KERROR_LOG(p->uid, "DATA ERR blackcount=%u", blackcount);
        blackcount = mole2_max_friend_num;
    }

    *(uint32_t*)p->tmpinfo = user_login_info->vip_flag;
	*(uint32_t*)(p->tmpinfo + 4) = user_login_info->last_online;
    *(uint32_t*)(p->tmpinfo + 8) = friendcount;
    *(uint32_t*)(p->tmpinfo + 12) = blackcount;
    memcpy(p->tmpinfo + 16, user_login_info->frienditem, 4 * (friendcount + blackcount));
    return get_recommended_svr_list(p);
}

int mole2_login::get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
    CHECK_VAL(bodylen, sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
    int max_online_id = svrlist->max_online_id;
    return send_recommeded_svr_list(p, svrlist->svrs, svrlist->online_cnt, max_online_id);
}

int mole2_login::get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
	CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t)); 
    return send_ranged_svrlist(p, svrlist->svrs, svrlist->online_cnt);
}

int mole2_login::mole2_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case mole2_success:		
            break;
		case mole2_key_existed_err:
        case mole2_user_role_existed_err:
            KERROR_LOG(p->uid,"IGNORE:ROLE CREATED ERR");
            break;
        default:
            KERROR_LOG(p->uid,"mole2_register UNRESOLVABLE DB ERR\t[err=%d]", ret);
            return send_error_to_user(p, clierr_system_error);
    }
	return db_add_game_flag(p,7,p->channel);
}

int mole2_login::update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //chk if the pkg len is valid
    CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t));
    m_backup_svrlist_cnt = svrlist->online_cnt;
    memcpy(m_backup_svrlist, svrlist->svrs, svrlist->online_cnt * sizeof(svr_info_t));
    return 0;
}

int mole2_login::check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	CHECK_VAL_GE(bodylen, 0);
	if ((ret && ret !=  dberr_session_check && verify_login_session(p->uid, p->tmpinfo) == -1) ||
		(ret == dberr_session_check)) {
		KERROR_LOG(p->uid, "INVALID SESSION ERR=%d",ret);
		return -1;
	}
	switch (p->waitcmd) {
		case proto_get_recommend_svr_list:
			send_to_dbproxy(p, p->uid, db_mole2_login, NULL, 0);
			break;
		case proto_create_role:
			do_create_role(p);
			break;
#ifdef mole2_key

		case proto_activation:
			do_create_role(p);
			break;
#endif
		default:
			KERROR_LOG(p->uid, "INVALID CMD=%u",p->waitcmd);
	}
       
    return 0;
}

#ifdef mole2_key


int mole2_login::check_mfan_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	typedef struct {
		uint32_t   mfan;
		uint32_t   create_flag;
		uint32_t   dx_gameflag;
		uint32_t   wt_gameflag;
	} __attribute__((packed)) flags_t;
	
	
	if (ret) {
		KERROR_LOG(p->uid, "GET CREATE FLAGS ERR=%d", ret);
		return send_error_to_user(p, mole2_system_err);
	}
	
	CHECK_VAL(bodylen, sizeof(flags_t));
	const flags_t *pflag = reinterpret_cast<const flags_t*>(body);

	KDEBUG_LOG(p->uid,"GET CREATE FLAGS [%u,%u,%u,%u]",pflag->mfan,pflag->create_flag,pflag->dx_gameflag,pflag->wt_gameflag);

	if(!(pflag->dx_gameflag & (1 << (mole2_game_flag-1))) &&
		!(pflag->wt_gameflag & (1 << (mole2_game_flag-1))) ) {
		if(!pflag->mfan && !(pflag->create_flag & (1 << (mole2_game_flag-1)))) {
			KDEBUG_LOG(p->uid,"CHECK CREATE FLAGS FAILED");
			return send_error_to_user(p, mole2_nokey_err);
		}
	}

	return create_role_cmd(p, p->tmpinfo, mole2_sess_len);
}

int mole2_login::activation_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{	
	if (ret) {
		KERROR_LOG(p->uid, "ACTIVATION ERR=%d", ret);	
		return send_error_to_user(p, mole2_normal_err + ret);	
	}
		
	return create_role_cmd(p, p->tmpinfo, mole2_sess_len);
}

#endif

int mole2_login::send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id)
{
    m_clipkg->init(proto_get_recommend_svr_list, p->uid, mole2_success);
    m_clipkg->pack(static_cast<uint32_t>(max_online_id)); // max online id  packed 
    m_clipkg->pack(*(uint32_t*)(p->tmpinfo)); // whether vip
    m_clipkg->pack(online_cnt);

    for (uint32_t i = 0; i != online_cnt; ++i) {
        m_clipkg->pack(svrs[i].id);
        m_clipkg->pack(get_online_usercnt_level(svrs[i].users));
        m_clipkg->pack(svrs[i].ip, 16);
        m_clipkg->pack(svrs[i].port);
        m_clipkg->pack(svrs[i].friends);
   }

    // pkg the frd id and timestamp
    uint32_t friendcount = *(uint32_t*)(p->tmpinfo + 8);
    uint32_t blackcount = *(uint32_t*)(p->tmpinfo + 12);
    uint32_t* friend_id = (uint32_t*)(p->tmpinfo + 16);
    uint32_t* black_id = (uint32_t*)(p->tmpinfo + 16 + sizeof(userid_t) * friendcount);

	uint32_t timestamp = 0;
    m_clipkg->pack(friendcount);
    for (uint32_t i = 0; i < friendcount; i++ ) {
        m_clipkg->pack(friend_id[i]);
		m_clipkg->pack(timestamp);
    }
    m_clipkg->pack(blackcount);
    for (uint32_t i = 0; i < blackcount; i++ ) {
        m_clipkg->pack(black_id[i]);
    }

    statistic_info_t info;
    uint32_t msglog_type;
    info.user_id = p->uid;
    msglog_type = mole2_login_offset + p->channel;
    if (p->channel > 56)
        msglog_type = mole2_not_normal_log_channel;
    statistic_msglog(msglog_type, (void *)&info, sizeof info);

    return send_to_user(p);
}

int mole2_login::send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt)
{
	//make a response package to the flash client
	m_clipkg->init(proto_get_ranged_svr_list, p->uid, mole2_success);
	m_clipkg->pack(online_cnt);
	for (uint32_t i = 0; i != online_cnt; ++i) {
		m_clipkg->pack(svrs[i].id);
		m_clipkg->pack(get_online_usercnt_level(svrs[i].users));
		m_clipkg->pack(svrs[i].ip, 16);
		m_clipkg->pack(svrs[i].port);
		m_clipkg->pack(svrs[i].friends);
	}
    return send_to_user(p);
}



//-----------------------------------------------------------
// switch definations
//-----------------------------------------------------------

int mole2_login::send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length)
{
    if (m_switch_fd == -1) {
        m_switch_fd = connect_to_service(m_my_opt.master_ser, 0, 65535, 1);
    }
    if (m_switch_fd == -1 && p) {
        return get_backup_svr_list(p);
    }
    if(p) {
        ADD_TIMER_EVENT(p, switch_timeout, this, get_now_tv()->tv_sec + 10); 
    }
    return net_send(m_switch_fd , buf, length);
}

void mole2_login::handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen)
{
    if (!dbpkg->seq) {
        ERROR_LOG("switch return seq err");
        return;
    }

    uint32_t counter = dbpkg->seq & 0xFFFF;
    int connfd  = dbpkg->seq >> 16;
    uint32_t bodylen = dbpkg->len - sizeof(svr_proto_t); 

    if(connfd == 0xFFFF) {// backup fd is 0xFFFF in func: update_backup_svrlist 
        update_backup_svrlist_callback(dbpkg->body, bodylen, dbpkg->ret);
        return;
    }

    usr_info_t* p = user_mng.get_user(connfd);
	
	if(!p || (counter != p->counter)) {
    	if (p) {
        	KERROR_LOG(p->uid, "switch return pkg err fd=%u %u",connfd, dbpkg->id);
    	} else {
			ERROR_LOG("user pointer is null");
		}
        return;
    }

    DEBUG_LOG("SWITCH RETURN\t[uid=%u %u fd=%d cmd=%u ret=%u len=%u]",
                p->uid, dbpkg->id, connfd, dbpkg->cmd, dbpkg->ret, dbpkg->len);
    int err = -1;
    REMOVE_TIMERS(p);
    switch(dbpkg->cmd) {
        case mole2_switch_get_recommend_svr_list:
            err = get_recommeded_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case mole2_switch_get_ranged_svr_list:
            err = get_ranged_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        default:
            ERROR_LOG("err cmd [%d %d]", dbpkg->cmd, dbpkg->id);
			break;
    }
    if(err) {
        close_client_conn(connfd);
    }
}


int mole2_login::init_timer()
{
    INIT_LIST_HEAD(&m_get_backup_tmr.timer_list);
    ADD_TIMER_EVENT(&m_get_backup_tmr, update_backup_svrlist, this, get_now_tv()->tv_sec + 60);
    return 0;
}

int mole2_login::my_read_conf()
{
	strcpy(m_my_opt.master_ser, config_get_strval("master_ip"));
	m_my_opt.dx_or_wt = config_get_intval("domain", 0);
    return 0;
}

int mole2_login::send_error_to_user(usr_info_t* p, uint32_t err_no)
{
	m_clipkg->init(p->waitcmd, p->uid, err_no);
	return send_to_user(p);
}

int mole2_login::send_to_dbproxy_check_session(usr_info_t* p,
	uint32_t gameid,
	uint8_t* session,
	uint32_t session_len,
	uint32_t del_flag)
{
	int len_idx = 0;
	pack_h(m_dbproxy_buf, gameid, len_idx);
	pack(m_dbproxy_buf, session, session_len, len_idx);
	pack_h(m_dbproxy_buf, del_flag, len_idx);

	return send_to_dbproxy(p, p->uid, db_base_session_check, m_dbproxy_buf, len_idx);
}

int mole2_login::do_create_role(usr_info_t* p)
{
	mole2_register_in reg = {0};
	des_data_t *dat = reinterpret_cast<des_data_t*>(p->tmpinfo + 256);

	if(memcmp(dat->magic,"hero",4)) {
		reg.parent = 0;
	} else {
		reg.parent = dat->parent;
	}

	KDEBUG_LOG(p->uid, "CREATE ROLE CMD=%d PARENT=%u",p->waitcmd,reg.parent);
	return send_to_dbproxy(p, p->uid, db_mole2_role_register, &reg, sizeof(mole2_register_in));
}

mole2_login login;
