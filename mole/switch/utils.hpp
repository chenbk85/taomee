#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_

extern "C" {
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %d is greater than max %d", (val_), (uint32_t)(max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %d is less than min %d", (val_), (uint32_t)(min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %d is no equal to the expected %d", (val_), (uint32_t)(expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)

#endif // SWITCH_UTILS_HPP_

