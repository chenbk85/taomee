#include <stdio.h>
#include <stdlib.h>
#include <confuse.h>
#include <string.h>
#include <errno.h>

#include "svc.h"

int32_t svcinfo_enable;
static int32_t cur_svcinfo_count;
int32_t cur_svcinfo_count_shadow;
struct svc_gameid_map_t sg_map[MAX_SVC_NUM];
struct svc_gameid_map_t sg_map_shadow[MAX_SVC_NUM];

int svcinfocmp(const void *si1, const void *si2)
{
	const struct svc_gameid_map_t *a = (const struct svc_gameid_map_t *)(si1);
	const struct svc_gameid_map_t *b = (const struct svc_gameid_map_t *)(si2);
	return ((int32_t)a->gameid - (int32_t)b->gameid);
}

/* return: -1: invalid, 0: valid */
int cb_validate_svcinfo(cfg_t *cfg, cfg_opt_t *opt)
{
	char *name;
	/* only validate the last svcinfo */
	cfg_t *sec = cfg_opt_getnsec(opt, cfg_opt_size(opt) - 1);
	if(!sec) {
		cfg_error(cfg, "section is NULL?!");
		return -1;
	}

	if (cfg_title(sec) == NULL) {
		cfg_error(cfg, "every svcinfo must have a name, but %d seems not",
				cfg_opt_size(opt) - 1);
		return -1;
	}

	name = cfg_getstr(sec, "name");
	if(name == 0 || strlen(name) >= MAX_SVC_NAME_LEN) {
		cfg_error(cfg, "name option must be set for svcinfo '%s'"
				" or strlen(name)(%zd) >= len(%u)",
				cfg_title(sec), strlen(name), MAX_SVC_NAME_LEN);
		return -1;
	}

	if(cfg_getint(sec, "gameid") <= 0) {
		cfg_error(cfg, "gameid option must be set for svcinfo '%s'",
				cfg_title(sec));
		return -1;
	}

	return 0;
}

/**
 * @brief 根据 conf 文件加载业务名称与gameid的对应关系;
 * @load_count 本次被加载的条数;
 * @return -1: failed, 0: succ
 */
int load_svc_gameid_map(const char *conf_path, uint32_t *load_count)
{
	uint32_t n, last;
	int i, ret;
	cfg_t *si;
	struct svc_gameid_map_t *sg;

    static cfg_opt_t svcinfo_opts[] = {
        CFG_STR("name", 0, CFGF_NONE),
        CFG_INT("gameid", 0, CFGF_NONE),
        CFG_END()
    };

    cfg_opt_t opts[] = {
        CFG_SEC("svcinfo", svcinfo_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()
    };

	cfg_t *cfg = cfg_init(opts, CFGF_NONE);

	/* set a validating callback function for svcinfo sections */
	cfg_set_validate_func(cfg, "svcinfo", &cb_validate_svcinfo);

	ret = cfg_parse(cfg, conf_path ? conf_path : DEF_SVC_CFG_PATH);
	if(ret == CFG_FILE_ERROR) {
		fprintf(stderr, "Access error, conf: %s, err: %s\n",
				conf_path ? conf_path : DEF_SVC_CFG_PATH, strerror(errno));
		return -1;
	} else if(ret == CFG_PARSE_ERROR) {
		fprintf(stderr, "Parse error, conf: %s\n",
				conf_path ? conf_path : DEF_SVC_CFG_PATH);
		return -1;
	}

	memset(sg_map_shadow, 0, sizeof(sg_map_shadow));

	n = cfg_size(cfg, "svcinfo");
	for(i = 0; i < n; i++) {
		if (i == MAX_SVC_NUM) break;

		si = cfg_getnsec(cfg, "svcinfo", i);
		sg = &(sg_map_shadow[i]);
		snprintf(sg->name, sizeof(sg->name), "%s", cfg_getstr(si, "name"));
		sg->gameid = cfg_getint(si, "gameid");
		cur_svcinfo_count_shadow++;
	}
	cfg_free(cfg);

	/* sort sg */
	qsort(sg_map_shadow, cur_svcinfo_count_shadow, sizeof(sg_map_shadow[0]), svcinfocmp);
	last = 0;
	for (i = 0; i < cur_svcinfo_count_shadow; i++) {
		sg = &(sg_map_shadow[i]);
		if (sg->gameid == last) {
			fprintf(stderr, "dup gameid: %u in conf: %s\n", last,
					conf_path ? conf_path : DEF_SVC_CFG_PATH);
			return -1;
		}

		if (sg->gameid > MAX_SVC_NUM) {
			fprintf(stderr, "gameid: %u > max(%u) in conf: %s\n",
					sg->gameid, MAX_SVC_NUM,
					conf_path ? conf_path : DEF_SVC_CFG_PATH);
			return -1;
		}
		last = sg->gameid;
	}

	svcinfo_enable = 0;
	memcpy(sg_map, sg_map_shadow, sizeof(sg_map));
	cur_svcinfo_count = cur_svcinfo_count_shadow;
	if (load_count) {
		*load_count = cur_svcinfo_count;
	}
	svcinfo_enable = 1;

	return 0;
}

int32_t get_cur_svcinfo_count(void)
{
	return cur_svcinfo_count;
}

uint32_t get_svc_gameid(const char *svc_name)
{
	if (svcinfo_enable == 0 || cur_svcinfo_count == 0
		|| strlen(svc_name) > MAX_SVC_NAME_LEN) return 0;

	int i;
	struct svc_gameid_map_t *sg;
	for (i = 0; i < cur_svcinfo_count; i++) {
		sg = &(sg_map[i]);
		if (strlen(sg->name) != strlen(svc_name)) continue;
		if (!strncmp(sg->name, svc_name, sizeof(sg->name))) {
			return sg->gameid;
		}
	}

	return 0;
}
