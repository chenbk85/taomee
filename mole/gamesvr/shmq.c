#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <ant/utils.h>

#include "shmq.h"
#include "daemon.h"
#include <libtaomee/log.h>
#include "config.h"
#include "util.h"

struct shm_queue sendq;
struct shm_queue recvq;

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
			ERROR_LOG ("shm_queue bug,head=%d,tail=%d,mb_len=%d,q_len=%d,total len",
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
	if (cur_mb->length > RCVBUFSZ)
		ERROR_RETURN (("too large packet, len=%d", cur_mb->length), -1);

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

	assert (mb->length >= sizeof (shm_block_t));

	if (mb->length > RCVBUFSZ)
		ERROR_RETURN (("too large packet, len=%d", mb->length), -1);

	if (align_queue_head (q, mb) == -1)
		return -1;

	int cnt = 0;
again:
	//queue is full, (PAGESIZE): prevent overwrite the buffer which shmq_pop refers
	if (q->addr->tail > q->addr->head
			&& q->addr->tail < q->addr->head + mb->length + RCVBUFSZ) {
		if (cnt++ < 10) {
			ALERT_LOG("queue is full, wait 10 microsecs. [cnt=%d]", cnt);
			usleep(10);
			goto again;
		}

		ALERT_LOG("queue [%p] is full.", q);
		return -1;
	}

	next_mb = (char *) head_mb (q);

	memcpy (next_mb, mb, sizeof (shm_block_t));
	if (likely (mb->length > sizeof (shm_block_t)))
		memcpy (next_mb + sizeof (shm_block_t), data, mb->length - sizeof (shm_block_t));

	q->addr->head += mb->length;
	atomic_inc (&q->addr->blk_cnt);

	if (q == &sendq)
		write (pipe_handles[3], q, 1);
	else if (q == &recvq)
		write (pipe_handles[1], q, 1);

	TRACE_LOG ("push queue: queue=%p,length=%d,tail=%d,head=%d,id=%lu,count=%d,fd=%d",
		 q, mb->length, q->addr->tail, q->addr->head, mb->id,
		 atomic_read (&q->addr->blk_cnt), mb->fd);
	return 0;
}

int
shmq_create ()
{
	int err;

	sendq.length = config_get_intval ("shmq_length", 1 << 26);
	recvq.length = sendq.length;

	err = do_shmq_create (&sendq) | do_shmq_create (&recvq);
	BOOT_LOG (err, "Create shared memory queue: %dMB",
			recvq.length / 1024 / 512);
}

void
shmq_destroy ()
{
	if (sendq.addr != NULL)
	{
		munmap (sendq.addr, sendq.length);
		sendq.addr = NULL;
	}

	if (recvq.addr != NULL)
	{
		munmap (recvq.addr, recvq.length);
		recvq.addr = NULL;
	}
}

/*
   char *shmblk_dump (const struct shm_block* mb)
   {
   static char dump[1024];
   if (mb == NULL) {
   sprintf (dump, "shm_block == NULL");
   } else {
   snprintf (dump, sizeof (dump) - 1, "id=%lu, type=%d, length=%u, \
   sk.localip=%u, sk.localport=%u, sk.peerip=%u, sk.peerport=%u \
   sk.recvtm =%u, sk.sendtm=%u, sk.timeout=%d, sk.flag=%u",
   mb->id, mb->type, mb->length,
   mb->skinfo.local_ip, mb->skinfo.local_port, mb->skinfo.remote_ip, mb->skinfo.remote_port,
   mb->skinfo.recvtm, mb->skinfo.sendtm, mb->skinfo.timeout, mb->skinfo.type);
   }

   return dump;
   }
   */
