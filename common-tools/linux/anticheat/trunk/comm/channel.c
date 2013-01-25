#include <stdio.h>
#include <string.h>

#include "common.h"
#include "struct.h"
#include "shm.h"
#include "channel.h"



/** @return 0: not-empty, 1: empty */
int is_channel_empty(struct channel_t *chl, uint8_t is_req)
{
	volatile uint32_t head, tail;

	if (is_req == REQUEST_CHANNEL) {
		head = chl->reqhead;
		tail = chl->reqtail;
	} else {
		head = chl->resphead;
		tail = chl->resptail;
	}

	return (head == tail) ? 1 : 0;
}

/**
 * @briel 获取 mgr->key 的 shm, 并将相应的 shm_info(shmid, size, ...) 初始化给 mgr;
 *
 * @mgr channel的shm的管理器, 调用者要负责 mgr->key 和 mgr->size 的赋值;
 * @create_noexist 当 mgr->key 的 shm:
 * 	不存在时, 创建它, 即: create_noexist == 1;
 * 	不存在时, 不创建它, 并直接返回失败, 即: create_noexist == 0;
 *
 * @return -1: 失败, 0: 成功
 */
int get_channel_shm(struct shm_mgr_t *mgr, int create_noexist)
{
	size_t size = sizeof(struct channel_t);
	if (get_shm_noinitexist(mgr->key, size, create_noexist, NULL, 0) == -1) {
		return -1;
	}

	if (update_shm_attach(mgr) == -1) { return -1; }

	return 0;
}

/**
 * @brief 初始化channel
 */
void init_channel(struct channel_t * chl)
{
	chl->reqhead = chl->reqtail = 0;
	chl->resphead = chl->resptail = 0;
}

/**
 * @brief 初始化 mgr 中 shm 指向的 channel
 */
void init_channel_by_shm_mgr(struct shm_mgr_t *mgr)
{
	init_channel((struct channel_t *)(mgr->shm));
}

int add_to_channel_shm_mgr(struct shm_mgr_t *mgr, uint32_t len, char *data, uint8_t is_req)
{
	struct channel_t *chl = (struct channel_t *)(mgr->shm);
	return add_to_channel(chl, len, data, is_req);
}

/**
 * @briel 将数据放入 channel
 * @chl channel
 * @len 数据(data)长度
 * @data 数据
 * @is_req 是放入 reqbuf 还是放入 respbuf
 *
 * @return -1: 失败, 0: 成功
 */
int add_to_channel(struct channel_t *chl, uint32_t len, char *data, uint8_t is_req)
{
	char *buf;
	uint32_t left;
	uint32_t add_len;
	struct channel_block_t *blk;
	volatile uint32_t head, tail;
	volatile uint32_t *phead, *ptail;

	if (is_req == REQUEST_CHANNEL) {
		buf = chl->reqbuf;
		phead = &(chl->reqhead);
		ptail = &(chl->reqtail);
	} else {
		buf = chl->respbuf;
		phead = &(chl->resphead);
		ptail = &(chl->resptail);
	}
	head = *phead;
	tail = *ptail;

	if (head >= CHANNEL_BUFFER_LEN || tail >= CHANNEL_BUFFER_LEN) {
		/* fatal error(BUG?), TODO: send warning */
		*phead = *ptail = 0;
		return -1;
	}

	add_len = sizeof(struct channel_block_t) + len;
	if (head <= tail) {
		/* |    head*********tail    | */
		left = CHANNEL_BUFFER_LEN - tail;
		if ((left == add_len) && (head == 0)) {
			/* shm is insufficient(1) TODO: send warning */
			return -1;
		}
		if (left < add_len) {
			if (head <= add_len) {
				/* shm is insufficient(2) TODO: send warning */
				return -1;
			}
			*(buf + tail) = ETX;
			tail = 0;
		}
	} else {
		/* |*****tail       head******| */
		if (head - tail <= add_len) {
			/* shm is insufficient(3) TODO: send warning */
			return -1;
		}
	}

	blk = (struct channel_block_t *)(buf + tail);
	blk->stx = STX;
	blk->len = len;
	memcpy(blk->data, data, len);

	*ptail = (tail + add_len) % CHANNEL_BUFFER_LEN;

	return 0;
}

int get_from_channel_shm_mgr(struct shm_mgr_t *mgr, uint32_t *len, char *data, uint8_t is_req)
{
	struct channel_t *chl = (struct channel_t *)(mgr->shm);
	return get_from_channel(chl, len, data, is_req);
}

/**
 * @briel 从 channel 中取出数据
 * @chl channel
 * @len 数据(data)长度
 * 注意: len 在进入该接口之前表示 data 缓冲区的长度;
 * 		 而在返回时:
 * 		 (1) 若返回  0: len 表示的是实际获取的 data 的长度;
 * 		 (2) 若返回 -1: len 和 data 中的数据都是没有意义的;
 *
 * @data 数据
 *
 * @is_req 是放入 reqbuf 还是放入 respbuf
 *
 * @return -1: 失败, 0: 成功
 */
int get_from_channel(struct channel_t *chl, uint32_t *len, char *data, uint8_t is_req)
{
	char *buf;
	uint32_t max_one_blk_len, declare_len;
	struct channel_block_t *blk;
	volatile uint32_t head, tail;
	volatile uint32_t *phead, *ptail;

	if (is_req == REQUEST_CHANNEL) {
		buf = chl->reqbuf;
		phead = &(chl->reqhead);
		ptail = &(chl->reqtail);
	} else {
		buf = chl->respbuf;
		phead = &(chl->resphead);
		ptail = &(chl->resptail);
	}
	head = *phead;
	tail = *ptail;

	if (head >= CHANNEL_BUFFER_LEN || tail >= CHANNEL_BUFFER_LEN) {
		/* fatal error(BUG?), TODO: send warning */
		*phead = *ptail = 0;
		return -1;
	}

	if (head == tail) { /* empty */
		*len = 0;
		return 0;
	}

	if (head > tail && *(buf + head) == ETX) {
		*phead = head = 0;
		if (head == tail) {
			*len = 0;
			return 0;
		}
	}

	if (head < tail) {
		max_one_blk_len = tail - head;
	} else {
		max_one_blk_len = CHANNEL_BUFFER_LEN - head;
	}

	if (max_one_blk_len < sizeof(struct channel_block_t)) {
		/* BUG! TODO: send warning */
		*phead = tail;
		return -1;
	}

	blk = (struct channel_block_t *)(buf + head);
	if (blk->stx != STX) {
		/* BUG! TODO: send warning */
		*phead = tail;
		return -1;
	}
	declare_len = sizeof(struct channel_block_t) + blk->len;
	if (max_one_blk_len < declare_len || *len < blk->len) {
		/* BUG! TODO: send warning */
		*phead = tail;
		return -1;
	}

	*len = blk->len;
	memcpy(data, blk->data, *len);

	*phead = (head + declare_len) % CHANNEL_BUFFER_LEN;

	return 0;
}
