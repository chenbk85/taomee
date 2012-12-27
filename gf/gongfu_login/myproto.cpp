extern "C" {
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/project/constants.h>
#include <kf/player_attr.hpp>
}
#include "libtype.h"
#include "mynet.h"
#include "myproto.h"
#include "mysvrlist.h"
#include "myinit.h"

const int role_type_min = 1;
const int role_type_max = 3;
static uint8_t dbpkgbuf[BUFFER_SIZE];

static int check_role_type(usr_info_t *p, uint32_t role_type)
{
	if( role_type < (uint32_t)role_type_min || role_type > (uint32_t)role_type_max ) {
		ERROR_LOG("ROLE_TYPE ERROR\t[uid=%u type=%d]",p->uid, role_type);
		return 1;
	} else {
		return 0;
	}
}

int get_role_list_cmd(usr_info_t* p, uint8_t* body, uint32_t bodylen)
{
	uint32_t role_tm = 0;
	return send_request_to_db(p, DB_GET_ROLE_LIST, &role_tm, sizeof(role_tm));
}

int get_role_list_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
	CHECK_VAL_GE(bodylen, 8);
	const role_list_header_t* role_list_header = reinterpret_cast<const role_list_header_t*>(body);

	int off_pos = 0;
	int idx = sizeof(cli_login_pk_header_t);
	taomee::pack(dbpkgbuf, role_list_header->role_count, idx);
	for ( uint32_t i = 0; i < role_list_header->role_count; i++) {
		const role_list_info_t* role_info = reinterpret_cast<const role_list_info_t*>(body + sizeof(role_list_header_t) + off_pos);
		taomee::pack(dbpkgbuf, role_info->role_tm, idx);
		taomee::pack(dbpkgbuf, role_info->role_type, idx);
		taomee::pack(dbpkgbuf, role_info->nick, NICK_LEN, idx);
		taomee::pack(dbpkgbuf, role_info->level, idx);
		taomee::pack(dbpkgbuf, role_info->clothes_count, idx);
		TRACE_LOG("[%u %u %16s %u %u]",role_info->role_tm, role_info->role_type,
				role_info->nick, role_info->level, role_info->clothes_count);
		for ( uint32_t j = 0; j < role_info->clothes_count; j++) {
			const clothes_info_t* info = &(role_info->clothes_item[j]);
			taomee::pack(dbpkgbuf, info->clothes_id, idx);
			taomee::pack(dbpkgbuf, info->get_time, idx);
			TRACE_LOG("clothes_info:[%u %u]",info->clothes_id,info->get_time);
		}
		off_pos += sizeof(role_list_info_t) + role_info->clothes_count * sizeof(clothes_info_t);
	}
	
	init_proto_head(dbpkgbuf, idx, PROTO_GET_ROLE_LIST, p->uid, SUCCESS);
	return send_to_self(p, dbpkgbuf, idx);
}

int create_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen)
{
	CHECK_VAL(bodylen, sizeof(role_info_t));
	role_info_t *role_info = (role_info_t*)body;
	
	char nick[NICK_LEN + 1];
	memcpy(nick, role_info->nick, NICK_LEN);
	nick[NICK_LEN] = '\0';
	if( check_dirty_word(p, nick)!=0 ) {
	    return 0;
	}
	role_info->role_type = ntohl(role_info->role_type);
	if( check_role_type(p, role_info->role_type) ) {
		return -1;
	}
	
	login_session_t sess;
	struct gf_register_in reg;
	memcpy(reg.nick, role_info->nick, NICK_LEN);
	reg.role_tm = time(NULL);
	reg.role_type = role_info->role_type;
	reg.level = 1;
	reg.hp = calc_hp[reg.role_type]( calc_body_quality[reg.role_type](reg.level) );
	reg.mp = calc_mp[reg.role_type]( calc_stamina[reg.role_type](reg.level) );
	
	//varify the session
	(void)unpkg_auth((uint8_t*)role_info->sess, 16, &sess);
	if ( verify_session(p->uid, &sess) != SUCCESS) {
	      clear_usr_info(p);
		return CLOSE_CONN;
	}
	
	DEBUG_LOG("ROLE REGISTER\t[uid=%u, role_type=%d, nick=%16s, level=%d, hp=%u, mp=%u]", 
			p->uid, reg.role_type, reg.nick, reg.level, reg.hp ,reg.mp);
	return send_request_to_db(p, DB_GF_REGISTER, &reg, sizeof(gf_register_in));
}

int gf_register_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
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
        case USER_ROLE_EXISTED_ERR:
            ERROR_LOG("ROLE CREATED ERR\t[uid=%u]", p->uid);
            return send_to_self_error(p, ROLE_EXISTED_ERR);
            break;
        case USER_ROLE_MAX_ERR:
            //ERROR_LOG("ROLE CREATED ERR\t[uid=%u]", p->uid);
            return send_to_self_error(p, ROLE_OVER_MAX_ERR);
            break;
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_to_self_error(p, SYSTEM_ERR);
    }

	CHECK_VAL(bodylen, 8);
	/*check user role num*/
	uint32_t role_num = *(reinterpret_cast<const uint32_t*>(body + 4));
    DEBUG_LOG("PLAYER ROLE NUM\t[uid=%u, role_num=%u]", p->uid, role_num);
	if ( role_num >= MAX_ROLE_NUM ) {
		uint32_t role_tm = 0;
		send_request_to_deluser(p, DELUSER_PHY_DEL_ROLE, &role_tm, sizeof(role_tm));
	}

	/*to add game flag*/
    uint32_t gf_flag = 6;
    return send_request_to_db(p, DB_ADD_GAME_FLAG, &gf_flag, sizeof(gf_flag));
}


int add_game_flag_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
        case SUCCESS:
            send_to_self_error(p, SUCCESS);
            break;
        case USER_ID_NOFIND_ERR:
            ERROR_LOG("USER NOT REGISTED\t[uid=%u]", p->uid);
            send_to_self_error(p, USER_NOT_FIND);
            break;
        default:
            ERROR_LOG("UNRESOLVABLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            uint32_t gf_flag = 6;
            send_request_to_db(p, DB_ADD_GAME_FLAG, &gf_flag, sizeof(gf_flag));
            send_to_self_error(p, SUCCESS);
            break;
    }
    return SUCCESS;
}

int delete_role_cmd(usr_info_t *p, uint8_t *body, uint32_t bodylen) 
{
    CHECK_VAL(bodylen, 20);
    char *sessbody = (char*)body;
	uint32_t role_tm = ntohl(*(uint32_t*)(body + SESS_LEN));

    DEBUG_LOG("DELETE ROLE\t[uid=%d,role_tm=%u]", p->uid, role_tm);
    
	//varify the session
    login_session_t sess;
    unpkg_auth((uint8_t*)sessbody, 16, &sess);
    if ( verify_session(p->uid, &sess) != SUCCESS ) {
           clear_usr_info(p);
        return CLOSE_CONN;
    }

    //return send_request_to_db(p, DB_DELETE_ROLE, &role_tm, sizeof(role_tm));
	return send_request_to_deluser(p, DELUSER_DELETE_ROLE, &role_tm, sizeof(role_tm));
}

int delete_role_callback(usr_info_t* p, const uint8_t* body, uint32_t bodylen, int ret)
{
    switch (ret) {
		case SUCCESS:
            send_to_self_error(p, SUCCESS);
            break;
        default:
            DEBUG_LOG("DELETE ROLE DB ERR\t[err=%d uid=%u]", ret, p->uid);
            return send_to_self_error(p, SYSTEM_ERR);
	}
	return SUCCESS;
}

