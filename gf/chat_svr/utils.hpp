#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_

extern "C" 
{
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %u is greater than max %ld", (val_), (max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %u is less than min %ld", (val_), (min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %u is no equal to the expected %ld", (val_), (expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %u is less than min %ld"); \
				return; \
			} \
		} while (0)


#endif // SWITCH_UTILS_HPP_

