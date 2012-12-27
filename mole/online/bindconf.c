// C89
#include <string.h>
// POSIX
#include <arpa/inet.h>
#include <sys/mman.h>

#include <libtaomee/conf_parser/config.h>

// Self-define
#include <libtaomee/log.h>
#include "util.h"

#include "bindconf.h"

// global varibles for bindconf.c
bind_config_t bindconf;


/**
 * load_bind_file - parse bind config file @file_name
 *
 * return: 0 on success, otherwise -1
 */
//int load_bind_file(const char* file_name)
int load_bind_file()
{
	bindconf.online_start_id = config_get_intval("online_start_id", 0);
	if (bindconf.online_start_id == 0) {
		BOOT_LOG(-1, "INVALID ONLINE START ID [%d]", 0);
	}

	bindconf.bind_num = config_get_intval("worker_num", 20);
	if (bindconf.bind_num > MAX_LISTEN_FDS) {
		BOOT_LOG(-1, "WORKER NUM OUT OF RANGE [%d]", bindconf.bind_num);
	}

	char*     online_ip     = config_get_strval("online_ip");
	in_port_t online_port   = config_get_intval("online_port", 0);
	char*     gameserv_ip   = config_get_strval("gameserv_ip");
	in_port_t gameserv_port = config_get_intval("gameserv_port", 0);
	char*     gameserv_test_ip   = config_get_strval("gameserv_test_ip");
	in_port_t gameserv_test_port = config_get_intval("gameserv_test_port", 0);
	if ((online_port == 0) || (gameserv_port == 0)) {
		BOOT_LOG(-1, "INVALID BIND PORT [%d %d]", online_port, gameserv_port);
	}

	int i;
	for (i = 0; i != bindconf.bind_num; ++i) {
		bind_config_elem_t* bc_elem = &(bindconf.configs[i]);
		bc_elem->online_id = bindconf.online_start_id + i;
		strncpy(bc_elem->bind_ip, online_ip, sizeof(bc_elem->bind_ip) - 1); // online ip
		bc_elem->bind_port = online_port; // online port
		strncpy(bc_elem->gameserv_ip, gameserv_ip, sizeof(bc_elem->gameserv_ip) - 1); // gameserv ip
		bc_elem->gameserv_port = gameserv_port; // gameserv port
		strncpy(bc_elem->gameserv_test_ip, gameserv_test_ip, sizeof(bc_elem->gameserv_test_ip) - 1); // gameserv ip
		bc_elem->gameserv_test_port = gameserv_test_port; // gameserv port
	}

	return 0;
}


