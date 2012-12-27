#include <errno.h>

#include <net/if.h>

#include <glib.h>

#include <libtaomee/conf_parser/config.h>

#include "logic.h"
#include "login.h"
#include "map.h"
#include "proto.h"
#include "mole_group.h"
#include "mole_class.h"
#include "lahm_sport.h"

#include "communicator.h"


// holds all the @sprites that are in @mapid
typedef struct SpriteWhere {
	map_id_t 	mapid;
	GHashTable* sprites;
} sprite_where_t;

// to cache sprite's basic info
typedef struct SpriteInfo {
	sprite_t	player;
	pet_t		pet;
} __attribute__((packed)) sprite_info_t;

// multicast socket
static int mcast_fd;
static struct sockaddr_in mcast_addr;
static const int   mcast_port = 5538;

int school_bar_fd = -1;

int home_serv_fd[MAX_HOMESERV_NUM] = {-1, -1, -1, -1, -1, -1, -1, -1};
#ifdef TW_VER
int use_homeserver = 0;
#else
int use_homeserver = 1;
#endif
// holds all the @maps where there are sprites in it
static GHashTable* maps;

static guint
g_int64_hash (gconstpointer v)
{
  return (guint) *(const gint64*) v;
}

static gboolean
g_int64_equal (gconstpointer v1, gconstpointer v2)
{
  return *((const gint64*) v1) == *((const gint64*) v2);
}

int send_to_broadcaster(int cmd, int body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid)
{
	static uint8_t bcpkg[PAGESIZE];

	int len = sizeof(communicator_header_t) + body_len;
	if ( (mcast_fd > epi.maxfd) ||
			(body_len > sizeof(bcpkg) - sizeof(communicator_header_t)) ) {
		ERROR_RETURN( ("Failed to Send Mcast Pkg, fd=%d", mcast_fd), -1 );
	}

	communicator_header_t* pkg = (void*)bcpkg;
	pkg->len        = len;
	pkg->online_id  = config_cache.bc_elem->online_id;
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	return sendto(mcast_fd, bcpkg, len, 0, (void*)&mcast_addr, sizeof mcast_addr);
}

int home_server_fd_idx(int fd)
{
	int loop;
	for (loop = 0; loop < MAX_HOMESERV_NUM; loop++) {
		if (home_serv_fd[loop] == fd)
			return loop;
	}
	return -1;
}

int send_to_homeserver(int cmd, int body_len, const void* body_buf, userid_t id, map_id_t mapid, uint32_t opid)
{
	static uint8_t bcpkg[PAGESIZE];

	int fd_idx = mapid % MAX_HOMESERV_NUM;
	if (home_serv_fd[fd_idx] == -1) {
		home_serv_fd[fd_idx] = connect_to_service(config_get_strval("home_serv"), fd_idx + 1, 65535, 1);
	}

	int home_fd = home_serv_fd[fd_idx];

	int len = sizeof(communicator_header_t) + body_len;
	if ( (home_fd == -1) || (home_fd > epi.maxfd)
		  || (body_len > sizeof(bcpkg) - sizeof(communicator_header_t))
		  || (epi.fds[home_fd].cb.sndbufsz < epi.fds[home_fd].cb.sendlen + len) ) {
		ERROR_RETURN( ("Failed to Send homeserver Pkg, fd=%d", home_fd), -1 );
	}

	communicator_header_t* pkg = (void*)bcpkg;
	pkg->len        = len;
	pkg->online_id  = config_cache.bc_elem->online_id;
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	pkg->mapid      = mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	DEBUG_LOG("SEND HOMESERV\t[%u %u %lu %u %u]", cmd, opid, mapid, id, len);
	return net_send(home_fd, bcpkg, len, 0);
}


/**
 * connect_to_broadcaster - connect to broadcaster and send an init pkg
 *
 */
static int connect_to_broadcaster()
{
	mcast_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (mcast_fd == -1) {
		ERROR_RETURN(("Failed to Create `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
	}

	memset(&mcast_addr, 0, sizeof mcast_addr);
	mcast_addr.sin_family = AF_INET;
	inet_pton(AF_INET, config_get_strval("mcast_ip"), &(mcast_addr.sin_addr));
	mcast_addr.sin_port = htons(mcast_port);

	int on = 1;
	setsockopt(mcast_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);

	// Set Default Interface For Outgoing Multicasts
	in_addr_t ipaddr;
	inet_pton(AF_INET, config_get_strval("local_ip"), &ipaddr);
	if (setsockopt(mcast_fd, IPPROTO_IP, IP_MULTICAST_IF, &ipaddr, sizeof ipaddr) == -1) {
		ERROR_RETURN(("Failed to Set Outgoing Interface: err=%d %s %s",
						errno, strerror(errno), config_get_strval("local_ip")), -1);
	}

	if (bind(mcast_fd, (struct sockaddr*)&mcast_addr, sizeof mcast_addr) == -1) {
		ERROR_RETURN(("Failed to Bind `mcast_fd`: err=%d %s", errno, strerror(errno)), -1);
	}

	// Join the Multicast Group
	struct group_req req;
	req.gr_interface = if_nametoindex(config_get_strval("mcast_incoming_if"));
	memcpy(&req.gr_group, &mcast_addr, sizeof mcast_addr);
	if (setsockopt(mcast_fd, IPPROTO_IP, MCAST_JOIN_GROUP, &req, sizeof req) == -1) {
		ERROR_RETURN(("Failed to Join Mcast Grp: err=%d %s", errno, strerror(errno)), -1);
	}

	return do_add_conn(mcast_fd, MCAST_TYPE_FD, &mcast_addr, 0);
}

//-------------------------------------------------------
//------------------------------------------------------
static inline void
free_sprite_info(void* sprite_info)
{
	//sprite_info_t* p = sprite_info;
	//DEBUG_LOG("Free Sprite: %u-----------------------------------------------------------", p->player.id);
	g_slice_free1(sizeof(sprite_info_t), sprite_info);
}

static inline void
free_sprite_where(void* sprite_where)
{
	sprite_where_t* p = sprite_where;
	g_hash_table_destroy(p->sprites);
	//DEBUG_LOG("Free Map: %llx-------------------------------------------------------------", p->mapid);
	g_slice_free1(sizeof *p, p);
}

int init_communicator()
{
	maps = g_hash_table_new_full(g_int64_hash, g_int64_equal, 0, free_sprite_where);

	return connect_to_broadcaster();
}

void fini_communicator()
{
	g_hash_table_destroy(maps);
}

void remove_all_across_svr_sprites()
{
	g_hash_table_remove_all(maps);
}


//----------------------------------------------
void traverse_across_svr_sprites(map_id_t mapid, void (*action)(void* key, void* sinfo, void* data), void* data)
{
	sprite_where_t* spwhere = g_hash_table_lookup(maps, &mapid);
	if (spwhere) {
		g_hash_table_foreach(spwhere->sprites, action, data);
	}
}

//
sprite_t* get_across_svr_sprite(userid_t uid, map_id_t mapid)
{
	if (!IS_NORMAL_MAP(mapid)){
		sprite_where_t* spwhere = g_hash_table_lookup(maps, &mapid);
		if (spwhere) {
			return g_hash_table_lookup(spwhere->sprites, &uid);
		}
	}
	return 0;
}

uint32_t get_across_svr_sprite_cnt(map_id_t mapid)
{
	if (!IS_NORMAL_MAP(mapid)){
		sprite_where_t* spwhere = g_hash_table_lookup(maps, &mapid);
		if (spwhere) {
			return g_hash_table_size(spwhere->sprites);
		}
	}
	return 0;
}

//----------------------------------------------
static inline void
enter_map_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	//DEBUG_LOG("Alloc Sprite [%u enter map %llx]----------------------------------------------", uid, spwhere->mapid);
	if (len != sizeof(sprite_info_t) && len != sizeof(sprite_t)) {
		ERROR_LOG("mcast: sprite_info len error[%u %lu %u]", uid, spwhere->mapid, len);
		return;
	}
	sprite_info_t* sinfo = g_slice_alloc(sizeof *sinfo);
	memcpy(sinfo, body, len);
	if (sinfo->player.followed) {
		sinfo->player.followed = &(sinfo->pet);
	}
	sinfo->player.tiles  = 0;
	sinfo->player.fdsess = 0;
	g_hash_table_replace(spwhere->sprites, &(sinfo->player.id), sinfo);
}

static inline void
leave_map_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	//DEBUG_LOG("Free user [%u %llx]-------------------------------------------------------------", uid, spwhere->mapid);
	g_hash_table_remove(spwhere->sprites, &uid);
	if (!g_hash_table_size(spwhere->sprites)) {
		g_hash_table_remove(maps, &(spwhere->mapid));
	}
}

static inline void
basic_info_chg_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		int i = sizeof(protocol_t) + 4;
		UNPKG_STR(body, spinfo->player.nick, i, USER_NICK_LEN);
		UNPKG_UINT32(body, spinfo->player.color, i);
		i += 9;
		UNPKG_UINT32(body, spinfo->player.action, i);
	}
}

static inline void
transform_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 37);

	int i = sizeof(protocol_t) + 4;
	userid_t rcvid, itmid;
	UNPKG_UINT32(body, rcvid, i);
	UNPKG_UINT32(body, itmid, i);

	sprite_t* p = get_sprite(rcvid);
	sprite_info_t* spinfo;
	if (p) {
		p->action = itmid;
		add_event(&(p->timer_list), show_item_use_expired, p, 0, now.tv_sec + 20, ADD_EVENT_REPLACE_UNCONDITIONALLY);
	} else if ( (spinfo = g_hash_table_lookup(spwhere->sprites, &rcvid)) ) {
		spinfo->player.action = itmid;
	}
}

static inline void
take_dragon_op(userid_t uid,sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, sizeof(protocol_t) + 16);

	int i = sizeof(protocol_t);

	userid_t userid;
	uint32_t dragon_id;
	uint32_t dragon_gorwth;
	uint32_t dragon_state;

	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, dragon_id, i);
	UNPKG_UINT32(body, dragon_gorwth, i);
	UNPKG_UINT32(body, dragon_state, i);

	DEBUG_LOG("take_dragon_op %u %u %u %u",uid,userid,dragon_id,dragon_gorwth);

	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &userid);
	if (spinfo) {
		DEBUG_LOG("take_dragon_op OK");
		if(dragon_state) {
			spinfo->player.dragon_id = dragon_id;
			spinfo->player.dragon_growth = dragon_gorwth;
		} else {
			spinfo->player.dragon_id = 0;
			spinfo->player.dragon_growth = 0;
		}
	}
}

static inline void
transform_revert_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		spinfo->player.action = 0;
	}
}

static inline void
cancel_transform_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_t* changee = get_sprite(uid);
	if (changee) {
		changee->action = 0;
		return;
	}

	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		spinfo->player.action = 0;
	}
}

static inline void
action_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		int i = sizeof(protocol_t) + 4;
		uint32_t action;
		UNPKG_UINT32(body, action, i);
		if ( (action == ACTION_SIT) || (action == ACTION_DANCE) ) {
			spinfo->player.action = action;
			UNPKG_UINT8(body, spinfo->player.direction, i);
		}
	}
}

static inline void
lahm_do_action_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 30);

	uint32_t userid, petid;
	int i = sizeof(protocol_t);
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);

	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &userid);
	if (spinfo) {
		UNPKG_UINT8(body, spinfo->player.lamu_action, i);
		pet_t* pet = spinfo->player.followed;
		if (pet && petid == pet->id) {
			UNPKG_UINT8(body, pet->hungry, i);
			UNPKG_UINT8(body, pet->thirsty, i);
			UNPKG_UINT8(body, pet->sanitary, i);
			UNPKG_UINT8(body, pet->spirit, i);
		}
	}
}

static inline void
lahm_action_expire_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 25);

	uint32_t userid;
	int i = sizeof(protocol_t);
	UNPKG_UINT32(body, userid, i);

	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &userid);
	if (spinfo) {
		spinfo->player.lamu_action = 0;
	}
}

static inline void
walk_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		int i = sizeof(protocol_t) + 4;
		UNPKG_UINT32(body, spinfo->player.posX, i);
		UNPKG_UINT32(body, spinfo->player.posY, i);
		reset_common_action(&(spinfo->player));
	}
}

static inline void
walk_pet_op(userid_t uid, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &uid);
	if (spinfo) {
		if (len) {
			memcpy(&(spinfo->pet), body, len);
			spinfo->player.followed = &(spinfo->pet);
		} else {
			spinfo->player.followed = 0;
		}
	}
}

static int unpkg_sprite_from_hs(sprite_info_t *aspinfo, sprite_to_online_t* pstot)
{
	int len = sizeof(sprite_to_online_t);

	sprite_t* p = &(aspinfo->player);
	p->id = pstot->id;
	memcpy(p->nick, pstot->nick, sizeof(pstot->nick));
	p->inviter_id = pstot->inviter_id;
	p->old_invitee_cnt = pstot->old_invitee_cnt;
	p->invitee_cnt = pstot->invitee_cnt;
	p->color = pstot->color;
	p->flag = pstot->flag;
	p->tiles = 0;

	p->login = pstot->login;
	p->group = 0;

	p->action = pstot->action;
	p->direction = pstot->direction;
	p->posX = pstot->posX;
	p->posY = pstot->posY;
	p->action2 = pstot->action2;
	p->sub_grid= pstot->sub_grid;
	memcpy(&(p->tmpinfo), &pstot->tmpinfo, sizeof(sprite_tmp_info_t));
	p->car_status = pstot->car_status;
	memcpy(&(p->car_driving), &pstot->car_driving, sizeof(car_t));

	p->exam_num = pstot->exam_num;
	p->teach_exp = pstot->teach_exp;
	p->student_count = pstot->student_count;
	memcpy(p->students,pstot->students,sizeof(p->students));

	p->dietitian_exp= pstot->dietitian_exp;

	p->animal_nbr = pstot->animal_nbr;
	memcpy(&(p->animal_follow_info.tag_animal), &pstot->animal, sizeof(animal_attr_t));
	p->item_cnt = pstot->item_cnt;
	memcpy(p->items, pstot->items, sizeof(p->items));
	p->lamu_action = pstot->lamu_action;
	p->followed = 0;
	if (pstot->followed) {
		p->followed = &(aspinfo->pet);
		memcpy(p->followed, (uint8_t*)(pstot) + len, sizeof(pet_t));
		len += sizeof(pet_t);
	}

	p->dragon_id = pstot->dragon_id;
	p->dragon_growth = pstot->dragon_growth;
	memcpy(p->dragon_name,pstot->dragon_name,sizeof(pstot->dragon_name));
	p->dungeon_explore_exp = pstot->dungeon_explore_exp;
	p->team_id = pstot->team_id;

	DEBUG_LOG("SPRITE FROM HS\t[%u %u %u %u]", p->id, (p->followed)?(p->followed->id):0, p->color, p->lamu_action);
	return len;
}

static inline void
get_users_from_hs(userid_t uid, sprite_where_t* spwhere, const uint8_t* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 4);
	uint32_t usr_cnt = *(uint32_t*)body;

	int loop;
	int idx = 4;
	for (loop = 0; loop < usr_cnt; loop++) {
		CHECK_BODY_LEN_GE_VOID(len, idx + sizeof(sprite_to_online_t));
		sprite_to_online_t* pstot = (sprite_to_online_t*)(body + idx);
		if (pstot->followed) {
			CHECK_BODY_LEN_GE_VOID(len, idx + sizeof(sprite_to_online_t) + sizeof(pet_t));
		}
		DEBUG_LOG("unpkg Sprite\t[id=%u idx=%u  mpid=%lu cnt=%u]", pstot->id, idx,  spwhere->mapid, usr_cnt);
		sprite_info_t* spinfo = g_hash_table_lookup(spwhere->sprites, &(pstot->id));
		if (spinfo) {
			idx += unpkg_sprite_from_hs(spinfo, pstot);
		} else {
			//DEBUG_LOG("Alloc Sprite [%u in map %llx-------------------------------------]", pstot->id, spwhere->mapid);
			spinfo = g_slice_alloc(sizeof *spinfo);
			idx += unpkg_sprite_from_hs(spinfo, pstot);
			spinfo->player.fdsess = 0;
			g_hash_table_replace(spwhere->sprites, &(spinfo->player.id), spinfo);
		}
	}

}

//-------------------------------------------------------
int sprite_info_chg_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);

	communicator_body_t* cbody = buf;

	if(cbody->opid == SOP_get_map_users_cnt) {
		uint32_t count = *(uint32_t*)cbody->body;
		DEBUG_LOG("HOME map=%lu USERS %d %u", cbody->mapid, uid, count);
		sprite_t * p = get_sprite(uid);
		if(p) get_map_user_cnt_callback(p,count);
		return 0;
	}

	sprite_where_t* spwhere    = g_hash_table_lookup(maps, &(cbody->mapid));
	if (!spwhere && cbody->opid != SOP_enter_map && cbody->opid != SOP_get_users_from_HS) {
		ERROR_LOG("not find maps\t[%u %u %lu]", cbody->opid, uid, cbody->mapid);
		return 0;
	}
	if (!spwhere) {
		//DEBUG_LOG("Alloc Map %llx", cbody->mapid);
		spwhere          = g_slice_alloc(sizeof *spwhere);
		spwhere->mapid   = cbody->mapid;
		spwhere->sprites = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_sprite_info);
		g_hash_table_insert(maps, &(spwhere->mapid), spwhere);
	}

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(uid, spwhere, cbody->body, len - 12); break
	//
	switch (cbody->opid) {
		DISPATCH_OP(SOP_enter_map, enter_map_op);
		DISPATCH_OP(SOP_walk_pet, walk_pet_op);
		DISPATCH_OP(SOP_get_users_from_HS, get_users_from_hs);

		DISPATCH_OP(PROTO_ACTION, action_op);
		DISPATCH_OP(PROTO_WALK, walk_op);
		DISPATCH_OP(PROTO_MAP_LEAVE, leave_map_op);
		DISPATCH_OP(PROTO_SHOW_ITEM_USE, transform_op);
		DISPATCH_OP(PROTO_ITEM_SHOW_EXPIRE, transform_revert_op);
		DISPATCH_OP(PROTO_CANCEL_CHANGE_FORM, cancel_transform_op);
		DISPATCH_OP(PROTO_LAMU_ACTION, lahm_do_action_op);
		DISPATCH_OP(PROTO_REMOVE_PET_ACTION, lahm_action_expire_op);
		DISPATCH_OP(PROTO_CALL_DRAGON, take_dragon_op);

		case PROTO_COLOR_MODIFY:
		case PROTO_NICK_MODIFY:
			basic_info_chg_op(uid, spwhere, cbody->body, len - 12);
			break;
	default:
		break;
	}
#undef DISPATCH_OP

	if (cbody->opid > SOP_max) {
		send_to_map3(cbody->mapid, cbody->body, len - 12);
	}
	return 0;
}

//---------------------------------------------------------------------
void handle_mcast_pkg(communicator_header_t* compkg, int len)
{
	if (compkg->online_id != config_cache.bc_elem->online_id) {
		server_proto_t* svrproto = (void*)compkg;
		switch (svrproto->cmd) {
		case MCAST_home_msg:
			sprite_info_chg_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
		case MCAST_mole_grp_msg:
			molegroup_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
		case MCAST_login_msg:
			login_server_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
		case MCAST_reload_conf:
			reload_conf_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
        case MCAST_mole_class_msg:
            moleclass_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
            break;
		case MCAST_mole_sprite_info:
            mole_sprite_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
            break;
		case MCAST_sport_team_score:
            sport_team_get_max_score(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
		default:
			break;
		}
	}
}

int handle_homeserv_pkg(communicator_header_t* compkg, int len)
{
	//DEBUG_LOG("HOME SERVER CMD\t[%u %llx %u %u %u]", compkg->online_id, compkg->mapid, compkg->opid, compkg->id, len);
	if (compkg->online_id != config_cache.bc_elem->online_id || compkg->opid == SOP_get_users_from_HS || compkg->opid == SOP_get_map_users_cnt) {
		//DEBUG_LOG("PROCESS IT\t");
		server_proto_t* svrproto = (void*)compkg;
		switch (svrproto->cmd) {
		case MCAST_home_msg:
			sprite_info_chg_op(svrproto->id, svrproto->body, len - sizeof(server_proto_t));
			break;
		default:
			break;
		}
	}
	return 0;
}

/* @brief 发数据到校吧
 */
int send_to_school_bar(int cmd, int body_len, const void* body_buf, userid_t id)
{
	static uint8_t bcpkg[PAGESIZE];

	if (school_bar_fd == -1) {
		school_bar_fd = connect_to_service(config_get_strval("school_bar"), 0, 65535, 1);
	}

	if (school_bar_fd == -1) {
		return 0;
	}
	int len = sizeof(server_proto_t) + body_len;

	server_proto_t * pkg = (void*)bcpkg;
	pkg->len        = len;
	pkg->seq 		= 0;
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = id;
	memcpy(pkg->body, body_buf, body_len);

	//DEBUG_LOG("SEND HOMESERV\t[%u %u %llx %u %u]", cmd, opid, mapid, id, len);
	return net_send(school_bar_fd, bcpkg, len, 0);
}
