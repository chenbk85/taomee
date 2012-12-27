
#include "proto.h"
#include "exclu_things.h"
#include "communicator.h"
#include "central_online.h"
#include "mole_group.h"

typedef struct  in_molegrp{
    uint32_t    groupid;
    uint32_t    flag;
}__attribute__((packed)) in_molegrp_t;

typedef struct  molegroup_list{
    uint32_t    	count;
    in_molegrp_t    group_list[];
}__attribute__((packed)) molegroup_list_t;


typedef struct  molegroup_memeber_without_id{
    userid_t    groupownerid;
    char        groupname[MAX_GROUP_NAME_LEN];
    uint32_t    type;
    char        announcement[MAX_GROUP_ANNOUNCEMENT_LEN];
    molegroup_member_t mbr_list;
}__attribute__((packed)) molegroup_memeber_without_id_t;

typedef struct  molegroup_simply_info{
    char        groupname[MAX_GROUP_NAME_LEN];
    char        announcement[MAX_GROUP_ANNOUNCEMENT_LEN];
}__attribute__((packed)) molegroup_simply_info_t;

typedef struct  create_molegroup{
    userid_t    groupownerid;
    char        groupname[MAX_GROUP_NAME_LEN];
    uint32_t    type;
    char        announcement[MAX_GROUP_ANNOUNCEMENT_LEN];
}__attribute__((packed)) create_molegroup_t;

typedef struct  del_molegroup{
    userid_t    groupownerid;
}__attribute__((packed))del_molegroup_t;


typedef struct  molegroup_add_member{
    userid_t    groupownerid;
    userid_t    memberid;
}__attribute__((packed))molegroup_add_member_t;

typedef struct  add_molegroup_to_user_table{
    userid_t    groupid;
    uint32_t    is_add_group;
}__attribute__((packed))add_molegroup_to_user_table_t;

typedef struct  check_friend{
    userid_t    friendid;
}__attribute__((packed))check_friend_t;

typedef struct  friend_existed{
    uint32_t  is_existed;
}__attribute__((packed))friend_existed_t;

#define db_apply_molegroup_id(p_) \
		send_request_to_db(SVR_PROTO_APPLY_GROUPID, (p_), 0, NULL, 0)

#define db_create_molegroup(p_, buf, groupid) \
	    send_request_to_db(SVR_PROTO_CREATE_GROUP, (p_), sizeof(create_molegroup_t), (buf), groupid)

#define db_del_molegroup(p_, buf, groupid) \
		send_request_to_db(SVR_PROTO_DEL_GROUP, (p_), sizeof(del_molegroup_t), (buf), groupid)

#define db_add_molegroup_to_userinfo(p_, buf, id) \
	    send_request_to_db(SVR_PROTO_ADD_GROUP_TO_USRINFO, (p_), sizeof(add_molegroup_to_user_table_t), (buf), id)

#define db_add_member_to_molegroup(p_, buf, groupid) \
	    send_request_to_db(SVR_PROTO_ADD_MEMBER_TO_GROUP, (p_), sizeof(molegroup_add_member_t), (buf), groupid)

#define db_get_owner_by_groupid(p_, groupid) \
	    send_request_to_db(SVR_PROTO_GET_OWNER_BY_GROUPID, (p_), 0, NULL, groupid)

#define db_mod_molegroup_info(p_, buf, id) \
	    send_request_to_db(SVR_PROTO_MOD_GROUP_INFO, (p_), sizeof(molegroup_simply_info_t), (buf), id)

#define db_get_molegroup_detail(p_, id) \
		send_request_to_db(SVR_PROTO_GET_GROUP_DETAIL, (p_), 0, NULL, id)

#define db_get_user_molegroup_list(p_, id) \
		send_request_to_db(SVR_PROTO_GET_GROUP_LIST, (p_), 0, NULL, id)

#define db_set_molegroup_flag(p_, buf) \
			send_request_to_db(SVR_PROTO_SET_GROUP_FLAG, (p_), 8, buf, (p_)->id)

/*
*@func:  search for mole group in p->my_molegroup_id[]  by group id
*@ret:	-1, fail; otherwise, successfully
*/
int get_molegroup_index(sprite_t* p, uint32_t groupid)
{
	int loop;
	for(loop = 0; loop < p->my_molegroup_num; loop++){
		DEBUG_LOG("MY GROUP\t[uid:%u groupindex %u groupid: %u]", p->id, loop, p->my_molegroup_id[loop]);
		if(groupid == p->my_molegroup_id[loop])
			return loop;
	}
	return -1;
}

/*
*@func:  create mole group(handle command from client)
*@ret:	-1, fail; 0, successfully
*/
int create_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN (bodylen, MAX_GROUP_NAME_LEN + 4 + MAX_GROUP_ANNOUNCEMENT_LEN);

	if (p->my_molegroup_num >= 3){
		ERROR_LOG("already have created %u mole group", MAX_OWNED_MOLEGROUP_NUM);
		return send_to_self_error(p, p->waitcmd, -ERR_max_owned_molegroup_exceeded, 1);
	}
	int j = 0;

	DEBUG_LOG("CREATE GROUP\t[uid=%u]", p->id);
	/* construct  create_molegroup_t*/
	*(uint32_t*)p->session = p->id;
	p->sess_len = 4;
	UNPKG_STR(body, p->session + p->sess_len, j, MAX_GROUP_NAME_LEN); //group name
	p->sess_len += MAX_GROUP_NAME_LEN;
	*(uint32_t*)(p->session + p->sess_len - 1) = '\0';
	CHECK_DIRTYWORD(p, p->session + p->sess_len - MAX_GROUP_NAME_LEN);

	UNPKG_UINT32(body, *(uint32_t*)(p->session + p->sess_len), j);  // group type
	p->sess_len += 4;
	UNPKG_STR(body, p->session + p->sess_len, j, MAX_GROUP_ANNOUNCEMENT_LEN);
	p->sess_len += MAX_GROUP_ANNOUNCEMENT_LEN;
	*(uint32_t*)(p->session + p->sess_len - 1) = '\0';
	CHECK_DIRTYWORD(p, p->session + p->sess_len - MAX_GROUP_ANNOUNCEMENT_LEN);

	return db_apply_molegroup_id(p);
}

/*
*@func:  apply for a group id  call back from DB
*@ret:	must be successful
*/
int apply_molegroup_id_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN (len, 4);

	create_molegroup_t* molegrp = (create_molegroup_t*)p->session;

	DEBUG_LOG("APPLY A GROUP ID\t[uid=%u groupid=%u]", p->id, *(uint32_t*)buf);
	/*add create_molegroup_t  to DB, to create molegroup*/
	return db_create_molegroup(p, molegrp, *(uint32_t*)buf);
}

/*
*@func:  create mole group call back
*@ret:	-1, fail; 0, succ
*/
int create_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0

	add_molegroup_to_user_table_t tmp;
	tmp.groupid = id;
	tmp.is_add_group = 1;

	*(uint32_t*)p->session = id;
	p->sess_len = 4;

	p->my_molegroup_id[p->my_molegroup_num] = id;
	p->my_molegroup_num++;

	DEBUG_LOG("CREATE GROUP SUCC\t[uid=%u groupid=%u num=%u]", p->id, id, p->my_molegroup_num);
	/*add groupid to userinfo table, so we know all group IDs that one have joinned */
	return db_add_molegroup_to_userinfo(p, &tmp, p->id);
}

/*
*@func:  add mole group to userinfo table call back
*@ret:	-1, fail; 0, successfully
*/
int add_molegroup_to_userinfo_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0
	switch (p->waitcmd) {
	case PROTO_CREATE_MOLEGROUP:
	{
		// add molegroup to userinfo for group owner successfully
		DEBUG_LOG("OWNER ADD GROUP TO USERINFO SUCC\t[uid=%u groupid=%u]", p->id, *(uint32_t*)p->session);
		response_proto_uint32(p, p->waitcmd, *(uint32_t*)p->session, 0);
		return 0;
	}
	case PROTO_INVITE_JOIN_MOLEGROUP:
	{
		// add molegroup to userinfo for member successfully
		uint32_t groupid = *(uint32_t*)p->session;
		molegroup_member_t* molegrp_list = (molegroup_member_t*)(p->session + 4);

		// need to broadcast all member
		uint8_t buffer[MAX_NUM_OF_GROUP_MEMBER * 4 + 8];
		int i = 0;
		PKG_H_UINT32(buffer, groupid, i);
		PKG_H_UINT32(buffer, id, i);
		i += 4; //reserved for counter

		int j = sizeof(protocol_t);
		PKG_UINT32(msg, groupid, j);
		PKG_UINT32(msg, id, j);
		init_proto_head(msg, p->waitcmd, j);

		int loop, counter = 0;
		for (loop = 0; loop < molegrp_list->member_count; loop++){
			/* send to self later*/
			if(p->id == molegrp_list->member_list[loop]){
				continue;
			}
			/* send to other member */
			sprite_t* ptmp = get_sprite(molegrp_list->member_list[loop]);
			if (ptmp){
				send_to_self(ptmp, msg, j, 0);
				continue;
			}
			counter++;
			PKG_H_UINT32(buffer, molegrp_list->member_list[loop], i);
		}

		*(uint32_t*)(buffer + 8) = counter;
		send_to_broadcaster(MCAST_mole_grp_msg, i, buffer, 0, 0, p->waitcmd);
		DEBUG_LOG("ADD GROUP TO USERINFO SUCC\t[inviter=%u groupid=%u invitee=%u]", p->id, groupid, id);
		return send_to_self(p, msg, j, 1);

	}
	default:
		ERROR_RETURN(("cmd id %u can not call db_add_molegroup_to_userinfo", p->waitcmd), -1);
	}
}

int add_member_to_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	int loop;
	uint32_t groupid;
	userid_t uid;
	UNPKG_UINT32(body, groupid, j);
	UNPKG_UINT32(body, uid, j);

	CHECK_VALID_MOLEGROUP_ID(p, groupid);
	CHECK_VALID_ID(uid);

	*(uint32_t*)p->session = uid;
	p->sess_len = 4;

	DEBUG_LOG("ADD MBR TO GROUP\t[inviter=%u groupid=%u invitee=%u]", p->id, groupid, uid);
	for (loop = 0; loop < p->my_molegroup_num; loop++) {
		if (p->my_molegroup_id[loop] == groupid) {
			molegroup_add_member_t tmp;
			tmp.groupownerid = p->id;
			tmp.memberid = uid;
			return db_add_member_to_molegroup(p, &tmp, groupid);
		}
	}

	ERROR_LOG("%u add %u to group %u, groupid error", p->id, uid, groupid);
	return send_to_self_error(p, p->waitcmd, -ERR_mole_groupid_not_existed, 1);
}

int add_member_to_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	/*return molegroup_member_t to handle*/
	userid_t uid = *(userid_t*)p->session;
	molegroup_member_t* molegrp_mbr_list = (molegroup_member_t*)buf;
	p->sess_len = molegrp_mbr_list->member_count * 4 + 8;

	CHECK_BODY_LEN(len, p->sess_len - 4);

	memcpy(p->session + 4, buf, p->sess_len);
	*(userid_t*)p->session = id;

	add_molegroup_to_user_table_t tmp;
	tmp.groupid = id;
	tmp.is_add_group = 0;

	DEBUG_LOG("ADD MBR TO GROUP CALLBACK\t[inviter=%u groupid=%u invitee=%u]", p->id, id, uid);
	return db_add_molegroup_to_userinfo(p, &tmp, uid);
}

int apply_for_joining_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN_GE(bodylen, 8);

	int j = 0;
	uint32_t groupid, msglen;
	UNPKG_UINT32(body, groupid, j); // save the group id
	UNPKG_UINT32(body, msglen, j);  // save the msg length
	CHECK_BODY_LEN(bodylen, 8 + msglen);

	*(uint32_t*)p->session = groupid;
	UNPKG_STR(body, p->session + 4, j, msglen);
	*(p->session + 3 + msglen) = '\0';
	CHECK_DIRTYWORD(p, p->session + 4);
	p->sess_len = 4 + msglen;

	DEBUG_LOG("APPLY FOR JOINING\t[inviter=%u groupid=%u]", p->id, groupid);
	return db_get_user_molegroup_list(p, p->id);
}

void req_molegroup_notifier(sprite_t* p, sprite_t* owner, uint32_t groupid, uint32_t ownerid, char* req_msg)
{
	char txt[256];

	int j = snprintf(txt, sizeof(txt), "%s", req_msg);

	j = text_notify_pkg(msg + 4, p->waitcmd, p->id, groupid, p->sub_grid, p->nick, 0, j, txt);
	if (owner) {
		send_to_self(owner, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = p->id;
		chat_across_svr(ownerid, msg, j + 4);
	}
}

void rsp_molegroup_notifier(uint32_t id, sprite_t* rsp, uint32_t groupid, uint8_t accept)
{
	int j;
	char txt[256];
	sprite_t* initiator;

	if (accept) {
		j = sprintf(txt, "%s accept you", rsp->nick);
	} else {
		j = sprintf(txt, "%s deny you", rsp->nick);
	}

	j = text_notify_pkg(msg + 4, rsp->waitcmd, rsp->id, groupid, rsp->sub_grid, rsp->nick, accept, j, txt);
	if ((initiator = get_sprite(id))) {
		send_to_self(initiator, msg + 4, j, 0);
	} else {
		*(uint32_t*)msg = rsp->id;
		chat_across_svr(id, msg, j + 4);
	}

}

static inline int
do_apply_for_molegroup_req(sprite_t* p, userid_t ownerid, uint32_t groupid)
{
	sprite_t* owner = get_sprite(ownerid);
	char* req_msg = (char*)p->session + 4;
	req_molegroup_notifier(p, owner, groupid, ownerid, req_msg);

	response_proto_head(p, p->waitcmd, 0);
	DEBUG_LOG("APPLY FOR JOINING MOLE GROUP\t[%u %u %u]", p->id, ownerid, groupid);
	return 0;
}

int db_get_owner_by_groupid_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	userid_t ownerid = *(uint32_t*)buf;

	return do_apply_for_molegroup_req(p, ownerid, id);
}

int reply_to_join_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 9);
	CHECK_VALID_ID(p->id);

	uint32_t initiator;
	uint32_t groupid;
	uint8_t accept;
	int j = 0;

	UNPKG_UINT8(body, accept, j);
	UNPKG_UINT32(body, initiator, j);
	UNPKG_UINT32(body, groupid, j);

	CHECK_VALID_ID(initiator);

	DEBUG_LOG("REPLY JOIN MOLE GROUP\t[owner=%u initiator=%u accept=%d groupid=%u]", p->id, initiator, accept, groupid);
	rsp_molegroup_notifier(initiator, p, groupid, accept);
	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int leave_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN (bodylen, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	molegroup_del_member_t tmp;
	tmp.groupownerid = 0; // leave freely, so groupownerid = 0
	tmp.memberid = p->id;

 	uint32_t groupid;
	UNPKG_UINT32(body, groupid, j);

	DEBUG_LOG("LEAVE GROUP\t[uid=%u groupid=%u]", p->id, groupid);
	return db_del_member_from_molegroup(p, &tmp, groupid);
}

int del_member_from_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 4);

	uint32_t count = *(uint32_t*)buf;
	userid_t uid;
	if (p->waitcmd == PROTO_KICK_OUT_OF_MOLEGROUP)
		uid = *(uint32_t*)p->session;
	else
		uid = p->id;

	p->sess_len = 4 + count * 4;

	CHECK_BODY_LEN(len, p->sess_len);

	*(uint32_t*)p->session = id;
	memcpy(p->session + 4, buf, p->sess_len);
	p->sess_len += 4;

	del_molegroup_from_user_table_t tmp;
	tmp.groupid = id;

	DEBUG_LOG("RM MBR FROM GROUP SUCC\t[uid=%u groupid=%u]", p->id, id);
	return db_del_molegroup_from_userinfo(p, &tmp, uid);
}

int del_molegroup_from_userinfo_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0

	switch	(p->waitcmd){
	case PROTO_LEAVE_MOLEGROUP:
	case PROTO_KICK_OUT_OF_MOLEGROUP:
	{

		uint32_t groupid = *(uint32_t*)p->session;
		molegroup_member_t* molegrp_list = (molegroup_member_t*)(p->session + 4);

		DEBUG_LOG("RM GRP FROM USERINFO SUCC\t[inviter=%u invitee=%u grpid=%u]", p->id, id, groupid);

		// need to broadcast all member
		uint8_t buffer[MAX_NUM_OF_GROUP_MEMBER * 4 + 8];
		int i = 0;
		PKG_H_UINT32(buffer, groupid, i);
		PKG_H_UINT32(buffer, id, i);
		i += 4; //reserved for counter

		int j = sizeof(protocol_t);
		PKG_UINT32(msg, groupid, j);
		PKG_UINT32(msg, id, j);
		init_proto_head(msg, p->waitcmd, j);

		/* the kicked user must be notified*/
		int counter = 0;
		sprite_t* sp = get_sprite(id);
		if (sp && id != p->id)
			send_to_self(sp, msg, j, 0);
		else{
			counter++;
			PKG_H_UINT32(buffer, id, i);
		}

		int loop;
		for (loop = 0; loop < molegrp_list->member_count; loop++){
			/* send self later*/
			if(p->id == molegrp_list->member_list[loop]){
				continue;
			}
			/* send to other member */
			sprite_t* ptmp = get_sprite(molegrp_list->member_list[loop]);
			if (ptmp){
				send_to_self(ptmp, msg, j, 0);
				continue;
			}
			counter++;
			PKG_H_UINT32(buffer, molegrp_list->member_list[loop], i);
		}

		*(uint32_t*)(buffer + 8) = counter;
		send_to_broadcaster(MCAST_mole_grp_msg, i, buffer, 0, 0, p->waitcmd);

		/*notify self*/
		return send_to_self(p, msg, j, 1);
	}
	default:
			ERROR_RETURN(("cmd id %u can not call db_del_molegroup_from_userinfo", p->waitcmd), -1);
	}
	return 0;
}

int kick_outof_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);

	int j = 0;
	uint32_t groupid;
	molegroup_del_member_t tmp;

	tmp.groupownerid = p->id; // be kicked out, so groupownerid != 0
	UNPKG_UINT32(body, tmp.memberid, j);
	UNPKG_UINT32(body, groupid, j);
	if (tmp.memberid == p->id)
		ERROR_RETURN(("you can't kick out of yourself uid = %u", p->id), -1);
	if (get_molegroup_index(p, groupid) == -1)
		ERROR_RETURN(("group id error groupid = %u uid = %u", groupid, p->id), -1);

	*(uint32_t*)p->session = tmp.memberid;

	CHECK_VALID_ID(tmp.memberid);

	return db_del_member_from_molegroup(p, &tmp, groupid);
}

int delete_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t groupid;
	UNPKG_UINT32(body, groupid, j);

	CHECK_VALID_MOLEGROUP_ID(p, groupid);

	del_molegroup_t tmp;
	tmp.groupownerid = p->id;
	return db_del_molegroup(p, &tmp, groupid);
}

int del_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 8);

	uint8_t  buffer[256];
	uint32_t count = *(uint32_t*)buf;

	int index = get_molegroup_index(p, id);
	if (index == -1)
		ERROR_RETURN(("bad group id[uid=%u groupid=%u]", p->id, id), -1);
	p->my_molegroup_num--;
	p->my_molegroup_id[index] = p->my_molegroup_id[p->my_molegroup_num];

	DEBUG_LOG("DEL MOLEGROUP SUCC\t[uid=%u groupid=%u]", p->id, id);

	CHECK_BODY_LEN(len, 4 + count * 4);

	del_molegroup_from_user_table_t tmp;
	tmp.groupid = id;
	int i = 0;
	PKG_H_UINT32(buffer, id, i);
	i += 4;
	uint32_t* puid = (uint32_t*)(buf + 4);
	int loop;
	uint32_t cnt = 0;
	for (loop = 0; loop < count; loop++, puid++){
		DEBUG_LOG("DEL GRP FROM USERINFO[uid=%u groupid=%u]", *puid, id);
		db_del_molegroup_from_userinfo(NULL, &tmp, *puid);

		/* send self later*/
		if(p->id == *puid){
			continue;
		}
		/* send to other member on the same online */
		sprite_t* ptmp = get_sprite(*puid);
		if (ptmp){
			//response_proto_uint32(ptmp, p->waitcmd, id, 0);
			response_uint32_not_complete(ptmp, p->waitcmd, id, 0);
			continue;
		}

		PKG_H_UINT32(buffer, *puid, i);
		cnt++;
	}

	if (cnt != 0) {
		*(uint32_t*)(buffer + 4) = cnt;
		send_to_broadcaster(MCAST_mole_grp_msg, i, buffer, 0, 0, p->waitcmd);
	}
	response_proto_uint32(p, p->waitcmd, id, 0);
	return 0;
}

int mod_molegroup_info_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, MAX_GROUP_NAME_LEN + 4 + MAX_GROUP_ANNOUNCEMENT_LEN);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t groupid;
	molegroup_simply_info_t tmp;

	UNPKG_UINT32(body, groupid, j);
	UNPKG_STR(body, tmp.groupname, j, MAX_GROUP_NAME_LEN);
	tmp.groupname[MAX_GROUP_NAME_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, tmp.groupname);
	//DEBUG_LOG("groupname:%s", tmp.groupname);

	UNPKG_STR(body, tmp.announcement, j, MAX_GROUP_ANNOUNCEMENT_LEN);
	tmp.announcement[MAX_GROUP_ANNOUNCEMENT_LEN - 1] = '\0';
	CHECK_DIRTYWORD(p, tmp.announcement);
	//DEBUG_LOG("announcement:%s", tmp.announcement);

	return db_mod_molegroup_info(p, &tmp, groupid);
}

int mod_molegroup_info_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len = 0

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

int get_user_molegroup_list_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	// bodylen = 0
	CHECK_VALID_ID(p->id);

	DEBUG_LOG("GET MOLE GROUP LIST\t[%u]", p->id);
	return db_get_user_molegroup_list(p, p->id);
}

int get_user_molegroup_list_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(molegroup_list_t));

	molegroup_list_t* molegrp_list = (molegroup_list_t*)buf;
	CHECK_BODY_LEN(len, sizeof(molegroup_list_t) + sizeof(in_molegrp_t) * molegrp_list->count);

	switch 	(p->waitcmd){
	case 	PROTO_GET_USER_MOLEGROUP_LIST:
	{
		int j = sizeof(protocol_t);
		PKG_UINT32(msg, molegrp_list->count, j);

		DEBUG_LOG("GET GROUP LIST\t[%u %u]", p->id, molegrp_list->count);
		int loop;
		for (loop = 0; loop < molegrp_list->count; loop++){
			PKG_UINT32(msg, molegrp_list->group_list[loop].groupid, j);
			PKG_UINT32(msg, molegrp_list->group_list[loop].flag, j);
			DEBUG_LOG("GROUP ID\t[%u %u %u]", p->id, molegrp_list->group_list[loop].groupid, molegrp_list->group_list[loop].flag);
		}

		init_proto_head(msg, p->waitcmd, j);
		return send_to_self(p, msg, j, 1);
	}
	case	PROTO_APPLY_FOR_JOINING_MOLEGROUP:
	{
		int groupid = *(uint32_t*)p->session;
		int loop;
		for (loop = 0; loop < molegrp_list->count; loop++){
			if (molegrp_list->group_list[loop].groupid == groupid)
				return send_to_self_error(p, p->waitcmd, -ERR_user_already_in_molegroup, 1);
		}
		return db_get_owner_by_groupid(p, groupid);
	}
	default:
		ERROR_RETURN(("bad cmd %u", p->waitcmd), -1);
	}
}

int get_molegroup_detail_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 4);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t groupid;
	UNPKG_UINT32(body, groupid, j);

	CHECK_VALID_MOLEGROUP_ID(p, groupid);
	return db_get_molegroup_detail(p, groupid);
}

int get_molegroup_detail_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN_GE(len, sizeof(molegroup_memeber_without_id_t));

	molegroup_memeber_without_id_t* detail = (molegroup_memeber_without_id_t*)buf;
	CHECK_BODY_LEN(len, MAX_GROUP_NAME_LEN + 12 + MAX_GROUP_ANNOUNCEMENT_LEN + 4 * detail->mbr_list.member_count);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, id, j);
	PKG_UINT32(msg, detail->groupownerid, j);
	PKG_STR(msg, detail->groupname, j, MAX_GROUP_NAME_LEN);
	PKG_UINT32(msg, detail->type, j);
	PKG_STR(msg, detail->announcement, j, MAX_GROUP_ANNOUNCEMENT_LEN);
	PKG_UINT32(msg, detail->mbr_list.member_count, j);

	int loop;
	for (loop = 0; loop < detail->mbr_list.member_count; loop++)
		PKG_UINT32(msg, detail->mbr_list.member_list[loop], j);

	init_proto_head(msg, p->waitcmd, j);
	return send_to_self(p, msg, j, 1);
}

void join_leave_molegroup_op(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 12);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)body, j);  //groupid
	PKG_UINT32(msg, *(uint32_t*)(body + 4), j);  // uid
	init_proto_head(msg, opid, j);

	molegroup_member_t* mbr_list = (molegroup_member_t*)(body + 8);
	int loop;
	for (loop = 0; loop < mbr_list->member_count; loop++) {
		sprite_t* p = get_sprite( mbr_list->member_list[loop]);
		if (p)
			send_to_self(p, msg, j, 0);
	}
}

int chat_in_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN_GE(bodylen, 12);
	CHECK_VALID_ID(p->id);

	int j = 0;
	int i = 0;
	uint32_t groupid, count, msglen, userid;
	uint8_t  buf[PAGESIZE];
	UNPKG_UINT32(body, groupid, j);
	UNPKG_UINT32(body, msglen, j);

	int k = sizeof(protocol_t);

	PKG_H_UINT32(buf, p->id, i);
	PKG_H_UINT32(buf, groupid, i);
	PKG_H_UINT32(buf, msglen, i);

	PKG_UINT32(msg, p->id, k);   //userid
	PKG_UINT32(msg, groupid, k);  //groupid
	PKG_UINT32(msg, msglen, k);  //msglen

	if (msglen > sizeof(buf) - 12) {
		ERROR_RETURN(("msg too long\t[%u]", p->id), -1);
	}
	UNPKG_STR(body, buf + i, j, msglen);
	//DEBUG_LOG("%u SAY:%s msglen=%u", p->id, buf + i, msglen);
	i += msglen;
	*(buf + i - 1) = '\0';

	//DEBUG_LOG("%u SAY:%s", p->id, buf + i - msglen);
	send_chatlog(SVR_PROTO_CHATLOG, p, 2, buf + i - msglen, msglen);
	CHECK_DIRTYWORD(p, buf + i - msglen);

	PKG_STR(msg, buf + i - msglen, k, msglen);  // chat message
	init_proto_head(msg, p->waitcmd, k);

	CHECK_BODY_LEN_GE(bodylen, 12 + msglen);
	UNPKG_UINT32(body, count, j);

	CHECK_BODY_LEN(bodylen, 12 + msglen + count * 4);

	int loop, cnt = 0;
	i += 4;  //reserved for counter
	for (loop = 0; loop < count; loop++) {
		UNPKG_UINT32(body, userid, j);
		sprite_t* tmp = get_sprite(userid);
		if (tmp) {
			if (tmp != p) {
				send_to_self(tmp, msg, k, 0);
				//DEBUG_LOG("send chat message to \t[uid=%u]", p->id);
			}
		} else {
			PKG_H_UINT32(buf, userid, i);
			cnt++;
		}
	}

	if (cnt != 0){
		*(uint32_t*)(buf + 12 + msglen) = cnt;
		send_to_broadcaster(MCAST_mole_grp_msg, i, buf, 0, 0, p->waitcmd);
	}

	return send_to_self(p, msg, k, 1);
}

/*
*@ if you don't like the messages in chatroom, can use this command to hide
*/
int set_molegroup_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen)
{
	CHECK_BODY_LEN(bodylen, 8);
	CHECK_VALID_ID(p->id);

	int j = 0;
	uint32_t groupid, flag;
	UNPKG_UINT32(body, groupid, j);
	UNPKG_UINT32(body, flag, j);

	if (flag != 0 && flag != 1)
		ERROR_RETURN(("bad flag\t[uid=%u flag=%u]", p->id, flag), -1);
	int i = 0;
	uint8_t buf[8];
	PKG_H_UINT32(buf, groupid, i);
	PKG_H_UINT32(buf, flag, i);

	CHECK_VALID_MOLEGROUP_ID(p, groupid);
	return db_set_molegroup_flag(p, &buf);
}

int set_molegroup_flag_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	// len == 0

	response_proto_head(p, p->waitcmd, 0);
	return 0;
}

void chat_in_molegroup_op(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 20);

	int j = sizeof(protocol_t);
	PKG_UINT32(msg, *(uint32_t*)body, j);   	//userid
	PKG_UINT32(msg, *(uint32_t*)(body + 4), j); //groupid

	uint32_t msglen = *(uint32_t*)(body + 8);
	PKG_UINT32(msg, msglen, j);  //msglen
	CHECK_BODY_LEN_GE_VOID(len, 20 + msglen);
	PKG_STR(msg, body + 12, j, msglen);  // chat message

	init_proto_head(msg, opid, j);

	uint32_t count = *(uint32_t*)(body + 12 + msglen);
	uint32_t* puserid = (uint32_t*)(body + 16 + msglen);
	int loop;
	for (loop = 0; loop < count; loop++) {
		sprite_t* p = get_sprite(*puserid);
		if (p){
			send_to_self(p, msg, j, 0);
		}
		puserid++;
	}
}

void del_molegroup_op(uint32_t opid, const void* body, int len)
{
	CHECK_BODY_LEN_GE_VOID(len, 12);

	uint32_t groupid = *(uint32_t*)(body);
	uint32_t count = *(uint32_t*)(body + 4);
    if (count == 0)
		return;
	uint32_t* puserid = (uint32_t*)(body + 8);
	int loop;
	for (loop = 0; loop < count; loop++) {
		sprite_t* p = get_sprite(*puserid);
		if (p)
			response_uint32_not_complete(p, opid, groupid, 0);
		puserid++;
	}
}

int molegroup_op(uint32_t uid, void* buf, int len)
{
	CHECK_BODY_LEN_GE(len, 12);

	communicator_body_t* cbody = buf;

#define DISPATCH_OP(op_, func_) \
		case (op_): (func_)(op_, cbody->body, len - 12); break

	switch (cbody->opid) {

		DISPATCH_OP(PROTO_INVITE_JOIN_MOLEGROUP, join_leave_molegroup_op);
		DISPATCH_OP(PROTO_KICK_OUT_OF_MOLEGROUP, join_leave_molegroup_op);
		DISPATCH_OP(PROTO_LEAVE_MOLEGROUP, join_leave_molegroup_op);
		DISPATCH_OP(PROTO_CHAT_IN_MOLEGROUP, chat_in_molegroup_op);
		DISPATCH_OP(PROTO_DEL_MOLEGROUP, del_molegroup_op);
	default:
		break;
	}
#undef DISPATCH_OP

	return 0;
}

