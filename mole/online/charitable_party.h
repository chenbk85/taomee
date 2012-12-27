/*
 * =====================================================================================
 *
 *       Filename:  charitable_party.h
 *
 *    Description:  爱心天使慈善派对
 *
 *        Version:  1.0
 *        Created:  12/26/2011 01:52:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ericlee
 *        Company:  taomee
 *
 * =====================================================================================
 */
#ifndef CHARITABLE_PARTY_H
#define CHARITABLE_PARTY_H

enum CHP_DONATE_RESULT
{
	CHPDR_OK				= 0,
	CHPDR_NOT_ENOUGH_ITEMS,
	CHPDR_INVALID_ITEM,
};

enum CHP_AUCTION_RESULT
{
	CHPAR_OK				= 0,
	CHPAR_INVALID_ITEM,
	CHPAR_LESS_MONEY,
	CHPAR_MULT_AUCTION,
	CHPAR_NOT_ENOUGH_MONEY,
};

enum CHP_DONATE_TYPE
{
	CHPDT_ARG		= 1,
	CHPDT_MU,
	CHPDT_ANGEL,
	CHPDT_PIG,
	CHPDT_COLLECT,
};

typedef struct chp_donate_info_s
{
	uint32_t type;
	uint32_t user_id;
	uint32_t item_id;
	uint32_t item_cnt;
	uint8_t nick[USER_NICK_LEN];
} chp_donate_info_t;

typedef struct auction_result_s
{
	uint32_t item_id;
	uint32_t contributor_id;
	uint8_t contributor_nick[USER_NICK_LEN];
	uint32_t owner_id;
	uint8_t owner_nick[USER_NICK_LEN];
	uint32_t prize;
}__attribute__((packed)) auction_result_t;

typedef struct chp_timer {
	list_head_t timer_list;
}__attribute__(( packed )) chp_timer_t;

typedef struct auction_info_s
{
	uint32_t item_id;
	uint32_t contributor_id;
	uint8_t contributor_nick[USER_NICK_LEN];
	uint32_t buyer_id;
	uint8_t buyer_nick[USER_NICK_LEN];
	uint32_t cur_prize;
}__attribute__((packed)) auction_info_t;

//捐赠物品
int charparty_donate_item_cmd(sprite_t* p, const uint8_t* body, int len);
int charparty_donate_item_callback(sprite_t* p, uint32_t id, char* buf, int len);
int charparty_broadcast_donate_info(const void* buf, int len);
//检测物品是否存在
int charparty_check_item_callback(sprite_t* p, uint32_t id, char* buf, int len);

//获取竞拍物品列表 竞拍结果列表
int charparty_get_auction_list_cmd(sprite_t* p, const uint8_t* body, int len);
int charparty_get_auction_list_callback(sprite_t* p, uint32_t id, char* buf, int len);

//竞拍物品
int charparty_auction_item_cmd(sprite_t* p, const uint8_t* body, int len);
int charparty_auction_item_callback(sprite_t* p, uint32_t id, char* buf, int len);

//竞拍列表刷新定时函数
int charparty_timer_initialize();
int charparty_auction_list_update_timer( void* onwer, void* data );
int charparty_auction_list_update_callback(sprite_t* p, uint32_t id, char* buf, int len);

//刷新玩家游戏币
int update_players_money_notice(const void* buf, int len);

//20120113新年盛宴 捐献物品协议
int donate_food_item_cmd(sprite_t* p, const uint8_t* body, int len);
int user_check_donate_food_item_cnt_callback(sprite_t* p, uint32_t id, char* buf, int len);
int donate_food_item_callback(sprite_t* p, uint32_t id, char* buf, int len);

#endif