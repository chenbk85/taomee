/*
 * =====================================================================================
 *
 *       Filename:  switch_henry.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月26日 17时04分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_SWITCH_H_20111226
#define H_SWITCH_H_20111226

#include "data_structure.h"
#include "user_manager.h"


void handle_switch_return(svr_msg_header_t *p_data, int len);

int process_notify_apply(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_active_user_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_get_shop_item_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_encourage_guide_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);

#endif //H_SWITCH_H_20111226
