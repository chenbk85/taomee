#ifndef SWITCH_UTILS_HPP_
#define SWITCH_UTILS_HPP_

extern "C" {
#include <libtaomee/log.h>
}

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

#define n_make_online_dead	1
#define n_report_usr_num	2
#define n_try_match_again	3
#define n_get_val_from_db	4
#define n_refresh_beast_cnt	5
#define n_start_refresh		6

#define REGISTER_TIMER_TYPE(nbr_, cb_, rstart_) \
		do { \
			if (register_timer_callback(nbr_, cb_) == -1) \
				ERROR_RETURN(("register timer type error\t[%u]", nbr_), -1); \
		} while(0)


#endif // SWITCH_UTILS_HPP_

