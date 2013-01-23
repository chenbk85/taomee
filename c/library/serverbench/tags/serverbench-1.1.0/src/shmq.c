#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include "shmq.h"
#include "daemon.h"
#include "config.h"
#include "benchapi.h"

#ifdef USE_SYSTEMV_SEM	
#include <sys/sem.h>
#define sem_destroy(sem) semctl (sem, 0, IPC_RMID);
#else
#include <semaphore.h>
#endif

#define SEM_LOCK(sem, flag)	do { \
	if (flag & LOCKED_MASK) \
		if (unlikely (sem_wait (sem))) \
			return -1; \
}while (0)

#define SEM_UNLOCK(sem, flag, result)	do { \
	if (flag & LOCKED_MASK) \
		sem_post (sem); \
	return result; \
}while (0)

struct shm_queue sendq;
struct shm_queue recvq;
inline struct shm_block* head_mb (const struct shm_queue *q) {
	return (struct shm_block*)((char*)q->addr + q->addr->head);
}
inline struct shm_block* tail_mb (const struct shm_queue *q) {
	return (struct shm_block*)((char*)q->addr + q->addr->tail);
}

static int do_shmq_create (struct shm_queue *q, key_t key)
{
#ifdef USE_SYSTEMV_SEM	
	if ((q->sem = sem_init (key)) < 0) 
		return -1;
#else
	if (sem_init (q->sem, 1, 1) == -1)
		return -1;
#endif

	q->addr = (shm_head_t*) mmap (NULL, q->length , PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON , -1 , 0);
	if (q->addr == MAP_FAILED)
		ERROR_RETURN (("mmap failed, %s", strerror (errno)), -1);
	
	q->addr->head = sizeof (shm_head_t);
	q->addr->tail = sizeof (shm_head_t);
	atomic_set (&(q->addr->blk_cnt), 0);
	return 0;
}

static int align_queue_tail (struct shm_queue* q)
{
	struct shm_block *pad;
	/*
	 *addr->head < addr->tail condition always is ok at here
	 */
	if (likely (q->addr->head >= q->addr->tail))
		return 0;

	pad = tail_mb (q);
	if (q->length - q->addr->tail < sizeof (shm_block_t) || pad->type == PAD_BLOCK)
		q->addr->tail = sizeof (shm_head_t);

	return 0;
}

static int align_queue_head (struct shm_queue* q, const struct shm_block* mb)
{
	int tail_pos = q->addr->tail;
	int head_pos = q->addr->head;
	struct shm_block* pad;

	int surplus = q->length - head_pos; 

	if (unlikely (surplus < mb->length)) {
		//queue is full
		if (unlikely (tail_pos == sizeof (shm_head_t)))
			ERROR_RETURN (("shm_queue is full,head=%d,tail=%d,mb_len=%d", head_pos, tail_pos, mb->length), -1); 
		//bug
		else if (unlikely (q->addr->tail > head_pos)) {
			ERROR_LOG ("shm_queue bug,head=%d,tail=%d,mb_len=%d,total len", 
				head_pos, tail_pos, mb->length, q->length); 

			//by jasonwang: 修复无端清空环形队列的bug
			//q->addr->tail = sizeof (shm_head_t);
			//q->addr->head = sizeof (shm_head_t);
		//no pad mb
		} else if (unlikely (surplus < sizeof (shm_block_t))) {
			q->addr->head = sizeof (shm_head_t);
		//pad mb 
		} else {
			pad = head_mb (q);
			pad->type = PAD_BLOCK;
			pad->length = surplus;
			pad->blk_id = 0;
			q->addr->head = sizeof (shm_head_t);
		}
	}

	return 0;
}

static int pop_wait (struct shm_queue* q, int flag)
{
	struct timespec tv = {0, CYCLE_WAIT_NANO_SEC};

	align_queue_tail (q);
pop_wait_again:
	//queue is empty
	while (q->addr->tail == q->addr->head) {
		if (stop == 0  && (flag & SLEEP_MASK)) 
			nanosleep (&tv, NULL);
		else
			return -1;
	}
	align_queue_tail (q);
	
	if (q->addr->tail == q->addr->head)
		goto pop_wait_again;

	return 0;
}

static int push_wait (struct shm_queue* q, struct shm_block* mb, int flag)
{
	struct timespec tv = {0, CYCLE_WAIT_NANO_SEC};

	if (align_queue_head (q, mb) == -1)
		return -1;
push_wait_again:
	//queue is full
	while (q->addr->tail > q->addr->head && q->addr->tail < q->addr->head + mb->length + 1) {
		if (stop == 0 && (flag & SLEEP_MASK)) 
			nanosleep (&tv, NULL);
		else
			return -1;
	}

	if (align_queue_head (q, mb) == -1)
		return -1;

	if (q->addr->tail > q->addr->head && q->addr->tail < q->addr->head + mb->length + 1) 
		goto push_wait_again;

	return 0;
}
/*
 *  fixme: malloc data memory, free in another
 */
int shmq_pop (struct shm_queue* q, struct shm_block **mb, int flag)
{
	struct shm_block* cur_mb;
	int ret_code = -1;
#ifdef DEBUG		
	int head_pos;
#endif
	SEM_LOCK (q->sem, flag);
	if (likely (pop_wait (q, flag) == 0)) {
		cur_mb = tail_mb (q);
#ifdef DEBUG		
		head_pos = q->addr->head;

		//tail block overflow
		if (cur_mb->length < sizeof (struct shm_block) ||
			(q->addr->tail < head_pos && q->addr->tail + cur_mb->length > head_pos) ||
			(q->addr->tail > head_pos && q->addr->tail + cur_mb->length > q->length)) {
				ERROR_LOG ("shm_queue tail=%d,head=%d,shm_block length=%d",
					q->addr->tail, head_pos, cur_mb->length);
				exit (-1);
		}
#endif		
		*mb = (struct shm_block *)malloc (cur_mb->length);
		if (unlikely (!mb)) 
			ERROR_RETURN (("malloc failed, %s", strerror (errno)), -1);

		memcpy (*mb, cur_mb, cur_mb->length);

		atomic_dec (&q->addr->blk_cnt);
		q->addr->tail += cur_mb->length;

		TRACE_LOG ("pop queue: length=%d,tail=%d,head=%d,id=%llu,count=%lu",
				cur_mb->length, q->addr->tail, q->addr->head, (*mb)->blk_id,
				atomic_read (&q->addr->blk_cnt));
		ret_code = 0;
	}

	SEM_UNLOCK (q->sem, flag, ret_code);
}
/*
 * fixme: data memory copy 
 */
int shmq_push(struct shm_queue* q, struct shm_block* mb, const char *data, int flag)
{
	int ret_code = -1;
	SEM_LOCK (q->sem, flag);
	if (likely (push_wait (q, mb, flag) == 0)) {
		char *next_mb = (char*)head_mb (q);

		*head_mb (q) = *mb;
		if (likely (mb->length > sizeof (shm_block_t)))
			memcpy (next_mb + sizeof (shm_block_t), data, mb->length - sizeof (shm_block_t));

		atomic_inc (&q->addr->blk_cnt);
		q->addr->head += mb->length;

		TRACE_LOG ("push queue: length=%d,tail=%d,head=%d,id=%llu,count=%d",
			mb->length, q->addr->tail, q->addr->head, mb->blk_id, 
			atomic_read (&q->addr->blk_cnt));
		ret_code = 0;
	}

	SEM_UNLOCK (q->sem, flag, ret_code);
}

int shmq_create ()
{
	int ret_code;

	sendq.length = config_get_intval ("shmq_length", 1 << 26);
	recvq.length = sendq.length;
#ifndef USE_SYSTEMV_SEM
	char *addr = mmap (NULL, 4096 , PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON , -1 , 0);
	if (addr == MAP_FAILED)
		ERROR_RETURN (("mmap failed, %s", strerror (errno)), -1);
	sendq.sem = (sem_t *) addr;
	recvq.sem = (sem_t *) (addr + sizeof (sem_t));
#endif

	ret_code = do_shmq_create (&sendq, config_get_intval("send_sem_key", IPC_PRIVATE)) 
		| do_shmq_create (&recvq, config_get_intval("recv_sem_key", IPC_PRIVATE));
	BOOT_LOG (ret_code, "create shared memory queue");
}

void shmq_destory ()
{
	if (sendq.addr != NULL) {
		munmap (sendq.addr, sendq.length);
		sendq.addr = NULL;
	}

	if (recvq.addr != NULL) {
		munmap (recvq.addr, recvq.length);
		recvq.addr = NULL;
	}

	if (sendq.sem) {
		sem_destroy (sendq.sem);
		sendq.sem = 0;
	}
	
	if (recvq.sem) {
		sem_destroy (recvq.sem);
		recvq.sem = 0;
	}
}
/*
char *shmblk_dump (const struct shm_block* mb)
{
	static char dump[1024];
	if (mb == NULL) {
		sprintf (dump, "shm_block == NULL");
	} else {
		snprintf (dump, sizeof (dump) - 1, "id=%llu, type=%d, length=%u, \
			sk.localip=%u, sk.localport=%u, sk.peerip=%u, sk.peerport=%u \
			sk.recvtm =%u, sk.sendtm=%u, sk.timeout=%d, sk.flag=%u",
			mb->blk_id, mb->type, mb->length, 
			mb->skinfo.local_ip, mb->skinfo.local_port, mb->skinfo.remote_ip, mb->skinfo.remote_port,
			mb->skinfo.recvtm, mb->skinfo.sendtm, mb->skinfo.timeout, mb->skinfo.type);
	}

	return dump;
}
*/
