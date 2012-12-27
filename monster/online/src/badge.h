/*
 * =====================================================================================
 *
 *       Filename:  badge.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年02月20日 18时30分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_BADGE_H_20120220
#define H_BADGE_H_20120220

#include <stdint.h>

#include "utility.h"
#include "data_structure.h"
#include "user_manager.h"
#include "message.h"

//精灵系别
const uint32_t tiaotiao_series  = 1;
const uint32_t mengchong_series  = 2;
const uint32_t hero_series  = 3;
const uint32_t plant_series  = 4;
const uint32_t dragon_series  = 5;
const uint32_t seans_series  = 6;

//第一期
const uint32_t badge_get_one_monster_const          = 1;    //成功领养1个小精灵
const uint32_t badge_get_two_same_monster_const     = 2;//成功领养2个同样的小精灵
const uint32_t badge_daily_puzzle_25_const          = 3;   //每日挑战达到25分
const uint32_t badge_heixia_5000_const              = 4;        //跳跳小黑侠达到5000分
const uint32_t badge_kuaigongfang_12000_const       = 5; //快快小工房达到12000分
const uint32_t badge_lianliankan_4000_const         = 6;  //怪卡连连看道道4000分
const uint32_t badge_stuffinhome_30_const           = 7; //自己小屋，拜访物品数量达到30个
const uint32_t badge_friends_100_const              = 8; //好友数达到100个
const uint32_t badge_visits_500_const               = 9; //得到500个拜访脚印
const uint32_t badge_thumb_50_const                 = 10; //得到50个赞
const uint32_t badge_message_50_const               = 11;//得到50个留言
//第二期
const uint32_t badge_miti_5_const                   = 12; //挑战自我，谜题大全，突破5次记录
const uint32_t badge_win_puzzle_50_const            = 13; //联机对战 ，赢得50场胜利
const uint32_t badge_get_tiaotiao_pet_const         = 14; //收集齐跳跳系精灵
const uint32_t badge_get_mengchong_pet_const        = 15; //收集齐萌宠系精灵
const uint32_t badge_get_hero_pet_const             = 16; //收集齐英雄系精灵
const uint32_t badge_get_plant_pet_const            = 17; //收集齐植物系精灵
const uint32_t badge_get_dragon_pet_const           = 18;//收集齐神龙系精灵
const uint32_t badge_get_seans_pet_const            = 19;//收集齐海洋系精灵
const uint32_t badge_first_open_box                 = 21;//第一次打开储物盒

const uint32_t badge_cake_game_const                = 22;//蛋糕店游戏达到10关
const uint32_t badge_npc_score_const                = 23;//小屋评分达到20分

int send_badge_to_db(usr_info_t *p_user, badge_info_t badge);
int badge_step(usr_info_t *p_user, uint32_t badge_id, uint32_t cond_val);

#endif //H_BADGE_H_20120220
