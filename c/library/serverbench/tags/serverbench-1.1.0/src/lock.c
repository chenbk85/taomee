#include <sys/sem.h>
#include <signal.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include "lock.h"
#include "benchapi.h"

int flock_init (char *lock_template)
{
	int ret_fd = mkstemp (lock_template);
	if (ret_fd < 0)
		ERROR_RETURN (("mkstemp %s error, %m", lock_template), -1);

	unlink (lock_template);
	return ret_fd;
}

int file_unlock (int fd, int offset)
{
	struct flock unlock_it;
	int     rc;

	unlock_it.l_type = F_UNLCK;
	unlock_it.l_whence = SEEK_SET;
	unlock_it.l_start = offset;
	unlock_it.l_len = 1;	

	while ( (rc = fcntl(fd, F_SETLKW, &unlock_it)) < 0) {
		if (errno == EINTR)
			continue;
		else 
			ERROR_RETURN (("funlock error,fd=%d,offset=%d, %m", fd, offset), -1);
	}

	return 0;
}

int file_rlock (int fd, int offset)
{
	struct flock lock_it;
	int     rc;

	lock_it.l_type = F_RDLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = offset;
	lock_it.l_len = 1;	

	if ( (rc = fcntl(fd, F_SETLKW, &lock_it)) < 0) {
		if (errno != EINTR)
			ERROR_LOG ("flock error,fd=%d,offset=%d, %m", fd, offset);
		return -1;
	}

	return 0;
}

int file_wlock (int fd, int offset)
{
	struct flock lock_it;
	int     rc;

	lock_it.l_type = F_WRLCK;
	lock_it.l_whence = SEEK_SET;
	lock_it.l_start = offset;
	lock_it.l_len = 1;	

	if ( (rc = fcntl(fd, F_SETLKW, &lock_it)) < 0) {
		if (errno != EINTR)
			ERROR_LOG ("flock error,fd=%d,offset=%d, %m", fd, offset);
		return -1;
	}

	return 0;
}
#ifdef USE_SYSTEMV_SEM
int sem_init (key_t key)
{
	int ret_semid = semget(key, 1, IPC_CREAT | 0664);
	if (ret_semid != -1) {
	    	union semun arg;
		arg.val = 1;
		semctl (ret_semid, 0, SETVAL, arg);
	} else {
		ERROR_LOG ("%s", "semget error, %m");
	}

	return ret_semid;
}

int sem_wait (int semid)
{
	struct sembuf op;
	op.sem_num = 0;
	op.sem_flg = SEM_UNDO;
	op.sem_op = -1;
	while (semop (semid, &op, 1) == -1) {
		if (errno != EINTR)
			ERROR_LOG ("semop %d error, %m", semid);
	}

	return 0;
}

int sem_post (int semid)
{
	struct sembuf op;
		
	op.sem_num = 0;
	op.sem_flg = SEM_UNDO;
	op.sem_op = 1;
		
	if (semop (semid, &op, 1) == -1){
		if (errno != EINTR)
			ERROR_LOG ("semop %d error, %m", semid);
		return -1;
	}
		
	return 0;
}
#endif
