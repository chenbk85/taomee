

#include <fcntl.h>
#include <sys/mman.h>
#include <libtaomee/timer.h>
#include <libtaomee/utils.h>
#include <libtaomee/conf_parser/config.h>
#include  <statistic_agent/msglog.h>

#include "utils.h"
#include "hs_online.h"

#define CHECK_USER_ID(uid_) \
		do { \
			if ((uid_) < 10000 || (uid_) > 2000000000) { \
				ERROR_LOG("invalid uid=%u", (uid_)); \
				return -1; \
			} \
		} while (0)
/*
typedef struct PackUser {
	uint8_t** 	spri;
	map_id_t*   map;
} psi_t;
*/

fdsession_t* online[MAX_ONLINE_NUM] = {0};
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

//------------------------------------------------------
static inline void
free_sprite_info(void* sprite_onli)
{
	//sprite_onli_t* p = sprite_onli;
	//DEBUG_LOG("Free Sprite: %u", p->sprit.player.id);
	g_slice_free1(sizeof(sprite_onli_t), sprite_onli);
}

static inline void
free_sprite_where(void* sprite_where)
{
	sprite_where_t* p = sprite_where;
	g_hash_table_destroy(p->sprites);
	//DEBUG_LOG("Free Map: %llx", p->mapid);
	g_slice_free1(sizeof *p, p);
}

void init_communicator()
{
	maps = g_hash_table_new_full(g_int64_hash, g_int64_equal, 0, free_sprite_where);
}

void fini_communicator()
{
	g_hash_table_destroy(maps);
}

void remove_all_across_svr_sprites()
{
	g_hash_table_remove_all(maps);
}

uint32_t get_across_svr_sprite_cnt(map_id_t mapid)
{
	sprite_where_t* spwhere = g_hash_table_lookup(maps, &mapid);
	if (spwhere) {
		return g_hash_table_size(spwhere->sprites);
	}
	return 0;
}

//----------------------------------------------
//
typedef struct PackHomeUsers {
	uint8_t** 	ppbuf;
	int*   		plen;
	int*   		puser_cnt;
	int*   		psended_cnt;
	int*   		ppassed_cnt;
	int*   		ptotal_cnt;
	uint32_t*   ponlineid;
	uint32_t*	puserid;
	map_id_t*	pmapid;
} phu_t;

#define MAX_PKG_USER_CNT (pkg_size / (sizeof(sprite_to_online_t) + sizeof(pet_t)) - 1)
void pkg_sprite_to_online(void* key, void* sinfo, void* data)
{
	sprite_onli_t* spon = sinfo;
	sprite_t* p = &(spon->sprit.player);
	phu_t* pphu = data;

	if (*pphu->ponlineid == spon->onlineid) {
		*(pphu->ppassed_cnt) += 1;
		return;
	}
	//int* plen = data;
	sprite_to_online_t* pstot = (sprite_to_online_t*)(*(pphu->ppbuf) + *(pphu->plen));
	pstot->id = p->id;
	memcpy(pstot->nick, p->nick, sizeof(pstot->nick));
	pstot->inviter_id = p->inviter_id;
	pstot->old_invitee_cnt = p->old_invitee_cnt;
	pstot->invitee_cnt = p->invitee_cnt;
	pstot->color = p->color;
	pstot->flag = p->flag;
	pstot->tiles = 0;
	
	pstot->login = p->login;
	pstot->group = 0;

	pstot->action = p->action;
	pstot->direction = p->direction;
	pstot->posX = p->posX;
	pstot->posY = p->posY;
	pstot->action2 = p->action2;
	pstot->sub_grid = p->sub_grid;
	memcpy(&pstot->tmpinfo, &p->tmpinfo, sizeof(sprite_tmp_info_t));
	pstot->car_status = p->car_status;
	memcpy(&pstot->car_driving, &p->car_driving, sizeof(car_t));

	pstot->exam_num = p->exam_num;
	pstot->teach_exp = p->teach_exp;
	pstot->student_count = p->student_count;
	memcpy(pstot->students,p->students,sizeof(p->students));

	pstot->dietitian_exp = p->dietitian_exp;
	
	pstot->animal_nbr = p->animal_nbr;
	memcpy(&pstot->animal, &p->animal, sizeof(animal_attr_t));
	pstot->item_cnt = p->item_cnt;
	memcpy(pstot->items, p->items, sizeof(pstot->items));
	pstot->lamu_action = p->lamu_action;
	pstot->followed = 0;

	pstot->dragon_id = p->dragon_id;
	pstot->dragon_growth = p->dragon_growth;
	memcpy(pstot->dragon_name,p->dragon_name,sizeof(p->dragon_name));
	pstot->dungeon_explore_exp = p->dungeon_explore_exp;
	pstot->team_id = p->team_id;
	
	*(pphu->plen) += sizeof(sprite_to_online_t);
	if (p->followed) {
		pet_t* ppet = (pet_t*)(*(pphu->ppbuf) + (*(pphu->plen))); 
		memcpy(ppet, p->followed, sizeof(pet_t));
		pstot->followed = ppet;
		*(pphu->plen) += sizeof(pet_t);
	}
	*(pphu->puser_cnt) += 1;
	*(pphu->psended_cnt) += 1;
	
	//DEBUG_LOG("pkg user=%u %u %u %u", pstot->id, sizeof(pstot->items), sizeof(sprite_to_online_t), *(pphu->plen));
	DEBUG_LOG("user=%u sended=%u passed=%u total=%u %lu", *(pphu->puser_cnt), *(pphu->psended_cnt), *(pphu->ppassed_cnt), *(pphu->ptotal_cnt), MAX_PKG_USER_CNT);
	if(*(pphu->puser_cnt) == MAX_PKG_USER_CNT || 
		(((*(pphu->psended_cnt)) + (*(pphu->ppassed_cnt)) == *(pphu->ptotal_cnt)) 
			&& (*(pphu->puser_cnt) != 0))) {
		*(uint32_t*)(*(pphu->ppbuf)) = *(pphu->puser_cnt);
		DEBUG_LOG("SEND HOME USRS\t[%u %lu %u]", pstot->id, *(pphu->pmapid), *(uint32_t*)(*(pphu->ppbuf)));
		send_to_online(MCAST_home_msg, *(pphu->plen), *(pphu->ppbuf), *(pphu->puserid), *(pphu->pmapid), HS_send_home_users, *pphu->ponlineid);
		*(pphu->puser_cnt) = 0;
		*(pphu->plen) = 4;
	}
}

static uint8_t home_all_usr_info[pkg_size];

static inline void
send_home_all_users(userid_t uid, uint32_t online_id, sprite_where_t* spwhere)
{
	int total_cnt = g_hash_table_size(spwhere->sprites);
	if (total_cnt) {
		memset(home_all_usr_info, 0, pkg_size);
		
		uint8_t* tmp_buf = home_all_usr_info;
		int len = 4;
		int user_cnt = 0;
		int sended_cnt = 0;
		int passed_cnt = 0;
		phu_t phu = {&tmp_buf, &len, &user_cnt, &sended_cnt, &passed_cnt, &total_cnt, &online_id, &uid, &spwhere->mapid};
		g_hash_table_foreach(spwhere->sprites, pkg_sprite_to_online, &phu);
		
		//DEBUG_LOG("SEND HOME USERS\t [%u %lu %u %u %x]", online_id, spwhere->mapid, uid, len, tmp_buf);
		//*(uint32_t*)home_all_usr_info = usr_cnt;
		//send_to_online(MCAST_home_msg, len, home_all_usr_info, uid, spwhere->mapid, HS_send_home_users, online_id);
	}
}

//
sprite_t* get_across_svr_sprite(userid_t uid, map_id_t mapid)
{
	sprite_where_t* spwhere = g_hash_table_lookup(maps, &mapid);
	if (spwhere) {
		return g_hash_table_lookup(spwhere->sprites, &uid);
	}
	return 0;
}

#define MAX_SEARCH_LATEST	10
//----------------------------------------------
static inline void
enter_map_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	//DEBUG_LOG("send home to map_id = %llu", spwhere->mapid);
	if (len != sizeof(sprite_info_t) && len != sizeof(sprite_t)) {
		EMERG_LOG("mcast: sprite_info len error\t[%lu %lu] len:%d", sizeof(sprite_info_t), sizeof(sprite_t),len);
		return;
	}

	int loop = spwhere->latest_idx - 1;
	int end = (spwhere->latest_idx > MAX_SEARCH_LATEST) ? (spwhere->latest_idx - MAX_SEARCH_LATEST) : 0;
	for ( ; loop >= end; loop--) {
		if (spwhere->onlineid[loop] == online_id) {
			break;
		}
	}
	if (loop == end - 1) {
		spwhere->onlineid[spwhere->latest_idx] = online_id;
		spwhere->latest_idx = (spwhere->latest_idx + 1) % MAX_LATEST_ONLINE;
	}
	
	sprite_onli_t* sonli = g_slice_alloc(sizeof(sprite_onli_t));
	if (!sonli) {
		ERROR_LOG("not enough memory\t[%u]", uid);
		return;
	}
	//DEBUG_LOG("Alloc Sprite %u %llx", uid, spwhere->mapid);
	sonli->onlineid = online_id;
	sprite_info_t* sinfo = &(sonli->sprit);
	memcpy(sinfo, body, len);
	if (sinfo->player.followed) {
		sinfo->player.followed = &(sinfo->pet);
	}
	sinfo->player.tiles  = 0;
	sinfo->player.fdsess = 0;

	send_home_all_users(uid, online_id, spwhere);
	//psi_t buff = {&sinfo, &spwhere->mapid};
	//g_hash_table_foreach(spwhere->sprites, notify_user_enter, &buff);
	g_hash_table_replace(spwhere->sprites, &(sonli->sprit.player.id), sonli);
}

static inline void
leave_map_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	//sprite_info_t* psit = g_hash_table_lookup(spwhere->sprites, &uid);
	//if (!psit) {
		//ERROR_LOG("not find user in map[%u %u]", uid, spwhere->mapid);
		//return;
	//}
	
	//psi_t buff = {&psit, &spwhere->mapid};
	//g_hash_table_foreach(spwhere->sprites, notify_user_leave, &buff);

	//DEBUG_LOG("REMOVE USER[%u %llx]", uid, spwhere->mapid);
	g_hash_table_remove(spwhere->sprites, &uid);
	if (!g_hash_table_size(spwhere->sprites)) {
		g_hash_table_remove(maps, &(spwhere->mapid));
	}
}

static inline void
basic_info_chg_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		int i = sizeof(protocol_t) + 4;
		UNPKG_STR(body, spinfo->player.nick, i, USER_NICK_LEN);
		UNPKG_UINT32(body, spinfo->player.color, i);
		i += 9;
		UNPKG_UINT32(body, spinfo->player.action, i);
	}
}

static inline void
take_dragon_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, sizeof(protocol_t) + 16);

	uint32_t i = sizeof(protocol_t);

	userid_t userid;
	uint32_t dragon_id;
	uint32_t dragon_gorwth;
	uint32_t dragon_state;
	
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, dragon_id, i);
	UNPKG_UINT32(body, dragon_gorwth, i);
	UNPKG_UINT32(body, dragon_state, i);

	DEBUG_LOG("take_dragon_op %u %u %u %u",uid,userid,dragon_id,dragon_gorwth);
	
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &userid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		if(dragon_state) {
			spinfo->player.dragon_id = dragon_id;
			spinfo->player.dragon_growth = dragon_gorwth;
		} else {
			spinfo->player.dragon_id = 0;
			spinfo->player.dragon_growth = 0;
		}
		DEBUG_LOG("take_dragon_op OK");
	}
}

static inline void
transform_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 37);

	int i = sizeof(protocol_t) + 4;
	userid_t rcvid, itmid;
	UNPKG_UINT32(body, rcvid, i);
	UNPKG_UINT32(body, itmid, i);

	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		spinfo->player.action = itmid;
	}
}

static inline void
transform_revert_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		spinfo->player.action = 0;
	}
}

static inline void
cancel_transform_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		spinfo->player.action = 0;
	}
}

static inline void
action_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
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
lahm_do_action_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 33);

	uint32_t userid, petid;
	int i = sizeof(protocol_t);
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);

	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &userid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		UNPKG_UINT32(body, spinfo->player.lamu_action, i);
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
lahm_action_expire_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	CHECK_INT_EQ_VOID(len, 25);

	uint32_t userid, petid;
	int i = sizeof(protocol_t);
	UNPKG_UINT32(body, userid, i);
	UNPKG_UINT32(body, petid, i);

	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &userid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		spinfo->player.lamu_action = 0;
	}
}

static void
reset_common_action(sprite_t *p)
{
	p->direction = 0;
	if (p->action < ACTION_MAX)
		p->action = 0;
	p->action2 = 0;
}

static inline void
walk_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		int i = sizeof(protocol_t) + 4;
		UNPKG_UINT32(body, spinfo->player.posX, i);
		UNPKG_UINT32(body, spinfo->player.posY, i);
		reset_common_action(&(spinfo->player));
	}
}

static inline void
walk_pet_op(userid_t uid, uint32_t online_id, sprite_where_t* spwhere, const void* body, int len)
{
	sprite_onli_t* sponli = g_hash_table_lookup(spwhere->sprites, &uid);
	if (sponli) {
		sprite_info_t* spinfo = &(sponli->sprit);
		if (len) {
			memcpy(&(spinfo->pet), body, len);
			spinfo->player.followed = &(spinfo->pet);
		} else {
			spinfo->player.followed = 0;
		}
	}
}

//-------------------------------------------------------------
// For Online Server
typedef struct PackBuf {
	void** 		pbuf;
	int*   		plen;
} pb_t;

static void tranfer_to_online(void* key, void* sinfo, void* data)
{
	pb_t* ppb = data;
	sprite_onli_t* pso = sinfo;
	//DEBUG_LOG("TRANS TO\t[%u %u %llx %u]", pso->onlineid, pso->sprit.player.id, *(ppb->pbuf), *(ppb->plen));
	if (online[pso->onlineid])
		send_pkg_to_client(online[pso->onlineid], *(ppb->pbuf), *(ppb->plen));
}

static void tranfer_to_all_onlines(void* buf, int len, sprite_where_t* spwhere)
{
	pb_t pb = {&buf, &len};
	g_hash_table_foreach(spwhere->sprites, tranfer_to_online, &pb);
}

static void tranfer_to_latest_onlines(void* buf, int len, sprite_where_t* spwhere)
{
	uint8_t u8loop;
	for (u8loop = 0; u8loop < MAX_LATEST_ONLINE; u8loop++) {
		uint16_t online_id = spwhere->onlineid[u8loop];
		if (online_id && online[online_id]) {
			DEBUG_LOG("TRANS TO LATEST\t[%u %u]", u8loop, online_id);
			send_pkg_to_client(online[online_id], buf, len);
		}
	}
}

void send_to_online_map(int cmd, int body_len, const void* body_buf, sprite_onli_t* aso, sprite_where_t* asw, uint32_t opid)
{
	static uint8_t mappkg[pkg_size];

	if (body_len > pkg_size - sizeof(communicator_header_t)) {
		ERROR_LOG("pkg too large\t[%u %u %u %u %lu]", cmd, opid, aso->onlineid, aso->sprit.player.id, asw->mapid);
	}

	int len = body_len + sizeof(communicator_header_t);
	communicator_header_t* pkg = (void*)mappkg;
	pkg->len        = len;
	pkg->online_id  = aso->onlineid;    // 0 is the homeserver
	pkg->cmd        = cmd;
	pkg->ret        = 0;
	pkg->id         = aso->sprit.player.id;
	pkg->mapid      = asw->mapid;
	pkg->opid       = opid;
	memcpy(pkg->body, body_buf, body_len);

	void* ptmp = mappkg;
	pb_t pb = {&ptmp, &len};
	g_hash_table_foreach(asw->sprites, tranfer_to_online, &pb);
}

static void notify_usr_leave_when_onli_down(sprite_onli_t* aso, sprite_where_t* asw)
{
	uint8_t buff[64];
	int i;
	i = sizeof (protocol_t);
	PKG_UINT32 (buff, 1, i);
	PKG_UINT32 (buff, aso->sprit.player.id, i);
	init_proto_head2(buff, HS_LEAVE_MAP, i);

	//DEBUG_LOG("NOTIFY USER LEAVE when connection break\t[%u %u %u]", aso->sprit.player.id, buff, i);
	
	send_to_online_map(MCAST_home_msg, i, buff, aso, asw, HS_LEAVE_MAP);
	//tranfer_to_all_onlines(buff, i, asw);
}

typedef struct clean_user {
	uint32_t**		 	onli_id;
	sprite_where_t**	spwh;
} clean_user_t;

static void clean_user(void* key, void* sinfo, void* data)
{
	clean_user_t* pcu = data;
	sprite_onli_t* pso = sinfo;
	sprite_where_t* psw = *(pcu->spwh);
	if (pso->onlineid == *(*(pcu->onli_id))) {
		//DEBUG_LOG("CLEAN USER\t[%u %u %llx]", pso->onlineid, pso->sprit.player.id, psw->mapid);
		sprite_onli_t sot;
		memcpy(&sot, pso, sizeof(sprite_onli_t));
		g_hash_table_remove(psw->sprites, &(pso->sprit.player.id));

		if (g_hash_table_size(psw->sprites)) {
			notify_usr_leave_when_onli_down(&sot, psw);
		}
	}
}

static void clean_onlinex_users_in_map(void* key, void* sinfo, void* data)
{
	sprite_where_t* psw = sinfo;

	//DEBUG_LOG("CLEAN MAP\t[%llx]", psw->mapid);
	clean_user_t cut = {(uint32_t**)&data, &psw};
	g_hash_table_foreach(psw->sprites, clean_user, &cut);
	if (!g_hash_table_size(psw->sprites)) {
		g_hash_table_remove(maps, &(psw->mapid));
		return;
	} 
}

void clean_all_users_onlinex_down(int fd)
{
	int loop;
	for (loop = 0; loop < MAX_ONLINE_NUM; loop++) {
		if (online[loop] && online[loop]->fd == fd) {
			online[loop] = 0;
			break;
		}
	}
	DEBUG_LOG("CLEAN USERS\t[online=%u]", loop);
	if (loop == MAX_ONLINE_NUM) {
		return;
	}
	
	g_hash_table_foreach(maps, clean_onlinex_users_in_map, &loop);
}

int sprite_info_chg_op(void* buf, int len, fdsession_t* fdsess)
{
	CHECK_VAL_GE(len, sizeof(communicator_header_t));
	
	communicator_header_t* cheader = buf;
	sprite_where_t* spwhere    = g_hash_table_lookup(maps, &(cheader->mapid));

	if(cheader->opid == HS_get_map_users_cnt) {
		int count = 0;
		if(spwhere) {
			count = g_hash_table_size(spwhere->sprites);
			DEBUG_LOG("map Users\t[%u %u %lu] %d", cheader->opid, cheader->id, spwhere->mapid,count);
		}
		DEBUG_LOG("HS_get_map_users_cnt\t[%d %u %u %lu %d]", cheader->online_id,cheader->opid, cheader->id, cheader->mapid,cheader->cmd);
		if (online[cheader->online_id]) {
			uint8_t buff[128] = {0};
			communicator_header_t* pkg = (void*)buff;
			memcpy(pkg, cheader, sizeof(communicator_header_t));
			pkg->ret = 0;
			memcpy(pkg->body, &count, 4);
			pkg->len = sizeof(communicator_header_t) + 4;
			send_pkg_to_client(online[cheader->online_id], pkg, sizeof(communicator_header_t) + 4);
		}
		return 0;
	}

	if (!spwhere && cheader->opid == HS_enter_map) {
		//DEBUG_LOG("Alloc Map %llu", cheader->mapid);
		spwhere          = g_slice_alloc0(sizeof *spwhere);
		spwhere->mapid   = cheader->mapid;
		spwhere->sprites = g_hash_table_new_full(g_int_hash, g_int_equal, 0, free_sprite_info);
		g_hash_table_insert(maps, &(spwhere->mapid), spwhere);
	}
	if (!spwhere) {
		ERROR_RETURN(("not find maps\t[%u %u %lu]", cheader->opid, cheader->id, cheader->mapid), 0);
	}

	//DEBUG_LOG("BroadCast Map Users\t[%u %u %llx]", cheader->opid, cheader->id, spwhere->mapid);
	if (cheader->opid == HS_LEAVE_MAP) {
		tranfer_to_latest_onlines(buf, len, spwhere);
	} else {
		tranfer_to_all_onlines(buf, len, spwhere);
	}

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(cheader->id, cheader->online_id, spwhere, cheader->body, len - sizeof(communicator_header_t)); break
	//
	switch (cheader->opid) {
		DISPATCH_OP(HS_enter_map, enter_map_op);
		DISPATCH_OP(HS_walk_pet, walk_pet_op);

		DISPATCH_OP(HS_ACTION, action_op);
		DISPATCH_OP(HS_WALK, walk_op);
		DISPATCH_OP(HS_LEAVE_MAP, leave_map_op);
		DISPATCH_OP(HS_SHOW_ITEM_USE, transform_op);
		DISPATCH_OP(HS_SHOW_ITEM_EXPIRE, transform_revert_op);
		DISPATCH_OP(HS_CANCEL_CHANGE_FORM, cancel_transform_op);
		DISPATCH_OP(HS_LAHM_DO_ACTION, lahm_do_action_op);
		DISPATCH_OP(HS_LAHM_ACTION_EXPIRE, lahm_action_expire_op);
		DISPATCH_OP(HS_PROTO_CALL_DRAGON, take_dragon_op);

		case HS_MODIFY_COLOR:
		case HS_MODIFY_NICK:
			basic_info_chg_op(cheader->id, cheader->online_id, spwhere, cheader->body, len - sizeof(communicator_header_t));
			break;
		default:
			break;
	}
#undef DISPATCH_OP

	return 0;
}

