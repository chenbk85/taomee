#ifndef _ericma_log_h
#define _ericma_log_h

#ifdef __cplusplus
	extern "C" {
#endif

#include <semaphore.h>

#define LOG_ERROR     0
#define LOG_WARNING    1
#define LOG_INFO      2
#define LOG_DEBUG     3


struct LOG_CTX;

typedef struct LOG_CTX* LOG_HANDLE;

LOG_HANDLE log_open(const char* path);
int log_close(LOG_HANDLE handle);

int log_write(LOG_HANDLE handle, int level, const char* fmt, ...);
int log_puts(LOG_HANDLE handle, int level, const char* msg, unsigned int msglen);

int os_sem_wait(sem_t* handle);

#ifdef __cplusplus
	}
#endif


#endif
