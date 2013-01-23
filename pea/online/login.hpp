#ifndef LOGIN_HPP_
#define LOGIN_HPP_

#include "fwd_decl.hpp"
#include "pea_common.hpp"

class player_t;

int send_login_rsp(player_t* p);

int process_login(player_t * p);

#endif
