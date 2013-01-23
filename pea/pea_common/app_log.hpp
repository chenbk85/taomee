#ifndef _APP_LOG_H_
#define _APP_LOG_H_

#include "log.hpp"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ENABLE_GONGFU_LOG

#define GF_LOG_OPEN 	gongfu_log_open
#define GF_LOG_CLOSE 	gongfu_log_close
#define GF_LOG_WRITE    gongfu_log_write

int gongfu_log_open(const char* url);
int gongfu_log_close();
void gongfu_log_write(int level, const char* src, const char* fmt, ...);

#else


#define GF_LOG_OPEN(...)
#define GF_LOG_CLOSE(...)
#define GF_LOG_WRITE(...)


#endif



#ifdef __cplusplus
}
#endif

#endif
