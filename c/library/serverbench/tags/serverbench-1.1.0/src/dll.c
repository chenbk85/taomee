#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include "dll.h"
#include "benchapi.h"

dll_func_t dll = {
	handle:  NULL,
};
/*
static int recv_package_len (const char* buffer, int len, const skinfo_t* sk)
{
	if (len < 4)
		return 0;

	len = ntohl (len);
	return len;
}
*/
int register_plugin (const char *file_name)
{
	char *error; 
	int ret_code = -1;

#define DLFUNC_NO_ERROR(h, v, name) do { \
	v = dlsym (h, name); \
	dlerror (); \
}while (0)

#define DLFUNC(h, v, name) do { \
	v = dlsym (h, name); \
	if ((error = dlerror ()) != NULL) { \
		ERROR_LOG ("dlsym error, %s", error); \
		dlclose (h); \
		h = NULL; \
		goto out; \
	} \
}while (0)
	
	dll.handle = dlopen (file_name, RTLD_NOW);
	if ((error = dlerror()) != NULL) {
		ERROR_LOG ("dlopen error, %s", error);
		goto out;
	}
	
	DLFUNC_NO_ERROR (dll.handle, dll.handle_init, "handle_init");
	DLFUNC_NO_ERROR (dll.handle, dll.handle_fini, "handle_fini");
	DLFUNC_NO_ERROR (dll.handle, dll.handle_open, "handle_open");
	DLFUNC_NO_ERROR (dll.handle, dll.handle_close, "handle_close");
	DLFUNC_NO_ERROR (dll.handle, dll.handle_timer, "handle_timer");

	DLFUNC (dll.handle, dll.handle_input, "handle_input");
	DLFUNC (dll.handle, dll.handle_process, "handle_process");
	ret_code = 0;
out:
	BOOT_LOG (ret_code, "dlopen %s", file_name);
}

void unregister_plugin ()
{
	if (dll.handle != NULL){
		dlclose (dll.handle);
		dll.handle = NULL;
	}
}
