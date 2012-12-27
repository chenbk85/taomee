#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "daemon.h"

struct shm_queue;
struct shm_block;
struct sprite;

typedef struct fdsession {
  uint32_t id;
  uint16_t remote_port;
  uint32_t remote_ip;
  uint32_t last_tm;
} fdsession_t;

typedef struct fd_array_session {
  int fdmax;
  int count;
  fdsession_t cn[MAXFDS];
} fd_array_session_t;

extern fd_array_session_t fds;

int  handle_input(const char *, int, int, int);
int  handle_process(const uint8_t *, int, int, int);

int  handle_init();
void handle_close(int, int);
int  handle_open(const struct shm_block *mb);
int  handle_fini();

int shm_ctl_block_push(struct shm_queue *q, int fd, int type);
void restart_child_process();

#endif
