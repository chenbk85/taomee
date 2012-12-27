
#ifndef MOLE_GROUP_H
#define MOLE_GROUP_H

#include "benchapi.h"

#define SVR_PROTO_GET_GROUP_LIST	     0x10C0
#define SVR_PROTO_ADD_GROUP_TO_USRINFO   0x11C1
#define SVR_PROTO_RM_GROUP_FROM_USRINFO  0x11C2
#define SVR_PROTO_SET_GROUP_FLAG		 0x11C5
#define SVR_PROTO_APPLY_GROUPID        	 0x3501
#define SVR_PROTO_CREATE_GROUP           0xD501
#define SVR_PROTO_ADD_MEMBER_TO_GROUP    0xD502
#define SVR_PROTO_DEL_MEMBER_FROM_GROUP  0xD503
#define SVR_PROTO_MOD_GROUP_INFO         0xD504
#define SVR_PROTO_GET_GROUP_DETAIL       0xD405
#define SVR_PROTO_DEL_GROUP              0xD506
#define SVR_PROTO_GET_OWNER_BY_GROUPID	 0xD408


typedef struct  molegroup_member{
    uint32_t    member_count;
    userid_t    member_list[];
}__attribute__((packed)) molegroup_member_t;

typedef struct  del_molegroup_from_user_table{
    uint32_t    groupid;
}__attribute__((packed))del_molegroup_from_user_table_t;

typedef struct  molegroup_del_member{
    userid_t    groupownerid;
    userid_t    memberid;
}__attribute__((packed)) molegroup_del_member_t;


#define MAX_GROUP_NAME_LEN               25
#define MAX_GROUP_ANNOUNCEMENT_LEN       121
#define MAX_MOLEGROUP_MESSAGE_LEN       151
#define MAX_NUM_OF_GROUP_MEMBER          21
#define MAX_NUM_OF_GROUP_ONE_CAN_JOIN    20

#define CHECK_VALID_MOLEGROUP_ID(p_, grpid)	if ((grpid) < 50000) ERROR_RETURN (("error mole group id=%u uid=%u", grpid, (p_)->id), -1)

#define db_del_molegroup_from_userinfo(p_, buf, id) \
	    send_request_to_db(SVR_PROTO_RM_GROUP_FROM_USRINFO, p_, sizeof(del_molegroup_from_user_table_t), (buf), id)

#define db_del_member_from_molegroup(p_, buf, groupid) \
	    send_request_to_db(SVR_PROTO_DEL_MEMBER_FROM_GROUP, p_, sizeof(molegroup_del_member_t), (buf), groupid);


int get_molegroup_index(sprite_t* p, uint32_t groupid);

int create_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int add_member_to_molegroup_cmd(sprite_t * p,const uint8_t * body,int bodylen);
int apply_for_joining_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int reply_to_join_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int leave_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int kick_outof_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int delete_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int mod_molegroup_info_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_user_molegroup_list_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int get_molegroup_detail_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int chat_in_molegroup_cmd(sprite_t *p, const uint8_t *body, int bodylen);

int apply_molegroup_id_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_molegroup_detail_callback(sprite_t* p, uint32_t id, char* buf, int len);
int create_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_member_to_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len);
int db_get_owner_by_groupid_callback(sprite_t* p, uint32_t id, char* buf, int len);
int add_molegroup_to_userinfo_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_molegroup_from_userinfo_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_member_from_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len);
int get_user_molegroup_list_callback(sprite_t* p, uint32_t id, char* buf, int len);
int del_molegroup_callback(sprite_t* p, uint32_t id, char* buf, int len);
int mod_molegroup_info_callback(sprite_t* p, uint32_t id, char* buf, int len);
int set_molegroup_flag_cmd(sprite_t *p, const uint8_t *body, int bodylen);
int set_molegroup_flag_callback(sprite_t * p,uint32_t id,char * buf,int len);

void chat_in_molegroup_op(uint32_t opid, const void* body, int len);
void join_leave_molegroup_op(uint32_t opid, const void* body, int len);

int molegroup_op(uint32_t uid, void* buf, int len);


#endif
