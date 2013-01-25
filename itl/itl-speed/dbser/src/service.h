#ifndef _SERVICE_H_
#define _SERVICE_H_
#include <sys/cdefs.h>
#include "benchapi.h"

__BEGIN_DECLS

int handle_init(int, char **, int);
int handle_input(const char*, int, const skinfo_t*);
int handle_process(char *, int , char **, int *, const skinfo_t*);
//下列函数可以不用实现
//int handle_open(char **, int *, const skinfo_t*);
//void handle_close(const skinfo_t*);
int handle_timer(int* intvl);
void handle_fini(int);

__END_DECLS
#endif
