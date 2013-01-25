#ifndef MOLE2_HOME_H
#define MOLE2_HOME_H

#include "benchapi.h"
#include "dbproxy.h"
#include "util.h"

#define HOME_NAME_LEN       16

enum {
	home_flag_tutorial,
	home_flag_pet_house,
};

typedef struct home {
    uint32_t        userid;
    uint32_t        flag;
    uint32_t        type;
    uint32_t        level;
    uint32_t        exp;
    char            name[HOME_NAME_LEN];
    uint32_t        photo;
    uint32_t        blackboard;
    uint32_t        bookshelf;
    uint32_t        honorbox;
    uint32_t        petfun;
    uint32_t        expbox;
    uint32_t        effigy;
    uint32_t        postbox;
    uint32_t        itembox;
    uint32_t        petbox;
    uint32_t        compose1;
    uint32_t        compose2;
    uint32_t        compose3;
    uint32_t        compose4;
    uint32_t        wall;
    uint32_t        floor;
    uint32_t        bed;
    uint32_t        desk;
    uint32_t        ladder;
	uint32_t		petbox_cnt;
	uint32_t		exp_inbox;
	uint32_t		proflv;
}__attribute__((packed))home_t;
int get_home_info_cmd(sprite_t *p, uint8_t *body, uint32_t len);
int get_home_info_callback(sprite_t *p, userid_t id, uint8_t *buf, uint32_t len, uint32_t ret);
int update_home_flag_cmd(sprite_t *p, uint8_t *body, uint32_t len);

static inline int pkg_home_info(uint8_t* buf, home_t* ph)
{
    int i = 0;
    PKG_UINT32(buf, ph->userid, i);
    PKG_UINT32(buf, ph->flag, i);
    PKG_UINT32(buf, ph->type, i);
    PKG_UINT32(buf, ph->level, i);
    PKG_UINT32(buf, ph->exp, i);
    PKG_STR(buf, ph->name, i, HOME_NAME_LEN);
    PKG_UINT32(buf, ph->photo, i);
    PKG_UINT32(buf, ph->blackboard, i);
    PKG_UINT32(buf, ph->bookshelf, i);
    PKG_UINT32(buf, ph->honorbox, i);
    PKG_UINT32(buf, ph->petfun, i);
    PKG_UINT32(buf, ph->expbox, i);
    PKG_UINT32(buf, ph->effigy, i);
    PKG_UINT32(buf, ph->postbox, i);
    PKG_UINT32(buf, ph->itembox, i);
    PKG_UINT32(buf, ph->petbox, i);
    PKG_UINT32(buf, ph->compose1, i);
    PKG_UINT32(buf, ph->compose2, i);
    PKG_UINT32(buf, ph->compose3, i);
    PKG_UINT32(buf, ph->compose4, i);
    PKG_UINT32(buf, ph->wall, i);
    PKG_UINT32(buf, ph->floor, i);
    PKG_UINT32(buf, ph->bed, i);
    PKG_UINT32(buf, ph->desk, i);
    PKG_UINT32(buf, ph->ladder, i);
	PKG_UINT32(buf, ph->petbox_cnt, i);
	PKG_UINT32(buf, ph->exp_inbox, i);
	PKG_UINT32(buf, ph->proflv, i);

    return i;
}


static inline int db_set_home_flag (sprite_t* p, uint32_t id, uint32_t bit, uint32_t value)
{
	uint32_t buf[2];
	buf[0] = bit;
	buf[1] = value;
	return send_request_to_db(p, id, proto_db_update_home_flag, buf, 8);
}
#endif
