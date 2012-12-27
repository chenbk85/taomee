/*
 * =====================================================================================
 *
 *       Filename:  multi_svr.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2011年12月15日 22时02分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_MULTI_SVR_H_20111215
#define H_MULTI_SVR_H_20111215

#include "data_structure.h"
#include "user_manager.h"

void handle_multi_svr_return(svr_msg_header_t *p_data, int len);
//协议处理函数
int process_enter_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len);
int process_two_puzzle_match_notice(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len);
int process_start_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t* p_pkg, int len);
int process_two_puzzle_start_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_answer_two_puzzle_return(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_two_puzzle_answer_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);
int process_two_puzzle_result_notice(usr_info_t *p_user, svr_msg_header_t *p_pkg, int len);


#endif //H_MULTI_SVR_H_20111215
