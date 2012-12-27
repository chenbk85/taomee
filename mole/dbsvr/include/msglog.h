#ifndef _MSGLOG_H_
#define _MSGLOG_H_
#include <sys/cdefs.h>

__BEGIN_DECLS

int msglog(const char *logfile, uint32_t type, time_t timestamp, const void *data, int len);

__END_DECLS
#endif

