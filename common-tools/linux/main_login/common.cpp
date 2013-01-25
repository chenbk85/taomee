#include <cstdio>
#include <cstring>

extern "C" {
#include <libtaomee/log.h>
#include <libtaomee/timer.h>
#include <libtaomee/conf_parser/config.h>

#include <async_serv/net_if.h>
}

#include "user_manager.hpp"
#include "common.hpp"

#pragma pack(1)

struct old_warning_pkg_t {
	/* package length */
	uint32_t	len;
	/* sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	/* command id */
	uint16_t	cmd;
	/* errno */
	uint32_t	ret;
	/* user id */
	userid_t	id;

    uint32_t	project_id; //项目类型;mole:1 ,seer:2
    uint32_t	server_id; // 服务类型:db:1 ,swtich:2...
    uint32_t	userid; //用户ID
    uint32_t	cmdid; //cmdid(操作的命令号)
};

struct warning_pkg_t {
	/* package length */
	uint32_t	len;
	/* sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	/* command id */
	uint16_t	cmd;
	/* errno */
	uint32_t	ret;
	/* user id */
	userid_t	id;

	char		svr[120];
	userid_t	uid;
	uint32_t	cmdid;
	uint32_t	hex;
	char		ip[16];
	uint32_t	burst_limit;
	uint32_t	warning_interval;
	char		phone_numbers[256];
};

#pragma pack()

const char* stat_file;
static const char* phone_nums;
static const char* prj_name;

static int project_id;
static int warning_fd;
static sockaddr_in warning_sockaddr;

void send_warning(const usr_info_t* p, int server_id, int cmdid)
{
	static old_warning_pkg_t pkg;

	pkg.len        = sizeof(pkg);
	pkg.seq        = 0;
	pkg.cmd        = 0xF000;
	pkg.ret        = 0;
	pkg.id         = p ? p->uid : 0;
	pkg.project_id = project_id;
	pkg.server_id  = server_id;
	pkg.userid     = p ? p->uid : 0;
	pkg.cmdid      = cmdid;

    sendto(warning_fd, &pkg, sizeof(pkg), 0, (sockaddr*)&warning_sockaddr, sizeof(warning_sockaddr));

    KDEBUG_LOG(0, "SEND WARNING\t[uid=%d svr=%d cmd=%X]", p ? p->uid : 0, server_id, cmdid);
}

void send_warning(const char* svr, userid_t uid, uint32_t cmd, int hex, const char* ip)
{
	static warning_pkg_t pkg;

	if (ip == 0) {
		ip = "";
	}

	pkg.len              = sizeof(pkg);
	pkg.seq              = 0;
	pkg.cmd              = 0xF101;
	pkg.ret              = 0;
	pkg.id               = uid;

	snprintf(pkg.svr, sizeof(pkg.svr), "%s.%s", prj_name, svr);
	pkg.uid              = uid;
	pkg.cmdid            = cmd;
	pkg.hex              = hex;
	strncpy(pkg.ip, ip, sizeof(pkg.ip));
	pkg.burst_limit      = 10;
	pkg.warning_interval = 15 * 60;
	strncpy(pkg.phone_numbers, phone_nums, sizeof(pkg.phone_numbers));

    sendto(warning_fd, &pkg, sizeof(pkg), 0, (sockaddr*)&warning_sockaddr, sizeof(warning_sockaddr));

    KDEBUG_LOG(0, "SEND WARNING\t[svr=%s uid=%d cmd=%X ip=%s]", svr, uid, cmd, ip);
}

void init_warning_system()
{
	warning_fd = create_udp_socket(&warning_sockaddr, config_get_strval("warning_ip"),
									config_get_intval("warning_port", 0));

	project_id = config_get_intval("project_id", -1);
	// 兼容老的配置文件
	if (project_id == -1) {
		project_id = config_get_intval("business_type", -1);
	}

	if (warning_fd == -1) {
		WARN_LOG("failed to init warning sys: ip=%s port=%d", config_get_strval("warning_ip"),
					config_get_intval("warning_port", 0));
	}

	prj_name   = config_get_strval("project_name");
	if (prj_name == 0) {
		prj_name = "unspecified";
		WARN_LOG("project name unspecified!");
	}

	phone_nums = config_get_strval("phone_numbers");
	if (phone_nums == 0) {
		phone_nums = "13761071357";
		WARN_LOG("no phone numbers inputted!");
	}
}

