#ifndef _DLL_H_
#define _DLL_H_
#include "benchapi.h"
#include <sys/types.h>
extern int __init register_plugin (const char *file_name);
extern void __exit unregister_plugin ();
#endif
