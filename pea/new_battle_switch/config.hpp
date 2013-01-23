#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#include "common.hpp"

int init_config(void);
void release_config(void);


inline bool is_valid_pvp_mapid(uint32_t mapid)
{
	/* TODO(zog): 是否是配置中以支持的 pvp地图id */
	return true;
}


#endif // __CONFIG_HPP__
