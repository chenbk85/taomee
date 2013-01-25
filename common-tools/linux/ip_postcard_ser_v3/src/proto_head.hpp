#ifndef PROTO_HEAD_HPP
#define PROTO_HEAD_HPP

extern "C"
{
#include <libtaomee/project/types.h>
}

struct online_proto_head_t
{
    uint32_t    len;
    uint32_t    seq;
    uint16_t    cmd;
    uint32_t    ret;
    userid_t    id;
}__attribute__((packed));

struct proto_head_t
{
    uint32_t    package_len;
    uint16_t    command_id;
    uint32_t    user_id;
    uint32_t    seq_num;
    uint32_t    status_code;
    uint32_t    role_tm;
    uint16_t    game_flag;
    uint16_t    game_zone;
    uint16_t    game_svr;
    uint16_t    channel_id;
    char        verify_code[32];
    char        data[];
}__attribute__((packed));

struct mail_proto_t
{
    proto_head_t ph;
    uint32_t    sender_id;
    char        nick[16];
    uint32_t    send_time;
    uint32_t    type_id;
    uint32_t    title_len;
    char        title[40];
    uint32_t    msglen;
    char        msg[150];
    uint32_t    enclosure_cnt;
}__attribute__((packed));

#define PROTO_HEAD_LEN sizeof(proto_head_t)
#define ONLINE_PROTO_HEAD_LEN sizeof(online_proto_head_t)

#define GAME_TYPE_MOLE      1
#define GAME_TYPE_SEER      2
#define GAME_TYPE_FAIRY     5
#define GAME_TYPE_GONGFU    6
#define GAME_TYPE_HERO      7
#define GAME_TYPE_MOLE3     8
#define GAME_TYPE_SEER2     9

struct post_card_t
{
    uint32_t game_type;
    uint32_t ip_address;
    uint16_t game_zone;
}__attribute__((packed));

   
#endif // PROTO_HEAD_HPP
