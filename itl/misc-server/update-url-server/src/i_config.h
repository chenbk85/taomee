/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_config.h
 * @author richard <richard@taomee.com>
 * @date 2010-03-10
 */

#ifndef I_CONFIG_H_2010_03_10
#define I_CONFIG_H_2010_03_10

#include <limits.h>

/**
 * @struct i_config
 * @brief 配置信息接口类
 */
struct i_config
{
public:
	/**
	 * @brief 初始化
	 * @param config_file_list 配置文件列表
	 * @param config_file_count 配置文件的个数
	 * @return 成功返回0，失败返回－1
	 */
	virtual int init(const char (*config_file_list)[PATH_MAX], int config_file_count) = 0;

    /**
     * @brief 获取配置信息
     * @param section 配置的节名
     * @param name 配置的名称
	 * @param buffer 接收配置值的缓冲区
	 * @param buffer_count 接收配置值的缓冲区的长度
     * @return 失败返回-1，成功时如果buffer等于NULL，返回相应配置值的长度，
	 *                           如果buffer不等于NULL，把相应配置值存入buffer后返回0
     */
	virtual int get_config(const char *section, const char *name, char *buffer, int buffer_count) const = 0;
    
	/**
     * @brief 设置配置信息
     * @param section 配置的节名
     * @param name 配置的名称
	 * @param value 相应配置的值
	 * @return 成功返回0，失败返回－1
     */
	virtual int set_config(const char *section, const char *name, const char *value) = 0;
	
	/**
	 * @brief 反初始化
	 * @return 成功返回0，失败返回－1
	 */
	virtual int uninit() = 0;
	
	/**
	 * @brief 释放自己
	 * @return 成功返回0，失败返回－1
	 */
	virtual int release() = 0;
};

int create_config_instance(i_config** pp_instance);

#endif //I_CONFIG_H_201_03_10

