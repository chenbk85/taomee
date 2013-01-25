#ifndef __CHANNEL_H__
#define __CHANNEL_H__


#include <stdint.h>


#define CHANNEL_BUFFER_LEN	(10*1024*1024)



#pragma pack(1)
struct channel_block_t {
	char		stx;
	uint32_t	len;
	char		data[];
};
#pragma pack()

struct channel_t {
	volatile uint32_t	reqhead;
	volatile uint32_t	reqtail;
	char				reqbuf[CHANNEL_BUFFER_LEN];

	volatile uint32_t	resphead;
	volatile uint32_t	resptail;
	char				respbuf[CHANNEL_BUFFER_LEN];
};



/** @return 0: not-empty, 1: empty */
int is_channel_empty(struct channel_t *chl, uint8_t is_req);

/**
 * @briel 获取 mgr->key 的 shm, 并将相应的 shm_info(shmid, size, ...) 初始化给 mgr;
 *
 * @mgr channel的shm的管理器, 在第一次获取时, 要先把其中的 key 赋值(否则会获取失败);
 * @flag 若mgr->key的shm尚不存在, 是否:
 * 	创建它, 即: flag == 1;
 * 	还是当shm不存在时, 不创建, 就直接返回失败, 即: flag == 0;
 *
 * @return -1: 失败, 0: 成功
 */
int get_channel_shm(struct shm_mgr_t *mgr, int create_noexist);

/**
 * @brief 初始化channel
 */
void init_channel(struct channel_t * chl);

/**
 * @brief 初始化 mgr 中 shm 指向的 channel
 */
void init_channel_by_shm_mgr(struct shm_mgr_t *mgr);

int add_to_channel_shm_mgr(struct shm_mgr_t *mgr, uint32_t len, char *data, uint8_t is_req);

/**
 * @briel 将数据放入 channel
 * @chl channel
 * @len 数据(data)长度
 * @data 数据
 * @is_req 是放入 reqbuf 还是放入 respbuf
 *
 * @return -1: 失败, 0: 成功
 */
int add_to_channel(struct channel_t *chl, uint32_t len, char *data, uint8_t is_req);

int get_from_channel_shm_mgr(struct shm_mgr_t *mgr, uint32_t *len, char *data, uint8_t is_req);

/**
 * @briel 从 channel 中取出数据
 * @chl channel
 * @len 数据(data)长度
 * @data 数据
 * @is_req 是放入 reqbuf 还是放入 respbuf
 *
 * @return -1: 失败, 0: 成功
 */
int get_from_channel(struct channel_t *chl, uint32_t *len, char *data, uint8_t is_req);


#endif /* __CHANNEL_H__ */
