#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_
// C89
#include <string.h>
#include <time.h>
// C99
#include <stdint.h>
#include <stdlib.h>
// Posix
#include <arpa/inet.h>

#include <glib.h>
#include <regex.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <libtaomee/log.h>
#include <libtaomee/tm_dirty/tm_dirty.h>
#include "libtaomee/project/stat_agent/msglog.h"
#include <libtaomee/crypt/qdes.h>
#include <libtaomee/dataformatter/bin_str.h>
#include <libtaomee/list.h>

#ifdef __cplusplus
}
#endif

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", int(val_), int(min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", int(val_), int(expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)

#include <list>
#include <map>


#endif // SWITCH_UTILS_HPP_

