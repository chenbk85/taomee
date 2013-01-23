#ifndef   _SHMQ_H_
#define   _SHMQ_H_
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "atomic.h"
#include "dll.h"
#include "lock.h"

#define LOCKED_MASK	0x01
#define SLEEP_MASK	0x02
// 修复nanosleep精度太高导致CPU压力过大的问题
#define CYCLE_WAIT_NANO_SEC	4000000
#define SHM_BLOCK_LEN_MAX	1<<23
enum {
	DAT_BLOCK,
	PAD_BLOCK,
	FIN_BLOCK
};

typedef struct shm_head {
	volatile int head;
	volatile int tail;
	atomic_t blk_cnt;
}__attribute__((packed)) shm_head_t;


struct shm_queue {
	shm_head_t	*addr;
	u_int		length;
#ifdef USE_SYSTEMV_SEM
	int		sem;
#else
	sem_t 		*sem;
#endif

};

struct shm_block {
	long long blk_id;	
	u_int length;		//total block length,including data
	char type;
	skinfo_t skinfo;
	u_short accept_fd;
	char data[0];
}__attribute__((packed)) shm_block_t;

extern struct shm_queue sendq;
extern struct shm_queue recvq;

extern int __init shmq_create ();
extern void __exit shmq_destory ();
extern int shmq_push(struct shm_queue* q, struct shm_block *mb, const char *data, int flag);
extern int shmq_pop (struct shm_queue* q, struct shm_block **mb, int flag);
extern char *shmblk_dump (const struct shm_block* mb);
#endif

