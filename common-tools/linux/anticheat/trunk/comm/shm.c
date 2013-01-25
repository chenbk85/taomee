#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "shm.h"




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
int get_shm(key_t key, size_t size, int create_nonexist, int init_exist, void *init_buf, size_t init_buf_len)
{
	void *shm = 0;
	int init_size = 0;
	int exist = 0;
	int elder_shmid = 0;
	size_t elder_size = 0;

	if (key == 0) {
		printf("error - forbid to create IPC_PRIVATE shm\n");
		return -1;
	}

	/* 尝试获取之前已经由相同的key建立起来的shm */
	get_shm_info(key, &exist, &elder_shmid, &elder_size);

	if (exist == get_shm_info_nonexist) {
		if (create_nonexist == create_noexist_no) {
			return -1;
		} else {
			/* key 的 shm 之前不存在, 因此新创建一个 */
			return create_shm_init(key, size, init_buf, init_buf_len);
		}
	}

	/* exist == get_shm_info_exist */
	if (size != elder_size) {
		/* key 的 shm 之前存在, 但大小不符合要求, 则返回错误 */
		printf("error - cannot create newsize=%zd against elder_size=%zd\n",
				size, elder_size);
		return -1;
	}

	/* key 的 shm 之前存在, 且大小符合要求 */

	/* 要求不要初始化已存在的shm */
	if (init_exist == init_exist_no) return 0;

	/* 要求初始化已存在的shm */
	shm = shmat(elder_shmid, 0, 0);
	if (shm == (void *)-1) {
		perror("error - attach elder shm failed: ");
		return -1;
	}

	if (init_buf) {
		init_size = (size < init_buf_len) ? size : init_buf_len;
		memcpy(shm, init_buf, init_size);
	} else {
		memset(shm, 0, size);
	}
	shmdt(shm);

	return 0;
}

/*
 * @brief 创建一个初始化成 0 的 shm, create_xxx 具有排他性, 用于新创建,
 * 即: 若在system-wide内有相同key的shm, 则返回错误
 */
int create_shm(key_t key, size_t size)
{
	return create_shm_init(key, size, 0, 0);
}

/*
 * @brief 创建一个初始化成 0 的 shm, create_xxx 具有排他性, 用于新创建,
 * 并用 init_buf 来初始化新创建的 shm;
 * 注意:
 * 1. 调用者要负责确认系统范围内没有 key 的 shm 存在;
 * 2. 如果 init_buf_len > size,
 * 		则仅把 init_buf 的前 init_buf_len字节 初始化到新创建的 shm 中;
 *
 * 3. 如果 key == IPC_PRIVATE (0); 则是创建私有shm
 * 		(其它进程无法attach, 并只能通过shmid来删除)
 *
 * 4. 若在system-wide内有相同key的shm, 则返回错误
 */
int create_shm_init(key_t key, size_t size, void *init_buf, size_t init_buf_len)
{
	int shmid = -1;
	void *shm = 0;
	size_t init_size = 0;

	shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0600);
	/* 0600: S_IRUSR | S_IWUSR: 可读可写 */
	if (shmid == -1) {
		fprintf(stderr, "error - do create shm failed, err(%d): %s\n",
				errno, strerror(errno));
		return -1;
	}

	shm = shmat(shmid, 0, 0);
	if (shm == (void *)-1) {
		fprintf(stderr, "error - attach shm failed, err(%d): %s\n",
				errno, strerror(errno));
		return -1;
	}

	if (init_buf) {
		init_size = (size < init_buf_len) ? size : init_buf_len;
		memcpy(shm, init_buf, init_size);
	}

	shmdt(shm);

	return 0;
}

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
int update_shm_attach(struct shm_mgr_t *shm_mgr)
{
	int shmid = 0;
	size_t size = 0;
	int exist = 0;
	void *shm = NULL;

	/* 此处不能检查 shm_mgr->size, 因为第1次 attach 时, shm_mgr->size为 0 */
	if (!shm_mgr || !shm_mgr->key) {
		printf("error - update shm: shm_mgr=%p, key=0x%x\n",
				shm_mgr, shm_mgr ? shm_mgr->key : 0);
		return -1;
	}

	get_shm_info(shm_mgr->key, &exist, &shmid, &size);
	if (exist == get_shm_info_nonexist) {
		printf("error - update shm: nonexist-shm, key=0x%x\n", shm_mgr->key);
		return -1;
	}

	if (shm_mgr->shmid == shmid) {
		if (shm_mgr->size == size) {
			/* key 的 shm 的参数 shmid 和 size 都没有变化, 因此直接返回成功 */
			return 0;
		} else { /* key 的 shm 的参数(size)有变, 返回失败 */
			printf("error - update same shmid(%d) but diff shm_segsz"
					"key=%#x, shm_mgr->size=%zd, exist_size=%zd\n",
					shm_mgr->shmid, shm_mgr->key, shm_mgr->size, size);
			return -1;
		}
	}

	/* 新 shmid (因为是新 key): 因此重新初始化 shm_info */
	shm = shmat(shmid, 0, 0);
	if (shm == (void *)-1) {
		printf("error - update new shmid(%d) of key(%#x), but shmat failed\n",
				shmid, shm_mgr->key);
		return -1;
	}

	/*
	 * 当第一次 update_attach 时,
	 * 由于 shm_mgr->shm == 0, 会导致 shmdt() 返回 -1,
	 * 但这不是错误, 因此忽略此错误
	 */
	shmdt(shm_mgr->shm);

	shm_mgr->shmid = shmid;
	shm_mgr->size = size;
	shm_mgr->shm = shm;

	return 0;
}

/**
 * @brief 获取 system-wide 是 key 的 shm 的信息
 *
 * @exist:
 * 		0: key 的 shm 还没建立;
 * 		1: key 的 shm 已经建立;
 *
 * @shmid: 仅当 shm 存在, 且 shmid 指针不为 NULL 时, 存储已建立的 shm 的 id;
 * @size: 仅当 shm 存在, 且 size 指针不为 NULL 时, 存储已建立的 shm 的 size;
 *
 * @return -1: 出错, 0: 检查正常返回 (检查结果在 exist 中);
 */
int get_shm_info(key_t key, int *exist, int *shmid, size_t *size)
{
	struct shmid_ds shm_stat;

	/* 尝试获取之前已经由相同的key建立起来的shm */
	int id = shmget(key, 0, 0);
	if (id == -1) {
		/* 无法获取shmid, shm不存在 */
		*exist = get_shm_info_nonexist;
		goto out;
	}

	if (shmctl(id, IPC_STAT, &shm_stat) == -1) {
		/* 无法获取shm_stat, shm也不存在 */
		*exist = get_shm_info_nonexist;
		goto out;
	}

	*exist = get_shm_info_exist;

	/* 如果 shm 已存在, 在获取一些信息 */
	if (shmid) {
		*shmid = id;
	}

	if (size) {
		*size = shm_stat.shm_segsz;
	}

out:
	return 0;
}


/*
 * -----------------------------------------------------------
 * inline interface functions
 * -----------------------------------------------------------
 */

inline void init_shm_mgr(struct shm_mgr_t *mgr, key_t key)
{
	mgr->key = key;
	mgr->shmid = -1;
	mgr->size = 0;
	mgr->shm = NULL;
}

inline int get_shm_nocreate_noinitexist(key_t key, size_t size)
{
	return get_shm(key, size, create_noexist_no, init_exist_no, NULL, 0);
}

inline int get_shm_nocreate_initexist(key_t key, size_t size, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_noexist_no, init_exist_yes, init_buf, init_buf_len);
}

inline int get_shm_create_noinitexist(key_t key, size_t size, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_noexist_yes, init_exist_no, init_buf, init_buf_len);
}

inline int get_shm_create_initexist(key_t key, size_t size, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_noexist_yes, init_exist_yes, init_buf, init_buf_len);
}

inline int get_shm_nocreate(key_t key, size_t size, int init_exist, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_noexist_no, init_exist, init_buf, init_buf_len);
}

inline int get_shm_create(key_t key, size_t size, int init_exist, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_noexist_yes, init_exist, init_buf, init_buf_len);
}

inline int get_shm_noinitexist(key_t key, size_t size, int create_exist, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_exist, init_exist_no, init_buf, init_buf_len);
}

inline int get_shm_initexist(key_t key, size_t size, int create_exist, void *init_buf, size_t init_buf_len)
{
	return get_shm(key, size, create_exist, init_exist_yes, init_buf, init_buf_len);
}
