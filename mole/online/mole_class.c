#include <libtaomee/log.h>
#include <statistic_agent/msglog.h>
#include "util.h"
#include "logic.h"
#include "proto.h"
#include "dbproxy.h"
#include "central_online.h"
#include "communicator.h"
#include "exclu_things.h"
#include "mole_work.h"

#include "mole_class.h"
#include "race_car.h"
#include "mole_pasture.h"
#include "hallowmas.h"

#define IS_AMBASSADOR(x) ((!!((x)&0x08)) || (!!((x)&0x40)))
#define LOGO_WORD_LEN       4
#define NICK_LEN            16
#define SLOGAN_LEN          60
#define ITEM_INFO_LEN       12
#define MAX_CLASS_MEMBER_NUM    100
#define MAX_CLASS_JOIN      10
#define MAX_TOP_CLASS       60

#define CHECK_CLASS(tmp_, uid_) do{ \
    if(!check_class(tmp_, uid_)) { \
        DEBUG_LOG("CHECK CLASS [%d %d %d]", tmp_->id, tmp_->waitcmd, uid_);\
        send_to_self_error(tmp_, tmp_->waitcmd, -ERR_class_not_exist, 1); \
        return 0; \
    } \
    } while(0)

static int cup_id[4] = {
    1260071, 1260072, 1260073, 1260082,
};
static int banner_id[4] = {
    1260068, 1260069, 1260070, 1260083,
};

enum join_flag_t {
    all_join,
    monitor_control,
    nobody_join,
};

enum visit_flag_t {
    all_visit,
    member_visit,
};
typedef struct _class_create {
    uint32_t interest;
    uint32_t logo_type;
    char     logo_word[LOGO_WORD_LEN];
    uint32_t logo_color;
    uint32_t join_flag;
    uint32_t visit_flag;
    char     name[NICK_LEN];
    char     slogan[SLOGAN_LEN];
}create_class_info_t;

typedef struct _class_info_set{
    uint32_t join_flag;
    uint32_t visit_flag;
    char     name[NICK_LEN];
    char     slogan[SLOGAN_LEN];
}set_class_info_t;

typedef struct _item_t {
    uint32_t itm_id;
    uint8_t  itm_content[ITEM_INFO_LEN];
}itm_info_t;

typedef struct _class_detail_info {
    create_class_info_t s_info;
    char    monitor_nick[NICK_LEN];
    uint32_t member_cnt;
    uint32_t itm_cnt;
}class_detail_info_t;

typedef struct _class_logo_info{
    uint32_t logo_type;
    char     logo_word[LOGO_WORD_LEN];
    uint32_t logo_color;
    uint32_t join_flag;
    uint32_t visit_flag;
    uint32_t member_cnt;
}class_logo_info_t;

typedef struct _class_simple_info{
    uint32_t logo_type;
    char     logo_word[LOGO_WORD_LEN];
    uint32_t logo_color;
    uint32_t join_flag;
    uint32_t visit_flag;
    char     name[NICK_LEN];
    char     slogan[SLOGAN_LEN];
    char     monitor_nick[NICK_LEN];
    uint32_t member_cnt;
}class_simple_info_t;

typedef struct _top_class_info {
    uint32_t classid;
    uint32_t member_cnt;
    uint32_t interest;
    char monitor_nick[NICK_LEN];
    char name[NICK_LEN];
}top_class_info_t;

typedef struct _honor_flag {
    uint32_t type;
    uint32_t itemid;
}honor_flag_t;

static top_class_info_t server_class[MAX_TOP_CLASS];
static int server_class_num = 0;
static int server_class_reflesh_pos = 0;
//tmp:class class_ques

typedef struct _c_q_t{
    uint32_t id;
    char content[192];
    char option_a[64];
    char option_b[64];
    char option_c[64];
    char option_d[64];
    uint32_t ans;
} __attribute__((packed)) class_ques_t;

static class_ques_t class_ques[1000];
static uint32_t class_ques_max;

int check_class(sprite_t *p, uint32_t uid)
{
    int valid = 0;
    int i;
    for(i = 0; i < p->my_moleclass_num; i++) {
        if(uid == p->my_moleclass_id[i]) {
            valid = 1;
        }
    }
    return valid;
}

int sprite_del_class(sprite_t *p, uint32_t classid)
{
    int i = 0;
    for(i = 0; i < p->my_moleclass_num; i++) {
        if(p->my_moleclass_id[i] == classid) {
            p->my_moleclass_id[i] = p->my_moleclass_id[p->my_moleclass_num - 1];
            p->my_moleclass_id[p->my_moleclass_num - 1] = 0;
            p->my_moleclass_num--;
            if(p->my_moleclass_first == classid) {
                p->my_moleclass_first = 0;
            }
            return 0;
        }
    }
    return -1;
}

int create_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_CLASS(p->id);
    CHECK_BODY_LEN(bodylen, sizeof(create_class_info_t));

    if(check_class(p, p->id)) {
        DEBUG_LOG("ALREADY HAVE CREATED CLASS [%d]", p->id);
        send_to_self_error(p, PROTO_CREATE_CLASS, -ERR_class_already_created, 1);
        return 0;
    }

    if(p->my_moleclass_num == MAX_CLASS_JOIN) {
        DEBUG_LOG("CREATE CLASS: ALREADY %d classes [%d]", p->my_moleclass_num, p->id);
        send_to_self_error(p, PROTO_CREATE_CLASS, -ERR_reach_max_class, 1);
        return 0;
    }

    int i = 0;
    create_class_info_t new_class_info;
    UNPKG_UINT32(body, new_class_info.interest, i);
    UNPKG_UINT32(body, new_class_info.logo_type, i);
    UNPKG_STR(body, new_class_info.logo_word, i, LOGO_WORD_LEN);
    UNPKG_UINT32(body, new_class_info.logo_color, i);
    UNPKG_UINT32(body, new_class_info.join_flag, i);
    UNPKG_UINT32(body, new_class_info.visit_flag, i);
    UNPKG_STR(body, new_class_info.name, i, NICK_LEN);
    UNPKG_STR(body, new_class_info.slogan, i, SLOGAN_LEN);


	CHECK_DIRTYWORD(p, new_class_info.logo_word);
	CHECK_DIRTYWORD(p, new_class_info.name);
	CHECK_DIRTYWORD(p, new_class_info.slogan);
    new_class_info.visit_flag = all_visit;
    if(new_class_info.join_flag > nobody_join) {
        ERROR_RETURN(("CLASS FLAG ERROR [%d %d]", p->id, new_class_info.join_flag), -1);
    }
    DEBUG_LOG("CREATE NEW CLASS [%d, %s]", p->id, new_class_info.name);

    memcpy(p->session, &new_class_info, sizeof(create_class_info_t));

    return send_request_to_db(SVR_PROTO_CREATE_CLASS, p, sizeof(new_class_info), &new_class_info, p->id);
}

int get_class_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    uint32_t classid;
	if (unpkg_uint32(body, bodylen, &classid) == -1) return -1;
	CHECK_VALID_CLASS(classid);
    DEBUG_LOG("GET CLASS INFO [%d %d]", p->id, classid);
	return send_request_to_db(SVR_PROTO_GET_CLASS_INFO, p, 0, NULL, classid);
}

int get_class_simple_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    uint32_t classid;
	if (unpkg_uint32(body, bodylen, &classid) == -1) return -1;
	CHECK_VALID_CLASS(classid);
    DEBUG_LOG("GET CLASS SIMPLE INFO [%d %d]", p->id, classid);
	return send_request_to_db(SVR_PROTO_GET_CLASS_SIMP_INFO, p, 0, NULL, classid);
}

int modify_class_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, sizeof(set_class_info_t));
    if(!check_class(p, p->id)) {
        DEBUG_LOG("MODIDY CLASS, NOT ADMIN [%d]", p->id);
        send_to_self_error(p, p->waitcmd, -ERR_class_already_created, 1);
        return 0;
    }
    DEBUG_LOG("MODIFY CLASS INFO [%d]", p->id);
    set_class_info_t set_cls;
    int len = 0;
    UNPKG_UINT32(body, set_cls.join_flag, len);
    UNPKG_UINT32(body, set_cls.visit_flag, len);
    UNPKG_STR(body, set_cls.name, len, NICK_LEN);
    UNPKG_STR(body, set_cls.slogan, len, SLOGAN_LEN);
	CHECK_DIRTYWORD(p, set_cls.name);
	CHECK_DIRTYWORD(p, set_cls.slogan);
    if(set_cls.join_flag > nobody_join || set_cls.visit_flag > member_visit) {
        ERROR_RETURN(("ERROR SETTING [%d %d %d]", p->id, set_cls.join_flag, set_cls.visit_flag), -1);
    }

    return send_request_to_db(SVR_PROTO_SET_CLASS_INFO, p, sizeof(set_class_info_t), &set_cls, p->id);

}

int get_class_logo_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int classid = 0;
    UNPKG_UINT32(body, classid, i);
    CHECK_VALID_CLASS(classid);
    DEBUG_LOG("GET LOGO CLASS INFO [%d]", p->id);
    return send_request_to_db(SVR_PROTO_GET_CLASS_LOGO_INFO, p, 0, NULL, classid);
}

int set_first_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int classid = 0;
    UNPKG_UINT32(body, classid, i);
    DEBUG_LOG("SET FIRST CLASS [%d %d]", p->id, classid);
    if(classid != 0) {
        CHECK_VALID_CLASS(classid);
        CHECK_CLASS(p, classid);
    }
    *(uint32_t*)p->session = classid;
    return send_request_to_db(SVR_PROTO_SET_FIRST_CLASS, p, 4, &classid,p->id);
}

int get_first_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int uid = 0;
    UNPKG_UINT32(body, uid, i);
    CHECK_VALID_CLASS(uid);
    DEBUG_LOG("GET FIRST CLASS [%d %d]", p->id, uid);
    if(uid == p->id) {
        response_proto_uint32(p, p->waitcmd, p->my_moleclass_first, 0);
        return 0;
    }
    sprite_t *tmp = get_sprite(uid);
    if(tmp) {
        response_proto_uint32(p, p->waitcmd, tmp->my_moleclass_first, 0);
        return 0;
    } else {
        return send_request_to_db(SVR_PROTO_GET_FIRST_CLASS, p, 0, NULL, uid);
    }
}

int join_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int classid = 0;
    UNPKG_UINT32(body, classid, i);
    CHECK_VALID_CLASS(classid);
    CHECK_VALID_CLASS(p->id);
    if(p->my_moleclass_num == MAX_CLASS_JOIN) {
        DEBUG_LOG("REACH THE MAX CLASS NUM [%d]", p->id);
        send_to_self_error(p, p->waitcmd, -ERR_reach_max_class, 1);
        return 0;
    }
    if(check_class(p, classid)) {
        DEBUG_LOG("ALREADY JOIN THIS CLASS [%d %d]", p->id, classid);
        send_to_self_error(p, p->waitcmd, -ERR_class_already_created, 1);
        return 0;
    }
    DEBUG_LOG("JOIN CLASS [%d %d]", p->id, classid);
    *(uint32_t*)p->session = classid;
    return send_request_to_db(SVR_PROTO_GET_CLASS_LOGO_INFO, p, 0, NULL, classid);
}

int quit_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    int i = 0;
    int classid = 0;
    UNPKG_UINT32(body, classid, i);
    CHECK_VALID_CLASS(classid);
    if(classid == p->id) {
        ERROR_RETURN(("quit class error [%d]", p->id), -1);
    }
    DEBUG_LOG("QUIT CLASS [%d %d]", p->id, classid);
    CHECK_CLASS(p, classid);
    *(uint32_t*)p->session = classid;
    return send_request_to_db(SVR_PROTO_DEL_CLASS_MEMBER, p, 4, &p->id, classid);
}

int request_join_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 64);

    if(p->my_moleclass_num == MAX_CLASS_JOIN) {
        DEBUG_LOG("REQUEST JOIN CLASS: ALREADY %d classes [%d]", p->my_moleclass_num, p->id);
        send_to_self_error(p, p->waitcmd, -ERR_reach_max_class, 1);
        return 0;
    }

    int i = 0;
    int classid = 0;
    UNPKG_UINT32(body, classid, i);

    if(check_class(p, classid)) {
        DEBUG_LOG("ALREADY JOIN CLASS [%d %d]", p->id, classid);
        send_to_self_error(p, p->waitcmd, -ERR_class_already_created, 1);
        return 0;
    }

	CHECK_DIRTYWORD(p, (char*)(body + 4));
    char txt[1024];
#ifndef TW_VER
	int j = snprintf(txt, sizeof(txt), "%s", (char*)(body + 4));
#else
	int j = snprintf(txt, sizeof(txt), "%s", (char*)(body + 4));
#endif
    j = text_notify_pkg(msg + 4, p->waitcmd, p->id, classid, p->sub_grid, p->nick, 0, j, txt);

	sprite_t* owner = get_sprite(classid);
	if (owner) {
		send_to_self(owner, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = p->id;
		chat_across_svr(classid, msg, j + 4);
	}
    DEBUG_LOG("REQ MONITOR REPLY [%d %d]", p->id, classid);
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int reply_request_join_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 8);
    int i = 0;
    int memberid = 0;
    int accept = 0;
    UNPKG_UINT32(body, memberid, i);
    UNPKG_UINT32(body, accept, i);
    CHECK_VALID_CLASS(memberid);
    DEBUG_LOG("REPLY REQUEST JOIN [%d, %d, %d]", p->id, memberid, accept);
    CHECK_CLASS(p, p->id);

    int j;
    char txt[1024];
    sprite_t* initiator;

    if (!accept) {
#ifndef TW_VER
        j = sprintf(txt, "%s拒绝了你加入班级的请求", p->nick);
#else
        j = sprintf(txt, "%s拒絕了你加入班級的請求", p->nick);
#endif

        j = text_notify_pkg(msg + 4, p->waitcmd, p->id, p->id, p->sub_grid, p->nick, accept, j, txt);
        if ((initiator = get_sprite(memberid))) {
            send_to_self(initiator, msg + 4, j, 0);
        } else {
            *(uint32_t*)msg = p->id;
            chat_across_svr(memberid, msg, j + 4);
        }
        response_proto_head(p, p->waitcmd, 0);
    } else {
        *(uint32_t*)p->session = memberid;
        send_request_to_db(SVR_PROTO_ADD_CLASS_MEMBER, p, 4, &memberid, p->id);
    }
    return 0;
}

int request_add_class_item(sprite_t* p, uint32_t classid, int itmid, int itm_cnt, int itm_max, int ret_needed)
{
    uint8_t pkg[1024];
    int l = 0;
    PKG_H_UINT32(pkg, itmid, l);
    PKG_H_UINT32(pkg, itm_cnt, l);
    PKG_H_UINT32(pkg, itm_max, l);
    PKG_H_UINT32(pkg, p->id, l);
    PKG_STR(pkg, p->nick, l, NICK_LEN);
    PKG_H_UINT32(pkg, p->color, l);
    PKG_UINT8(pkg, ISVIP(p->flag), l);
    PKG_H_UINT32(pkg, get_now_tv()->tv_sec, l);
    DEBUG_LOG("CLASS ADD ITM [%d %d %d %d]", p->id, classid, itmid, itm_cnt);
    return send_request_to_db(SVR_PROTO_CLASS_ADD_ITEM, ret_needed ? p : NULL, l, pkg, classid);

}

int buy_itm_class(sprite_t* p, int itmid, int itm_cnt, int itm_max)
{
    uint32_t classid;
    if(itmid != 1260093 && itmid != 1260100 && itmid != 1260067) {
        if(!p->tiles || !IS_CLASS_MAP(p->tiles->id)) {
            ERROR_RETURN(("buy class itm not in class [%d]", p->id), -1);
        }
        if(!check_class(p, (uint32_t)(p->tiles->id))) {
            send_to_self_error(p, p->waitcmd, -ERR_not_class_member, 0);
            return 0;
        }
        classid = (uint32_t)p->tiles->id;
    } else {
        if(check_class(p, p->id)) {
            classid = p->id;
        } else {
            ERROR_RETURN(("buy class da'shu'guai not monitor[%d]", p->id), -1);
        }
    }
    return request_add_class_item(p, classid, itmid, itm_cnt, itm_max, 1);
}

static int
unpkg_set_class_item(uint8_t* body, int len, int *used)
{
    int i, j;
	if (len < 4)
		ERROR_RETURN (("error len=%d", len), -1);

    i = 0;
	UNPKG_UINT32 (body, *used, i);
	j = 0;
	PKG_H_UINT32 (body, *used, j);

	CHECK_BODY_LEN (len, 4 + (*used) * sizeof (itm_info_t));
    for(i = 0; i < *used; i++) {
        itm_info_t *t = (itm_info_t *) (body + 4 + i * sizeof(itm_info_t));
        t->itm_id = ntohl(t->itm_id);
		if (!get_item_prop (t->itm_id))
			ERROR_RETURN (("error item id=%d", t->itm_id), -1);
    }
    return 0;
}

int set_itm_class_cmd(sprite_t* p, uint8_t *body, int bodylen)
{
    DEBUG_LOG("SET CLASS ITEM [%d]", p->id);
    CHECK_CLASS(p, p->id);

	int used;
	if (unpkg_set_class_item (body, bodylen, &used) == -1)
        return -1;
    return send_request_to_db(SVR_PROTO_SET_CLASS_ITEM, p, bodylen, body, p->id);
}

int delete_class_member_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    CHECK_CLASS(p, p->id);
    uint32_t memberid = 0;
    int i = 0;
    UNPKG_UINT32(body, memberid, i);
    *(uint32_t*)p->session = memberid;
    DEBUG_LOG("DELETE CLASS MEMBER [%d, %d]", p->id, memberid);
    return send_request_to_db(SVR_PROTO_DEL_CLASS_MEMBER, p, 4, &memberid, p->id);
}

int delete_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_CLASS(p, p->id);
    DEBUG_LOG("DELETE CLASS [%d]", p->id);

	return send_request_to_db(SVR_PROTO_DEL_CLASS, p, 0, NULL, p->id);
}

int get_usr_class_list_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    return send_request_to_db(SVR_PROTO_CLASS_LIST, p, 0, NULL, p->id);
}

int lock_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_CLASS(p, p->id);
    uint32_t visit_flag = member_visit;
    return send_request_to_db(SVR_PROTO_SET_VISIT_FLAG, p, 4, &visit_flag, p->id);
}

int unlock_class_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_CLASS(p, p->id);
    uint32_t visit_flag = all_visit;
    return send_request_to_db(SVR_PROTO_SET_VISIT_FLAG, p, 4, &visit_flag, p->id);
}

int get_class_unused_itm_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    if(!IS_CLASS_MAP(p->tiles->id)) {
        ERROR_RETURN(("get class unused item not in class [%d]", p->id), -1);
    }
    uint32_t classid = (uint32_t)p->tiles->id;
    CHECK_CLASS(p, classid);
    return send_request_to_db(SVR_PROTO_GET_CLASS_ITM_UNUSED, p, 0, NULL, classid);
}

int get_member_list_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t classid = 0;
    int i = 0;
    UNPKG_UINT32(body, classid, i);
    return send_request_to_db(SVR_PROTO_GET_CLASS_MEMBER_LIST, p, 0, NULL, classid);
}

int del_class_item(sprite_t* p)
{
    uint32_t mesg[2];
    mesg[0] = *(uint32_t*)(p->session + 4);
    item_kind_t* kind = find_kind_of_item(mesg[0]);
    if(kind && kind->kind == CLASS_ITEM_KIND) {

        mesg[1] = *(uint32_t*)(p->session + 8);
        DEBUG_LOG("BUY CLASS ITEM ROLL BACK [%d %d]", p->id, mesg[0]);
        return send_request_to_db(SVR_PROTO_DELETE_CLASS_ITM_UNUSED, NULL, sizeof(mesg), mesg, (uint32_t)p->tiles->id);
    }
    return 0;
}

int clean_class(sprite_t* p, uint32_t classid)
{
    if(p->waitcmd == PROTO_GET_CLASS_QUES_REWARD || p->waitcmd == PROTO_GET_CLASS_QUES_MEMBER_REWARD)
        return 0;
    if(check_class(p, classid)) {
        sprite_del_class(p, classid);
        DEBUG_LOG("CLEAN CLASS [%d %d %d]", p->id, p->waitcmd, classid);
        return send_request_to_db(SVR_PROTO_QUIT_CLASS, NULL, 4, &classid, p->id);
    }
    return 0;
}

int get_top_class_list_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, server_class_num, l);
    DEBUG_LOG("GET TOP CLASS [%d %d %d]", p->id, server_class_num, server_class_reflesh_pos);
    int i;
    for(i = 0; i < server_class_num; i ++) {
        PKG_UINT32(msg, server_class[i].classid, l);
        PKG_UINT32(msg, server_class[i].member_cnt, l);
        PKG_UINT32(msg, server_class[i].interest, l);
        PKG_STR(msg, server_class[i].monitor_nick, l, NICK_LEN);
        PKG_STR(msg, server_class[i].name, l, NICK_LEN);
        //DEBUG_LOG("top class interest %d", server_class[i].interest);
    }
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

int get_class_honor_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tiles || !IS_CLASS_MAP(p->tiles->id)) {
        ERROR_RETURN(("buy class itm not in class [%d]", p->id), -1);
    }
    uint32_t classid = (uint32_t)p->tiles->id;
    return send_request_to_db(SVR_PROTO_GET_CLASS_HONOR, p, 0, NULL, classid);
}

int create_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    p->my_moleclass_id[p->my_moleclass_num] = p->id;
    p->my_moleclass_num++;
    response_proto_head(p, PROTO_CREATE_CLASS, 0);
    uint32_t val = 1;
    create_class_info_t *cls_info = (create_class_info_t*)p->session;
    int i;
    int server_class_exist = 0;
    for(i = 0; i < MAX_TOP_CLASS; i++) {
        if(server_class[i].classid == id) {
            server_class_exist = 1;
        }
    }
    if(!server_class_exist) {

        server_class[server_class_reflesh_pos].classid = id;
        server_class[server_class_reflesh_pos].member_cnt = 1;
        server_class[server_class_reflesh_pos].interest = cls_info->interest;
        memcpy(server_class[server_class_reflesh_pos].monitor_nick, p->nick, NICK_LEN);
        memcpy(server_class[server_class_reflesh_pos].name, cls_info->name, NICK_LEN);
        server_class_reflesh_pos = (server_class_reflesh_pos + 1) % MAX_TOP_CLASS;
        server_class_num = (server_class_num + 1) > MAX_TOP_CLASS ? MAX_TOP_CLASS : (server_class_num + 1);
    }

	msglog(statistic_logfile, 0x0207EEEB, get_now_tv()->tv_sec, &val, 4);
    return 0;
}

int get_class_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    class_detail_info_t* cls_info = (class_detail_info_t*)buf;
    CHECK_BODY_LEN(len, sizeof(class_detail_info_t) + cls_info->member_cnt * sizeof(userid_t) + cls_info->itm_cnt * sizeof(itm_info_t));

    uint32_t* member_id = (uint32_t*)(buf + sizeof(class_detail_info_t));

    int i;
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, cls_info->s_info.interest, l);
    PKG_UINT32(msg, cls_info->s_info.logo_type, l);
    PKG_STR(msg, cls_info->s_info.logo_word, l, LOGO_WORD_LEN);
    PKG_UINT32(msg, cls_info->s_info.logo_color, l);
    PKG_UINT32(msg, cls_info->s_info.join_flag, l);
    PKG_UINT32(msg, cls_info->s_info.visit_flag, l);
    PKG_STR(msg, cls_info->s_info.name, l, NICK_LEN);
    PKG_STR(msg, cls_info->s_info.slogan, l, SLOGAN_LEN);
    PKG_STR(msg, cls_info->monitor_nick, l, NICK_LEN);
    PKG_UINT32(msg, cls_info->member_cnt + 1, l);
    PKG_UINT32(msg, cls_info->itm_cnt, l);
    PKG_UINT32(msg, id, l);

    //buffer this class
    int server_class_exist = 0;
    for(i = 0; i < MAX_TOP_CLASS; i++) {
        if(server_class[i].classid == id) {
            server_class_exist = 1;
        }
    }
    if(!server_class_exist) {

        server_class[server_class_reflesh_pos].classid = id;
        server_class[server_class_reflesh_pos].member_cnt = cls_info->member_cnt;
        server_class[server_class_reflesh_pos].interest = cls_info->s_info.interest;
        memcpy(server_class[server_class_reflesh_pos].monitor_nick, cls_info->monitor_nick, NICK_LEN);
        memcpy(server_class[server_class_reflesh_pos].name, cls_info->s_info.name, NICK_LEN);
        server_class_reflesh_pos = (server_class_reflesh_pos + 1)%MAX_TOP_CLASS;
        server_class_num = (server_class_num + 1) > MAX_TOP_CLASS ? MAX_TOP_CLASS : (server_class_num + 1);
    }


    for(i = 0; i < cls_info->member_cnt; i++) {
        PKG_UINT32(msg, member_id[i], l);
    }
    DEBUG_LOG("CLASS INFO\t[%d itm cnt %d member cnt %d]", p->id, cls_info->itm_cnt,cls_info->member_cnt);
    itm_info_t* itm = (itm_info_t*)(buf + sizeof(class_detail_info_t) + cls_info->member_cnt * sizeof(userid_t));

    for(i = 0; i < cls_info->itm_cnt; i++) {
        PKG_UINT32(msg, itm[i].itm_id, l);
    //    DEBUG_LOG("CLASS %d item[%d] = %d", id, i, itm[i].itm_id);
        PKG_STR(msg, itm[i].itm_content, l, ITEM_INFO_LEN);
    }
    init_proto_head(msg, PROTO_GET_CLASS_DETAIL, l);
    send_to_self(p, msg, l, 1);
    return 0;
}

int get_first_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    response_proto_uint32(p, PROTO_GET_FIRST_CLASS, *(uint32_t*)buf, 0);
    return 0;
}

int get_class_simple_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, sizeof(class_simple_info_t));
    class_simple_info_t *info = (class_simple_info_t*)buf;
    int length = sizeof(protocol_t);
    PKG_UINT32(msg, id, length);
    PKG_UINT32(msg, info->logo_type, length);
    PKG_STR(msg, info->logo_word, length, LOGO_WORD_LEN);
    PKG_UINT32(msg, info->logo_color, length);
    PKG_UINT32(msg, info->join_flag, length);
    PKG_UINT32(msg, info->visit_flag, length);
    PKG_STR(msg, info->name, length, NICK_LEN);
    PKG_STR(msg, info->slogan, length, SLOGAN_LEN);
    PKG_STR(msg, info->monitor_nick, length, NICK_LEN);
    PKG_UINT32(msg, info->member_cnt, length);
    init_proto_head(msg, PROTO_GET_CLASS_SIMPLE, length);
    return send_to_self(p, msg, length, 1);

}

int get_class_logo_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, sizeof(class_logo_info_t));
    class_logo_info_t *sim_info = (class_logo_info_t*)buf;
    switch(p->waitcmd) {
        case PROTO_GET_CLASS_LOGO_INFO:
            {
                int len = sizeof(protocol_t);
                PKG_UINT32(msg, id, len);
                PKG_UINT32(msg, sim_info->logo_type, len);
                PKG_STR(msg, sim_info->logo_word, len, LOGO_WORD_LEN);
                PKG_UINT32(msg, sim_info->logo_color, len);
                PKG_UINT32(msg, sim_info->join_flag, len);
                PKG_UINT32(msg, sim_info->visit_flag, len);
	            init_proto_head(msg, PROTO_GET_CLASS_LOGO_INFO, len);
                return send_to_self(p, msg, len, 1);
            }
            break;
        case PROTO_JOIN_CLASS:
            {
                int result;
                if(sim_info->member_cnt == MAX_CLASS_MEMBER_NUM) {
                    result = 4;
                    response_proto_uint32(p, PROTO_JOIN_CLASS, result, 0);
                    return 0;
                }
                switch(sim_info->join_flag) {
                    case all_join:
                        result = 0;
                        return send_request_to_db(SVR_PROTO_ADD_CLASS_MEMBER, p, 4, &p->id, *(uint32_t*)p->session);
                    case monitor_control:
                        result = 1;
                        break;
                    case nobody_join:
                        result = 2;
                        break;
                    default:
                        ERROR_RETURN(("error join flag[%d %d]", p->id, sim_info->join_flag), -1);
                }
                response_proto_uint32(p, PROTO_JOIN_CLASS, result, 0);
            }
            break;
        default:
            ERROR_RETURN(("error get sim class cmd id[%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int quit_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    switch(p->waitcmd) {
        case PROTO_QUIT_CLASS:
            {
                uint32_t classid = *(uint32_t*)p->session;
                sprite_del_class(p, classid);
            }
            break;
        case PROTO_DEL_CLASS_MEMBER:
            {
                uint8_t buffer[1024];
                int buffer_l = 0;
                sprite_t* tmp = get_sprite(*(uint32_t*)p->session);
                if(tmp) {
                    sprite_del_class(tmp, id);
                } else {
                    PKG_H_UINT32(buffer, (*(uint32_t*)p->session), buffer_l);
                    PKG_H_UINT32(buffer, p->id, buffer_l);
                    send_to_broadcaster(MCAST_mole_class_msg, buffer_l, buffer, 0, 0, PROTO_QUIT_CLASS);
                }
            }
            break;
        default:
            break;

    }
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int change_xiaomee_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN(len, 4);
    p->yxb = *(uint32_t*)buf;

    switch(p->waitcmd) {
        case PROTO_ITEM_BUY:
		case PROTO_PAY_MONEY:
		case PROTO_GET_WORK_SALARY:
			response_proto_uint32(p, p->waitcmd, p->yxb, 0);
            break;
        case PROTO_GET_SMC_SALARY:
        {
            DEBUG_LOG("GET SMC SALARY CALLBACK[uid=%d yxb=%d]", p->id, p->yxb);

			response_proto_str(p, p->waitcmd, p->sess_len, p->session, 0);
            return 0;
        }
		case PROTO_GET_500_XIAOME:
        {
            DEBUG_LOG("GET 500 XIAOME CALLBACK[uid=%d yxb=%d]", p->id, p->yxb);
			uint32_t* itemid = (uint32_t*)p->session;
			int j = sizeof(protocol_t);
			PKG_UINT32(msg, *itemid, j);
			PKG_UINT32(msg, 1, j);
			PKG_UINT32(msg, 50, j); //count
			init_proto_head(msg, p->waitcmd, j);
			return send_to_self(p, msg, j, 1);
        }
		case PROTO_SET_WORK_NUM:
		{
			if (p->engineer) {
				return db_add_work_num(p, *(uint32_t*)p->session, p->id);
			} else {
				response_proto_uint32_uint32(p, p->waitcmd, p->yxb, 0, 0);
				return 0;
			}
		}
		case PROTO_ENGLISH_CAN_GET_XIAOME:
		{
			DEBUG_LOG("GET XIAOME CALLBACK[uid=%d yxb=%d]", p->id, p->yxb);
			response_proto_uint32(p, p->waitcmd, 1, 0);
			return 0;
		}
	case PROTO_PASTURE_GET_NETS:
	{
		return send_request_to_db(SVR_PROTO_PASTURE_GET_NETS, p, 0, NULL, p->id);
	}
	case PROTO_VIP_PATCH_WORK:
	{
		return do_real_send_gift(p);
	}
        default:
            ERROR_RETURN(("error cmd id\t[%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int add_class_itm_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    int mesg[3];
    //price
    mesg[0] = *(uint32_t*)p->session;
    mesg[1] = 0;
    mesg[2] = 0;
    return send_request_to_db(SVR_PROTO_CHANGE_XIAOMEE, p, sizeof(mesg), mesg, p->id);
}

int modify_class_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    response_proto_head(p, PROTO_MODIFY_CLASS, 0);
    return 0;
}

int set_first_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    p->my_moleclass_first = *(uint32_t*)p->session;
    response_proto_head(p, PROTO_SET_FIRST_CLASS, 0);
    return 0;
}

int set_itm_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    response_proto_head(p, PROTO_SET_CLASS_ITEM, 0);
    return 0;
}

int join_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    switch(p->waitcmd) {
        case PROTO_MONITOR_VERIFY:
            {
                uint32_t memberid = *(uint32_t*)p->session;
                int j;
                char txt[1024];
                sprite_t* initiator;
#ifndef TW_VER
                j = sprintf(txt, "%s 同意了你加入他/她的班级", p->nick);
#else
                j = sprintf(txt, "%s 同意了你加入他/她的班級", p->nick);
#endif
                DEBUG_LOG("txt it self len %d", j);
                j = text_notify_pkg(msg + 4, p->waitcmd, p->id, p->id, p->sub_grid, p->nick, 1, j, txt);
                if ((initiator = get_sprite(memberid))) {
                    send_to_self(initiator, msg + 4, j, 0);
                    initiator->my_moleclass_id[initiator->my_moleclass_num] = p->id;
                    initiator->my_moleclass_num++;
                } else {
                    *(uint32_t*)msg = p->id;
                    chat_across_svr(memberid, msg, j + 4);
                    uint8_t buffer[1024];
                    int buffer_l = 0;
                    PKG_H_UINT32(buffer, memberid, buffer_l);
                    PKG_H_UINT32(buffer, p->id, buffer_l);
                    send_to_broadcaster(MCAST_mole_class_msg, buffer_l, buffer, 0, 0, PROTO_JOIN_CLASS);
                }
                response_proto_head(p, p->waitcmd, 0);
            }
            break;
        case PROTO_JOIN_CLASS:
            p->my_moleclass_id[p->my_moleclass_num] = *(uint32_t*)p->session;
            p->my_moleclass_num++;
            DEBUG_LOG("JOIN CLASS SUC [%d %d]", p->id, *(uint32_t*)p->session);
            response_proto_uint32(p, p->waitcmd, 0,0);
            break;
        default:
            ERROR_RETURN(("join class error cmd %d %d", p->id, p->waitcmd), -1);
    }
    return 0;
}

int delete_class_member_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    switch(p->waitcmd) {
        case PROTO_QUIT_CLASS:
            return send_request_to_db(SVR_PROTO_QUIT_CLASS, p, 4, &id, p->id);
        case PROTO_DEL_CLASS_MEMBER:
            return send_request_to_db(SVR_PROTO_QUIT_CLASS, p, 4, &p->id,(*(uint32_t*)p->session));
        default:
            break;
    }
    return 0;
}

int delete_class_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t member_cnt = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + 4 * member_cnt);
    uint32_t *member_id = (uint32_t*)(buf + 4);
    int i = 0;
    int cnt = 0;
	uint8_t  buffer[1024];
    int l = 0;
	PKG_H_UINT32(buffer, id, l);
	l += 4;
    for(i = 0; i < member_cnt; i++) {
        send_request_to_db(SVR_PROTO_QUIT_CLASS, NULL, 4, &id, member_id[i]);
        sprite_t *tmp = get_sprite(member_id[i]);
        if(tmp) {
            sprite_del_class(tmp, id);
            int j = sizeof(protocol_t);
            PKG_UINT32(msg, id, j);
            init_proto_head(msg, p->waitcmd, j);
            send_to_self(tmp, msg, j, 0);
            continue;
        }
		PKG_H_UINT32(buffer, member_id[i], l);
        cnt++;
    }
    //monitor delete
    sprite_del_class(p, p->id);
    send_request_to_db(SVR_PROTO_QUIT_CLASS, NULL, 4, &p->id, p->id);
    if(cnt != 0) {
		*(uint32_t*)(buffer + 4) = cnt;
		send_to_broadcaster(MCAST_mole_class_msg, l, buffer, 0, 0, p->waitcmd);
    }
    response_proto_uint32(p, p->waitcmd, id, 0);
    return 0;
}

int add_class_member_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    switch(p->waitcmd) {
        case PROTO_JOIN_CLASS:
            return send_request_to_db(SVR_PROTO_JOIN_CLASS, p, 4, p->session, p->id);
        case PROTO_MONITOR_VERIFY:
            return send_request_to_db(SVR_PROTO_JOIN_CLASS, p, 4, &p->id, *(uint32_t*)p->session);
        default:
            ERROR_RETURN(("error add class callback\t[%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int set_class_visit_flag_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    response_proto_head(p, p->waitcmd, 0);
    return 0;
}

int get_class_unused_itm_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t itm_cnt = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + itm_cnt * 8);
    int i;
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, itm_cnt, l);
    for(i = 0; i < itm_cnt; i++) {
        PKG_UINT32(msg, *(uint32_t*)(buf + 4 + 8 * i), l);
        PKG_UINT32(msg, *(uint32_t*)(buf + 8 + 8 * i), l);
    }
    init_proto_head(msg, PROTO_GET_CLASS_ITM_UNUSED, l);
    return send_to_self(p, msg, l, 1);
}

int del_class_unused_itm_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    return 0;
}

void del_moleclass_op(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 8);

	uint32_t classid= *(uint32_t*)(body);
	uint32_t count = *(uint32_t*)(body + 4);
    CHECK_BODY_LEN_VOID(len, 8 + count * 4);
    if (count == 0)
		return;
	uint32_t* memberid = (uint32_t*)(body + 8);
	int loop;
	for (loop = 0; loop < count; loop++) {
		sprite_t* p = get_sprite(*memberid);
        if (p) {
            int j = sizeof(protocol_t);
            PKG_UINT32(msg, classid, j);
            init_proto_head(msg, PROTO_DEL_CLASS, j);
            send_to_self(p, msg, j, 0);
            sprite_del_class(p, classid);
            DEBUG_LOG("CLASS MONITOR DELETE CLASS [%d %d]", *memberid, classid);
        }
        memberid++;
	}
}

int get_member_list_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);
    int i = 0;
    uint32_t member_cnt = *(uint32_t*)buf;
    CHECK_BODY_LEN(len, 4 + 4 * member_cnt);
    uint32_t* member_id = (uint32_t*)(buf + 4);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, id, l);
    PKG_UINT32(msg, member_cnt + 1, l);
    PKG_UINT32(msg, id, l);
    for(i = 0; i < member_cnt; i++) {
        PKG_UINT32(msg, member_id[i], l);
    }
    init_proto_head(msg, PROTO_GET_CLASS_MEMBER_LIST, l);
    DEBUG_LOG("get member list [%d %d %d %d]", p->id, member_cnt, len, l);
    send_to_self(p, msg, l, 1);
    return 0;
}

void join_moleclass_op(uint32_t opid, const void* body, int len)
{
    CHECK_BODY_LEN_VOID(len, 8);
    uint32_t uid = *(uint32_t*)(body);
    uint32_t classid = *(uint32_t*)(body + 4);
    sprite_t* p = get_sprite(uid);
    if (p) {
        if(!check_class(p, classid) && p->my_moleclass_num < MAX_CLASS_JOIN) {
            p->my_moleclass_id[p->my_moleclass_num] = classid;
            p->my_moleclass_num++;
            DEBUG_LOG("CLASS MONITOR ADD YOU [%d %d]", uid, classid);
        }
    }
}

void quit_moleclass_op(uint32_t opid, const void* body, int len)
{
    CHECK_BODY_LEN_VOID(len, 8);
    uint32_t uid = *(uint32_t*)(body);
    uint32_t classid = *(uint32_t*)(body + 4);
    sprite_t* p = get_sprite(uid);
    if (p) {
        sprite_del_class(p, classid);
        DEBUG_LOG("CLASS MONITOR DELETE YOU [%d %d]", uid, classid);
    }
}


int moleclass_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);

	communicator_body_t* cbody = buf;

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(op_, cbody->body, len - 12); break

	switch (cbody->opid) {

		DISPATCH_OP(PROTO_DEL_CLASS, del_moleclass_op);
		DISPATCH_OP(PROTO_JOIN_CLASS, join_moleclass_op);
		DISPATCH_OP(PROTO_QUIT_CLASS, quit_moleclass_op);
	default:
		break;
	}
#undef DISPATCH_OP

	return 0;
}

int get_usr_class_list_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 8);
    uint32_t class_first = *(uint32_t*)buf;
    uint32_t class_cnt = *(uint32_t*)(buf + 4);
    uint32_t* class_id = (uint32_t*)(buf + 8);
    CHECK_BODY_LEN(len, 8 + 4 * class_cnt);
    p->my_moleclass_first = class_first;
    p->my_moleclass_num = class_cnt;
    int l = sizeof(protocol_t);
    int i;
    PKG_UINT32(msg, class_first, l);
    PKG_UINT32(msg, class_cnt, l);
    for(i = 0; i < class_cnt; i++) {
        PKG_UINT32(msg, class_id[i], l);
        p->my_moleclass_id[i] = class_id[i];
    }
    init_proto_head(msg, PROTO_GET_CLASS_LIST, l);
    send_to_self(p, msg, l, 1);
    return 0;
}

int set_class_reward_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 0);
    int bonus_level = *(uint32_t*)(p->session);
    request_add_class_item(p, p->id, cup_id[bonus_level], 1, 1, 0);
    request_add_class_item(p, p->id, banner_id[bonus_level], 1, 1, 0);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, 2, l);
    PKG_UINT32(msg, cup_id[bonus_level], l);
    PKG_UINT32(msg, banner_id[bonus_level], l);
    init_proto_head(msg, p->waitcmd, l);
    DEBUG_LOG("SET CLASS REWARD CALLBACK [%d %d]", p->id, bonus_level);
    return send_to_self(p, msg, l, 1);
}

int set_class_member_reward_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 0);
    static int bonus_itemid[4] = {
        160290, 160291, 160292, 160341,
    };
    uint32_t classid = *(uint32_t*)p->session;
    uint32_t bonus_level = *(uint32_t*)(p->session + 4);
    do_buy_item(p, bonus_itemid[bonus_level], 1, 1, 0);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, 1, l);
    PKG_UINT32(msg, bonus_itemid[bonus_level], l);
    init_proto_head(msg, p->waitcmd, l);
    DEBUG_LOG("SET CLASS MEMBER REWARD CALLBACK [%d %d %d %d]", p->id, classid, bonus_level, bonus_itemid[bonus_level]);
    return send_to_self(p, msg, l, 1);
}

int get_class_honor_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN_GE(len, 4);
    uint32_t count = *(uint32_t*)buf;
    DEBUG_LOG("GET CLASS HONOR CALLBACK [%d %d %d]", p->id, id, count);
    CHECK_BODY_LEN(len, 4 + count * sizeof(honor_flag_t));
    honor_flag_t* honor = (honor_flag_t*)(buf + 4);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, id, l);
    PKG_UINT32(msg, count, l);
    int i;
    for(i = 0; i < count; i++) {
        PKG_UINT32(msg, honor[i].type, l);
        PKG_UINT32(msg, honor[i].itemid, l);
        DEBUG_LOG("GET CLASS HONOR CALLBACK [%d %d %d]", p->id, count, honor[i].itemid);
    }
    init_proto_head(msg, p->waitcmd, l);
    return send_to_self(p, msg, l, 1);
}

//tmp:class ques
#define CAN_ANS_CLASS_QUES(p_) !!(p_->class_id_for_ques)

int get_class_ques_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    CHECK_CLASS(p, p->id);
    DEBUG_LOG("GET CLASS QUES REWARD %d", p->id);
    return send_request_to_db(SVR_PROTO_GET_CLASS_QUESTION_INFO, p, 0, NULL, p->id);
}

int get_class_ques_member_reward_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 0);
    if(!p->tiles || !IS_CLASS_MAP(p->tiles->id)) {
        ERROR_RETURN(("buy class itm not in class [%d]", p->id), -1);
    }
    CHECK_CLASS(p, (uint32_t)(p->tiles->id));
    uint32_t classid = (uint32_t)p->tiles->id;
    *(uint32_t*)p->session = classid;
    DEBUG_LOG("GET CLASS QUES MEMBER REWARD [%d %d]", p->id, classid);
    return send_request_to_db(SVR_PROTO_GET_CLASS_QUESTION_INFO, p, 0, NULL, classid);

}

int get_class_ques_stat_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t classid;
    int i = 0;
    UNPKG_UINT32(body, classid, i);
    CHECK_CLASS(p, classid);
    *(uint32_t*)p->session = classid;
    DEBUG_LOG("CLASS QUES STAT REQ [%d %d]", p->id, classid);
    return send_request_to_db(SVR_PROTO_GET_CLASS_QUESTION_STAT, p, 4, &p->id, classid);
}

int get_class_ques_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    if(!CAN_ANS_CLASS_QUES(p)) {
        ERROR_RETURN(("can not answer class ques [%d]", p->id), -1);
    }
    p->class_ques_id_start = rand()%(class_ques_max - 20) + 1 ;
    DEBUG_LOG("GET CLASS QUES[%d %d %d]", p->id, p->class_id_for_ques, p->class_ques_id_start);
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, 10, l);
    int i;
    for(i = 0; i < 10; i++) {
        PKG_STR(msg, class_ques[p->class_ques_id_start + i].content, l, 192);
        PKG_STR(msg, class_ques[p->class_ques_id_start + i].option_a, l, 64);
        PKG_STR(msg, class_ques[p->class_ques_id_start + i].option_b, l, 64);
        PKG_STR(msg, class_ques[p->class_ques_id_start + i].option_c, l, 64);
        PKG_STR(msg, class_ques[p->class_ques_id_start + i].option_d, l, 64);
        //DEBUG_LOG("QUES[%d] :%s", (p->class_ques_id_start + i), class_ques[p->class_ques_id_start + i].content);
    }
    init_proto_head(msg, PROTO_GET_CLASS_QUESTION, l);
    return send_to_self(p, msg, l, 1);
}

int answer_class_ques_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    if(!CAN_ANS_CLASS_QUES(p)) {
        ERROR_RETURN(("can not answer class ques [%d]", p->id), -1);
    }
    CHECK_BODY_LEN(bodylen, 4 * 10);
    int i;
    int cnt = 0;
    int length = 0;
    uint32_t answer[10];
    int result[10];
    for(i = 0; i < 10; i++) {
        UNPKG_UINT32(body, answer[i], length);
    }
    for(i = 0; i < 10; i++) {
        result[i]  = 0;
        if(answer[i] == class_ques[p->class_ques_id_start + i].ans) {
            result[i]  = 1;
            cnt++;
        }
        //DEBUG_LOG("answer[%d] %d : %d %d", i, answer[i], (p->class_ques_id_start + i),class_ques[p->class_ques_id_start + i].ans);
    }
    *(uint32_t*)p->session = cnt;
    DEBUG_LOG("ANSWER CLASS QUES [%d %d %d %d]", p->id, p->class_id_for_ques, p->class_ques_id_start, cnt);
    memcpy(p->session + 4, result, sizeof(result));
    uint32_t dbmsg[2];
    dbmsg[0] = cnt;
    dbmsg[1] = p->id;
    return send_request_to_db(SVR_PROTO_SUBMIT_CLASS_QUESTION, p, sizeof(dbmsg), dbmsg, p->class_id_for_ques);
}


int get_class_ques_info_cmd(sprite_t* p, const uint8_t *body, int bodylen)
{
    CHECK_BODY_LEN(bodylen, 4);
    uint32_t classid;
    int i = 0;
    UNPKG_UINT32(body, classid, i);
    return send_request_to_db(SVR_PROTO_GET_CLASS_QUESTION_INFO, p, 0, NULL, classid);
}



int get_class_ques_stat_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t record_exist = *(uint32_t*)buf;
    if(!record_exist) {
        p->class_id_for_ques = *(uint32_t*)p->session;
        //set class class_ques start id
    } else {
        p->class_id_for_ques = 0;
        p->class_ques_id_start = 0;
    }
    DEBUG_LOG("CLASS QUES STAT CALLBACK[%d %d]", p->id, record_exist);
    response_proto_uint32(p, p->waitcmd, record_exist, 0);
    return 0;
}

int submit_class_ques_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t itm_id = 0;
    uint32_t correct_cnt = *(uint32_t*)p->session;
    uint32_t* result = (uint32_t*)(p->session + 4);
    uint32_t member_score = *(uint32_t*)buf;
    if(correct_cnt >= 6) {
        int present[5] = {180025,180014,180008,180044,180015};
        itm_id = present[rand()%5];
        DEBUG_LOG("CLASS QUES SUBMIT PRESENT [%d %d]", p->id, itm_id);
        do_buy_item(p, itm_id, 1, 1, 0);
    }
    int l = sizeof(protocol_t);
    PKG_UINT32(msg, correct_cnt, l);
    //score = cnt * 1;
    PKG_UINT32(msg, correct_cnt, l);
    PKG_UINT32(msg, member_score, l);
    PKG_UINT32(msg, itm_id, l);
    int i;
    int idx = p->class_ques_id_start;
    DEBUG_LOG("SUBMIT CLASS QUES [%d %d %d]", p->id, id, idx);
    for(i = 0; i < 10; i++) {
        char txt[256];
        int ans_idx = class_ques[idx + i].ans - 1;
        snprintf(txt, sizeof(txt), "%s -- %s", class_ques[idx + i].content, (class_ques[idx + i].option_a + 64 * ans_idx));
        //DEBUG_LOG("PRINT ANSWER [%s]", txt);
        PKG_STR(msg, txt, l, sizeof(txt));
        PKG_UINT32(msg, result[i], l);
    }
    p->class_id_for_ques = 0;
    p->class_ques_id_start = 0;
    init_proto_head(msg, PROTO_ANSWER_CLASS_QUESTION, l);
    return send_to_self(p, msg, l, 1);
}

int get_class_ques_info_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t total_score = *(uint32_t*)buf;

    int bonus_level = -1;
    if(total_score >= 300 && total_score <= 999) {
        bonus_level = 3;
    } else if(total_score >= 1000 && total_score <= 1999) {
        bonus_level = 2;
    } else if (total_score >= 2000 && total_score <= 3999) {
        bonus_level = 1;
    } else if (total_score >= 4000){
        bonus_level = 0;
    }
    DEBUG_LOG("CLASS TOTAL_SCORE [%d %d %d %d]", p->id, id, total_score, bonus_level);

    switch(p->waitcmd) {
        case PROTO_GET_CLASS_QUESTION_INFO:
            {
                int l = sizeof(protocol_t);
                PKG_UINT32(msg, id, l);
                PKG_UINT32(msg, total_score, l);
                DEBUG_LOG("CLASS TOTAL_SCORE [%d %d %d]", p->id, id, total_score);
                init_proto_head(msg, PROTO_GET_CLASS_QUESTION_INFO, l);
                return send_to_self(p, msg, l, 1);
            }
            break;
        case PROTO_GET_CLASS_QUES_MEMBER_REWARD:
            {
                if(bonus_level == -1) {
                    response_proto_uint32(p, p->waitcmd, 0, 0);
                    return 0;
                }
                uint32_t classid = *(uint32_t*)p->session;
                *(uint32_t*)(p->session + 4) = bonus_level;
                typedef struct _class_flag {
                    uint32_t memberid;
                    uint32_t index;
                }class_flag_t;
                class_flag_t class_flag;
                class_flag.memberid = p->id;
                class_flag.index = 0;
                DEBUG_LOG("CLASS MEMBER REWARD [%d %d %d %d %d]", p->id, id, classid, total_score, bonus_level);
                return send_request_to_db(SVR_PROTO_SET_CLASS_MEMBER_REWARD, p, sizeof(class_flag), &class_flag, classid);
            }
            break;
        case PROTO_GET_CLASS_QUES_REWARD:
            {
                if(bonus_level == -1) {
                    response_proto_uint32(p, p->waitcmd, 0, 0);
                    return 0;
                }
                *(uint32_t*)p->session = bonus_level;
                uint32_t pkg[4];
                pkg[0] = 0;
                pkg[1] = cup_id[bonus_level];
                pkg[2] = cup_id[bonus_level];
                pkg[3] = banner_id[bonus_level];
                DEBUG_LOG("CLASS REWARD [%d %d %d]", p->id, pkg[2], pkg[3]);
                return send_request_to_db(SVR_PROTO_GET_CLASS_QUES_REWARD, p, 16, pkg, p->id);
            }
        default:
            ERROR_RETURN(("error [%d %d]", p->id, p->waitcmd), -1);
    }
    return 0;
}

int get_class_ques_reward_callback(sprite_t *p, uint32_t id, char *buf, int len)
{
    CHECK_BODY_LEN(len, 4);
    uint32_t result = *(uint32_t*)buf;
switch (p->waitcmd) {
	case PROTO_GET_CLASS_QUES_REWARD:
    		switch(result) {
        		case 1:
        		case 2:
            		{
		                uint32_t bonus_level= *(uint32_t*)p->session;
		                int l = sizeof(protocol_t);
		                PKG_UINT32(msg, 2, l);
		                PKG_UINT32(msg, cup_id[bonus_level], l);
		                PKG_UINT32(msg, banner_id[bonus_level], l);
		                DEBUG_LOG("GET CLASS QUES REWARD CLLLBACK [%d %d %d]", p->id, result, bonus_level);
		                init_proto_head(msg, p->waitcmd, l);
		                return send_to_self(p, msg, l, 1);

            		}
        		default:
           	 		ERROR_LOG("db error %d %d", p->id, result);
    		}
	//case PROTO_ENGLISH_CLASS:
		//return do_check_give_or_no_ex(p);
}
    return 0;
}

int load_class_questions(const char *file)
{
	int i, err = -1;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile (file);
	if (!doc)
		ERROR_RETURN (("load questions config failed"), -1);

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("xmlDocGetRootElement error");
        err = -1;
		goto exit;
	}

	cur = cur->xmlChildrenNode;
	i = 0;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Question"))){
			DECODE_XML_PROP_UINT32(class_ques[i].id, cur, "ID");
//            DEBUG_LOG("QUESTION %d",class_ques[i].id);
            if(class_ques[i].id != (i+1)) {
                ERROR_LOG("class_ques[%d] ID error %d",i, class_ques[i].id);
                err = -1;
                goto exit;
            }
			DECODE_XML_PROP_STR(class_ques[i].content, cur, "Content");
			DECODE_XML_PROP_STR(class_ques[i].option_a, cur, "OptionA");
			DECODE_XML_PROP_STR(class_ques[i].option_b, cur, "OptionB");
			DECODE_XML_PROP_STR(class_ques[i].option_c, cur, "OptionC");
			DECODE_XML_PROP_STR(class_ques[i].option_d, cur, "OptionD");
            //DEBUG_LOG("ID[%d]:Content[%s], A[%s],B[%s],C[%s],D[%s]",class_ques[i].id,class_ques[i].content, class_ques[i].option_a,class_ques[i].option_b,class_ques[i].option_c,class_ques[i].option_d);
			DECODE_XML_PROP_UINT32(class_ques[i].ans, cur, "Answer");
            if(class_ques[i].ans > 4 || class_ques[i].ans < 1) {
                ERROR_LOG("class_ques[%d] answer error %d",i, class_ques[i].ans);
                err = -1;
                goto exit;
            }
			++i;
		}
		cur = cur->next;
	}
    class_ques_max = i;
    DEBUG_LOG("CLASS QUESTION TOTAL NUM %d", class_ques_max);
	err = 0;
exit:
	xmlFreeDoc (doc);
	return err;
}
