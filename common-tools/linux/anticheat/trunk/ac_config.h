#ifndef __AC_CONFIGD_H__
#define __AC_CONFIGD_H__


#include <stdint.h>
#include "common.h"


#define SVC_TEST_NAME		"test"
#define SVC_TEST_GAMEID		(0)

#define DEF_CFG_PATH		"./conf/test.cfgd.conf"


struct ac_cfgd_config_t {
    /** 是否是后台进程 */
    cfg_bool_t  background;
    /** 是否是 debug 模式(debug不关闭0,1,2) */
    cfg_bool_t  debug_mode;

    /** 负责的业务 */
    char        svc_name[MAX_SVC_NAME_LEN];
    /** 业务id */
	uint32_t	svc_gameid;

	/** config_center 地址(字符串) */
	char		cc_ip[INET_ADDRSTRLEN];
	/** config_center 端口号(本机序) */
	uint16_t	cc_port;

	char		cc_my_user[MAX_DB_USER_LEN];
	char		cc_my_passwd[MAX_DB_PASSWD_LEN];
	char		cc_my_dbname[MAX_DB_NAME_LEN];
	char		cc_my_tw_tab_basename[MAX_TAB_NAME_LEN];
	char		cc_my_sw_tab_basename[MAX_TAB_NAME_LEN];
};
extern struct ac_cfgd_config_t ac_cfgd_config;



#endif /* __AC_CONFIGD_H__ */
