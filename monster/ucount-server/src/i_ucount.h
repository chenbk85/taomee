/* vim: set tabstop=4 softtabstop=4 shiftwidth=4: */
/**
 * @file i_ucount.h
 * @author richard <richard@taomee.com>
 * @date 2010-05-10
 */

#ifndef I_UCOUNT_H_2010_05_10
#define I_UCOUNT_H_2010_05_10

#include <stdint.h>
#include <sys/types.h>

/**
 * @brief i_ucount接口类
 */
struct i_ucount
{
public:
	enum {
		BLOCK_SIZE = 4096,
		UFILE_SIZE = 128 * 1024 * 1024
	};

	/**
	 * @brief key_opcode_t 键的操作码
	 */
	typedef enum key_opcode {
		INTERSECT,           /**< 交集 */
		EXCEPT,              /**< 差集 */
		UNION                /**< 合集 */
	} key_opcode_t;

	/**
	 * @brief value_opcode_t 值的操作码
	 */
	typedef enum value_opcode {
		SET,                 /**< 置1 */
		UNSET,               /**< 置0 */
		NOT                  /**< 反置 */
	} value_opcode_t;

	/**
	 * @brief cb_traverse_ucount_t遍历唯一数文件时的回调函数类型
	 * @param ucount 唯一数
	 * @param isset 当前唯一数所对应的值
	 * @param p_user_data 用户自定义数据
	 * @return 成功返回0，失败返回-1
	 * @note 当返回-1时，则停止遍历唯一数文件
	 */
	typedef int (*cb_traverse_t)(uint32_t ucount, uint32_t isset, void *p_user_data);

	/**
	 * @brief init_flag 初始化标记
	 */
	typedef enum init_flag {
		CREATE = 1 << 0,     /**< 文件不存在时创建文件，文件存在时打开文件 */
		EXCL   = 1 << 1,     /**< EXCL和CREATE一起使用时才有意义。当文件存在时指定EXCL则出错 */
	} init_flag_t;

	/**
	 * @brief 初始化
	 * @param p_ufile_dir 存放唯一数文件的目录
	 * @param swap_size 交换空间的大小
	 * @param block_size 块的大小
	 * @return 成功返回0，失败返回－1
	 */
	virtual int init(const char *p_ucount_path_name, uint32_t flags, mode_t mode) = 0;

	/**
	 * @brief 设置唯一数所对应的值
	 * @param p_ufile_name 要设置的唯一数文件
	 * @param unumber 唯一数
	 * @param uvalue 唯一数所对应的值
	 * @param oper 对唯一值所进行的操作
	 * @return 成功返回当前唯一数文件已设置的唯一数的总个数，失败返回-1
	 * @note 如果唯一值超出了所能存储最大值，则相应的值会被截取
	 */
	virtual int set(uint32_t ucount, value_opcode_t value_opcode) = 0;

	/**
	 * @brief 获取唯一数所对应的值
	 * @param p_ufile_name 唯一数文件
	 * @param ucount 唯一数
	 * @return 成功返回相应的唯一数是否被置1，失败返回-1
	 */
	virtual int get(uint32_t ucount) = 0;

    /**
     * @brief set_on_noucount 判断数字是否在ucount里面,不在就设置一下
     *
     * @param ucount
     *
     * @return
     */
    virtual int set_on_noucount(uint32_t ucount, value_opcode_t value_opcode) = 0;

	/**
	 * @brief 获取所对应的值不为0的所有唯一数的总个数
	 * @param p_ufile_name 唯一数文件
	 * @param p_unumber_count 唯一数的总个数
	 * @return 成功返回0，失败返回-1
	 */
	virtual int get() = 0;

	/**
	 * @brief 遍历唯一数文件
	 * @param p_ufile_name 唯一数文件
	 * @param traverse_ucount 回调函数
	 * @param p_data 用户数据
	 * @param skip_zero 是否调过0
	 * @return 成功返回0，失败返回-1
	 */
	virtual int traverse(cb_traverse_t cb_traverse, void *p_user_data, int skip_zero) = 0;

	virtual int merge(const char *p_ucount_path_name, key_opcode_t key_opcode) = 0;

	virtual int merge(i_ucount *p_ucount, key_opcode_t key_opcode) = 0;


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

/**
 * @brief 创建i_ucount接口的实例
 */
int create_ucount_instance(i_ucount **pp_instance);

#endif /* I_UCOUNT_H_2010_05_10 */

