#ifndef __SVC_H__
#define __SVC_H__


#include "common.h"

#define DEF_SVC_CFG_PATH		"./conf/svc.conf"

/* some svc_gameid's value here, but not all */
#define SVC_GAMEID_TEST			(0)
#define SVC_GAMEID_MOLE			(1)
#define SVC_GAMEID_SEER			(2)

struct svc_gameid_map_t {
	char		name[MAX_SVC_NAME_LEN];
	uint32_t	gameid;
};



/**
 * @brief 根据 conf 文件加载业务名称与gameid的对应关系;
 * @load_count 本次被加载的条数;
 * @return -1: failed, 0: succ
 */
int load_svc_gameid_map(const char *conf_path, uint32_t *load_count);
int32_t get_cur_svcinfo_count(void);
uint32_t get_svc_gameid(const char *svc_name);

#endif /* __SVC_H__ */
