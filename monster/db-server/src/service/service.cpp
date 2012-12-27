

#include <stdio.h>
#include <string.h>
#include <time.h>
extern "C"
{
#include <libtaomee/log.h>
}
#include <benchapi.h>
#include "../../../common/message.h"
#include "../../../common/pack/c_pack.h"
#include "../../../common/data_structure.h"
#include "../../../common/constant.h"
#include "../util.h"
#include "../db_constant.h"
#include "service.h"

extern c_pack g_pack;
static char g_sql_str[4096] = {0};


uint32_t query_role_info_boke(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type, request_len, 0) != 0)
	{
		return ERR_MSG_LEN;
	}

	char buffer[4096] = {0};
	boke_msg_get_role_t *boke_msg = (boke_msg_get_role_t *)buffer;
	int buf_len = sizeof(boke_msg_get_role_t);

	sprintf(g_sql_str, "SELECT name, mon_level, mon_id,  mon_main_color, mon_exp_color, mon_eye_color "
				"FROM db_monster_%d.t_role_%d WHERE user_id = %u",DB_ID(user_id), TABLE_ID(user_id), user_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if (0 == result_count)			//没有记录，用户未注册
	{
		boke_msg->is_register = 0;
		g_pack.pack(buffer, buf_len);
		return 0;
	}
	else if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}
	else
	{
		boke_msg->is_register = 1;
	}



	strncpy(boke_msg->user_name, row[0], sizeof(boke_msg->user_name));


	if(str2uint(&boke_msg->monster_level, row[1]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_level:%s to uint16 failed(%s).", row[1], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&boke_msg->monster_id, row[2]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_id:%s to uint32 failed(%s).", row[2], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&boke_msg->monster_main_color, row[3]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_main_color:%s to uint32 failed(%s).", row[3], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&boke_msg->monster_exp_color, row[4]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_exp_color:%s to uint32 failed(%s).", row[4], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&boke_msg->monster_eye_color, row[5]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_eye_color:%s to uint32 failed(%s).", row[5], g_sql_str);
		return ERR_SQL_ERR;
	}

	g_pack.pack(buffer, buf_len);
	return 0;
}


uint32_t query_role_info_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type, request_len, 0) != 0)
	{
		return ERR_MSG_LEN;
	}

	char buffer[4096] = {0};
	service_msg_get_role_t *service_msg = (service_msg_get_role_t *)buffer;
	int buf_len = sizeof(service_msg_get_role_t);

	sprintf(g_sql_str, "SELECT mon_name, name, birthday, fav_pet, personal_sign, fav_color, fav_fruit, "
				"mood, coins, mon_level, mon_exp, mon_health, mon_happy, visits, thumb, last_login_time "
				"FROM db_monster_%d.t_role_%d WHERE user_id = %u",DB_ID(user_id), TABLE_ID(user_id), user_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if (0 == result_count)			//没有记录，用户未注册
	{
		service_msg->is_register = 0;
		g_pack.pack(buffer, buf_len);
		return 0;
	}
	else if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}
	else
	{
		service_msg->is_register = 1;
	}


	strncpy(service_msg->role_info.monster_name, row[0], sizeof(service_msg->role_info.monster_name));

	strncpy(service_msg->role_info.user_name, row[1], sizeof(service_msg->role_info.user_name));

	if(str2uint(&service_msg->role_info.birthday, row[2]) !=0)
	{
		KCRIT_LOG(user_id, "convert birthday:%s to uint32 failed(%s).", row[2], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.fav_pet, row[3]) !=0)
	{
		KCRIT_LOG(user_id, "convert fav_pet:%s to uint8 failed(%s).", row[3], g_sql_str);
		return ERR_SQL_ERR;
	}

	strncpy(service_msg->role_info.personal_sign, row[4], sizeof(service_msg->role_info.personal_sign));

	if(str2uint(&service_msg->role_info.fav_color, row[5]) !=0)
	{
		KCRIT_LOG(user_id, "convert fav_color:%s to uint8 failed(%s).", row[5], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.fav_fruit, row[6]) !=0)
	{
		KCRIT_LOG(user_id, "convert fav_fruit:%s to uint8 failed(%s).", row[6], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.mood, row[7]) != 0)
	{
		KCRIT_LOG(user_id, "convert mood:%s to uint8 failed(%s).", row[7], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.coins, row[8]) !=0)
	{
		KCRIT_LOG(user_id, "convert coins:%s to uint32 failed(%s).", row[8], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.monster_level, row[9]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_level:%s to uint16 failed(%s).", row[9], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.monster_exp, row[10]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_exp:%s to uint32 failed(%s).", row[10], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.monster_health, row[11]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_health:%s to uint32 failed(%s).", row[11], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.monster_happy, row[12]) !=0)
	{
		KCRIT_LOG(user_id, "convert monster_happy:%s to uint32 failed(%s).", row[12], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.visits, row[13]) !=0)
	{
		KCRIT_LOG(user_id, "convert visits:%s to uint32 failed(%s).", row[13], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.thumb, row[14]) !=0)
	{
		KCRIT_LOG(user_id, "convert thumb:%s to uint32 failed(%s).", row[14], g_sql_str);
		return ERR_SQL_ERR;
	}

	if(str2uint(&service_msg->role_info.last_login_time, row[15]) !=0)
	{
		KCRIT_LOG(user_id, "convert last_login_time:%s to uint32 failed(%s).", row[15], g_sql_str);
		return ERR_SQL_ERR;
	}

	service_msg->role_info.limit_op = 0;

	g_pack.pack(buffer, buf_len);
	return 0;
}

uint32_t query_friend_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type,request_len,0) != 0)
	{
		return ERR_MSG_LEN;
	}

	sprintf(g_sql_str, "SELECT friend_id FROM db_monster_%d.t_friend_%d WHERE user_id = %u AND type not IN(%u, %u) limit 500;",
				DB_ID(user_id), TABLE_ID(user_id), user_id, FRIEND_PENDING, FRIEND_BLOCK);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	uint32_t friend_id;
	g_pack.pack(num);

	while(row !=NULL)
	{
		if(str2uint(&friend_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert friend_id:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		g_pack.pack(friend_id);
		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}

uint32_t query_badge_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type,request_len,0) != 0)
	{
		return ERR_MSG_LEN;
	}

	sprintf(g_sql_str, "SELECT badge_id, status, progress FROM db_monster_%d.t_badge_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	service_badge_t service_badge;
	g_pack.pack(num);

	while(row !=NULL)
	{
		if(str2uint(&service_badge.badge_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert badge_id:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_badge.status, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert status:%s to uint8 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_badge.progress, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "convert progress:%s to uint32 failed!", row[2]);
			return ERR_SQL_ERR;
		}

		service_badge.timestamp = 0;
		g_pack.pack((char *)(&service_badge), sizeof(service_badge));

		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}

uint32_t query_pinboard_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type,request_len,0) != 0)
	{
		return ERR_MSG_LEN;
	}

	sprintf(g_sql_str, "SELECT peer_id, status, message FROM db_monster_%d.t_pinboard_%d WHERE user_id = %u order by create_time desc limit 200;", DB_ID(user_id), TABLE_ID(user_id), user_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	service_pinboard_t service_pinboard;
	g_pack.pack(num);

	while(row !=NULL)
	{
		memset(&service_pinboard, 0, sizeof(service_pinboard_t));

		if(str2uint(&service_pinboard.peer_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert peer_id:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_pinboard.status, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert status:%s to uint8 failed!", row[1]);
			return ERR_SQL_ERR;
		}

		strncpy(service_pinboard.msg_content, row[2], sizeof(service_pinboard.msg_content));
		service_pinboard.msg_len = strlen(service_pinboard.msg_content);

		g_pack.pack((char *)(&service_pinboard), sizeof(service_pinboard_t) - sizeof(service_pinboard.msg_content) + service_pinboard.msg_len);

		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}

uint32_t query_stuff_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type,request_len,0) != 0)
	{
		return ERR_MSG_LEN;
	}

	sprintf(g_sql_str, "SELECT stuff_id, stuff_num, used_num FROM db_monster_%d.t_stuff_%d WHERE user_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	service_stuff_t service_stuff;
	g_pack.pack(num);

	while(row !=NULL)
	{
		if(str2uint(&service_stuff.stuff_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert stuff_id:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_stuff.owned_num, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert owned_num:%s to uint32 failed!", row[1]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_stuff.used_num, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "convert used_num:%s to uint32 failed!", row[2]);
			return ERR_SQL_ERR;
		}

		g_pack.pack((char *)(&service_stuff), sizeof(service_stuff));

		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}

uint32_t query_game_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
	{
		return ERR_MSG_LEN;
	}

	uint32_t *p_game_id = (uint32_t *)p_request_body;
	sprintf(g_sql_str, "SELECT level_id, max_score, max_star FROM db_monster_%d.t_game_%d WHERE user_id = %u AND game_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, *p_game_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	service_level_t service_level;
	g_pack.pack(num);

	while(row !=NULL)
	{
		if(str2uint(&service_level.level_id, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert level_id:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_level.level_score, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert level_score:%s to uint32 failed!", row[1]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_level.level_star, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "convert level_star:%s to uint32 failed!", row[2]);
			return ERR_SQL_ERR;
		}

		g_pack.pack((char *)(&service_level), sizeof(service_level));

		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}

uint32_t query_puzzle_service(i_mysql_iface *p_mysql_conn, c_memcached *p_memcached, uint32_t user_id, uint32_t msg_type, char *p_request_body, uint32_t request_len)
{
	if(check_val_len(msg_type, request_len, sizeof(uint32_t)) != 0)
	{
		return ERR_MSG_LEN;
	}

	uint32_t *p_puzzle_id = (uint32_t *)p_request_body;
	sprintf(g_sql_str, "SELECT max_score, total_score, total_num FROM db_monster_%d.t_puzzle_%d WHERE user_id = %u AND type_id = %u", DB_ID(user_id), TABLE_ID(user_id), user_id, *p_puzzle_id);

	MYSQL_ROW row = NULL;
	int result_count = p_mysql_conn->select_first_row(&row, g_sql_str);
	if(result_count < 0)
	{
		KCRIT_LOG(user_id, "sql exec failed(%s).", p_mysql_conn->get_last_errstr());
		return ERR_SQL_ERR;
	}

	uint32_t num = 0;
	service_puzzle_t service_puzzle;
	g_pack.pack(num);

	while(row !=NULL)
	{
		if(str2uint(&service_puzzle.max_score, row[0]) != 0)
		{
			KCRIT_LOG(user_id, "convert max_score:%s to uint32 failed!", row[0]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_puzzle.total_score, row[1]) != 0)
		{
			KCRIT_LOG(user_id, "convert total_score:%s to uint32 failed!", row[1]);
			return ERR_SQL_ERR;
		}

		if(str2uint(&service_puzzle.total_num, row[2]) != 0)
		{
			KCRIT_LOG(user_id, "convert level_star:%s to uint32 failed!", row[2]);
			return ERR_SQL_ERR;
		}

		service_puzzle.avg_score = service_puzzle.total_score / service_puzzle.total_num;
		g_pack.pack((char *)(&service_puzzle), sizeof(service_puzzle) - sizeof(uint32_t));

		num++;
		row = p_mysql_conn->select_next_row(true);
	}

	g_pack.pack(num, sizeof(svr_msg_header_t));

	return 0;
}



