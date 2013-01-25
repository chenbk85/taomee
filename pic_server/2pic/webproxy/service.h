#ifndef _SERVICE_H_
#define _SERVICE_H_

#include "benchapi.h"
#include "proto.h"
#include <stdint.h>

int handle_init (int, char **, int);
int handle_input (const char*, int, const skinfo_t*);
int handle_filter_key (const char*, int, uint32_t*);
int handle_process (char *, int , char **, int *, const skinfo_t*);
int handle_open (char **, int *, const skinfo_t*);
int handle_close (const skinfo_t*);
int handle_timer (int *);
void handle_fini (int);

#endif
