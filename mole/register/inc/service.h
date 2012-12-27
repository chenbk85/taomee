#include <sys/cdefs.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "benchapi.h"
#include "util.h"

#ifndef _SERVICE_H_
#define _SERVICE_H_

#ifdef __cplusplus
extern "C"{
#endif

int handle_init(int, char **, int);
int handle_input(const char*, int, const skinfo_t*);
int handle_process(char *, int , char **, int *, const skinfo_t*);
int handle_timer(int *);

//下列函数可以不用实现
//int handle_open(char **, int *, const skinfo_t*);
//void handle_close(const skinfo_t*);
void handle_fini(int);

#ifdef __cplusplus
}
#endif

#endif
