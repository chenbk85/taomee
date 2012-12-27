/*
 * =====================================================================================
 *
 *       Filename:  cache.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/21/2012 04:22:52 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Plus Wu (), plus.wu1986@gmail.com
 *        Company:  Taomee
 *
 * =====================================================================================
 */
#ifndef CACHE_HPP__
#define CACHE_HPP__

#include "cli_proto.hpp"

int init_redis_connect();

void keep_redis_server_connect_alive();

int fini_redis_connect();

class CacheSvr {
 public:
  static int report_user_basic_info(svr_proto_t * pkg, fdsession_t * fdsess);
  static int set_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess);
  static int get_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess);
  static int player_get_team_contest_server(svr_proto_t * pkg, fdsession_t * fdsess);

  static int list_team_ranker_info(svr_proto_t * pkg, fdsession_t * fdsess);
  static int save_team_active_info(svr_proto_t * pkg, fdsession_t * fdsess);

  static int player_save_active_info(svr_proto_t * pkg, fdsession_t * fdsess);
  static int player_list_active_ranker_info(svr_proto_t * pkg, fdsession_t * fdsess);

  static int reset_player_achieve(svr_proto_t * pkg, fdsession_t * fdsess);

  static int player_generate_magic_number(svr_proto_t * pkg, fdsession_t * fdsess);
  static int player_bind_magic_number(svr_proto_t * pkg, fdsession_t * fdsess);
  static int get_player_maigc_invitee(svr_proto_t * pkg, fdsession_t * fdsess);

 private:
};

#endif


