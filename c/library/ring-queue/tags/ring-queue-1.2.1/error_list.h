/**
 * @file error_list.h
 * @author richard <richard@taomee.com>
 * @date 2010-09-21
 */

#ifndef ERROR_LIST_H_2010_09_21
#define ERROR_LIST_H_2010_09_21

enum {
	E_NO_ERROR,
	E_NOT_INIT,
	E_ALREADY_INITED,
	E_PARAM,
	E_MEMORY,
	E_SYSTEM,
	E_IDLE_BUFFER,
	E_MAX
};

const char *get_errstr(int err);

#endif /* ERROR_LIST_H_2010_09_21 */

