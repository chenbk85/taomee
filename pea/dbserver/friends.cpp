#include <algorithm>

#include <time.h>
#include "user.hpp"
#include "friends.hpp"

using namespace std;

enum {
	FRIEND_TYPE_FRIEND		= 0,//普通朋友
	FRIEND_TYPE_BLACKLIST	= 1,//黑名单
	FRIEND_TYPE_RECENT		= 2,//最近联系人(普通+陌生)
	FRIEND_TYPE_SPECIAL		= 3,//特别关心
};

pea_friends::pea_friends(mysql_interface* db)
    :CtableRoute100x10(db, "pea", "pea_friends", "user_id")
{


}

int pea_friends::add_friend(uid_role_t &user, uid_role_t &ufriend, uint32_t server_id)
{
	uint32_t now = (uint32_t)time(NULL);
	GEN_SQLSTR(sqlstr, "insert ignore into %s (user_id, role_tm, friend_id, friend_role_tm, server_id, friend_type, add_time) values (%u, %u, %u, %u, %u, %u, %u)",
			get_table_name(user.user_id), 
			user.user_id,
			user.role_tm,
			ufriend.user_id,
			ufriend.role_tm,
			server_id,
			FRIEND_TYPE_FRIEND,
			now);
	return exec_insert_sql(sqlstr, SUCC);
}

int pea_friends::del_friend(uid_role_t &user, uid_role_t &ufriend, uint32_t server_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s where user_id = %u and role_tm = %u and friend_id = %u and friend_role_tm = %u and server_id = %u and friend_type = %u",
			get_table_name(user.user_id), 
			user.user_id,
			user.role_tm,
			ufriend.user_id,
			ufriend.role_tm,
			server_id,
			FRIEND_TYPE_FRIEND);
	return exec_delete_sql(sqlstr, SUCC);
}

int pea_friends::add_to_blacklist(uid_role_t &user, uid_role_t &blacked_user, uint32_t server_id)
{
	uint32_t now = (uint32_t)time(NULL);
	GEN_SQLSTR(sqlstr, "insert ignore into %s (user_id, role_tm, friend_id, friend_role_tm, server_id, friend_type, add_time) values (%u, %u, %u, %u, %u, %u, %u)",
			get_table_name(user.user_id), 
			user.user_id,
			user.role_tm,
			blacked_user.user_id,
			blacked_user.role_tm,
			server_id,
			FRIEND_TYPE_BLACKLIST,
			now);
	return exec_insert_sql(sqlstr, SUCC);
}

int pea_friends::del_from_blacklist(uid_role_t &user, uid_role_t &unblacked_user, uint32_t server_id)
{
	GEN_SQLSTR(sqlstr, "delete from %s where user_id = %u and role_tm = %u and friend_id = %u and friend_role_tm = %u and server_id = %u and friend_type = %u",
			get_table_name(user.user_id), 
			user.user_id,
			user.role_tm,
			unblacked_user.user_id,
			unblacked_user.role_tm,
			server_id,
			FRIEND_TYPE_BLACKLIST);
	return exec_delete_sql(sqlstr, SUCC);
}

int pea_friends::get_friends_list(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec)
{
	GEN_SQLSTR(sqlstr, "select friend_id, friend_role_tm from %s where user_id = %u and role_tm = %u and server_id = %u and friend_type = %u",
            get_table_name(user.user_id),
            user.user_id,
			user.role_tm,
            server_id,
			FRIEND_TYPE_FRIEND);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, vec) {
        INT_CPY_NEXT_FIELD(item.user_id);
		INT_CPY_NEXT_FIELD(item.role_tm);
    }
    STD_QUERY_WHILE_END_NEW()
}

int pea_friends::get_blacklist(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec)
{
	GEN_SQLSTR(sqlstr, "select friend_id, friend_role_tm from %s where user_id = %u and role_tm = %u and server_id = %u and friend_type = %u",
            get_table_name(user.user_id),
            user.user_id,
			user.role_tm,
            server_id,
			FRIEND_TYPE_BLACKLIST);

    STD_QUERY_WHILE_BEGIN_NEW(sqlstr, vec) {
        INT_CPY_NEXT_FIELD(item.user_id);
		INT_CPY_NEXT_FIELD(item.role_tm);
    }
    STD_QUERY_WHILE_END_NEW()
}

//TODO
int pea_friends::get_union_list(uid_role_t &user, uint32_t server_id, std::vector<uid_role_t> &vec)
{
	return 0;
}

