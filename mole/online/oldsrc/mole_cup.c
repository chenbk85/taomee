#include "proto.h"

#include "mole_cup.h"

int db_add_medal(sprite_t* p, int medal, int gameid)
{
	char buf[12];

	int  i = 0;
	// pack team id
	if ( test_bit_on32(p->flag, 28) ) {
		PKG_H_UINT32(buf, 1, i);
	} else if ( test_bit_on32(p->flag, 29) ) {
		PKG_H_UINT32(buf, 2, i);
	} else if ( test_bit_on32(p->flag, 30) ) {
		PKG_H_UINT32(buf, 3, i);
	} else if ( test_bit_on32(p->flag, 31) ) {
		PKG_H_UINT32(buf, 4, i);
	} else if ( test_bit_on32(p->flag, 32) ) {
		PKG_H_UINT32(buf, 5, i);
	} else {
		return -1;
	}
	// pack game id
	PKG_H_UINT32(buf, gameid, i);
	PKG_H_UINT32(buf, medal - 100, i);

	return send_request_to_db(SVR_PROTO_ADD_MEDAL, p, sizeof buf, buf, p->id);
}

//------- Callbacks ----------------------
int add_medal_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	int i = sizeof(protocol_t);
	uint32_t* pInt = (void*)(p->session + i + 32);
	uint32_t  gid  = *(uint32_t*)(p->session + i);
	if ( !*((uint32_t*)buf) ) {
		pInt[5] = 0;
	}
	PKG_UINT32(p->session, gid, i);
	db_attr_op(p, pInt[4], pInt[0], pInt[1], pInt[2], pInt[3], ATTR_CHG_play_game, gid);
	i += 28;
	PKG_UINT32(p->session, pInt[0], i);
	PKG_UINT32(p->session, pInt[1], i);
	PKG_UINT32(p->session, pInt[2], i);
	PKG_UINT32(p->session, pInt[3], i);
	PKG_UINT32(p->session, pInt[4], i);

	return 0;
}

int chk_recent_medals_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t uid, teamid, gid;
	int i = 0, j = sizeof(protocol_t) + 1, k = 0, cnt, cnt2 = 0;
	UNPKG_H_UINT32(buf, cnt, i);
	for ( ; k != cnt; ++k ) {
		UNPKG_H_UINT32(buf, uid, i);
		if (uid) {
			UNPKG_H_UINT32(buf, teamid, i);
			UNPKG_H_UINT32(buf, gid, i);
			//
			PKG_UINT32(msg, uid, j);
			PKG_UINT8(msg, teamid, j);
			PKG_UINT32(msg, gid, j);
			++cnt2;
		}
	}
	i = sizeof(protocol_t);
	PKG_UINT8(msg, cnt2, i);
	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

int chk_cup_history_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t date, grpid;
	int i = 0, j = 0, k = sizeof(protocol_t), cnt;
	UNPKG_H_UINT32(buf, cnt, i);
	memset(msg + k, 0, 21);
	for ( ; j != cnt; ++j ) {
		UNPKG_H_UINT32(buf, date, i);
		UNPKG_H_UINT32(buf, grpid, i);
		//
		PKG_UINT8(msg, grpid, k);
	}
	init_proto_head(msg, p->waitcmd, 38);
	return send_to_self(p, msg, 38, 1);
}

int chk_cup_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 96);

	int i = 0, j = sizeof(protocol_t), k = 0;
	uint32_t gold, silver, bronze, tmp;
	UNPKG_H_UINT32(buf, gold, i);
	UNPKG_H_UINT32(buf, silver, i);
	UNPKG_H_UINT32(buf, bronze, i);
	UNPKG_H_UINT32(buf, tmp, i);
	//
	p->tmpinfo.gold   = gold;
	p->tmpinfo.silver = silver;
	p->tmpinfo.bronze = bronze;
	//
	switch (p->waitcmd) {
	case PROTO_CHK_CUP_INFO:
		PKG_UINT16(msg, gold, j);
		PKG_UINT16(msg, silver, j);
		PKG_UINT16(msg, bronze, j);
		for ( ; k != 5; ++k ) {
			UNPKG_H_UINT32(buf, tmp, i);
			UNPKG_H_UINT32(buf, gold, i);
			UNPKG_H_UINT32(buf, silver, i);
			UNPKG_H_UINT32(buf, bronze, i);
			//
			PKG_UINT16(msg, gold, j);
			PKG_UINT16(msg, silver, j);
			PKG_UINT16(msg, bronze, j);
		}
		init_proto_head(msg, p->waitcmd, j);
		return send_to_self(p, msg, j, 1);
	case PROTO_LOGIN:
		return proc_final_login_step(p);
	default:
		ERROR_RETURN(("Invalid CMD %d", p->waitcmd), -1);
	}

	return 0;
}
