/**
 * @file error_list.cpp
 * @author richard <richard@taomee.com>
 * @date 2010-09-21
 */

#include <stdio.h>

#include "error_list.h"

static const char g_errstr[][32] = {
	"no error",
	"not init",
	"already inited",
	"bad parameter",
	"out of memory",
	"no enough idle buffer",
	"system error"
};

const char *get_errstr(int errno)
{
	if (errno < 0 || errno >= E_MAX) {
		return NULL;
	} 

	return g_errstr[errno];
}

