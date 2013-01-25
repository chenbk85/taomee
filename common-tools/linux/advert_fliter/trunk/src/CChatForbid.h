/*
 * 管理被禁言的用户
 */

#ifndef CCHATFORBID_H_
#define CCHATFORBID_H_
#include <set>
#include <map>
#include <list>
#include <vector>
#include <stdint.h>

// 被禁言用户的信息（米米号、禁言时间）
typedef struct forbid_info {
	uint32_t		userid;
	uint32_t		forbid_time;
}__attribute__((packed))forbid_info_t;

class CChatForbid {
private:
	std::set<uint32_t>			forbid_user_set;
	std::list<forbid_info_t>	forbid_user_list;

	int					switch_fd;
	char				ip[16];
	int 				port;

	int					switch_fd_2;
	char				ip_2[16];
	int 				port_2;
public:
	// 将用户加入到禁言列表中
	uint32_t add_forbid_user(uint32_t userid);
	// 取消禁言（返回被取消用户的列表）
	uint32_t pop_forbid_user(std::vector<uint32_t>& userlist);
	// 发送禁言命令到游戏服务器
	int send_pkg_forbid_user(uint32_t userid, uint32_t cmd, uint32_t sendval);
	// 重置与游戏服务器的连接
	bool try_reset_fd(int fd);
	// 检查用户是否在禁言列表中
	bool is_forbidden(uint32_t userid);
	// 初始化，建立与游戏服务器的连接
	bool init(const char* ip, int port, const char* ip_2 = NULL, int port_2 = 0);

	CChatForbid() : switch_fd(-1), port(0), switch_fd_2(-1), port_2(0) {}
};

#endif
