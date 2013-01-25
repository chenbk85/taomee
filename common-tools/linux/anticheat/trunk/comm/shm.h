#ifndef __SHM_H__
#define __SHM_H__

#include <sys/types.h>


/*
 * -----------------------------------------------------------
 * exported enums
 * -----------------------------------------------------------
 */
enum get_shm_info_result_e {
	/* shm 还没建立 */
	get_shm_info_nonexist		= 0,
	/* shm 已经建立 (已经获取了shmid, size 等信息) */
	get_shm_info_exist			= 1,
};

enum create_noexist_opt_e {
	create_noexist_no	= 0,
	create_noexist_yes	= 1,
};

enum init_exist_opt_e {
	init_exist_no	= 0,
	init_exist_yes	= 1,
};


/*
 * -----------------------------------------------------------
 * exported structers
 * -----------------------------------------------------------
 */
struct shm_mgr_t {
	/*! 0: 表示无效 */
	key_t	key;
	int		shmid;
	size_t	size;
	void	*shm;
};


/*
 * -----------------------------------------------------------
 * helper functions
 * -----------------------------------------------------------
 */



/*
 * -----------------------------------------------------------
 * exported interface
 * -----------------------------------------------------------
 */

/*
 * @brief 获得一个初始化成 0 的 shm segment (也许已经创建或需要创建), 并用init_buf初始化它;
 * 注意:
 * (1) 如果 init_buf_len > size, 则仅把 init_buf 的前 init_buf_len字节 初始化到新创建的 shm 中;
 * (2) 不允许创建 IPC_PRIVATE 类型的 shm (key不能是0);
 *
 * @create_nonexist 当 key 的 shm 不存在时, 是报错(0), 还是创建新的(1);
 * @init_exist 是否初始化已经存在的 shm: 0: 不初始化, 1: 初始化;
 * @return -1: 失败, 0: 成功
 */
int get_shm(key_t key, size_t size, int create_nonexist, int init_exist, void *init_buf, size_t init_buf_len);

/*
 * @brief 创建一个初始化成 0 的 shm, create_xxx 具有排他性, 用于新创建,
 * 即: 若在system-wide内有相同key的shm, 则返回错误
 */
int create_shm(key_t key, size_t size);

/*
 * @brief 创建一个初始化成 0 的 shm, create_xxx 具有排他性, 用于新创建,
 * 并用 init_buf 来初始化新创建的 shm;
 * 注意:
 * 1. 如果 init_buf_len > size,
 * 		则仅把 init_buf 的前 init_buf_len字节 初始化到新创建的 shm 中;
 *
 * 2. 如果 key == IPC_PRIVATE (0); 则是创建私有shm
 * 		(其它进程无法attach, 并只能通过shmid来删除)
 *
 * 3. 若在system-wide内有相同key的shm, 则返回错误
 */
int create_shm_init(key_t key, size_t size, void *init_buf, size_t init_buf_len);

/**
 * @brief attach 到 new_shmid 的 shm 上, 同时 detach old_shmid 的 shm;
 * 即:
 * (1) 当 key 相同时, 其它参数: shmid, size, shm 都必须与key保持一致,
 * 		其中有任何一个参数与 key 的 shm_info 中的不相同, 则 update 失败;
 * (2) 而当 key 不相同时, shmid 通常就不同, 此时就会把 size, shm 都同步成与key保持一致;
 * 	   
 * 注意: 由于不允许申请 IPC_PRIVATE 类型的 shm,
 * 		 所以第一次 update_attach 的时候 (此时: mgr->key == 0), 要给 shm_mgr->key 赋值;
 *
 * @return -1: 失败, 0: 成功
 */
int update_shm_attach(struct shm_mgr_t *shm_mgr);

/**
 * @brief 获取 system-wide 是 key 的 shm 的信息
 *
 * @return -1: 出错, 0: 检查正常返回 (检查结果在 exist 中);
 *
 * @exist:
 * 		0: key 的 shm 还没建立;
 * 		1: key 的 shm 已经建立;
 *
 * @elder_shmid: 仅当 shm 存在, 且 elder_shmid 指针不为 NULL 时, 存储已建立的 shm id;
 * @elder_size: 仅当 shm 存在, 且 elder_size 指针不为 NULL 时, 存储已建立的 shm 大小;
 */
int get_shm_info(key_t key, int *exist, int *shmid, size_t *size);


/*
 * -----------------------------------------------------------
 * inline interface functions
 * -----------------------------------------------------------
 */

inline void init_shm_mgr(struct shm_mgr_t *mgr, key_t key);
inline int get_shm_nocreate_noinitexist(key_t key, size_t size);
inline int get_shm_nocreate_initexist(key_t key, size_t size, void *init_buf, size_t init_buf_len);
inline int get_shm_create_noinitexist(key_t key, size_t size, void *init_buf, size_t init_buf_len);
inline int get_shm_create_initexist(key_t key, size_t size, void *init_buf, size_t init_buf_len);
inline int get_shm_nocreate(key_t key, size_t size, int init_exist, void *init_buf, size_t init_buf_len);
inline int get_shm_create(key_t key, size_t size, int init_exist, void *init_buf, size_t init_buf_len);
inline int get_shm_noinitexist(key_t key, size_t size, int create_exist, void *init_buf, size_t init_buf_len);
inline int get_shm_initexist(key_t key, size_t size, int create_exist, void *init_buf, size_t init_buf_len);

#endif /* __SHM_H__ */
