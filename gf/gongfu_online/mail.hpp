/**
 ============================================================
 @file      mail.h
 @brief     mail related functions are declared here
 ** 
 **  compiler   gcc4.1.2
 **  platform   Linux
 **
 **  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 **
 **============================================================
**/

#ifndef _MAIL_H_
#define _MAIL_H_

#include"fwd_decl.hpp"
#include <vector>


//the max mail title string length
#define MAX_MAIL_TITLE_LEN     (40)
//the max mail content string length
#define MAX_MAIL_CONTENT_LEN   (300)
//the mail which you should pay for
#define MAX_MAIL_PAY_FOR       (10)
//the system mail sender id
#define SYSTEM_SENDER_ID       (0)
#define SYSTEM_SENDER_NAME     ("系统邮件")
//the max count of player's  mail
#define MAX_MAIL_COUNT         (100)
//the max num enclosure count
#define MAX_NUM_ENCLOSURE_COUNT 3
#define MAX_ITEM_ENCLOSURE_COUNT 3
#define MAX_EQUIP_ENCLOSURE_COUNT 3

typedef struct mail_numerical_enclosure      mail_numercal_enclosure; 
typedef struct mail_item_enclosure           mail_item_enclosure;
typedef struct mail_equip_enclosure          mail_equip_enclosure;

//邮件类型
enum mail_type
{
	mail_type_begin = 0,
	mail_type_system = 1,
	mail_type_user = 2,
	mail_type_end 
};

//邮件状态
enum mail_state
{
	mail_state_begin = 0,
	mail_state_read = 1,
	mail_state_unread = 2,
	mail_state_end
};

//邮件模板
enum mail_templet_type
{
	mail_templet_begin = 0,
	mail_templet_normal = 999,       //当包裹已经满，通过系统邮件发送物品给玩家，该模板类型需要服务器填充邮件标题和内容
	mail_templet_level_up = 1000,    //玩家每升5级，发送传世布袋给玩家，该模板类型不需要填充标题和内容
	mail_templet_level_up_limit = 1001, //玩家升到顶级，发送系统邮件通知，该模板类型不需要填充标题和内容
    mail_templet_up_active = 1005,  //玩家参加周冲级活动，发送奖励邮件
    mail_templet_sum_event = 1017,  /* 灵兽随机事件 */
	mail_templet_end                       
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
	uint32_t mail_id;           						//邮件ID唯一
	uint32_t mail_time;         						//邮件发送时间,unix time
	uint32_t mail_state;        						//邮件状态 1:已读 2：未读
	uint32_t mail_templet;      						//邮件模板
	uint32_t mail_type;         						//邮件类型
	uint32_t sender_id;         						//发件人米米号，系统邮件为0
	uint32_t sender_role_tm;    						//发件人角色ROLE_TM，系统邮件为0（该字段暂时没有用）
	
	char     mail_title[ MAX_MAIL_TITLE_LEN+1 ];        //邮件标题
	char     mail_content[ MAX_MAIL_CONTENT_LEN+1 ];    //邮件内容

	uint32_t receive_id;                                //接收人米米号
	uint32_t receive_role_tm;                           //接收人ROLE_TM，系统邮件用，否则为0

	std::vector<mail_numerical_enclosure>  mail_nums;   //数值附件
	std::vector<mail_item_enclosure>       mail_items;  //物品附件
	std::vector<mail_equip_enclosure>	   mail_equips; //装备附件
}mail_data;

//邮件头
typedef struct mail_header
{
	mail_header()
	{
		mail_id = 0;
		mail_time = 0;
		mail_state = 0;
		mail_templet = 0;
		mail_type = 0;
		sender_id = 0;
		sender_role_tm = 0;
		memset(mail_title, 0, sizeof(mail_title));
	}
	uint32_t mail_id; 	 							   //邮件ID，唯一
	uint32_t mail_time;                                //邮件发送时间 unix time
	uint32_t mail_state;                               //邮件状态 1:已读 2：未读
	uint32_t mail_templet;                             //邮件模板
	uint32_t mail_type;                                //邮件类型
	uint32_t sender_id;                                //发件人米米号，系统邮件为0
	uint32_t sender_role_tm;                           //发件人角色ROLE_TM，系统邮件为0（该字段暂时没有用）
	char     mail_title[ MAX_MAIL_TITLE_LEN+1 ];       //邮件标题
}mail_header;

//邮件体
typedef struct mail_body
{
	mail_body()
	{
		mail_id = 0;
		memset(mail_content, 0, sizeof(mail_content));
	}
	uint32_t mail_id; 								   //邮件ID
	char     mail_content[ MAX_MAIL_CONTENT_LEN+1 ];   //邮件内容	 
}mail_body;


//邮件数值附件
typedef struct mail_numerical_enclosure
{
	mail_numerical_enclosure()
	{
		number = 0;                                        
		type = 0;
	}
	uint32_t  number;                                //附件数值
	uint32_t  type;                                  //附件类型1:功夫豆 2:经验 3:技能点 4：伏魔点
}mail_numerical_enclosure;

//邮件物品附件
typedef struct mail_item_enclosure
{
	mail_item_enclosure()
	{
		item_id = 0;
		item_count = 0;
	}
	mail_item_enclosure(uint32_t in_id, uint32_t in_count)
	{
		item_id = in_id;
		item_count = in_count;
	}
	uint32_t item_id;								//物品附件ID
	uint32_t item_count;	                        //物品附件数量
}mail_item_enclosure;


//邮件装备附件
typedef struct mail_equip_enclosure
{
	mail_equip_enclosure()
	{
		equip_id = 0;
		equip_get_time = 0;
		equip_rank = 0;
		equip_duration = 0;	
        equip_lifetime = 0;
	}
	mail_equip_enclosure(uint32_t in_id, uint32_t in_tm, uint32_t in_rank, uint32_t in_duration, uint32_t lifetime)
	{
		equip_id = in_id;
		equip_get_time = in_tm;
		equip_rank = in_rank;
		equip_duration = in_duration;
		equip_lifetime = lifetime;
	}
	uint32_t equip_id;                             //装备附件ID
	uint32_t equip_get_time;                       //装备获取时间，UNIX TIME
	uint32_t equip_rank;                           //装备级别
	uint32_t equip_duration;	                   //装备耐久度
	uint32_t equip_lifetime;	                   //装备寿命
}mail_equip_enclosure;


typedef struct mail_head_list_rsp_t
{
	uint32_t count;
	mail_header   headers[];	
}mail_head_list_rsp_t;

typedef struct mail_body_rsp_t
{
	uint32_t mail_id ;
	char     mail_body[MAX_MAIL_CONTENT_LEN + 1];
	char     mail_numerical_enclosure[1024];
	char     mail_item_enclosure[1024];
	char     mail_equip_enclosure[1024];
}mail_body_rsp_t;

typedef struct delete_mail_rsp_t
{
	uint32_t mail_id;
}delete_mail_rsp_t;

struct send_mail_rsp_t
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


/**
 *   @brief initialization player's mail 
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int init_player_mail(player_t *p);


/**
 *  @brief uninitialization player's mail
 *  @param player_t* 
 *  @return  0 sucess,  -1 otherwirse fail
**/
int final_player_mail(player_t *p);


int mail_list_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 *   @brief send mail with item
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
void send_add_item_mail(player_t*p, uint32_t itemid, uint32_t cnt);

/**
 *  @brief send mail list request to dbserver
 *  @param player_t* 
 *  @return  0 sucess,  -1 otherwirse fail
**/
int db_mail_head_list(player_t* p);

/**
 *  @brief callback function for mail list db request
 *  @param player_t* 
 *  @return  0 sucess,  -1 otherwirse fail
**/
int db_mail_head_list_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 *   @brief init player mail list
 *   @param player_t* 
 *   @return  0 sucess,  -1 otherwirse fail
**/
int init_player_mail_head_list(player_t* p, mail_head_list_rsp_t* rsp);


int send_player_mail_head_list(player_t* p);
/**
 *  @brief add mail to player from mail data
 *  @param player_t*, mail_data&
 *  @return  true sucess, false otherwirse fail
**/
bool add_player_mail(player_t* p, mail_data& data);

/**
 *   @brief add mail to player from mail header
 *   @param player_t*, mail_header&
 *   @return  true sucess, false otherwirse fail
**/
bool add_player_mail(player_t* p, mail_header& header);


/**
 *  @brief delete mail from player by mail id
 *  @param player_t*, uin32_t 
 *  @return  true sucess, false otherwirse fail
**/
bool delete_player_mail(player_t* p, uint32_t mail_id);

/**
 *   @brief get mail data by mail id
 *   @param player_t*, uint32_t
 *   @return  mail_data* sucess, NULL otherwirse fail
**/
mail_data* get_player_mail(player_t* p, uint32_t mail_id);

/**
 *   @brief chech the mail exist
 *   @param player_t*, uint32_t
 *   @return  true sucess, false otherwirse fail
**/
bool is_mail_exist(player_t*p, uint32_t mail_id);


/**
 *   @brief check the mail has enclosure
 *   @param player_t*, uint32_t
 *   @return  true sucess, false otherwirse fail
**/
bool is_mail_include_enclosure(player_t* p, uint32_t mail_id);

/**
 *   @brief clear mail enclosure
 *   @param player_t*, uint32_t
 *   @return  void
**/
void clear_mail_enclosure(player_t* p, uint32_t mail_id);

/**
 *    @brief check mail has been read
 *    @param player_t*, uint32_t
 *    @return  true sucess, false otherwirse fail
**/
bool has_mail_read(player_t* p, uint32_t mail_id);

/**
 *   @brief process mail body cmd from client
 *   @param player_t*,
 *   @return  0 sucess, -1 otherwirse fail
**/
int mail_body_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 *   @brief send mail body db request to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_mail_body(player_t* p, uint32_t mail_id);

/**
 *    @brief db callback function for sending mail body db request
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_mail_body_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 *   @brief send mail body info to client
 *   @param player_t*, uint32_t
 *   @return 0 sucess, -1 otherwirse fail
**/
int send_mail_body_info(player_t*p, uint32_t mail_id);

/**
 *   @brief process delete mail cmd from client
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int delete_mail_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 *    @brief send delete mail db request to dbserver
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_delete_mail(player_t* p, uint32_t mail_id);

/**
 *    @brief db callback function for sending delete mail db request
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_delete_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


/**
 *   @brief process take mail enclosure cmd from client
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int take_mail_enclosure_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 *  @brief send take mail enclosure db request to dbserver
 *  @param player_t*, uint32_t
 *  @return  0 sucess, -1 otherwirse fail
**/
int db_take_mail_enclosure(player_t* p, uint32_t mail_id);


/**
 *  @brief db callback funtion for sending take mail enclosure db request
 *  @param player_t*, uint32_t
 *  @return  0 sucess, -1 otherwirse fail
**/
int db_take_mail_enclosure_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);


/**
 *   @brief process send mail cmd from client
 *   @param player_t* 
 *   @return  0 sucess, -1 otherwirse fail
**/
int send_mail_cmd(player_t* p, uint8_t* body, uint32_t bodylen);


/**
 *   @brief send  new mail db request to dbserver
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_send_mail(player_t* p, const char* title, uint32_t title_len, const char* content, uint32_t content_len, uint32_t mail_templet, uint32_t player_id);

/**
 *   @brief db callback funtion for sending new mail 
 *   @param player_t*, uint32_t
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_send_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);

/**
 *    @brief send reduce money to dbserver
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int db_reduce_money(player_t*p, uint32_t money, bool callback = false);


/**
 *    @brief send new mail header info to client 
 *    @param player_t*, uint32_t
 *    @return  0 sucess, -1 otherwirse fail
**/
int send_player_new_mail(player_t* p, mail_header* header);


/**
 *   @brief send new system mail header info to client 
 *   @param player_t*, 
 *   @return  0 sucess, -1 otherwirse fail
**/


int db_send_system_mail(player_t*p,  const char* title, uint32_t title_len, const char* content, uint32_t content_len, uint32_t mail_templet, std::vector<mail_numerical_enclosure>* nums = NULL,  std::vector<mail_item_enclosure>* items =NULL, std::vector<mail_equip_enclosure>* equips = NULL, uint32_t uid = 0, uint32_t roletm = 0);
/**
 *   @brief db callback funtion for sending new system mail  
 *   @param player_t*, 
 *   @return  0 sucess, -1 otherwirse fail
**/
int db_send_system_mail_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);	

int login_send_mail_action(player_t* p);


int mail_active_event(player_t* p, uint32_t title_idx, uint32_t content_idx, uint32_t mail_temp, uint32_t item_id, uint32_t cnt, uint32_t uid = 0, uint32_t roletm = 0);

#endif














