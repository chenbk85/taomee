/*
 * =====================================================================================
 *
 *       Filename:  i_mmtree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/15/2010 02:39:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_I_mmtree_H_2010_08_15
#define H_I_mmtree_H_2010_08_15

#include <stdint.h>

/** 
 * @brief 遍历树时的回调函数
 */
typedef int (*mmtree_callback_t)(int key, int data, void *p_param);

/**
 * @brief 用于输出数据的结构
 */
typedef struct
{
    int key;
    int data;
} mt_key_data_t;
/** 
 * @brief +
 */
#define MMT_OP_ADD	1

/** 
 * @brief -
 */
#define MMT_OP_SUB	2

/** 
 * @brief *
 */
#define MMT_OP_MUL	3

/** 
 * @brief /
 */
#define MMT_OP_DIV	4

#define MMT_OP_MAX	5
#define MMT_OP_MIN	6
#define MMT_OP_SET	7


/** 
 * @brief 合并两颗树时指定的合并标志
 */

/** 
 * @brief键不存在的情况，插入新的键 
 */
#define MMT_KEY_ADD	 1

/** 
 * @brief 对于已经存在的键，对其值进行加操作
 */
#define MMT_DATA_ADD	 2

/** 
 * @brief 对于已经存在的键，对其值进行减操作
 */
#define MMT_DATA_SUB 	4

/** 
 * @brief 对于已经存在的键，对其值进行减操作
 */
#define MMT_DATA_MUL	8

/** 
 * @brief 对于已经存在的键，对其值进行减操作
 */
#define MMT_DATA_DIV	16

struct i_mmtree
{
public:

	/** 
	 * @brief 初始化接口实例
	 * 
	 * @param file  接口对应的文件名
	 * @param flags 文件打开标志
	 * @param check_create_flag 文件已经存在时，指示是否验证文件正确性0不验证 1验证
	 * 			    文件不存在时，指示文件的权限 
	 *
	 * @return 0success -1failed 
	 */
	virtual int init(const char *file, int flags, uint32_t check_create_flag) = 0;	

	/** 
	 * @brief 向树中插入一个节点，如果节点已经存在，返回错误
	 * 
	 * @param key 要插入的键值
	 * @param data 要插入的键对应的值
	 * 
	 * @return 0success -1failed 
	 */
	virtual int insert(int key, int data) = 0;

	/** 
	 * @brief 更新节点的值,如果节点不存在，返回错误
	 * 
	 * @param key 要更新的节点的键
	 * @param data 键对应的值
	 * @param op 更新方式 (1:+ 2:- 3:* 4:/ 5:max 6:min 7:set)
	 * 
	 * @return 0success -1failed
	 */
	virtual int update(int key, int data, int op) = 0;

	/** 
	 * @brief 设置节点的值,如果节点不存在，则插入新的节点 
	 * 
	 * @param key 节点的键值
	 * @param data 键对应的值
	 * @param op 节点已经存在的情况下，设置的方式(1:+ 2:- 3:* 4:/ 5:max 6:min 7:set)
	 * 
	 * @return 0success -1failed 
	 */
	virtual int set(int key, int data, int op) = 0;

	/** 
	 * @brief 获得树中键对应的值
	 * 
	 * @param key 键
	 * @param p_data 值
	 * 
	 * @return 0success -1failed
	 */
	virtual int get(int key, int *p_data) = 0;
	/** 
	 * @brief 查找树中的键是否存在
	 * 
	 * @param key 键
	 * 
	 * @return 0success -1failed
	 */
	virtual int find(int key) = 0;

	/** 
	 * @brief 移除树种键值等于key的节点 
	 * 
	 * @param key 键
	 * 
	 * @return 0success -1failed
	 */
	virtual int remove(int key) = 0;

	/** 
	 * @brief 获得树中值非0的节点个数 
	 * 
	 * @param p_nonzero_num 返回节点个数
	 * 
	 * @return 0success -1failed
	 */
	virtual int get_nonzero_num(int *p_nonzero_num) = 0;

	/** 
	 * @brief 获得树中节点个数 
	 * 
	 * @param p_node_num 返回节点个数
	 * 
	 * @return 0success -1failed
	 */
	virtual int get_node_num(int *p_node_num) = 0;

	/** 
	 * @brief 遍历树 
	 * 
	 * @param cb 遍历树时对每个节点调用的回调函数
	 * @param p_param回调函数的参数
	 * 
	 * @return 0success -1failed
	 */
	virtual int traversal(mmtree_callback_t cb, void *p_param) = 0;

	/** 
	 * @brief 合并两颗树
	 * 
	 * @param file 被合并的树文件名
	 * @param flag 合并标志(KEY_ADD DATA_ADD DATA_SUB)
	 * 
	 * @return 0success -1failed
	 */
	virtual int combine_tree(const char *file, int flag) = 0;

	/**
	 * @brief 将树序列化到文件中
	 *
	 * @param file 输出的文件名
     *
     * @param flags 文件打开标志
	 *
	 * @return 0success -1failed
 	*/
	virtual int serialize(const char *file, const int flags) = 0;	

	/** 
	 * @brief  将内存文件映射到磁盘中去
	 * 
	 * @return 0success -1failed
	 */
	virtual int flush() = 0;

    /** 
     * @brief output_topdata 将mmtree中非0的key和data输出，并按照其data值排序，从大到小取前num个，保存到p_buf中
     * 如果mmtree中非0数据的个数node_count不足num个，则将num修改为node_count，并按上述方式输出
     * 
     * @param p_buf
     * @param num
     * 
     * @return 0成功，-1失败
     */
    virtual int output_top_data(mt_key_data_t * p_buf, uint32_t * num) = 0;
	/** 
	 * @brief 获得最后一次错误的错误码
	 *  
	 * @return 错误码
	 */
	virtual int get_last_errno() = 0;

	/** 
	 * @brief 获得最后一次错误的错误描述符
	 * 
	 * @return 错误描述 
	 */
	virtual const char* get_last_errstr() = 0;

	/** 
	 * @brief 反初始化实例 
	 * 
	 * @return 0success -1failed
	 */
	virtual int uninit() = 0;

	/** 
	 * @brief 释放实例
	 * 
	 * @return 0success -1failed
	 */
	virtual int release() = 0;
};

int create_mmtree_instance(i_mmtree **pp_instance);

#endif //H_I_mmtree_H_2010_08_15 
