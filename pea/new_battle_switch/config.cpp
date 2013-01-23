#include "config.hpp"



/**
 * @brief 加载 so 的所有配置
 *
 * return: -1: 错误, 0: 成功;
 */
int init_config(void)
{
/* MYTEST(zog): 测试 不分时log 和 TRACE_TLOG */
#if 1
	SET_TIME_SLICE_SECS(0);
	SET_LOG_LEVEL(tlog_lvl_trace);
#endif

	return 0;
}

/**
 * @brief 释放配置数据
 */
void release_config(void)
{

}
