#ifndef _PEA_FRIENDS_H_
#define _PEA_FRIENDS_H_

#include "CtableRoute100x10.h"
#include "proto.hpp"
#include "benchapi.h"
#include "user.hpp"
#include <vector>



class pea_friends : public CtableRoute100x10
{
    public:
        pea_friends(mysql_interface* db);
		
		int add_friend(uid_role_t &user, uid_role_t &ufriend, uint32_t server_id);
		int del_friend(uid_role_t &user, uid_role_t &ufriend, uint32_t server_id);
		int add_to_blacklist(uid_role_t &user, uid_role_t &blacked_user, uint32_t server_id);
		int del_from_blacklist(uid_role_t &user, uid_role_t &unblacked_user, uint32_t server_id);
		int get_friends_list(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec);
		int get_blacklist(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec);
		int get_union_list(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec);
};

#endif
