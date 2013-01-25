#include "dispatcher.h"

#include "version.h"

#include "onlinehome.h"

#include "home.h"
#include "sprite.h"

inline int dispatcher(void* data, fdsession_t* fdsess)
{
	char version[256] = {0};
	home_proto_t* proto = data;

	if(proto->cmd == proto_get_version) {
		proto = (home_proto_t*)version;
		proto->len = 256;
		strncpy((char*)proto->body,g_version,200);
		return send_pkg_to_client(fdsess, proto, 256);
	}

	if (proto->onlineid > MAX_ONLINE_NUM) {
		KERROR_LOG(proto->id,"invaild onlineid=%u", proto->onlineid);
		return -1;
	}

	if ((proto->opid <= proto_begin) || (proto->opid >= proto_max)) {
		KERROR_LOG(proto->id, "invalid opid [len=%u cmd=%u onineid=%u homeid=%u opid=%u]",proto->len,proto->cmd,proto->onlineid,(uint32_t)proto->homeid,proto->opid);
		return -1;
	}

	all_fds[proto->onlineid] = fdsess;

	switch(proto->opid) {
		case proto_join_group:
			return join_hero_cup_op(proto);
		case proto_cancel_group:
			return cancel_hero_cup_op(proto);
		case proto_enter_home:
			return proto_enter_home_op(proto);
	}

	home_t* p_home	= g_hash_table_lookup(all_home, &proto->homeid);
	if (!p_home) {
		KDEBUG_LOG(proto->id,"MAP NOT EXSIT[onlineid=%u mapid=(%x,%u) opid=%u]",proto->onlineid,HI32(proto->homeid),LO32(proto->homeid),proto->opid);
		return 0;
	}
	sprite_ol_t* p = g_hash_table_lookup(p_home->sprites, &proto->id);
	if(!p) {
		KDEBUG_LOG(proto->id,"PLAYER NOT EXSIT[onlineid=%u mapid=(%x,%u) opid=%u]",proto->onlineid,HI32(proto->homeid),LO32(proto->homeid),proto->opid);
		return 0;
	}

	return handle_online_op(p_home, p, proto);
}

