#ifndef _SEM_H_
#define _SEM_H_

#ifdef USE_SYSTEMV_SEM
union semun{ 
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;    /* array for GETALL, SETALL */
                              /* Linux specific part: */
    struct seminfo *__buf;    /* buffer for IPC_INFO */
}; 

extern int sem_init (key_t key);
extern int sem_post (int semid);
extern int sem_wait (int semid);
#else
#include <semaphore.h>
#endif

extern int flock_init (char *lock_template);
extern int file_unlock (int fd, int offset);
extern int file_rlock (int fd, int offset);
extern int file_wlock (int fd, int offset);
#endif
