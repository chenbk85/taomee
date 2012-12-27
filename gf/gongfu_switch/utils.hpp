#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_

extern "C" 
{
#include <libtaomee/log.h>
}

#define CHECK_VAL_LE(val_, max_) \
		if ( (val_) > (max_) ) ERROR_RETURN(("val %lu is greater than max %lu", (unsigned long)(val_), (unsigned long)(max_)), -1)
#define CHECK_VAL_GE(val_, min_) \
		if ( (val_) < (min_) ) ERROR_RETURN(("val %lu is less than min %lu", (unsigned long)(val_), (unsigned long)(min_)), -1)
#define CHECK_VAL(val_, expect_) if ( (val_) != (expect_) ) ERROR_RETURN(("val %lu is no equal to the expected %lu", (unsigned long)(val_), (unsigned long)(expect_)), -1)

#define CHECK_VAL_GE_VOID(val_, min_) \
		do { \
			if ( (val_) < (min_) ) { \
				ERROR_LOG("val %d is less than min %d"); \
				return; \
			} \
		} while (0)


#endif // SWITCH_UTILS_HPP_

