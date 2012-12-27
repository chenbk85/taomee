#ifndef GF_SERVICE_H
#define GF_SERVICE_H
#include <sys/cdefs.h>
#include "benchapi.h"
#include "proto.h"

__BEGIN_DECLS

int handle_init(int, char **, int);
int handle_input(const char*, int, const skinfo_t*);
int handle_process(char *, int , char **, int *, const skinfo_t*);

void handle_fini(int);

__END_DECLS
#endif
