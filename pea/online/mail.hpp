#ifndef _MAIL_HPP_
#define _MAIL_HPP_

#include "fwd_decl.hpp"
#include "pea_common.hpp"
#include "item.hpp"
#include "proto.hpp"
#include <vector>

using namespace std;


typedef struct player_t player_t;
typedef struct mail_numerical_enclosure      mail_numercal_enclosure;
typedef struct mail_item_enclosure           mail_item_enclosure;
typedef struct mail_equip_enclosure          mail_equip_enclosure;


enum mail_type
{
	mail_type_begin = 0,
	mail_type_system = 1,
	mail_type_user = 2,
	mail_type_end
};

enum mail_state
{
	mail_state_begin = 0,
	mail_state_read = 1,
	mail_state_unread = 2,
	mail_state_end
};


//邮件数据
typedef struct mail_data
{
	mail_data()
	{
		mail_id = 0;
		mail_time = 0;
		mail_state = 0;
		mail_templet = 0;
		mail_type = 0;
		sender_id = 0;
		sender_role_tm = 0;
		receive_id = 0;
		receive_role_tm = 0;
		memset(mail_title, 0, sizeof(mail_title));
		memset(mail_content, 0, sizeof(mail_content));
	}
	uint32_t mail_id;                                   //邮件ID唯一
	uint32_t mail_time;                                 //邮件发送时间,unix time
	uint32_t mail_state;                                //邮件状态 1:已读 2：未读
	uint32_t mail_templet;                              //邮件模板
	uint32_t mail_type;                                 //邮件类型
	uint32_t sender_id;                                 //发件人米米号，系统邮件为0
	uint32_t sender_role_tm;                            //发件人角色ROLE_TM，系统邮件为0（该字段暂时没有用）
	char     sender_nick[MAX_NICK_SIZE];                //发件人姓名

	char     mail_title[ MAX_MAIL_TITLE_LEN ];        //邮件标题
	char     mail_content[ MAX_MAIL_CONTENT_LEN ];    //邮件内容

	uint32_t receive_id;                                //接收人米米号
	uint32_t receive_role_tm;                           //接收人ROLE_TM，系统邮件用，否则为0
	char     receive_nick[MAX_NICK_SIZE];

	std::vector<mail_numerical_enclosure>  mail_nums;   //数值附件
	std::vector<mail_item_enclosure>       mail_items;  //物品附件
	std::vector<mail_equip_enclosure>      mail_equips; //装备附件
}mail_data;


typedef struct mail_numerical_enclosure
{
	mail_numerical_enclosure()
	{
		number = 0;
		type = 0;
	}
	uint32_t  number;                                //附件数值
	uint32_t  type;                                  //附件类型
}mail_numerical_enclosure;

typedef struct mail_item_enclosure
{
	mail_item_enclosure()
	{
		item_id = 0;
	}
	uint32_t item_id;                               //物品附件ID
	uint32_t item_count;                            //物品附件数量
	uint32_t duration_time;                         //过期时间段
	uint32_t end_time;                              //过期时间戳
}mail_item_enclosure;

typedef struct mail_equip_enclosure
{
	mail_equip_enclosure()
	{
		equip_id = 0;
	}
	uint32_t equip_id;                             //装备附件ID
	uint32_t equip_count;                          //装备附件数量
	uint32_t duration_time;                        //过期时间段
	uint32_t end_time;                             //过期时间戳
}mail_equip_enclosure;

//--------------------------------------------------------------------------//
bool mail_numerical_enclosure_to_str( vector<mail_numerical_enclosure> *mail_nums,  char* buf, uint32_t buflen);

bool str_to_mail_numberical_enclosure(char* buf, vector<mail_numerical_enclosure> *mail_nums);

bool mail_item_enclosure_to_str( vector<mail_item_enclosure>* mail_items, char* buf, uint32_t buflen);

bool str_to_mail_item_enclosure(char* buf,  vector<mail_item_enclosure>* mail_items);

bool mail_equip_enclosure_to_str( vector<mail_equip_enclosure>* mail_equips,  char* buf, uint32_t buflen);

bool str_to_mail_equip_enclosure(char* buf,  vector<mail_equip_enclosure>* mail_equips);


//---------------------------------------------------------------------------//

bool init_player_mail(player_t *p);

bool final_player_mail(player_t *p);

bool add_player_mail(player_t* p, mail_data& data);

bool add_player_mail(player_t* p, db_mail_head_info_t& head);

bool is_mail_exist(player_t* p, uint32_t mail_id);

mail_data* get_player_mail(player_t* p, uint32_t mail_id);

bool delete_player_mail(player_t* p, uint32_t mail_id);

void clear_mail_enclosure(player_t* p, uint32_t mail_id);

bool has_mail_read(player_t* p, uint32_t mail_id);

bool is_mail_include_enclosure(player_t* p, uint32_t mail_id);

void clear_mail_enclosure(player_t* p, uint32_t mail_id);

bool is_mail_load_ok(player_t* p, uint32_t mail_id);
//--------------------------------------------------------------------------//
int cli_proto_mail_head_list(DEFAULT_ARG);

int send_mail_head_list(player_t* p);

int cli_proto_mail_body(DEFAULT_ARG);

int send_mail_body_info(player_t* p, int mail_id);

int cli_proto_del_mail(DEFAULT_ARG);

int send_del_mail(player_t* p, int mail_id);

int cli_proto_take_mail_enclosure(DEFAULT_ARG);

int send_take_mail_enclosure(player_t* p, int mail_id);

int cli_proto_send_mail(DEFAULT_ARG);

int notify_player_new_mail(player_t* p, db_mail_head_info_t* info);


#endif
