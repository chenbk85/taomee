extern "C" {
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}
#include "asyc_main_login_type.h"
#include "net.h"
#include "proto.h"
#include "dbproxy.h"
#include "init.h"
#include "util.h"
#include "ip_counter.h"

usr_info_t all_players[MAX_CONNECTION];

int check_dirty_word(usr_info_t* p, char* msg)
{
	CHECK_DIRTYWORD(p, msg);
	return 0;
}

int dispatch(void* data, fdsession_t* fdsess)
{
    cli_login_pk_header_t* hdr = reinterpret_cast<cli_login_pk_header_t*>(data); 
    uint32_t cmdid = ntohs(hdr->commandid);
    uint32_t uid = ntohl(hdr->userid);
    uint32_t pkglen = ntohl(hdr->length);
    usr_info_t* p = get_usr_by_fd(fdsess->fd);
    if(p->waitcmd) {
        DEBUG_LOG("cmd uncomplete [%d %d %d %d]", p->uid, fdsess->fd, cmdid, p->waitcmd);
        return 0;
    }
    p->uid = uid;
    p->waitcmd = cmdid;
    p->session = fdsess;
    p->counter = (p->counter + 1)%SVR_USR_COUNTER;
    DEBUG_LOG("DISPATCH [len=%d %d %d %d]",pkglen, uid, cmdid, fdsess->fd);
    uint32_t bodylen = pkglen - sizeof(cli_login_pk_header_t);
    
    switch (cmdid) {
    case PROTO_USER_LOGIN:
	case PROTO_USER_LOGIN_EX:
        return login_cmd(p, hdr->body, bodylen);
    case 30000:
        return send_to_self_error(p, 0);
    case 1:
        return 0;
    default:
        return process_client_cmd(p, hdr->body, bodylen);
    }
    return 0;
}

int check_usr_valid(usr_info_t* p)
{
    ip_info_t ipc_info;
    ip_counter_info(p->uid, ipc_info);
    //DEBUG_LOG("IPC RETURN\t\t[uid=%d cnt=%u time=%u]",
                    //p->uid, ipc_info.ip_cnt,ipc_info.first_time);

    time_t now;
    double gap;
    time(&now);
    gap = difftime(now, ipc_info.first_time);
    if ( gap >=  g_ds_ini.ban_time
        || ( gap >=  g_ds_ini.passwd_fail_time_limited && ipc_info.ip_cnt <=  g_ds_ini.count_limited )) {
        //clear the uid info.
        ip_counter_clear(p->uid);
        goto suc;
    }
    if ( ipc_info.ip_cnt <= g_ds_ini.count_limited ) {
        TRACE_LOG("check_uid_security: no problem");
        goto suc;
    } else {
        ERROR_LOG("check_uid_security: login error too much %d", p->uid);

#ifndef TW_VER
        // log statistic info
        statistic_info_t info;
        uint32_t msglog_type;
        msglog_type = MIMI_BASE + USER_BANNED_OFFSET;
        info.user_id = p->uid;
        statistic_msglog(msglog_type, (void *)&info, sizeof info);
#endif

        goto error;
    }
suc:
    return check_usr_login(p);
error:
    return send_to_self_error(p, PASSWD_ERR_TOO_MUCH);
}

int login_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
    uint32_t uid = p->uid;
	
	//get the client's request packet body data
	int idx = 0;
	svr_login_req_t* loginreq = reinterpret_cast<svr_login_req_t*>(p->tmpinfo);
	if(uid == ULOGIN_EMAIL_USERID) {
		// verify data len
		CHECK_VAL(bodylen, sizeof(cli_login_req_t) + EMAIL_MAX_LEN);
		// unpack
	    taomee::unpack(body, loginreq->email, EMAIL_MAX_LEN, idx);
		DEBUG_LOG("EMAIL LOGIN\t[email=%.64s]", loginreq->email);
	} else {
		// verify data len
		CHECK_VAL(bodylen, sizeof(cli_login_req_t));
		DEBUG_LOG("UID LOGIN\t[uid=%u fd=%d game=%d]", uid, p->session->fd, loginreq->which_game);
	}
	cli_login_req_t* tmp= reinterpret_cast<cli_login_req_t*>(body + idx);

	loginreq->channel = ntohl(tmp->channel);
	loginreq->which_game = ntohl(tmp->which_game);
	loginreq->user_ip = ntohl(tmp->user_ip);
	str2hex(tmp->passwd, sizeof(loginreq->passwd), loginreq->passwd);

	//DEBUG_LOG("UID=%u, SK IP=%X, USER IP=%X", p->uid, p->session->remote_ip, loginreq->user_ip);
	if ((p->session->remote_ip & 0x0000FFFF) != 0xA8C0 && 
		(p->session->remote_ip & 0x000000FF) != 0x0A ) { //WAI WANG  address, then loginreq address is set to this address
		//DEBUG_LOG("WAI WANG ADDR");
		if ( loginreq->user_ip != 0 ) {
			ERROR_RETURN(("invalid user ip info [p->session ip=%x, user ip=%x]", p->session->remote_ip, loginreq->user_ip), CLOSE_CONN);
		}
		loginreq->user_ip = p->session->remote_ip;
	} else { //NEI WANG ADDRESS, (192.168.*, 10.*)
	}
	
	if (loginreq->which_game < 1 || loginreq->which_game > MAX_GAME_NR) {
		ERROR_RETURN(("invalid game number %u", loginreq->which_game), CLOSE_CONN);
	}

    //check the uid login count
    if (p->uid == ULOGIN_EMAIL_USERID) {
        return send_request_to_mainlogin_db(p, DB_GET_UID_BY_EMAIL, loginreq->email, EMAIL_MAX_LEN);

    } 
#ifndef TW_VER
    else {
        //return send_to_ip_counter(p, GET_UID_INFO);
        return check_usr_valid(p);
    }
#else
    else {
        return check_usr_login(p);
    }
#endif
}


int check_usr_login(usr_info_t* p) 
{
    switch(p->waitcmd) {
        case PROTO_USER_LOGIN:
        case PROTO_USER_LOGIN_EX:
            {
                svr_login_req_t* loginreq = reinterpret_cast<svr_login_req_t*>(p->tmpinfo);
                chk_pwd_pkg_t chk_pkg;
                chk_pkg.ip = loginreq->user_ip;
                chk_pkg.channel= loginreq->which_game;
                memcpy(&chk_pkg.pwd, loginreq->passwd, 16);
                if(p->waitcmd == PROTO_USER_LOGIN) {
                    return send_request_to_mainlogin_db(p, DB_CHECK_UID_PWD, &chk_pkg, sizeof(chk_pwd_pkg_t));
                } else {
                    return send_request_to_mainlogin_db(p, DB_CHECK_UID_PWD_EX, &chk_pkg, sizeof(chk_pwd_pkg_t));
                }
            }
            break;
        default:
            ERROR_RETURN(("error waitcmd [%d %d]", p->uid, p->waitcmd), 0);
    }
}

int get_uid_by_mail_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case SUCCESS:
            break;
        case USER_ID_NOFIND_ERR:
            return send_to_self_error(p, USER_NOT_FIND);
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_to_self_error(p, SYSTEM_ERR);
    }
    //cli_login_req_t* loginreq = reinterpret_cast<cli_login_req_t*>(p->tmpinfo);
    CHECK_VAL(bodylen, 4);
    p->uid = *(uint32_t*)body;
#ifndef TW_VER
    //return send_to_ip_counter(p, GET_UID_INFO);
    return check_usr_valid(p);
#else
    return check_usr_login(p);
#endif
    //chk_pwd_pkg_t chk_pkg;
    //chk_pkg.ip = loginreq->user_ip;
    //chk_pkg.channel= loginreq->which_game;
    //memcpy(&chk_pkg.pwd, loginreq->passwd, 16);
    //return send_request_to_mainlogin_db(p, DB_CHECK_UID_PWD, &chk_pkg, sizeof(chk_pwd_pkg_t));
}

void encrypt_session_id(uint8_t* buf, int& idx, uint32_t uid, uint32_t ip)
{

    static const char* key = LOGIN_DES_KEY;

    uint32_t inbuf[4];
    inbuf[0] = ip;
    inbuf[1] = time(0);
    inbuf[2] = uid;
    inbuf[3] = inbuf[1];

    des_encrypt_n(key, inbuf, buf + idx, 2);
    idx += 16;
}

int check_pwd_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    
    statistic_info_t info;
    uint32_t msglog_type;

    if(!ret) {
        CHECK_VAL(bodylen, 4);

        svr_login_req_t* loginreq = reinterpret_cast<svr_login_req_t*>(p->tmpinfo);
        int l = sizeof(cli_login_pk_header_t);
        uint32_t gameflag = *(uint32_t*)body; 
        DEBUG_LOG("LOGIN SUC\t[uid=%u ip=%X %d]", p->uid, p->session->remote_ip, gameflag);

        //login success, then encrypt the respond package body data.
        encrypt_session_id(cli_buf, l, p->uid, p->session->remote_ip);

        uint32_t create_role_flag = (gameflag & (0x01 << (loginreq->which_game - 1))) ? 1 : 0; 
        taomee::pack(cli_buf, create_role_flag, l);
        init_proto_head(cli_buf, l, p->waitcmd, p->uid, SUCCESS);
        //log statistic
#ifndef TW_VER
        //clear the uid count info
        ip_counter_clear(p->uid);
        // log statistic info
        info.user_id = p->uid;
        if (loginreq->channel <=  g_ds_ini.channel_total) {
            msglog_type = MIMI_BASE + USER_LOGIN_CHANNEL_OFFSET + loginreq->channel;
            p->channel = loginreq->channel;
        } else {
            p->channel = g_ds_ini.channel_total;
            msglog_type = MIMI_BASE + UNDEFINE_LOCATION_OFFSET;
        }
        statistic_msglog(msglog_type, (void *)&info, sizeof info);
#endif
        send_to_self(p, cli_buf, l);
        return 0;
    }
	switch (ret) {
		case CHECK_PASSWD_ERR:

#ifndef TW_VER
			// update the ip count
            //send_to_ip_counter(p, ADD_UIDCOUNT);
            ip_counter_add(p->uid);
			//do statistic log
			
			info.user_id = p->uid;
			msglog_type = MIMI_BASE + USER_PASSWD_ERROR_OFFSET;
			statistic_msglog(msglog_type, (void *)&info, sizeof info);
#endif
            send_to_self_error(p, PASSWD_ERR);
			break;

		case USER_ID_NOFIND_ERR:
            send_to_self_error(p, USER_NOT_FIND);
			break;
			
		default:
            send_to_self_error(p, SYSTEM_ERR);
			ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
			break;
		}
    //return 0;
//--------------test------------
    return CLOSE_CONN;
}

usr_info_t* get_usr_by_fd(int fd)
{
    return &all_players[fd];
}


