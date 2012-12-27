/*
 * =====================================================================================
 *
 *       Filename:  activity.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2012年05月30日 11时09分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_ACTIVITY_H_20120530
#define H_ACTIVITY_H_20120530

#include <stdint.h>

#include "data_structure.h"
#include "data.h"
#include "utility.h"
#include "badge.h"

#include "user_manager.h"





/**
 * @brief get_cur_activity_from_conf 从配表拉取当前时间段内的活动项
 *
 * @param p_list
 *
 * @return
 */
int get_cur_activity_from_conf(activity_list_t *p_list);


/**
 * @brief get_a_activity_info 获取单个活动的信息
 *
 * @param p_user
 * @param act_id
 *
 * @return
 */
int get_a_activity_info(usr_info_t *p_user, uint32_t act_id);

int get_actinfo_from_conf(single_activity_t *p_act);

int get_act_reward(usr_info_t *p_user, as_request_activity_reward_t req);




int process_open_activity(usr_info_t *p_user, uint32_t act_id);
int process_open_act_reward(usr_info_t *p_user, uint32_t act_id,  uint32_t reward_id);

int process_invitor_activity(usr_info_t *p_user, uint32_t act_id);
int process_get_invite_info_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg);
int process_get_activity_reward_ex_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg);

int process_dragon_boat_activity(usr_info_t * p_user,uint32_t act_id);
int process_update_dragon_boat_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg);
int process_get_dragon_boat_return(usr_info_t * p_user,svr_msg_header_t * p_db_pkg);

int activity_open(usr_info_t *p_user, uint32_t act_id, single_activity_t *p_sa);
#endif //H_ACTIVITY_H_20120530
