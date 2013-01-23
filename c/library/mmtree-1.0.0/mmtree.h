/*
 * =====================================================================================
 *
 *       Filename:  mmtree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/17/2010 10:36:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  henry (韩林), henry@taomee.com
 *        Company:  TaoMee.Inc, ShangHai
 *
 * =====================================================================================
 */
#ifndef H_mmtree_H_2010_08_17
#define H_mmtree_H_2010_08_17

#include <stdarg.h>
#include "i_mmtree.h"

#define MMTREE_VERSION "1.0"

#define MMT_IFACE_ESUCCESS	0
#define MMT_IFACE_EINIT		1
#define MMT_IFACE_EMMAP		2
#define MMT_IFACE_EINSERT	3
#define MMT_IFACE_EFIND		4
#define MMT_IFACE_EOP		5

/** 
 * @brief 树中节点的结构信息
 */
typedef struct
{
	int key;		/**<@ 节点的key值*/
	int data;		/**<@ 节点中key出现的次数*/
	int left;		/**<@ 节点的左子树节点的id*/
	int right;		/**<@ 节点的右子树节点的id */
	int parent;		/**<@ 节点的父节点id*/
	int color;		/**<@ 节点的颜色*/
} __attribute__((aligned(sizeof(int)))) mt_node_t;

/** 
 * @brief 树的状态结构信息
 */
typedef struct
{
	char md5[32];		/**<@ 树文件的md5值*/
	char version[4];	/**<@ 版本信息*/
	int root_id;		/**<@ 树的根节点的ID*/
	int left;		/**<@ 树中剩下的可用节点数，当left=0时,需要给树扩充容量*/
	int current;		/**<@ 树中当前使用到的节点数 */
	int nonzero;		/**<@ 树中值非0的节点个数 */
	int total;		/**<@ 树中总共分配的节点数*/
	int qleft;		/**<@ 回收的节点个数*/
	int qfirst;		/**<@ 最先回收的节点ID*/
	int qlast;		/**<@ 最后回收的节点ID*/
} __attribute__((aligned(sizeof(int)))) mt_state_t;

class c_mmtree: public i_mmtree
{
public:
	c_mmtree();
	virtual ~c_mmtree();
	virtual int init(const char *file, int flags, uint32_t check_create_flag);	
	virtual int insert(int key, int data);
	virtual int update(int key, int data, int op);
	virtual int set(int key, int data, int op);
	virtual int get(int key, int *p_data);
	virtual int find(int key);
	virtual int remove(int key);
	virtual int get_nonzero_num(int *p_nonzero_num);
	virtual int get_node_num(int *p_node_num);
	virtual int traversal(mmtree_callback_t cb, void *p_param);
	virtual int combine_tree(const char *file, int flag);
	virtual int serialize(const char *file, const int flags);
	virtual int flush();
    virtual int output_top_data(mt_key_data_t * p_buf, uint32_t * num);
	virtual int get_last_errno();
	virtual const char* get_last_errstr();
	virtual int uninit();
	virtual int release();
private:
	int mmtree_mmap();
	int mmtree_increment();
	int mmtree_munmap();
	void mmtree_insert_color(int *p_rootid, int node_id, int lid, int rid, int uid, int pid, int g_pid, int p_pid);
	void mmtree_rotate_left(int *p_rootid, int id, int lid, int rid, int ppid);
	void mmtree_rotate_right(int *p_rootid, int id, int lid, int rid, int ppid);

	int insert_root_node(int key, int data);
	int insert_new_node(int node_id, int key, int data);
	int update_node_value(mt_node_t *p_node, int data, int op);
	int remove_node(int *p_rootid, int node_id);
	void mmt_remove_color(int *p_rootid, int id, int pid, int lid, int rid, int uid, int ppid);
	int next(int node_id);
	int find(int key, int *p_nodeid);
	static int write_to_file(int key, int data, void *p_param);
	static int set_cb(int key, int data, void *p_param);
	static int update_cb(int key, int data, void *p_param);
    static int get_node_cb(int key, int data, void *p_param);
    int compare_key_data(mt_key_data_t * d1, mt_key_data_t * d2);
    int copy_key_data(mt_key_data_t * from, mt_key_data_t * to);
    int swap_key_data(mt_key_data_t *a, mt_key_data_t *b);
    int quicksort(mt_key_data_t *p, uint32_t begin, uint32_t end, uint32_t num);
	void set_err(int err_no, const char *msg, ...);

private:
	typedef struct
	{
		int op;
		void *p_obj;
	}param_t;

    typedef struct
    {
        int index;
        mt_key_data_t * p_buf;
    } param_output_t;

	uint64_t m_size;	 	/**<@ 文件大小*/
	void *m_start;  		/**<@ 文件映射的起始地址 */
	mt_state_t *m_state;		/**<@ 记录文件状态的起始地址 */
	mt_node_t *m_map;		/**<@ 文件节点的起始地址 */
	int m_fd;			/**<@ 文件描述符 */
	int m_inited;			/**<@ 实例是否初始化标志*/
	int m_increment_num; 		/**<@ 每次扩充容量时增加的节点个数 */
	void *m_p_mtree; 		/**<@ 指向树的实例*/
	int m_errno;			/**<@ 保存本对象的错误码*/
	char m_strerr[1024];		/**<@ 保存本对象的错误描述*/
};

#endif //H_I_mmtree_H_2010_08_15 
