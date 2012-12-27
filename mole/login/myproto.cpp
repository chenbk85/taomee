extern "C" {
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
}
#include "libtype.h"
#include "mynet.h"
#include "myproto.h"
#include "mydbproxy.h"
#include "mysvrlist.h"
#include "myinit.h"

int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, sizeof(role_info_t));
	role_info_t *role_info = (role_info_t*)body;
	
	char nick[NICK_LEN + 1];
	memcpy(nick, role_info->nick, NICK_LEN);
	nick[NICK_LEN] = '\0';
    if(tm_dirty_check(7, (char*)(nick)) > 0) {
		DEBUG_LOG("nick  : %s", nick);
        return 0;
    }

	login_session_t sess;
	struct mole_register_in reg;
	memcpy(reg.nick, (char*)(body + SESS_LEN), NICK_LEN);
	reg.color = ntohl(*(uint32_t*)(body + SESS_LEN + NICK_LEN));
	*(uint32_t*)p->tmpinfo = ntohl(*(uint32_t*)(body + SESS_LEN + NICK_LEN + 4));
	
	//varify the session
	(void)unpkg_auth((uint8_t*)role_info->sess, 16, &sess);
	if ( verify_session(p->uid, &sess) != SUCCESS) {
        clear_usr_info(p);
		return CLOSE_CONN;
	}

	DEBUG_LOG("ROLE REGISTER\t[uid=%u, nick=%16s, color=%u]", p->uid, reg.nick, reg.color);
    return send_request_to_db(p, DB_MOLE_REGISTER, &reg, sizeof(mole_register_in));
}

int mole_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case SUCCESS:
#ifndef TW_VER	
            {
                int tmp = 1;
                uint32_t msglog_type;
                uint32_t channel = *(uint32_t*)p->tmpinfo;
                msglog_type = MOLE_REGISTER_CHANNEL_OFFSET + channel;
				if (channel > 56) 
					msglog_type = MOLE_NOT_NORMAL_REG_CHANNEL;
                statistic_msglog(msglog_type, (void *)&tmp, sizeof tmp);
                msglog_type = MOLE_REGISTER_MIMI_OFFSET;
                statistic_msglog(msglog_type, &p->uid, sizeof p->uid);
            }

#endif
            // update the gameflag
            DEBUG_LOG("ADD GAME FLAG\t[uid=%u]", p->uid);
            break;
        case USER_ID_EXISTED_ERR:
            DEBUG_LOG("ROLE CREATED\t[uid=%u]", p->uid);
            break;
        default:
            DEBUG_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_to_self_error(p, SYSTEM_ERR);
    }
    uint32_t mole_flag = 1;
    return send_request_to_db(p, DB_ADD_GAME_FLAG, &mole_flag, sizeof(mole_flag));
}


int add_mole_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case SUCCESS:
            send_to_self_error(p, SUCCESS);
            break;
        case USER_ID_NOFIND_ERR:
            DEBUG_LOG("USER NOT REGISTED\t[uid=%u]", p->uid);
            send_to_self_error(p, USER_NOT_FIND);
            break;
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            uint32_t mole_flag = 1;
            send_request_to_db(p, DB_ADD_GAME_FLAG, &mole_flag, sizeof(mole_flag));
            send_to_self_error(p, SUCCESS);
            break;
    }
    return SUCCESS;
}

