#include "pet.h"

#include "wish_pool.h"

typedef struct MakeWishSess {
	userid_t	sender_id;
	char		sender_nick[USER_NICK_LEN];
	userid_t	rcvid;
	uint32_t	itmid;
	char		itmname[30];
} __attribute__((packed)) make_wish_sess_t;

typedef struct WishContent {
	uint32_t	date;
	userid_t	sender_id;
	char		sender_nick[USER_NICK_LEN];
	uint32_t	itmid;
} __attribute__((packed)) wish_content_t;

typedef struct WishList {
	uint32_t		cnt;
	wish_content_t	wishes[];
} __attribute__((packed)) wish_list_t;

// pkg from db when after a given wish was deleted
typedef struct DelWishRet {
	userid_t	rcvid;
	uint32_t	itmid;
} __attribute__((packed)) del_wish_ret_t;

//---------------------------------------------------
int del_wish_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
#ifdef TW_VER
	static const char* nick[]  = { "紅色", "黃色", "粉紅色", "天藍色" };
#else
	static const char* nick[]  = { "红色", "黄色", "粉红色", "天蓝色" };
#endif
	static const int   color[] = { 1, 2, 4, 3 };

	CHECK_BODY_LEN(len, sizeof(del_wish_ret_t));

	const del_wish_ret_t* ret     = (void*)buf;
	const item_kind_t*    itmkind = find_kind_of_item(ret->itmid);
	const item_t*         itm;

	if (itmkind && (itm = get_item(itmkind, ret->itmid))) {

		/*许愿池领取物品统计*/
		uint32_t data[] = {0, p->id};
		msglog(statistic_logfile, 0x202000c, now.tv_sec, data, sizeof(data));

		switch (itmkind->kind) {
		case SUIT_ITEM_KIND:
		case CHG_COLOR_ITEM_KIND:
		case CHG_SHOW_ITEM_KIND:
		case THROW_ITEM_KIND:
		case HOME_ITEM_KIND:
		case FEED_ITEM_KIND:
		case HOMELAND_ITEM_KIND:
			return db_buy_items(p, itm, 1, itmkind, 1, 1);
		case PET_ITEM_KIND:
			return do_buy_pet(p, ret->itmid, color[ret->itmid - 170001], nick[ret->itmid - 170001], 0, 0);
		case ATTR_ITEM_KIND:
			return db_attr_op(p, 200, 0, 0, 0, 0, ATTR_CHG_fetch_wish, 0);
		default:
			ERROR_LOG("invalid itmkind=%u itm=%u", itmkind->kind, ret->itmid);
			break;
		}
	}

	return send_to_self_error(p, p->waitcmd, -ERR_system_error, 1);
}

int get_wish_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	const wish_list_t* wlist = (void*)buf;
	CHECK_BODY_LEN(len, (4 + wlist->cnt * sizeof(wish_content_t)));

	int i = sizeof(protocol_t), j;
	PKG_UINT32(msg, wlist->cnt, i);
	for (j = 0; j != wlist->cnt; ++j) {
		PKG_UINT32(msg, wlist->wishes[j].date, i);
		PKG_UINT32(msg, wlist->wishes[j].sender_id, i);
		PKG_STR(msg, wlist->wishes[j].sender_nick, i, USER_NICK_LEN);
		PKG_UINT32(msg, wlist->wishes[j].itmid, i);
	}
	init_proto_head(msg, p->waitcmd, i);

	return send_to_self(p, msg, i, 1);
}

//---------------------------------------------------
static inline int
wish_acceptable_item_kind(uint32_t kind_id)
{
	return kind_id < 128;
}

//---------------------------------------------------
int make_wish_cmd(sprite_t* p, const uint8_t* body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(bodylen, 42);

	int i = 0;
	uint32_t molebean;
	make_wish_sess_t* wishsess = (void*)(p->session);
	// unpack
	UNPKG_UINT32(body, wishsess->rcvid, i);
	UNPKG_UINT32(body, wishsess->itmid, i);
	UNPKG_STR(body, wishsess->itmname, i, 30);
	UNPKG_UINT32(body, molebean, i);

	const item_kind_t* itmkind;
	const item_t*      itm;
	if ( (molebean > 0)
			&& ( (wishsess->itmid == 1001)
					|| ((itmkind = find_kind_of_item(wishsess->itmid))
						&& wish_acceptable_item_kind(itmkind->kind)
						&& (itm = get_item(itmkind, wishsess->itmid))
						&& itm_wishable(p, itm)) ) ) {
		p->sess_len = sizeof *wishsess;
		wishsess->sender_id = p->id;
		memcpy(wishsess->sender_nick, p->nick, USER_NICK_LEN);
		DEBUG_LOG( "MAKE WISH\t[uid=%u rcvid=%u itmid=%u coins=%u]",
					p->id, wishsess->rcvid, wishsess->itmid, molebean );

		/*许愿人数统计*/
		uint32_t data[] = {p->id, 0};
		msglog(statistic_logfile, 0x202000c, now.tv_sec, data, sizeof(data));

		return db_attr_op(p, -molebean, 0, 0, 0, 0, ATTR_CHG_add_wish, molebean);
	}

	ERROR_RETURN( ("invalid wish: uid=%u rcv=%u itm=%u coins=%u",
					p->id, wishsess->rcvid, wishsess->itmid, molebean), -1 );
}
