#ifndef   _SHMQ_H_
#define   _SHMQ_H_

#include <stdint.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

#include <libtaomee/atomic.h>

#include "net.h"

#define LOCKED_MASK	0x01
#define SLEEP_MASK	0x02
#define SHM_BLOCK_LEN_MAX	1<<23

enum {
	DATA_BLOCK = 0,
	PAD_BLOCK,
	FIN_BLOCK,	// child informs parent to close a connection
	OPEN_BLOCK,
	CLOSE_BLOCK	// parent informs child that a connection had been closed
};

typedef struct shm_head {
	volatile int head;
	volatile int tail;
	atomic_t blk_cnt;
} __attribute__ ((packed)) shm_head_t;

struct shm_queue {
	shm_head_t * addr;
	u_int length;
};

typedef struct shm_block {
	uint32_t id;
	uint32_t fd;
	uint16_t length; ///< total block length,including data
	char type;
	uint8_t data[0];
} __attribute__ ((packed)) shm_block_t;

extern struct shm_queue sendq;
extern struct shm_queue recvq;
extern int shmq_create ();
extern void shmq_destroy ();
extern int shmq_push (struct shm_queue *q, struct shm_block *mb, const uint8_t * data);
extern int shmq_pop (struct shm_queue *q, struct shm_block **mb);
extern char *shmblk_dump (const struct shm_block *mb);

static inline void
epi2shm (int fd, struct shm_block *mb)
{
	mb->id = epi.fds[fd].id;
	mb->fd = fd;
	mb->type = DATA_BLOCK;
	mb->length = epi.fds[fd].cb.rcvprotlen + sizeof (struct shm_block);
}

#endif	/*  */
