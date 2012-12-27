#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include <libtaomee/conf_parser/config.h>

#include "bindconf.h"
#include "shmq.h"
#include "daemon.h"
#include <libtaomee/log.h>
#include "util.h"
#include "benchapi.h"

inline struct shm_block *
head_mb (const struct shm_queue *q)
{
	return (struct shm_block *) ((char *) q->addr + q->addr->head);
}
inline struct shm_block *
tail_mb (const struct shm_queue *q)
{
	return (struct shm_block *) ((char *) q->addr + q->addr->tail);
}

static int
do_shmq_create (struct shm_queue *q)
{
	q->addr = (shm_head_t *) mmap (NULL, q->length, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANON, -1, 0);
	if (q->addr == MAP_FAILED)
		ERROR_RETURN (("mmap failed, %s", strerror (errno)), -1);

	q->addr->head = sizeof (shm_head_t);
	q->addr->tail = sizeof (shm_head_t);
	atomic_set (&(q->addr->blk_cnt), 0);
	pipe_create (q->pipe_handles);
	return 0;
}

static int
align_queue_tail (struct shm_queue *q)
{
	struct shm_block *pad;
	/*
	 *addr->head < addr->tail condition always is ok at here
	 */
	if (likely (q->addr->head >= q->addr->tail))
		return 0;

	pad = tail_mb (q);
	if (q->length - q->addr->tail < sizeof (shm_block_t)
			|| pad->type == PAD_BLOCK)
		q->addr->tail = sizeof (shm_head_t);

	return 0;
}

static int
align_queue_head (struct shm_queue *q, const struct shm_block *mb)
{
	int tail_pos = q->addr->tail;
	int head_pos = q->addr->head;
	struct shm_block *pad;

	int surplus = q->length - head_pos;

	if (unlikely (surplus < mb->length))
	{
		//queue is full
		if (unlikely (tail_pos == sizeof (shm_head_t)))
			ERROR_RETURN (("shm_queue is full,head=%d,tail=%d,mb_len=%d",
						head_pos, tail_pos, mb->length), -1);
		//bug
		else if (unlikely (q->addr->tail > head_pos))
		{
			ERROR_LOG ("shm_queue bug,head=%d,tail=%d,mb_len=%d,total len=%d",
					head_pos, tail_pos, mb->length, q->length);

			q->addr->tail = sizeof (shm_head_t);
			q->addr->head = sizeof (shm_head_t);
			//no pad mb
		}
		else if (unlikely (surplus < sizeof (shm_block_t)))
		{
			q->addr->head = sizeof (shm_head_t);
			//pad mb
		}
		else
		{
			pad = head_mb (q);
			pad->type = PAD_BLOCK;
			pad->length = surplus;
			pad->id = 0;
			q->addr->head = sizeof (shm_head_t);
		}
	}

	return 0;
}

int shmq_pop(struct shm_queue* q, struct shm_block** mb)
{
	struct shm_block* cur_mb;

	int head_pos = q->addr->head;
	align_queue_tail(q);
	//queue is empty
	if (q->addr->tail == head_pos)
		return -1;

	cur_mb = tail_mb(q);
#ifdef DEBUG
	//tail block overflow
	if (cur_mb->length < sizeof(struct shm_block) ||
		(q->addr->tail < head_pos && q->addr->tail + cur_mb->length > head_pos)
		|| (q->addr->tail > head_pos
			&& q->addr->tail + cur_mb->length > q->length))	{
		ERROR_LOG("shm_queue tail=%d,head=%d,shm_block length=%d", q->addr->tail, head_pos, cur_mb->length);
		exit(-1);
	}
#endif
	if (cur_mb->length > PAGESIZE)
		ERROR_RETURN(("too large packet, len=%d", cur_mb->length), -1);

	*mb = cur_mb;
	atomic_dec (&q->addr->blk_cnt);
	q->addr->tail += cur_mb->length;

	TRACE_LOG ("pop queue: q=%p, length=%d,tail=%d,head=%d,id=%lu,count=%lu",
			q, cur_mb->length, q->addr->tail, q->addr->head, (*mb)->id,
			atomic_read (&q->addr->blk_cnt));
	return 0;
}

/*
 * FIXME: data memory copy
 */
int
shmq_push (struct shm_queue *q, struct shm_block *mb, const uint8_t * data)
{
	char *next_mb;

	assert(mb->length >= sizeof(shm_block_t));

	if (mb->length > PAGESIZE) {
		ERROR_RETURN(("too large packet, len=%d", mb->length), -1);
	}

	if (align_queue_head(q, mb) == -1) {
		return -1;
	}

	int cnt = 0;
again:
	//queue is full, (PAGESIZE): prevent overwrite the buffer which shmq_pop refers
	if (q->addr->tail > q->addr->head
			&& q->addr->tail < q->addr->head + mb->length + PAGESIZE) {
		if (cnt++ < 10) {
			ALERT_LOG("queue is full, wait 10 microsecs. [cnt=%d]", cnt);
			usleep(10);
			goto again;
		}

		ALERT_LOG("queue [%p] is full.", q);
		return -1;
	}

	next_mb = (char*)head_mb(q);

	memcpy(next_mb, mb, sizeof(shm_block_t));
	if (likely(mb->length > sizeof(shm_block_t))) {
		memcpy(next_mb + sizeof(shm_block_t), data, mb->length - sizeof(shm_block_t));
	}

	q->addr->head += mb->length;
	atomic_inc(&q->addr->blk_cnt);

	write(q->pipe_handles[1], q, 1);
	TRACE_LOG("push queue: queue=%p,length=%d,tail=%d,head=%d,id=%lu,count=%d,fd=%d",
				 q, mb->length, q->addr->tail, q->addr->head, mb->id,
				 atomic_read (&q->addr->blk_cnt), mb->fd);
	return 0;
}

int shmq_create(bind_config_elem_t* p)
{
	int err;

	p->sendq.length = config_get_intval("shmq_length", 1 << 26); //default 32 M, realy 8 M
	p->recvq.length = p->sendq.length;

	err = do_shmq_create(&(p->sendq)) | do_shmq_create(&(p->recvq));
	BOOT_LOG (err, "Create shared memory queue: %dMB", p->recvq.length / 1024 / 512);
}

/**
 * close_shmq_pipe - close needless pipe fd
 *   @bc - bind config
 *   @idx - indicates which pipe fd to close for parent process, or upperbound for child process
 *   @is_child - indicates child or parent, 0 parent, otherwise child
 *
 */
void close_shmq_pipe(bind_config_t* bc, int idx, int is_child)
{
	if (is_child) {
		int i = 0;
		// close fds inherited from parent process
		for ( ; i != idx; ++i ) {
			close(bc->configs[i].recvq.pipe_handles[1]);
			close(bc->configs[i].sendq.pipe_handles[0]);
		}
	} else {
		close(bc->configs[idx].recvq.pipe_handles[0]);
		close(bc->configs[idx].sendq.pipe_handles[1]);
	}
}

void do_destroy_shmq(bind_config_elem_t* bc_elem)
{
	struct shm_queue* q = &(bc_elem->sendq);
	// close send queue
	if ( q->addr ) {
		munmap(q->addr, q->length);
		q->addr = 0;
	}
	// close receive queue
	q = &(bc_elem->recvq);
	if ( q->addr ) {
		munmap(q->addr, q->length);
		q->addr = 0;
	}
}

void shmq_destroy(const bind_config_elem_t* exclu_bc_elem, int max_shmq_num)
{
	bind_config_t* bc = get_bind_conf();

	int i = 0;
	for ( ; i != max_shmq_num; ++i ) {
		if (&(bc->configs[i]) != exclu_bc_elem) {
			do_destroy_shmq(&(bc->configs[i]));
		}
	}
}


