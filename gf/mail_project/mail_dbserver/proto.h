#ifndef  GF_PROTO_H
#define  GF_PROTO_H
#ifndef  MK_PROTO_DOC 
#include "proto_header.h"
#endif

#define RECVBUF_ROLETM                              (RECVBUF_HEADER->role_tm)
#define USERID_ROLETM								RECVBUF_USERID,RECVBUF_ROLETM

enum {
	max_client_buf_len = 40,
};

#define gf_mail_head_list_cmd                          (0x0633)
#define gf_mail_body_cmd                               (0x0636)
#define gf_delete_mail_cmd                             (0x0637)
#define gf_take_mail_enclosure_cmd                     (0x0638)
#define gf_send_mail_cmd                               (0x0639)
#define gf_send_system_mail_cmd                        (0x063A)
#define gf_reduce_money_cmd                            (0x0640)


#define MAX_MAIL_TITLE_LEN 40 
#define MAX_MAIL_BODY_LEN  100
#define GF_MAIL_FULL   5121
#define GF_MAIL_ID_NOT_EXIST                5120

struct  stru_count{
	uint32_t    count;
}__attribute__((packed));


typedef stru_count gf_mail_head_list_out_header;


struct gf_mail_head_list_out_element
{
	uint32_t        mail_id;
	uint32_t        mail_time;
	uint32_t        mail_state;
	uint32_t        mail_templet;
	uint32_t        mail_type;  
	uint32_t        sender_id;
	uint32_t        sender_role_tm;
	char            mail_title[ MAX_MAIL_TITLE_LEN + 1];    
};//为了和ONLINE匹配这里就不吧结构给压实了

struct gf_mail_body_in
{
	uint32_t mail_id;
}__attribute__((packed));

struct gf_mail_body_out
{
	uint32_t mail_id;
	char     mail_content[MAX_MAIL_BODY_LEN +1 ];
	char     mail_numerical_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
};//为了和ONLINE匹配这里就不吧结构给压实了

struct gf_delete_mail_in
{
	uint32_t mail_id;
}__attribute__((packed));

struct gf_delete_mail_out
{
	uint32_t mail_id;
}__attribute__((packed));

struct gf_mail_nums_enclosure
{
	uint32_t type;
	uint32_t number;
}__attribute__((packed));

struct gf_mail_items_enclosure
{
	uint32_t item_id;
	uint32_t item_count;
	uint32_t max_item_count;
}__attribute__((packed));

struct gf_mail_equips_enclosure
{
	uint32_t equip_id;
	uint32_t equip_get_time;
	uint32_t equip_rank;
	uint32_t equip_duration;
	uint32_t equip_lifetime;
}__attribute__((packed));

struct gf_take_mail_enclosure_in_header
{
	uint32_t mail_id;
	uint32_t max_item_bag_grid;
	uint32_t mail_nums_count;
	uint32_t mail_items_count;
	uint32_t mail_equips_count;
}__attribute__((packed));

struct gf_send_mail_in
{
	uint32_t sender_id;
	uint32_t receive_id;
	uint32_t mail_templet;
	uint32_t max_mail_limit;
	char     mail_title[MAX_MAIL_TITLE_LEN+1];
	char     mail_body[MAX_MAIL_BODY_LEN + 1];
}__attribute__((packed));

struct gf_send_mail_out
{
	uint32_t        mail_id;
	uint32_t        mail_time;
	uint32_t        mail_state;
	uint32_t        mail_templet;
	uint32_t        mail_type;
	uint32_t        sender_id;
	uint32_t        receive_id;
	char            mail_title[ MAX_MAIL_TITLE_LEN + 1];
}__attribute__((packed));

struct gf_send_system_mail_in
{
	uint32_t sender_id;
	uint32_t receive_id;
	uint32_t mail_templet;
	uint32_t max_mail_limit;
	char     mail_title[MAX_MAIL_TITLE_LEN+1];
	char     mail_body[MAX_MAIL_BODY_LEN + 1];
	char     mail_num_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
}__attribute__((packed));
















#endif
