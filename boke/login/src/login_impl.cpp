#include "login_impl.hpp"
#include  <libtaomee++/utils/strings.hpp> 

using namespace taomee;

static char version[200] = __DATE__" "__TIME__;
svr_info_t g_online_item;
	

int switch_timeout(void* owner, void* data)
{
    usr_info_t*		p = reinterpret_cast<usr_info_t*>(owner);
	pop_login*	plogin = reinterpret_cast<pop_login*>(data);
    KDEBUG_LOG(p->uid, "SWITCH TIMEOUT CMD=%d", p->waitcmd);
    if (p->waitcmd) {
        plogin->get_backup_svr_list(p);
    }
    return 0;
}

int update_backup_svrlist(void* owner, void* data)
{
	pop_login* plogin = reinterpret_cast<pop_login*>(data);
    int idx = sizeof(svr_proto_t);
    taomee::pack_h(plogin->m_login_switch_sendbuf, pop_min_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, pop_max_backup_svr, idx);
    taomee::pack_h(plogin->m_login_switch_sendbuf, static_cast<uint32_t>(0), idx);
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(plogin->m_login_switch_sendbuf);

    pkg->len = idx;
    pkg->seq = (0xFFFF<< 16) | 0; // backup fd
    pkg->cmd = pop_switch_get_ranged_svr_list;
    pkg->ret = 0;
    pkg->id  = 0;
    plogin->send_to_switch(NULL, plogin->m_login_switch_sendbuf, idx);

    ADD_TIMER_EVENT(&(plogin->m_get_backup_tmr), update_backup_svrlist, plogin, get_now_tv()->tv_sec + 60);
    return 0;
}

//====================================

bool pop_login::init_service()
{

	/*
	if (tm_load_dirty("./data/tm_dirty.dat") < 0) {
			ERROR_LOG("Failed to load dirty word!");
			return -1;
	}
	*/
	const char *ip= get_ip_ex(0x01);
	if ( strncmp( ip,"10.",3 )==0 ) {
		this->is_test_env=true;
		DEBUG_LOG("EVN: TEST ");
	}else{
		this->is_test_env=false;
		DEBUG_LOG("EVN: PRODUCTION ");
	}
	g_online_item.friends=0;
	g_online_item.id=0;
	g_online_item.users=0;

	init_timer();
    this->my_read_conf();

	return true;
}


void pop_login::fini_service()
{
	    return;
}

int pop_login::get_other_svrs_pkg_len(int fd, const void* avail_data, int avail_len)
{
    return get_dbproxy_pkg_len(avail_data);	
}

int pop_login::proc_other_cmds(usr_info_t* usr, uint8_t* body, uint32_t bodylen)
{
	//要保留用的seq 
	//HACK 
	usr->channel= ((CliProto3*)(body-sizeof(CliProto3) ))->m_seqno;
	
	
	KDEBUG_LOG(usr->uid,"CLIENT CMD=%u",usr->waitcmd);
	switch (usr->waitcmd) {
		case proto_get_recommend_svr_list:
			return get_recommeded_svr_list_cmd(usr, body, bodylen);
		case proto_get_ranged_svr_list:
        	return get_ranged_svr_list_cmd(usr, body, bodylen);				
		case proto_version:
			m_clipkg->init(usr->waitcmd, usr->uid, pop_success);
			m_clipkg->pack(version, 200);
			send_to_user(usr);
		default:
        	KERROR_LOG(usr->uid, "INVALID CLIENT CMD=%u",usr->waitcmd);
			break;
	}
	return -1;
}

int pop_login::proc_other_dbproxy_return(usr_info_t* p,
		uint16_t cmd,
		const uint8_t* body,
		uint32_t bodylen,
		uint32_t ret)
{
    int err = -1;
    switch(cmd) {
		case db_base_session_check:
			err = check_session_callback(p, body, bodylen, ret);
			break;
        default:
            KERROR_LOG(p->uid,"INVALID CMD RETURN [0x%X]", cmd);
			break;
    }
    if(err) {
        return -1;
    }

    return 0;
}

void pop_login::proc_other_svrs_return(int fd, void* dbpkg, uint32_t pkglen)
{
	DEBUG_LOG(" svrs return %u %u", fd, pkglen);
	if (fd == m_switch_fd) {
		DEBUG_LOG("m_switch_fd svrs return %u %u", fd, pkglen);
		handle_switch_return(reinterpret_cast<svr_proto_t*>(dbpkg), pkglen);
	}
}

void pop_login::on_other_svrs_fd_closed(int fd)
{
	if (fd == m_switch_fd) {
		DEBUG_LOG("SWITCH CONNECTION CLOSED\t[fd=%d]", m_switch_fd);
		m_switch_fd = -1;
	}
}

//=============================
int pop_login::get_recommeded_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, 32);
	char session_16[16];
	hex2bin(session_16, (char*)body ,16);
    KDEBUG_LOG(p->uid, "RECOMMEND SVR: %s", bin2hex(NULL,session_16,16 ) );
		

 	if (send_to_dbproxy_check_session(p, pop_game_flag, (uint8_t*)session_16, pop_sess_len, pop_del_flag) != pop_success) {
		if (verify_login_session(p->uid, session_16 ) == -1){
			return pop_close_conn;
		}else{//本地验证成功
			do_get_recommeded_svr_list(p);
		}
	}
	memcpy(p->tmpinfo, body, bodylen);
	return 0;
}

int pop_login::get_ranged_svr_list_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL_GE(bodylen, 8);

	int idx = 0;
	uint32_t endid;
	uint32_t startid;
	
	KDEBUG_LOG(p->uid,"LIST SVR");
	taomee::unpack_h(body, startid, idx);
	taomee::unpack_h(body, endid, idx);
	if ((startid < pop_min_valid_svr) || (endid > pop_max_valid_svr)
            || (startid > endid) || ((endid - startid) > 100)) {
        KERROR_LOG(p->uid,"INVALID SVR RANGE\t[startid=%u endid=%u]", startid, endid);
        m_clipkg->init(p->waitcmd, p->uid, pop_success);
        m_clipkg->pack(static_cast<uint32_t>(0));
        return send_to_user(p);
    }
	return get_ranged_svr_list(p, startid, endid);
}

int pop_login::get_backup_svr_list(usr_info_t* p)
{
    switch (p->waitcmd) {
        case proto_get_recommend_svr_list:{
            return send_recommeded_svr_list(p,&g_online_item ,1,1);
		}
        case proto_get_ranged_svr_list:
            return send_ranged_svrlist(p, m_backup_svrlist, m_backup_svrlist_cnt);
    default:
        KERROR_LOG(p->uid, "INVALID CMD=%u", p->waitcmd);
    }
    return 0;
}


int pop_login::get_ranged_svr_list(usr_info_t* p, int start_id, int end_id)
{
	KDEBUG_LOG(p->uid, "RANGED SVR\t[startid=%u endid=%u]",start_id, end_id);

	int friendcount = 0;
    int idx = sizeof(svr_proto_t);
	KDEBUG_LOG(p->uid, "sizeof(svr_proto_t):%lu ", sizeof(svr_proto_t));

    taomee::pack_h(m_login_switch_sendbuf, start_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, end_id, idx);
    taomee::pack_h(m_login_switch_sendbuf, friendcount, idx);
	KDEBUG_LOG(p->uid, "idx:%u ", idx );
    init_switch_head(p, m_login_switch_sendbuf, idx, pop_switch_get_ranged_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, idx);
}


void pop_login::init_switch_head(usr_info_t* p, void* buf, uint32_t len, uint32_t cmd)
{
    svr_proto_t* pkg = reinterpret_cast<svr_proto_t*>(buf);

    pkg->len = len;
    pkg->seq = (p->session->fd << 16) | p->counter;
    pkg->cmd = cmd;
    pkg->ret = 0;
    pkg->id  = p->uid;
}


int pop_login::get_recommeded_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	DEBUG_LOG("pop_login::get_recommeded_svr_list_callback");
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
    CHECK_VAL(bodylen, sizeof(svr_list_t) + sizeof(svr_info_t) * svrlist->online_cnt);
    int max_online_id = svrlist->max_online_id;
//TODO	
	if(g_online_item.friends<time(NULL)-60 && svrlist->online_cnt>0 ){
		DEBUG_LOG("XXX update g_online_item");
		memcpy(g_online_item.ip,svrlist->svrs[0].ip,sizeof( g_online_item.ip) );
		g_online_item.port=svrlist->svrs[0].port;
		g_online_item.friends=time(NULL);
	}

    return send_recommeded_svr_list(p, svrlist->svrs, svrlist->online_cnt, max_online_id);
}

int pop_login::get_ranged_svr_list_callback(usr_info_t* p, uint8_t* body, uint32_t bodylen, int ret)
{
	CHECK_VAL_GE(bodylen, sizeof(svr_list_t));
	svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);
	CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t)); 
    return send_ranged_svrlist(p, svrlist->svrs, svrlist->online_cnt);
}


int pop_login::update_backup_svrlist_callback(uint8_t* body, uint32_t bodylen, int ret)
{
    CHECK_VAL_GE(bodylen, sizeof(svr_list_t));

    svr_list_t* svrlist = reinterpret_cast<svr_list_t*>(body);

    //chk if the pkg len is valid
    CHECK_VAL(bodylen, sizeof(svr_list_t) + svrlist->online_cnt * sizeof(svr_info_t));
    m_backup_svrlist_cnt = svrlist->online_cnt;
    memcpy(m_backup_svrlist, svrlist->svrs, svrlist->online_cnt * sizeof(svr_info_t));
    return 0;
}

int pop_login::check_session_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	CHECK_VAL_GE(bodylen, 0);
	if ( !this->is_test_env){//测试环境不检查
		if ((ret && ret !=  dberr_session_check && verify_login_session(p->uid, p->tmpinfo) == -1) ||
			(ret == dberr_session_check)) {
			KERROR_LOG(p->uid, "INVALID SESSION ERR=%d",ret);
			return -1;
		}
	}

	switch (p->waitcmd) {
		case proto_get_recommend_svr_list:
			do_get_recommeded_svr_list(p);
			break;
		default:
			KERROR_LOG(p->uid, "INVALID CMD=%u",p->waitcmd);
	}
       
    return 0;
}


int pop_login::send_recommeded_svr_list(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt, int max_online_id)
{
    m_clipkg->init(proto_get_recommend_svr_list, p->uid, pop_success);
	m_clipkg->m_seqno=p->channel ;
    m_clipkg->pack(static_cast<uint32_t>(max_online_id)); // max online id  packed 
    m_clipkg->pack((uint32_t)0); // whether vip
    m_clipkg->pack(online_cnt);


    for (uint32_t i = 0; i != online_cnt; ++i) {
        m_clipkg->pack(svrs[i].id);
        m_clipkg->pack(get_svr_stat(svrs[i].users));
        m_clipkg->pack(svrs[i].ip, 16);
        m_clipkg->pack(svrs[i].port);
        m_clipkg->pack(svrs[i].friends);
   }

	/*
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
	*/

	/*
    statistic_info_t info;
    uint32_t msglog_type;
    info.user_id = p->uid;
    msglog_type = pop_login_offset + p->channel;
    if (p->channel > 56)
        msglog_type = pop_not_normal_log_channel;
    statistic_msglog(msglog_type, (void *)&info, sizeof info);
	*/

    return send_to_user(p);
}

int pop_login::send_ranged_svrlist(usr_info_t* p, svr_info_t* svrs, uint32_t online_cnt)
{
	//make a response package to the flash client
	m_clipkg->init(proto_get_ranged_svr_list, p->uid, pop_success);
	m_clipkg->pack(online_cnt);
	for (uint32_t i = 0; i != online_cnt; ++i) {
		m_clipkg->pack(svrs[i].id);
		m_clipkg->pack(get_svr_stat(svrs[i].users));
		m_clipkg->pack(svrs[i].ip, 16);
		m_clipkg->pack(svrs[i].port);
		m_clipkg->pack(svrs[i].friends);
	}
    return send_to_user(p);
}



//-----------------------------------------------------------
// switch definations
//-----------------------------------------------------------

int pop_login::send_to_switch(usr_info_t* p, const uint8_t *buf, uint32_t length)
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

void pop_login::handle_switch_return(svr_proto_t* dbpkg, uint32_t pkglen)
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
        case pop_switch_get_recommend_svr_list:
            err = get_recommeded_svr_list_callback(p, dbpkg->body, bodylen, dbpkg->ret);
            break;
        case pop_switch_get_ranged_svr_list:
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


int pop_login::init_timer()
{
    //INIT_LIST_HEAD(&m_get_backup_tmr.timer_list);
    //ADD_TIMER_EVENT(&m_get_backup_tmr, update_backup_svrlist, this, get_now_tv()->tv_sec + 60);
    return 0;
}

int pop_login::my_read_conf()
{
    /*read all configuration param into global variable*/
	/*
    strcpy(m_my_opt.multicast_interface, config_get_strval("multicast_interface"));
    strcpy(m_my_opt.multicast_ip, config_get_strval("multicast_ip"));
    m_my_opt.multicast_port = config_get_intval("multicast_port", 0);
	*/

	strcpy(m_my_opt.master_ser, config_get_strval("master_ip"));
	DEBUG_LOG("m_my_opt.master_ser: %s ",m_my_opt.master_ser);
	
	m_my_opt.dx_or_wt = config_get_intval("domain", 0);

    return 0;
}


int pop_login::send_error_to_user(usr_info_t* p, uint32_t err_no)
{
	m_clipkg->init(p->waitcmd, p->uid, err_no);
	return send_to_user(p);
}

int pop_login::send_to_dbproxy_check_session(usr_info_t* p,
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


int pop_login::do_get_recommeded_svr_list(usr_info_t* p)
{
	DEBUG_LOG("pop_login::do_get_recommeded_svr_list");
    init_switch_head(p, m_login_switch_sendbuf, sizeof(svr_proto_t ), pop_switch_get_recommend_svr_list);
    return send_to_switch(p, m_login_switch_sendbuf, sizeof(svr_proto_t ) );
}

pop_login login;
