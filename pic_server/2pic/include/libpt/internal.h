#ifndef INTERNAL_H
#define INTERNAL_H

#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <dirent.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

/*
 * Internal Macros
 */
#ifdef BUILD_DEBUG
#define P_DEBUG(fmt, args...)	fprintf(stderr, "[P_DEBUG] " fmt " (%s:%d)\n", ##args, __FILE__, __LINE__);
#else
#define P_DEBUG(fms, args...)
#endif

/*
 * Macro Functions
 */
#define	CONST_STRLEN(x)		(sizeof(x) - 1)

#define	DYNAMIC_VSPRINTF(s, f)							\
do {										\
	size_t _strsize;							\
	for(_strsize = 1024; ; _strsize *= 2) {					\
		s = (char*)malloc(_strsize);					\
		if(s == NULL) {							\
			P_DEBUG("DYNAMIC_VSPRINTF(): can't allocate memory.");	\
			break;							\
		}								\
		va_list _arglist;						\
		va_start(_arglist, f);						\
		int _n = vsnprintf(s, _strsize, f, _arglist);			\
		va_end(_arglist);						\
		if(_n >= 0 && _n < _strsize) break;				\
		free(s);							\
	}									\
} while(0)

/*
 * Q_MUTEX Macros
 */
#ifndef _MULTI_THREADED
#define	_MULTI_THREADED
#endif

#include <pthread.h>
#include <errno.h>

struct _Q_MUTEX {
	pthread_mutex_t	mutex;
	pthread_t	owner;
	int		count;
};
typedef struct _Q_MUTEX	Q_MUTEX_T;


#define Q_MUTEX_INIT(x,r)								\
do {											\
	memset((void*)&x, 0, sizeof(Q_MUTEX_T));					\
	pthread_mutexattr_t _mutexattr;							\
	pthread_mutexattr_init(&_mutexattr);						\
	if(r == 1) {									\
		pthread_mutexattr_settype(&_mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);	\
	}										\
	int _ret = pthread_mutex_init(&x.mutex, &_mutexattr);				\
	pthread_mutexattr_destroy(&_mutexattr);						\
	if(_ret != 0) {									\
		char _errmsg[64];							\
		strerror_r(_ret, _errmsg, sizeof(_errmsg));				\
		P_DEBUG("Q_MUTEX: can't initialize mutex. [%d:%s]", _ret, _errmsg);	\
		exit(EXIT_FAILURE);							\
	}										\
	P_DEBUG("Q_MUTEX: initialized.");							\
} while(0)
//pthread_mutexattr_setprotocol(&_mutexattr, PTHREAD_PRIO_INHERIT);

#define	Q_MUTEX_LEAVE(x)								\
do {											\
	if(!pthread_equal(x.owner, pthread_self())) P_DEBUG("Q_MUTEX: unlock - owner mismatch.");	\
	x.count--;									\
	pthread_mutex_unlock(&x.mutex);							\
} while(0)
//	P_DEBUG("Q_MUTEX: unlock, cnt=%d", x.count);

#define MAX_MUTEX_LOCK_WAIT	(5000)
#define	Q_MUTEX_ENTER(x)								\
do {											\
	while(1) {									\
		int _ret, i;								\
 		for(i = 0; (_ret = pthread_mutex_trylock(&x.mutex)) != 0 && i < MAX_MUTEX_LOCK_WAIT; i++) {	\
			if(i == 0) P_DEBUG("Q_MUTEX: mutex is already locked - try again");	\
			usleep(1);							\
		}									\
		if(_ret == 0) break;							\
		char _errmsg[64];							\
		strerror_r(_ret, _errmsg, sizeof(_errmsg));				\
		P_DEBUG("Q_MUTEX: can't get lock - force to unlock. [%d:%s]", _ret, _errmsg);	\
		Q_MUTEX_LEAVE(x);							\
	}										\
	x.count++;									\
	x.owner = pthread_self();							\
} while(0)
//	P_DEBUG("Q_MUTEX: locked, cnt=%d", x.count);

#define Q_MUTEX_DESTROY(x)								\
do {											\
	if(x.count != 0) P_DEBUG("Q_MUTEX: mutex counter is not 0.");			\
	int _ret;									\
	while((_ret = pthread_mutex_destroy(&x.mutex)) != 0) {				\
		char _errmsg[64];							\
		strerror_r(_ret, _errmsg, sizeof(_errmsg));				\
		P_DEBUG("Q_MUTEX: force to unlock mutex. [%d:%s]", _ret, _errmsg);	\
		Q_MUTEX_LEAVE(x);							\
	}										\
	P_DEBUG("Q_MUTEX: destroyed.");							\
} while(0)

/*
 * Internal Definitions
 */
#define QDECODER_PRIVATEKEY	"qDecoder-by-SEUNGYOUNG.KIM"
#define	MAX_LINEBUF		(1023+1)
#define	DEF_DIR_MODE		(S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH)
#define	DEF_FILE_MODE		(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)


#endif	/* _QINTERNAL_H */

